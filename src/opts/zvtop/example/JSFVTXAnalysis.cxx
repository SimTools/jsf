//*LastUpdate : 11-June-2000     Akiya Miyamoto 
//*-- Author  : Akiya Miyamoto  11-June-2000

///////////////////////////////////////////////////////////////////
//
//  Study vertexing performance
//
//$Id: 
//  
//////////////////////////////////////////////////////////////////

// #define debug 

#include "JSFVTXAnalysis.h"
#include "JSFSteer.h"
#include "JSFSIMDST.h"
#include "TH1.h"
#include "TNtuple.h"
#include "JSFGeneratorVertex.h"
#include "JSFVertexing.h"
#include "JSFGeoCFit.h"
#include "ANL4DVector.h"
#include "JSFGenerator.h"

using namespace std;

ClassImp(JSFVTXAnalysis)
ClassImp(JSFVTXAnalysisBuf)

TH1F *hEvis,*hNpart, *hNjets, *hYmax, *hNpInJets, *hJetCosTheta;
TH1F *hDrn;
TH1F *hMSPTM, *hNVertices, *hDecayLength, *hVmass;
TH1F *hK0, *hLambda, *hConfl, *hNtry, *hV0decay;
TH1F *hJetEnergy;

TNtuple *ntuple;

//_____________________________________________________________________________
JSFVTXAnalysis::JSFVTXAnalysis(const char *name, const char *title)
       : JSFModule(name,title)
{
  fEventBuf = new JSFVTXAnalysisBuf("JSFVTXAnalysisBuf", 
	 "JSFVTXAnalysis event buffer",this);

  //  fZVTOP3=0;
  //  fJclust=0;

}

//_____________________________________________________________________________
JSFVTXAnalysis::~JSFVTXAnalysis()
{

  fZVTOP3.Delete();

  //  if( fJclust ) delete fJclust;

}

// ---------------------------------------------------------------
Bool_t JSFVTXAnalysis::Initialize()
{
  // Initialize
  // Put histogram defiition here, if needed.

  hEvis=new TH1F("hEvis","Visible energy",100,0,200);
  hNpart=new TH1F("hNpart","Number of particles",100,0,100);
  hNjets=new TH1F("hNjets","Njets for Ycut=0.01",10,0,10);
  hYmax =new TH1F("hYmax","Ymax for forced 2 jets",50,0,0.5);
  hNpInJets=new TH1F("hNpInjets","Number of particles in jets",50,0,50);
  hJetCosTheta=new TH1F("hJetCosTheta","CosTheta of jets",100,-1,1);
  hJetEnergy=new TH1F("hJetEnergy","JetEnergy",100,0,50.0);
  hDrn= new TH1F("hDrn","Normalized distance to IP",50,0,10);
  
  hMSPTM = new TH1F("hMSPTM","MSPTM",100,0,5.0);
  hNVertices = new TH1F("hNVertices", "Number of vertices",20,0,20);
  hDecayLength=new TH1F("hDecayLength", "Decay Length",100,0.0,5.0);
  hVmass=new TH1F("hVmass", "Vertex mass",100,0.0,5.0);

  hK0=new TH1F("hK0","K0s mass",20,0.48,0.52);
  hLambda=new TH1F("hLambda","Lambda",20,1.0,1.4);
  hConfl=new TH1F("hConfl","Confidence Level of VTX fit",100,0.0,1.0);
  hNtry=new TH1F("hNtry","No. of Try for fit",100,0,100);
  hV0decay=new TH1F("hV0decay","V0 decay length",100,0.0,1.0);

  ntuple=new TNtuple("ntuple","Vertex properties",
     "nevt:jetno:jcs:jnt:nvtx:msptm:dl:msec:chisq:prob:nsec:evis:ymax:mspt:gdl:vdif:gns:gmsec:gmsptm:gpt:ptm2:prob2:gdl2:dl2:dl0:probo:nseco:jete:psec");

  return kTRUE;
}

// ---------------------------------------------------------------
void JSFVTXAnalysis::Delete()
{
  
  TIter nextvtx(&fZVTOP3);
  JSFZVTOP3 *v;
  while( (v=(JSFZVTOP3 *)nextvtx()) ) {
    v->Delete();
  }

}


