//*LastUpdate :  jsf-1-12  2-September-1999  By A.Miyamoto
//*LastUpdate :  jsf-1-11  23-July-1999  By A.Miyamoto
//*LastUpdate :  jsf-1-4  15-Feburary-1999  By A.Miyamoto
//*LastUpdate :  0.02/01  10-September-1998  By A.Miyamoto
//*-- Author  : A.Miyamoto  10-September-1998

/*
2-September-1999 A.Miyamoto  Memory leak by fPartons are fixed.
                             Add function, TBGETsSpringPartonList()
*/

////////////////////////////////////////////////////////////////////////
//
// JSFSpring
//
//   Create parto lists
//
//  For example of how to use JSFSpring and JSFBases, please see
//  begin_html <a href="http://www-jlc.kek.jp/subg/offl/jsf/docs/bsguide/index.html">html </a> end_html or begin_html <a href="http://www-jlc.kek.jp/subg/offl/jsf/docs/bsguide.ps.gz">postscript</a> end_html documents. 
// 
//$Id$
//
////////////////////////////////////////////////////////////////////////

#include <TObjectTable.h>
#include <TKey.h>
#include "TRandom.h"
#include "JSFSteer.h"
#include "JSFLCFULL.h"
#include "JSFSpring.h"
#include "JSFSpringParton.h"

extern "C" {
extern void spring_(void *adr, int *mxtry);
};

ClassImp(JSFSpringBuf)
ClassImp(JSFSpring)


JSFSpring     *lSpring;

// JSFSpringParam gJSFSpringParam;

Double_t Springfunc(double x[]);
Double_t Springfunc(double x[])
{
  double val=lSpring->Bases()->Func(x);
  return val; 
}


//_____________________________________________________________________________
JSFSpring::JSFSpring(const char *name, const char *title, JSFBases *bases)
       : JSFModule(name,title)
{
// Create one JSFSpring object
// User class should set addresses to the event buf here, as follows.
//   fEventBuf = new JSFSpringBuf("JSFSpringBuf", "Spring Event buffer", this);

   lSpring  = this;
   fMXTRY   = 500;
   fDoBases = kFALSE;
   fSetSeed = kFALSE;
   fBases   = NULL;
   if( bases ) {
     SetBases( bases );
   }
}

//_____________________________________________________________________________
JSFSpring::~JSFSpring()
{
  if( fBases ) { 
    delete fBases; 
    fBases=NULL;
  }
}


//_____________________________________________________________________________
void JSFSpring::ReadBases(const char *name)
{
  TDirectory *last=gDirectory;
  TFile *fBasesFile=new TFile(name);
  fBasesFile->cd("/");
  fDoBases = kFALSE;
  fBases->Read(fBases->GetName());
  
  fBases->Initialize();

  if( fSetSeed ) {
    Int_t i;
    for(i=0;i<33;i++){ randm_.rdm[i]=fSeedRdm[i]; }
    for(i=0;i<12;i++){ randm_.ia1[i]=fSeedIa1[i]; }
    printf("JSFSpring::ReadBases() .. random seed is overridden by values");
    printf(" taken from a file.\n");
  }

  fBases->Userin();

  last->cd();
}

//_____________________________________________________________________________
Bool_t JSFSpring::BeginRun(Int_t nrun)
{
   
  if( fFile->IsWritable() ) {
    if( !JSFModule::BeginRun(nrun) ) return kFALSE;
    Write();
  }
  return kTRUE;
}


