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
  extern void kzopen_(int *lu, char *fn, int *ierr, int lfn);
  extern void kzeclr_();
  extern void kzbcr1_(char *bn, int *nwmast, char *cformm,
		     int *nsegm, int *lsegm, char *cforms, int *ierr,
		     int lbn, int lcformm, int lcforms);
  extern void kzwrit_(int *lu);
  extern void kzend_(int *lu);
  extern void kzrep1_(char *bn, int *iseg, int *ndat, int idata[], int lbn);
  extern Float_t ulctau_(int *kf);
  extern Int_t luchge_(int *kf);

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

//_____________________________________________________________________________
  MakeJIMInput::MakeJIMInput(const char *name, const char *title)
       : JSFModule(name,title)
{
//  <<+EMAnal>>
  //  Example to get parameter  

  sscanf(gJSF->Env()->GetValue("MakeJIMInput.OutputFileName","jiminput.bnk"),
	 "%s",fOutputFile);

  fLunit=10;
  fFirstEvent=gJSF->Env()->GetValue("MakeJIMInput.FirstEvent",1);


//  <<-EMAnal>>

}

//_____________________________________________________________________________
MakeJIMInput::~MakeJIMInput()
{
}

//_____________________________________________________________________________
Bool_t MakeJIMInput::Initialize()
{
  // Initialize

  Int_t lu=fLunit;

  kzinit_();

  Int_t lfn=strlen(fOutputFile);
  Int_t ier=0;
  kzopen_(&lu, fOutputFile, &ier, lfn);


  return kTRUE;
}

//_____________________________________________________________________________
Bool_t MakeJIMInput::Process(Int_t nev)
{
//  Output pythia 4 vectors into a file of zbs format

  //  JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  //  JSFGeneratorBuf *gbuf=(JSFGeneratorBuf*)gen->EventBuf();
  //  Int_t ngen=gbuf->GetNparticles();

  kzeclr_();

  Int_t mode=1;
  luhepc_(&mode);

  Char_t *bn="MCEVENT";
  Int_t  lbn=strlen(bn);
  Int_t  nwmmce=2;
  Int_t  lsgmce=19;
  Char_t *fmsmce="2i";
  Int_t   lfmsmce=strlen(fmsmce);
  Char_t *fsgmce="7i 11f 1i";
  Int_t   lfsgmce=strlen(fsgmce);

  // -- create zbs bank mcevent to store primary state information of event.
  Int_t nhep=hepevt_.nhep;
  Int_t ier=0;
  kzbcr1_(bn, &nwmmce, fmsmce, &nhep, &lsgmce, fsgmce, &ier, lbn, lfmsmce, lfsgmce);

  // -- store data to master segment of bank mcevent 
  Int_t idata[30];
  //  idata[0]=hepevt_.nevhep;
  idata[0]=gJSF->GetEventNumber();
  idata[1]=hepevt_.nhep;
  Int_t zero=0;
  kzrep1_(bn,&zero, &nwmmce, idata, lbn);

  // *-- store data to given segment of bank mcevent 
  Float_t *rdata=(Float_t*)&idata[0];
  for(Int_t iseg=1;iseg<=nhep;iseg++){
    Int_t   jseg=iseg-1;
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

    /*
    printf(" jseg=%d",jseg);
    printf(" phep=%g %g %g %g %g\n",hepevt_.phep[jseg][0],
	   hepevt_.phep[jseg][1],hepevt_.phep[jseg][2],
	   hepevt_.phep[jseg][3],hepevt_.phep[jseg][4]);
    */

    rdata[16]=charge;
    rdata[17]=ctau;
    rdata[18]=0;

    kzrep1_(bn, &iseg, &lsgmce, idata, lbn);
  }

  if( gJSF->GetEventNumber() >= fFirstEvent ) {
    kzwrit_(&fLunit);
  }  
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t MakeJIMInput::BeginRun(Int_t nrun)
{
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t MakeJIMInput::EndRun()
{
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t MakeJIMInput::Terminate()
{

  kzend_(&fLunit);
  return kTRUE;
}