// ---------------------------------------------------------------
Bool_t JSFVTXAnalysis::Process(Int_t nev)
{
// 


  //  printf("\n\n Event #=%d\n",nev);

  JSFSIMDST *sds=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
  JSFSIMDSTBuf *evt=(JSFSIMDSTBuf*)sds->EventBuf(); 

  JSFGeneratorVertexing gvtx;
  gvtx.MakeVertices(evt->GetGeneratorParticles());

#if debug
   gvtx.DebugPrint();


   //PythiaGenerator *pyg=(PythiaGenerator*)gJSF->FindModule("PythiaGenerator");
   //   pyg->GetPythia()->Lulist(1);

   printf("\n\n");
   TIter next(&gvtx);
   JSFGeneratorVertex *v;
   while( (v=(JSFGeneratorVertex*)next())){
     if(v->IsNormalVertex()) {
       printf(" Generator NGeneration is %d parent ID is %d\n",
	      v->GetNGeneration(), v->GetParentFlavourID());
       if( v->GetParent() != 0 ) {
	 printf(" Momentum of parent track is %g %g %g\n",
		v->GetParent()->GetPx(),
		v->GetParent()->GetPy(),
		v->GetParent()->GetPz());
       }
     }
   }
  
#endif

  fZVTOP3.Delete();

  TObjArray particles;
  ANL4DVector sum;   

  fJclust.SetYcut(0.01);
  // fJclust=new ANLJadeEJetFinder(0.01);
  Int_t npart=JetAnalysis(particles, sum, &fJclust);   

  hEvis->Fill(sum.E());
  hNpart->Fill((Float_t)npart);
  if( npart < 2 ) {
    particles.Delete();
    return kTRUE;
  }
  Int_t njet=fJclust.GetNjets();
  hNjets->Fill((Float_t)njet);
  fJclust.ForceNJets(2);
  Float_t ymax=fJclust.GetYmax();
  hYmax->Fill(ymax);

  if( ymax < 0.1 ) {
    particles.Delete();
    return kTRUE;
  }
 
  //  TClonesArray *tracks=evt->GetLTKCLTracks();
  TObjArray *tracks=evt->GetLTKCLTracks();
  JSFQuickSimParam *fpar=sds->Param(); 

  Int_t ndfmin=(Int_t)fpar->GetCDCNSample();
  Float_t cutcosth;
  sscanf(gJSF->Env()->GetValue("JSFVTXAnalysis.COSTH","0.85"),"%g",&cutcosth);
  Int_t cutnvtx=gJSF->Env()->GetValue("JSFVTXAnalysis.NVTXMIN",3);

  // Does VTX and Jet matching
  /*
  fGenVTX.Clear();
  fGenVTX.MakeVertices(evt->GetGeneratorParticles());
  MatchJetWithVTX();
  */


  // Loops ofer LTKCL tracks and flags duplicated CDC tracks
  Int_t nt=evt->GetNLTKCLTracks();
  Int_t poison[500];
  for(Int_t i=0;i<nt;i++){ poison[i]=0; }
  for(Int_t i=0;i<nt;i++){
    JSFLTKCLTrack *t1=(JSFLTKCLTrack*)tracks->UncheckedAt(i);
    JSFCDCTrack *ct1=t1->GetCDC();
    if( ct1 == 0 ) continue;
    for(Int_t j=i+1;j<nt;j++){
      JSFLTKCLTrack *t2=(JSFLTKCLTrack*)tracks->UncheckedAt(j);
      JSFCDCTrack *ct2=t2->GetCDC();
      if( ct2 == 0 ) continue;
      if( TMath::Abs(ct1->GetPx()-ct2->GetPx()) < 0.0001 &&
	  TMath::Abs(ct1->GetPy()-ct2->GetPy()) < 0.0001 &&
	  TMath::Abs(ct1->GetPz()-ct2->GetPz()) < 0.0001 ){
	  poison[i]=-(j+1);
      }
    }
  }


  //  TClonesArray *cdc=(TClonesArray*)evt->GetCDCTracks();
  TObjArray &jets = fJclust.GetJets();
  TIter nextjet(&jets);
  ANLJet *jet;
  Float_t varnt[30];
  for(Int_t i=0;i<30;i++){ varnt[i]=0.0; }
  Int_t ij=-1;
  Int_t ievt=gJSF->GetEventNumber();

  JSFGeneratorVertex *vg=0;
  JSFGeneratorVertex *vg2=0;

  // Start loop over jets
  while ( (jet = (ANLJet *)nextjet()) ){

#ifdef debug
    printf(" ***** srtat vertexing \n");
#endif
    vg=gvtx.GetGeneratorVertex(*jet);
    vg2=gvtx.GetGeneratorVertex2(*jet);
    //#ifdef longdecay
    //    if( vg->GetDecayLength() < 1.0 ) continue; /* Select long decay length events */
    //   if( vg != 0 && vg->GetDecayLength() < 0.1 ) continue; /* Select long decay length events */
    //#endif
    //    if( vg->GetNcharged() < 1 ) continue;
    ANL3DVector bvtx;
    ANL3DVector gvpos;
    Float_t gdl2=0;
    Int_t ngs=0;
    Float_t gmsptm=0;
    Float_t gdl=0;
    Float_t gmsec=0;
    Float_t gptm2=0;
    if( vg != 0 ) {
      bvtx=ANL3DVector(vg->GetParent()->GetPx(), vg->GetParent()->GetPy(), 
		     vg->GetParent()->GetPz()); 
    //    if( TMath::Abs(bvtx.CosTheta()) > 0.8 ) continue;
      gvpos=TVector3(vg->GetVertexPosition());

      if( vg2 != 0 ) {
	TVector3 gvpos2=vg2->GetVertexPosition();
	gdl2=gvpos2.Mag();
      }

      TIter ngp(vg->GetTracks());
      JSFGeneratorParticle *gp=0;
      ANL4DVector gsum;
      while((gp=(JSFGeneratorParticle*)ngp())){
	if( TMath::Abs(gp->GetCharge()) > 0.5 && gp->GetNDaughter() == 0 ) {
	//	gp->ls();
	Double_t egp=TMath::Sqrt(gp->GetPx()*gp->GetPx()+
		 gp->GetPy()*gp->GetPy()+gp->GetPz()*gp->GetPz()
		 + 0.139*0.139);
	ANL4DVector g4v((Double_t)egp, (Double_t)gp->GetPx(), 
			(Double_t)gp->GetPy(), (Double_t)gp->GetPz());
	gsum=gsum+g4v;
	ngs++;
	}
      }
      ANL3DVector gsum3=gsum.Get3D();
      ANL3DVector gvposunit=gvpos.Unit();
      ANL3DVector gpt=gsum3.Cross(gvposunit);
      gmsptm=TMath::Sqrt(gpt.Mag2()+
		gsum.GetMass()*gsum.GetMass())+TMath::Abs(gpt.Mag());
      gmsec=sum.Mag();
      gdl=vg->GetDecayLength();
      gptm2=gpt.Mag();

      //      if( gdl < 0.2 ) continue; // Select long decay events only.
#ifdef debug
      if( TMath::Abs(gsum.GetMass()-0.5)<0.1 ) {
      printf(" jet momentum is %g %g %g costh=%g\n",jet->X(), jet->Y(), 
	     jet->Z(),bvtx.CosTheta());
      printf(" Decay length=%g ",vg->GetDecayLength());
      printf(" Vertex position is %g %g %g\n",gvpos.X(),gvpos.Y(),gvpos.Z());
      printf(" Charged generator particles :\n");
      ngp.Reset();
      while((gp=(JSFGeneratorParticle*)ngp())){
	if( TMath::Abs(gp->GetCharge()) > 0.5 ) {
	  printf(" %d ",gp->GetSerial());
	  printf(" id=%d",gp->GetID());
	  printf(" px,py,pz=%g %g %g costh=%g\n",
		 gp->GetPx(),gp->GetPy(),gp->GetPz(),gp->GetCosth());
	}
      }
      printf("\n");
      printf("Msec=%g ",gsum.GetMass());
      printf(" gpt=%g ",gpt.Mag());
      printf(" gmsptm=%g",gmsptm);
      
      printf("\n");
      }
#endif
    }


    ij++;
    hNpInJets->Fill(jet->GetNparticles());
    hJetCosTheta->Fill(jet->CosTheta());
    hJetEnergy->Fill(jet->E());
    varnt[0]=ievt;
    varnt[1]=ij;
    varnt[2]=jet->CosTheta();
    const TObjArray &jetp = jet->GetParticlesInJet();
    TIter nextp(&jetp);
    ANL4DVector *p4;
    TObjArray   jettracks;
    Int_t notracks=0;

 // Start jet loop

    while( (p4 = (ANL4DVector*)nextp()) ) {
      Int_t idx=particles.IndexOf(p4);
      if( poison[idx] < 0 ) { continue; }
      JSFLTKCLTrack *t=(JSFLTKCLTrack*)tracks->UncheckedAt(idx);
      JSFCDCTrack *ct=t->GetCDC(); 
      if( ct == 0 ) continue;

      Double_t ctcosth=ct->GetPz()/TMath::Sqrt(ct->GetPx()*ct->GetPx()+
		       ct->GetPy()*ct->GetPy()+ct->GetPz()*ct->GetPz());
#ifdef debug
      printf(" Nt=%d GenID=%d NDF=%d nvtx=%d px,py,pz=%g %g %g costh=%g",notracks, 
	     ct->GetGenID(),ct->GetNDF(),ct->GetNVTX(), 
	     ct->GetPx(), ct->GetPy(), ct->GetPz(), ctcosth);
#endif
      if( TMath::Abs(ctcosth) > cutcosth ) { continue; }
      //      if( ct->GetNDF() < ndfmin ) { printf("\n"); continue; }
      if( ct->GetNVTX() < cutnvtx  ) { continue; }
#ifdef debug      
      printf(" *** OK\n");
#endif
      jettracks.Add(ct);
      notracks++;
    }
#ifdef debug
    printf(" notracks =%d\n",notracks);
#endif
    varnt[3]=notracks;
    // Now calls Vertex finder for the tracks belong to this jet.


// ================== Topological vertex finding is called.
    JSFZVTOP3 *v=new JSFZVTOP3();
    v->MakeVertices(&jettracks);
// ================== Topological vertex finding is called.
    fZVTOP3.Add(v);

    Int_t nvtx=v->GetNVertices();
    hNVertices->Fill((Float_t)nvtx);
    Float_t msptm=v->GetMSPTM();
    varnt[4]=nvtx;
    if( nvtx > 1 ) {
      hMSPTM->Fill(msptm);
      hDecayLength->Fill(v->GetDecayLength());
      hVmass->Fill(v->GetSecondaryMass());
    }
    varnt[5]=msptm;
    varnt[6]=v->GetDecayLength();
    varnt[7]=v->GetSecondaryMass();
    varnt[8]=v->GetChisquare();
    varnt[9]=v->GetProbability();
    varnt[10]=v->GetNsec();
    varnt[11]=sum.E();
    varnt[12]=ymax;
    varnt[13]=v->GetMSPT();

    varnt[14]=gdl;
    varnt[16]=ngs;
    if( nvtx > 1 && vg != 0 ) {
      JSFZVTOP3Vertex *rvtx=(JSFZVTOP3Vertex*)v->UncheckedAt(1);
      //      TVector3 vdif=gvpos - rvtx->GetVertexPosition();
      ANL3DVector vdif=gvpos - (ANL3DVector)(rvtx->GetVertexPosition());
      varnt[15]=vdif.Mag();
    }

    varnt[17]=gmsec;
    varnt[18]=gmsptm;
    varnt[19]=gptm2;
    varnt[20]=v->GetPTM2();
    varnt[21]=v->GetProbability2();
    varnt[22]=gdl2;
    if( v->GetNVRT() >1 ) {
      JSFZVTOP3Vertex *v2=0;
      if( v->GetNVRT() == 2 ) { v2=(JSFZVTOP3Vertex*)v->At(1); }
      else { v2=(JSFZVTOP3Vertex*)v->At(2); }
      TVector3 vpos2=v2->GetVertexPosition();
      //      printf(" D vertex decay length is %g\n",vpos2.Mag());
      varnt[23]=vpos2.Mag();
    }
    varnt[24]=v->GetDecayLength0();
    varnt[25]=v->GetProbabilityOriginalVertex();
    varnt[26]=v->GetNsecOriginalVertex();
    varnt[27]=jet->E();
    varnt[28]=v->GetSecondaryMomentum();

#ifdef debug
      v->DebugPrint();
      gvtx.DebugPrint();

    if( nvtx < 2 ) {
      printf(" Jset Momentum is %g %g %g\n",bvtx.X(),bvtx.Y(),bvtx.Z());
    }
#endif

    ntuple->Fill((Float_t*)varnt);


  }  // End of loop on jets.



  particles.Delete();




  return kTRUE;

}

