//*LastUpdate : v.01.01 undefined by undefined
//*-- Author  : undefined undefined

///////////////////////////////////////////////////////////////////
//
//  MakeJIMInput
//
//  Make a JIM input data with ZBS format
//
//$Id: 
//  
//////////////////////////////////////////////////////////////////


#include "JSFSteer.h"
#include "JSFGenerator.h"
#include "MakeJIMInput.h"

extern "C" {
  extern void kzinit_();
  extern void kzopen_(int *lu, const char *fn, int *ierr, int lfn);
  extern void kzeclr_();
  extern void kzbcr1_(char *bn, int *nwmast, char *cformm,
		     int *nsegm, int *lsegm, char *cforms, int *ierr,
		     int lbn, int lcformm, int lcforms);
  extern void kzwrit_(int *lu);
  extern void kzend_(int *lu);
  extern void kzrep1_(char *bn, int *iseg, int *ndat, int idata[], int lbn);
  extern Float_t ulctau_(int *kf);
  extern Int_t luchge_(int *kf);
  extern void msgfin_();
  extern void luhepc_(int *mode);

};  

#define NMXHEP 2000
typedef struct {
  Int_t nevhep, nhep;
  Int_t isthep[NMXHEP], idhep[NMXHEP];
  Int_t jmohep[NMXHEP][2], jdahep[NMXHEP][2];
  Double_t phep[NMXHEP][5], vhep[NMXHEP][4];
} COMMON_HEPEVT ;

extern COMMON_HEPEVT hepevt_;

ClassImp(MakeJIMInput)
ClassImp(MakeJIMInputForHadronizer)

//_____________________________________________________________________________
MakeJIMInput::MakeJIMInput(const char *name, const char *title)
       : JSFModule(name,title)
{

  fOutputFile=gJSF->Env()->GetValue("MakeJIMInput.OutputFileName","jiminput.bnk");
  fLunit=10;
  fFirstEvent=gJSF->Env()->GetValue("JSFGUI.FirstEvent",1);

}

//_____________________________________________________________________________
MakeJIMInput::~MakeJIMInput()
{
}