//_____________________________________________________________________________
Bool_t JSFSpring::EndRun()
{
  Int_t i;
  for(i=0;i<33;i++){ fSeedRdm[i]=randm_.rdm[i]; }
  for(i=0;i<12;i++){ fSeedIa1[i]=randm_.ia1[i]; }

  if( fFile->IsWritable() ) {
    if( !JSFModule::EndRun() ) return kFALSE;
    Write();
  }
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFSpring::Process(Int_t ev)
{
  Int_t i;
  for(i=0;i<33;i++){ fSeedRdm[i]=randm_.rdm[i]; }
  for(i=0;i<12;i++){ fSeedIa1[i]=randm_.ia1[i]; }

  fBases->fInBases=kFALSE;
  Int_t mxtry=fMXTRY;
  spring_(Springfunc, &mxtry);

  if( fEventBuf ) {
    JSFSpringBuf *buf=(JSFSpringBuf*)fEventBuf;
    if ( !buf->SetPartons() ) return kFALSE; 
  }

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFSpring::GetLastRunInfo()
{
  // Read seed of previous run 

  Read(GetName());
  fSetSeed=kTRUE;
  //  printf("Random seeds for JSFSpring will be reset by ");
  //  printf("values from a file.\n");

  return kTRUE;
}


//_____________________________________________________________________________
JSFSpringBuf::JSFSpringBuf(const char *name, const char *title, 
			   JSFSpring *spring)
  : JSFEventBuf(name, title, (JSFModule*)spring)
{
   fNparton = 0 ;
   if( !fPartons ) fPartons = new TClonesArray("JSFSpringParton",100);
}



//_____________________________________________________________________________
JSFSpringBuf::~JSFSpringBuf()
{
   Clear();
   if( fPartons ) delete fPartons;
}


//_____________________________________________________________________________
void JSFSpringBuf::Clear(Option_t *option)
{
  if( fPartons ) fPartons->Clear(option);
}

//_____________________________________________________________________________
Bool_t JSFSpringBuf::SetPartons()
{
  //  This function is provided as an interface to old TBS routines.
  //  Use of TBS routine is strongly discouraged as they will be removed
  //  when LCLIB is converted to C++
  Int_t iret;
  gJSFLCFULL->TBCRTE(1,"Spring:Parton_List",0,0, iret);
  gJSFLCFULL->TBCRTE(1,"Spring:Header",0,0, iret);
  gJSFLCFULL->TBCRTE(1,"Generator:Particle_List",0,0, iret);
  gJSFLCFULL->TBCRTE(1,"Generator:Header",0,0, iret);

  Spevnt(iret);

  Int_t nelm, neary[1000];

  // Save Particle information
  gJSFLCFULL->TBNOEL(1,"Spring:Parton_List",nelm, neary);
  if( nelm <= 0 ) {
    fNparton=0;
    return kTRUE;
  }

  Int_t nw;
  Float_t data[20];
  TClonesArray &particles = *fPartons;
  Int_t npart=0;
  for(Int_t j=0;j<nelm;j++){
     gJSFLCFULL->TBGET(1,"Spring:Parton_List",neary[j],nw,(Int_t*)data,iret);
     if( iret < 0 ) return kFALSE;
     new( particles[npart++] ) JSFSpringParton(data);
  }
  fNparton=npart;
  return kTRUE;
}

//_____________________________________________________________________________
void JSFSpring::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFSpring.

   Int_t lc;
   if (R__b.IsReading()) {

      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      JSFModule::Streamer(R__b);
      //      R__b >> fDoBases;

      Char_t cname[100], name[100], title[100],cmdstr[300];
      lc=R__b.ReadStaticArray(cname);
      lc=R__b.ReadStaticArray(name);
      lc=R__b.ReadStaticArray(title);
      if( !fBases ) {
	sprintf(cmdstr,"bases=new %s(\"%s\",\"%s\") ; \n\
        bases->SetSpring((Int_t)%d);", cname, name, title,(Int_t)this);
	gROOT->ProcessLine(cmdstr);
      }

      lc=R__b.ReadStaticArray(fSeedRdm);
      lc=R__b.ReadStaticArray(fSeedIa1);

   } else {
      R__b.WriteVersion(JSFSpring::IsA());
      JSFModule::Streamer(R__b);

      lc=strlen(fBases->ClassName())+1;
      R__b.WriteArray(fBases->ClassName(),lc);
      lc=strlen(fBases->GetName())+1;
      R__b.WriteArray(fBases->GetName(),lc);
      lc=strlen(fBases->GetTitle())+1;
      R__b.WriteArray(fBases->GetTitle(),lc);

      R__b.WriteArray(fSeedRdm,33);
      R__b.WriteArray(fSeedIa1,12);

   }
}