//_________________________________________________________
void JSFVTXAnalysis::FindV0()
{
  // Search K0 and Lambda vertices priar to B/D tagging
  // and remove those tracks from CDC tracks.

  JSFSIMDST *simdst=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
  JSFQuickSimParam *parm=(JSFQuickSimParam*)simdst->Param();
  Float_t bf=parm->GetBField();
  
  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();
  Int_t ncdc=sdb->GetNCDCTracks();
  fNCDC=ncdc;
  for(Int_t i=0;i<ncdc;i++){ fCDCFlag[i]=0; }

  if( ncdc < 2 ) return;
  TClonesArray *cdc=(TClonesArray*)sdb->GetCDCTracks();

  // Initialize vertexing class for two tracks vertexing.
  JSFVertexing vtx(2); 

  Int_t i,j;

  
  TClonesArray fitbuf("JSFHelicalTrack",10); 
  
  for(i=0;i<ncdc;i++){
    JSFCDCTrack *ct0=(JSFCDCTrack*)cdc->UncheckedAt(i);
    JSFHelicalTrack ht0=ct0->GetHelicalTrack();
    ht0.SetBfield(bf);
    vtx.SetTrack(i, ht0);
    for(j=i+1;j<ncdc;j++){
      JSFCDCTrack *ct1=(JSFCDCTrack*)cdc->UncheckedAt(j);
      JSFHelicalTrack ht1=ct1->GetHelicalTrack();
      ht1.SetBfield(bf);
      vtx.SetTrack(j,ht1);
      Double_t qual=vtx.FindV0();

      if( qual > 10.0 ) continue;

      //      hV0Qual->Fill(vtx.GetQuality());
      TVector3 v(0.0,0.0,0.0);
      vtx.GetVertex(v);

      // We put candidate helical track parameter into
      // buffer for Geometrical Constraint fit.
      new( fitbuf[0] ) JSFHelicalTrack(ht0);
      new( fitbuf[1] ) JSFHelicalTrack(ht1);

      JSFGeoCFit fit(2, &fitbuf, v);
      fit.Fit();
#if __ROOT_FULLVERSION__ < 30000
      JSFDMatrix ans(fit.GetParameter());
#else
      TMatrixD ans(fit.GetParameter());
#endif
      hConfl->Fill((Float_t)fit.GetConfidenceLevel());
      hNtry->Fill((Float_t)fit.GetNtry());

      TVector3 vfit((Float_t)fit.GetParameter(0), 
		    (Float_t)fit.GetParameter(1),(Float_t)fit.GetParameter(2));
      hV0decay->Fill(vfit.Mag());
      if( vfit.Mag() < 1.0 ) continue;
      if( fit.GetConfidenceLevel() < 0.02 ) continue;

      Double_t pt1=1.0/TMath::Abs(ans(4,0));
      Double_t phi01=ans(3,0);
      Double_t tnl1 =ans(5,0);
      Double_t psq1 = pt1*pt1*(1.0+tnl1*tnl1);
      Double_t e1   = TMath::Sqrt(psq1+0.13957*0.13957);
      ANL4DVector pi1(e1,
        -pt1*TMath::Sin(phi01), pt1*TMath::Cos(phi01),pt1*tnl1);
      Double_t pt2=1.0/TMath::Abs(ans(7,0));
      Double_t phi02=ans(6,0);
      Double_t tnl2 =ans(8,0);
      Double_t psq2 = pt2*pt2*(1.0+tnl2*tnl2);
      Double_t e2   = TMath::Sqrt(psq2+0.13957*0.13957);
      ANL4DVector pi2(e2,
        -pt2*TMath::Sin(phi02), pt2*TMath::Cos(phi02),pt2*tnl2);
      ANL4DVector pk0s=pi1+pi2;

      ANL4DVector prot1(TMath::Sqrt(psq1+0.938*0.938), 
	-pt1*TMath::Sin(phi01), pt1*TMath::Cos(phi01),pt1*tnl1);
      if( psq1 > psq2 ) {			
        prot1=ANL4DVector(TMath::Sqrt(psq2+0.938*0.938), 
          -pt2*TMath::Sin(phi02), pt2*TMath::Cos(phi02),pt2*tnl2);
	pi2 = pi1;
      }
      ANL4DVector plamd=pi2+prot1;
      Float_t k0mass=pk0s.GetMass();
      Float_t lambdamass=plamd.GetMass();
      hK0->Fill(k0mass);

      if( k0mass > 0.495 && k0mass < 0.505 ) { fCDCFlag[i]=1; fCDCFlag[j]=1; }
      else {      hLambda->Fill(lambdamass); }
      if( lambdamass > 1.05 && lambdamass < 1.15 ) { fCDCFlag[i]=2; fCDCFlag[j]=2; }


    }
  }

}
  
