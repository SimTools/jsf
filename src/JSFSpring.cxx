//*LastUpdate :  jsf-1-4  15-Feburary-1999  By A.Miyamoto
//*LastUpdate :  0.02/01  10-September-1998  By A.Miyamoto
//*-- Author  : A.Miyamoto  10-September-1998

////////////////////////////////////////////////////////////////////////
//
// JSFSpring
//
//   Create parto lists
//
//  For example of how to use JSFSpring and JSFBases, please see
//  begin_html <a href="http://www-jlc.kek.jp/subg/offl/jsf/docs/bsgiude/index.html">a html </a> end_html or 
//  begin_html <a href="http://www-jlc.kek.jp/subg/offl/jsf/docs/bsgiude.ps">a postscript</a> end_html documents. 
// 
//$Id$
//
////////////////////////////////////////////////////////////////////////

#include <TObjectTable.h>
#include "TRandom.h"
#include "JSFSteer.h"
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
   if( bases ) SetBases( bases );
   }

//_____________________________________________________________________________
JSFSpring::~JSFSpring()
{
}


//_____________________________________________________________________________
void JSFSpring::ReadBases(const char *name)
{
  TFile *last=gFile;
  TFile *fBasesFile=new TFile(name);
  fBasesFile->cd("/");
  fDoBases = kFALSE;
  fBases->Read(fBases->GetName());
  fBases->Initialize();
  fBases->Userin();

  last->cd("/");
}

//_____________________________________________________________________________
Bool_t JSFSpring::BeginRun(Int_t nrun)
{
  if( fFile->IsWritable() ) Write();
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFSpring::Process(Int_t ev)
{
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
}


//_____________________________________________________________________________
void JSFSpringBuf::Clear(Option_t *option)
{
  if( fPartons ) fPartons->Clear(option);
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
        sprintf(cmdstr,"bases=new %s(\"%s\",\"%s\") ; bases->SetSpring((Int_t)%d);", cname, name, title,(Int_t)this);
	gROOT->ProcessLine(cmdstr);
      }
   } else {
      R__b.WriteVersion(JSFSpring::IsA());
      JSFModule::Streamer(R__b);
      //      R__b << fDoBases;
      lc=strlen(fBases->ClassName())+1;
      R__b.WriteArray(fBases->ClassName(),lc);
      lc=strlen(fBases->GetName())+1;
      R__b.WriteArray(fBases->GetName(),lc);
      lc=strlen(fBases->GetTitle())+1;
      R__b.WriteArray(fBases->GetTitle(),lc);
      // printf(" Bases file name is %s\n",fBasesFile->GetName());
      //      lc=strlen(fBasesFile->GetName())+1;
      // R__b.WriteArray(fBasesFile->GetName(),lc);
   }
}

