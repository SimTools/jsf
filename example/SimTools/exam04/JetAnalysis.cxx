///////////////////////////////////////////////////////////////////
//
//  JetAnalysis
//
//  Sample program for Jet Analysis
//
//$Id: 
//  
//////////////////////////////////////////////////////////////////


#include "JSFSteer.h"
#include "JSFSIMDST.h"
#include "JetAnalysis.h"
#include "ANLJetFinder.h"
#include "ANLTrack.h"
#include "ANLEventShape.h"
#include "JSFZVTOP3.h"

ClassImp(JetAnalysis)
ClassImp(JetEvent)
ClassImp(JSFJet)

TClonesArray *JetEvent::fgJets=0;

Int_t    JetAnalysis::fForcedNJets=0;
Double_t JetAnalysis::fYcut=0.0;
Int_t    JetAnalysis::fJetFinderAlgorithm=2;
Double_t JetAnalysis::fVTXNSigCut=3.0;
Double_t JetAnalysis::fCutCosth=0.8;
Int_t    JetAnalysis::fCutNDF=20;

//_____________________________________________________________________________
JetAnalysis::JetAnalysis(const char *name, const char *title)
       : JSFModule(name,title)
{

  fForcedNJets=gJSF->Env()->GetValue("JetAnalysis.ForcedNJets",4);
  sscanf(gJSF->Env()->GetValue("JetAnalysis.ForcedNJets","0.0005"),"%lg",&fYcut);
  fJetFinderAlgorithm=gJSF->Env()->GetValue("JetAnalysis.JetFinderAlgorithm",0);
  sscanf(gJSF->Env()->GetValue("JetAnalysis.VTXNSigCut","3.0"),"%lg",&fVTXNSigCut);

  sscanf(gJSF->Env()->GetValue("JetAnalysis.CutCosth","0.80"),"%lg",&fCutCosth);
  sscanf(gJSF->Env()->GetValue("JetAnalysis.CutNDF","20"),"%d",&fCutNDF);
  


  TDirectory *last=gDirectory;
  gFile->cd("/");

  fJetTree=new TTree("A", "Jet Analysis");
  fJetEvent=new JetEvent();
  fJetTree->Branch("Event","JetEvent",&fJetEvent, 32000, 2);
  last->cd();

  SetMakeBranch(kFALSE);

}

//_____________________________________________________________________________
JetAnalysis::~JetAnalysis()
{
  delete fJetTree;
  delete fJetEvent;
}

//_____________________________________________________________________________
Bool_t JetAnalysis::Process(Int_t nev)
{
// 
  fJetEvent->Clear();

  TObjArray *particles=new TObjArray(1000);

  ShapeAnalysis(particles);

  JetClustering(particles);

  fJetTree->Fill();

  particles->Delete();
  delete particles;


  return kTRUE;
}