//_____________________________________________________________________________
Bool_t MakeJIMInput::Initialize()
{
  // Initialize

  kzinit_();
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t MakeJIMInput::Process(Int_t nev)
{
//  Output pythia 4 vectors into a file of zbs format

  kzeclr_();

  Int_t nhep=GetNtracks();

  Char_t *bn="MCEVENT";
  Int_t  lbn=strlen(bn);
  Int_t  nwmmce=2;
  Int_t  lsgmce=19;
  Char_t *fmsmce="2i";
  Int_t   lfmsmce=strlen(fmsmce);
  Char_t *fsgmce="7i 11f 1i";
  Int_t   lfsgmce=strlen(fsgmce);

  // -- create zbs bank mcevent to store primary state information of event.
  Int_t ier=0;
  kzbcr1_(bn, &nwmmce, fmsmce, &nhep, &lsgmce, fsgmce, &ier, lbn, lfmsmce, lfsgmce);

  // -- store data to master segment of bank mcevent 
  Int_t idata[30];
  //  idata[0]=hepevt_.nevhep;
  idata[0]=gJSF->GetEventNumber();
  idata[1]=nhep;
  Int_t zero=0;
  kzrep1_(bn,&zero, &nwmmce, idata, lbn);

  // *-- store data to given segment of bank mcevent 
  for(Int_t iseg=1;iseg<=nhep;iseg++){

    MakeTrack(iseg-1, idata);

    kzrep1_(bn, &iseg, &lsgmce, idata, lbn);
  }

  if( gJSF->GetEventNumber() >= fFirstEvent ) {
    kzwrit_(&fLunit);
  }  
  return kTRUE;
}

//___________________________________________________________________
Int_t MakeJIMInput::GetNtracks()
{
  // Returns number of track of events.

  Int_t mode=1;
  luhepc_(&mode);

  return hepevt_.nhep;

}

//___________________________________________________________________
void MakeJIMInput::MakeTrack(Int_t jseg, Int_t idata[])
{
  // Save generator track information of itrk-th track.
  // itrk is a value from 0 to n-1

    Float_t *rdata=(Float_t*)&idata[0];
    Float_t ctau;
    //    pyctau_(&hepevt_.idhep[jseg], &ctau);
    ctau=ulctau_(&hepevt_.idhep[jseg]);
    //    Float_t pycharge=pychge_(&hepevt_.idhep[jseg]);
    Float_t pycharge=(Float_t)luchge_(&hepevt_.idhep[jseg]);
    Float_t charge=pycharge/3.0;

    idata[0]=hepevt_.isthep[jseg];
    idata[1]=hepevt_.idhep[jseg];
    idata[2]=hepevt_.jmohep[jseg][0];
    idata[3]=hepevt_.jmohep[jseg][1];
    idata[4]=hepevt_.jdahep[jseg][0];
    idata[5]=hepevt_.jdahep[jseg][1];
    idata[6]=0;
    rdata[7]=hepevt_.phep[jseg][0];
    rdata[8]=hepevt_.phep[jseg][1];
    rdata[9]=hepevt_.phep[jseg][2];
    rdata[10]=hepevt_.phep[jseg][3];
    rdata[11]=hepevt_.phep[jseg][4];
    rdata[12]=hepevt_.vhep[jseg][0]/10.0;
    rdata[13]=hepevt_.vhep[jseg][1]/10.0;
    rdata[14]=hepevt_.vhep[jseg][2]/10.0;
    rdata[15]=hepevt_.vhep[jseg][3]/10.0;
    rdata[16]=charge;
    rdata[17]=ctau;
    rdata[18]=0;

    return;
}

//_____________________________________________________________________________
Bool_t MakeJIMInput::BeginRun(Int_t nrun)
{

  Int_t lastrun=gJSF->Env()->GetValue("JSFGUI.LastRun",-1);

  TString fout=fOutputFile;

  if( lastrun > 0 ) {
    Char_t post[64];
    sprintf(post,".r%5.5d.bnk",nrun);
    Int_t lstr=fout.Length();
    if( fout.EndsWith(".bnk") ) {
      fout.Replace(lstr-4,4,post);
    }
    else {
      fout.Append(post);
    }
  }

  printf(" Begin Run Run# %d File name %s\n",gJSF->GetRunNumber(),fout.Data());

  Int_t ier=0;
  kzopen_(&fLunit, fout.Data(), &ier, fout.Length());


  return kTRUE;
}

//_____________________________________________________________________________
Bool_t MakeJIMInput::EndRun()
{
  kzend_(&fLunit);
  printf(" End of Run %d",gJSF->GetRunNumber());
  
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t MakeJIMInput::Terminate()
{

  msgfin_();
  return kTRUE;
}

//_____________________________________________________________________________
MakeJIMInputForHadronizer::MakeJIMInputForHadronizer(const char *name, const char *title)
       : MakeJIMInput(name,title)
{
}

//_____________________________________________________________________________
Int_t MakeJIMInputForHadronizer::GetNtracks()
{
  JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *genb=(JSFGeneratorBuf*)gen->EventBuf();

  fNparticles = genb->GetNparticles();
  fParticles  = genb->GetParticles();

  return fNparticles;

}

//___________________________________________________________________
void MakeJIMInputForHadronizer::MakeTrack(Int_t itrk, Int_t idata[])
{
  // Save generator track information of itrk-th track.
  // itrk is a value from 0 to n-1

  Float_t *rdata=(Float_t*)&idata[0];
  
  JSFGeneratorParticle *gp=(JSFGeneratorParticle*)fParticles->UncheckedAt(itrk);
  

  if( gp->GetMother() < 0 ) {
    idata[0]=3;
  }
  else if( gp->GetNDaughter() == 0 ) { 
    idata[0]=1;
  }
  else {
    idata[0]=2;
  }
  idata[1]=gp->GetID();
  idata[2]=gp->GetMother();
  idata[3]=0;
  idata[4]=gp->GetNDaughter();
  idata[5]=gp->GetMother()+gp->GetNDaughter()-1;
  idata[6]=0;
  rdata[7]=gp->GetPx();
  rdata[8]=gp->GetPy();
  rdata[9]=gp->GetPz();
  rdata[10]=gp->GetE();
  rdata[11]=gp->GetMass();
  rdata[12]=gp->GetX();
  rdata[13]=gp->GetY();
  rdata[14]=gp->GetZ();
  rdata[15]=gp->GetT();
  rdata[16]=gp->GetCharge();
  rdata[17]=gp->GetLifeTime();
  rdata[18]=0;

}