//_________________________________________________________
void JSFVTXAnalysis::MatchJetWithVTX()
{
  // Create a TObjArray of JSFGeneratedVertex coresponding to 
  // jets, using generator information.
  // This program is valid only for e+e- -> two jets events.

  
 
  JSFGeneratorVertex *vgen[10];

  Int_t ngen=0;
  TObjArrayIter next(&fGenVTX);
  JSFGeneratorVertex *v;

  while((v=(JSFGeneratorVertex*)next())){
    if( !v->IsNormalVertex() ) continue;
    JSFGeneratorVertex *pv=v->GetParentVertex();
    if( pv == 0 ) continue;

    printf(" parent ID=%d ",v->GetParentFlavourID());
    printf(" ID of parent is %d ",v->GetParent()->GetID());
    printf(" ngen=%d ..NGeneration is %d\n",ngen,v->GetNGeneration());

    if( v->GetNGeneration() != 1 ) continue;
    vgen[ngen++]=v;
  }
  printf(" Find %d primary vertices..\n",ngen);

  TVector3 vp1=vgen[0]->GetVertexPosition();
  TVector3 vp2=vgen[1]->GetVertexPosition();

  printf(" Vertex 1 is %g %g %g\n",vp1.X(),vp1.Y(),vp1.Z());
  printf(" Vertex 2 is %g %g %g\n",vp2.X(),vp2.Y(),vp2.Z());

}