//_____________________________________________________________________________
void JetAnalysis::ShapeAnalysis(TObjArray *tracks)
{
  JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *geb=(JSFGeneratorBuf*)gen->EventBuf();

  Double_t ecm=geb->GetEcm();

  fJetEvent->SetEcm(ecm);

  //-----------------------------------------------------
  // Save tracks for Jet clustering
  //-----------------------------------------------------

  JSFSIMDST     *sds     = (JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
  JSFSIMDSTBuf  *evt     = (JSFSIMDSTBuf*)sds->EventBuf();

  Int_t          ntrks   = evt->GetNLTKCLTracks();      // No. of tracks
  TObjArray     *trks    = evt->GetLTKCLTracks();       // combined tracks

  Int_t ntracks=0;
  Int_t nc=0;
  ANL4DVector qsum;
  for ( Int_t i = 0; i < ntrks; i++ ) {
    JSFLTKCLTrack *t = (JSFLTKCLTrack*)trks->UncheckedAt(i);
    ANLTrack *qt = new ANLTrack(t);
    tracks->Add(qt);           // track 4-momentum
    qsum += *qt;               // total 4-momentum
    ntracks++;
    if( t->GetCharge() != 0 ) { nc++ ; }
  }

  //
  ANLEventShape evshape;
  evshape.Initialize(*tracks);

  fJetEvent->SetEventNumber(gJSF->GetEventNumber());
  fJetEvent->SetThrust(evshape.GetThrust());
  fJetEvent->SetNTracks(ntracks);
  fJetEvent->SetNCharged(nc);

  ANL4DVector cm((Double_t)ecm,0.0,0.0,0.0);
  ANL4DVector miss=cm-qsum;

  Float_t ptsum=TMath::Sqrt(qsum.Px()*qsum.Px()+qsum.Py()*qsum.Py());

  fJetEvent->SetMissingMass(miss.GetMass());
  fJetEvent->SetESum(qsum.E());
  fJetEvent->SetPtSum(ptsum);
  fJetEvent->SetPlSum(qsum.Pz());
 
}


//_____________________________________________________________________________
void JetAnalysis::JetClustering(TObjArray *tracks)
{

  if( tracks->GetEntries() <= fForcedNJets ) return;

  ANLJetFinder *jclust=0;
  if( fJetFinderAlgorithm == 0 ) {
    jclust=new ANLJadeJetFinder(fYcut);
  }
  else if( fJetFinderAlgorithm == 1 ) {
    jclust=new ANLJadeEJetFinder(fYcut);
  }
  else {
    jclust=new ANLDurhamJetFinder(fYcut);
  }
  jclust->Initialize(*tracks);
  jclust->FindJets();
  Int_t njets=jclust->GetNjets();

  fJetEvent->SetNumJets(njets);  // If number of jets is less than requested,
  if( njets < fForcedNJets ) {  // return after delete jclust object.
    delete jclust;
    return;
  }
  
  //----------------------------------------------------
  // Does Forced njet clustering
  //----------------------------------------------------
  jclust->ForceNJets(fForcedNJets);
  fJetEvent->SetYmax(jclust->GetYmax());

  //-----------------------------------------------------
  // Save clustered jet energy
  //-----------------------------------------------------

  TClonesArray &savejet=*(fJetEvent->GetJets());

  TObjArray &jets = jclust->GetJets();
  TIter nextjet(&jets);
  ANLJet *jetp;
  Int_t nj=0;
  while( (jetp=(ANLJet*)nextjet()) ) {
    JSFJet *jsfjet=new( savejet[nj++] ) JSFJet(jetp->E(), jetp->Px(), jetp->Py(), jetp->Pz());
    jsfjet->SetNTracks(jetp->GetNparticles());
    const TObjArray &particles=(jetp->GetParticlesInJet());
    TIter nextpart(&particles);
    ANLTrack *track;
    Int_t nc=0;
    Int_t nsig=0;
    while( (track=(ANLTrack*)nextpart()) ) {
      JSFLTKCLTrack *t=track->GetLTKCLTrack();
      if( t->GetCharge() != 0 ) nc++ ;
      if( t->GetVTXNSig() > fVTXNSigCut ) nsig++;
    }
    jsfjet->SetNOffIPTracks(nsig);
    jsfjet->SetNCharged(nc);
    // Does Vertex analysis
    jsfjet->JetVertexing(&particles);
    //
  }  
  fJetEvent->SetNumJets(nj);

  delete jclust;

}

//_____________________________________________________________________________
void JSFJet::JetVertexing(const TObjArray *particles)
{
  // Does vertexing using particles in jets.
  // particles is an array of pointer to LTKCLTrack tracks in a jet.

  TObjArray jettracks;
  TIter nextpart(particles);
  ANLTrack *track;
  Int_t nt=0;

  while( (track=(ANLTrack*)nextpart()) ) {
    JSFLTKCLTrack *t=track->GetLTKCLTrack();
    if( t->GetCharge() == 0 ) continue;
    JSFCDCTrack *ct=t->GetCDC();
    if( ct == 0 ) continue;

    Double_t ctcosth=ct->GetPz()/TMath::Sqrt(ct->GetPx()*ct->GetPx() + 
					     ct->GetPy()*ct->GetPy() +
					     ct->GetPz()*ct->GetPz() );

    if( ct->GetNVTX() < 5 ) continue;  // Skip if # of associated VTX hits is less than 5.
    if( TMath::Abs(ctcosth) > JetAnalysis::GetCutCosth() ) continue;
    if( ct->GetNDF() < JetAnalysis::GetCutNDF() ) continue;
    JSF3DV_f pivot=ct->GetPivot();
    if( pivot.Abs()  >  3.0 ) continue;    // Omit track if its pivot is away from IP

    JSFCDCTrack *ctr=new JSFCDCTrack(*ct);
    
    jettracks.Add(ctr);
    nt++;
  }
  
  JSFZVTOP3 v;  
  if( nt > 3 ) {
    v.MakeVertices(&jettracks);
    fVertex.SetZVTOP3Info(v);
  }

  jettracks.Delete();

}

//_____________________________________________________________________________
void JSFVertex::SetZVTOP3Info(JSFZVTOP3 &v)
{

  fNTRK=v.GetNVRT();
  fNVRT=v.GetNVRT();
  fNISO=v.GetNISO();
  fMSPTM=v.GetMSPTM();
  fPTM2=v.GetPTM2();
  fChisq=v.GetChisquare();
  fNSec=v.GetNsec();
  fDecayLength=v.GetDecayLength();
  fSecondaryMass=v.GetSecondaryMass();
  fSecondaryMomentum=v.GetSecondaryMomentum();
}

//_____________________________________________________________________________
JetEvent::JetEvent()
{
  if( !fgJets ) fgJets=new TClonesArray("JSFJet", 20);
  fJets=fgJets;

}

//_____________________________________________________________________________
JetEvent::~JetEvent()
{
  Reset();
}


//_______________________________________
void JetEvent::Clear(Option_t *opt)
{
  fNumJets=0;
  fYmax=0;
  fThrust=0.0;
  
  fJets->Delete(opt);
}


//_______________________________________
void JetEvent::Reset(Option_t *opt)
{
  delete fgJets;
  fgJets=0;
}