//_________________________________________________________
Int_t JSFVTXAnalysis::JetAnalysis(TObjArray &particles, ANL4DVector &sum, 
		       ANLJadeEJetFinder *jclust)
{
  // Calculate event parameters such as Visible energy, etc.

  JSFSIMDST *sds=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
  JSFSIMDSTBuf *evt=(JSFSIMDSTBuf*)sds->EventBuf();

   // ***************************************
   // Make histograms of simulator information
   // ***************************************

   Int_t nt=evt->GetNLTKCLTracks();
   //   TClonesArray *tracks=evt->GetLTKCLTracks();
   TObjArray *tracks=evt->GetLTKCLTracks();

   //   Int_t ievt=gJSF->GetEventNumber();

   // Accumulate 4 momentum of the event

   Int_t nprt=0;  
   for(Int_t i=0;i<nt;i++){
     JSFLTKCLTrack *t=(JSFLTKCLTrack*)tracks->UncheckedAt(i);
     Double_t px=t->GetPx();      Double_t py=t->GetPy(); 
     Double_t pz=t->GetPz();      Double_t e=t->GetE(); 
     ANL4DVector *p=new ANL4DVector(e,px,py,pz);

     /*
     if( ievt> 145 ) {
       JSFCDCTrack *ct=t->GetCDC();
       printf(" i=%d ",i);
       printf(" 1stCDC=%d type=%d ",t->Get1stCDC(),t->GetType());
       printf(" ct=%d",(UInt_t)ct);
       if( ct != 0 ) {
	 printf(" px,py,pz=%g %g %g\n",ct->GetPx(),ct->GetPy(),ct->GetPz());
       }
       p->DebugPrint();
     }
     */

     particles.Add(p);
     nprt++;
     sum += *p ;  // Sum 4 momentum
   }

   // Does jet analysis when there are more than one tracks
   if( nprt > 1 )  {
     //     jclust->Initialize((const TObjArray&)particles);
     jclust->Initialize(particles);
     jclust->FindJets();
   }

  return nprt;
 
}


#undef debug
