//*LastUpdate :  jsf-1-14  11-Feburary-2000  By A.Miyamoto
//*LastUpdate :  jsf-1-12  2-September-1999  By A.Miyamoto
//*LastUpdate :  jsf-1-11  23-July-1999  By A.Miyamoto
//*LastUpdate :  jsf-1-4  15-Feburary-1999  By A.Miyamoto
//*LastUpdate :  0.02/01  10-September-1998  By A.Miyamoto
//*-- Author  : A.Miyamoto  10-September-1998


////////////////////////////////////////////////////////////////////////
//
// JSFSpring
//
//   Create parton lists
//
//  For example of how to use JSFSpring and JSFBases, please see
//  begin_html <a href="http://www-jlc.kek.jp/subg/offl/jsf/docs/bsguide/index.html">html </a> end_html or begin_html <a href="http://www-jlc.kek.jp/subg/offl/jsf/docs/bsguide.ps.gz">postscript</a> end_html documents. 
// 
// (Update)
// 2-September-1999 A.Miyamoto  Memory leak by fPartons are fixed.
//                              Add function, TBGETsSpringPartonList()
// 11-Feb-2000 A.Miyamoto Use new C++ Bases/Spring Library
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

ClassImp(JSFSpringBuf)
ClassImp(JSFSpring)

//_____________________________________________________________________________
JSFSpring::JSFSpring(const char *name, const char *title, JSFBases *bases)
       : JSFModule(name,title)
{
// Create one JSFSpring object
// User class should set addresses to the event buf here, as follows.
//   fEventBuf = new JSFSpringBuf("JSFSpringBuf", "Spring Event buffer", this);

   fMXTRY   = 50;
   fDoBases = kFALSE;
   fSetSeed = kFALSE;
   fBases   = NULL;

   fPrintInfo = gJSF->Env()->GetValue("JSFSpring.PrintInfo",kFALSE);
   fPrintHist = gJSF->Env()->GetValue("JSFSpring.PrintHist",kFALSE);

   if( bases ) {
     SetBases( bases );
   }

  // Allocate IO variables
  fioNTAB=NTAB;
  fSeedIV=new Long_t [NTAB];

}

//_____________________________________________________________________________
JSFSpring::~JSFSpring()
{
  if( fBases ) { 
    delete fBases; 
    fBases=NULL;
  }
  delete fSeedIV;

}

//_____________________________________________________________________________
void JSFSpring::ReadBases(const char *name)
{
  // Read bases data and bases histogram from a file, name.

  TDirectory *now=gDirectory;
  TFile *f=new TFile(name);
  fDoBases = kFALSE;
  fBases->Read(fBases->GetName());

  TDirectory *cur=gDirectory;
  now->cd();
  fBases->Userin();
  cur->cd();

  fBases->CopyHists(f,now); // Read Hists data into Bases area
  f->Close();
  now->cd();

  if( fSetSeed ) {
    bases_ran1 *rn=fBases->GetRan1();
    rn->SetSeed(fSeed);
    rn->SetSeed2(fSeedIY, fSeedIV);
    printf("JSFSpring::ReadBases() .. random seed is overridden by values");
    printf(" taken from a file.\n");
  } 
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
Bool_t JSFSpring::Terminate()
{
  if( fPrintInfo ) GetBases()->Sp_info();
  if( fPrintHist ) GetBases()->Sh_plot();
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFSpring::EndRun()
{

  bases_ran1 *rn=fBases->GetRan1();
  fSeed=rn->GetSeed();
  rn->GetSeed2(fSeedIY, fSeedIV);

  if( fFile->IsWritable() ) {
    if( !JSFModule::EndRun() ) return kFALSE;
    Write();
  }
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFSpring::Process(Int_t ev)
{

  fBases->Spring(fMXTRY);

  if( fEventBuf ) {
    JSFSpringBuf *buf=(JSFSpringBuf*)fEventBuf;
    if ( !buf->SetPartons() ) return kFALSE; 
  }

  return kTRUE;
}



//_____________________________________________________________________________
void JSFSpring::Spring(Int_t maxtry)
{
  if( fBases ) {
    fBases->Spring(maxtry);
  }
  else {  
    printf("Error in JSFSpring::Spring.. fBases is not set.\n");
  }
}

//_____________________________________________________________________________
void JSFSpring::Spring()
{
  Spring(fMXTRY);
}


//_____________________________________________________________________________
Bool_t JSFSpring::GetLastRunInfo()
{
  // Read seed of previous run 

  Read(GetName());
  fSetSeed=kTRUE;

  return kTRUE;
}


//_____________________________________________________________________________
JSFSpringBuf::JSFSpringBuf(const char *name, const char *title, 
			   JSFSpring *spring)
  : JSFEventBuf(name, title, (JSFModule*)spring)
{
   fNparton = 0 ;
#ifndef __DARWIN__ 
   if( !fPartons ) fPartons = new TClonesArray("JSFSpringParton",100);
#else
   fPartons = new TClonesArray("JSFSpringParton",100);
#endif
}



//_____________________________________________________________________________
JSFSpringBuf::~JSFSpringBuf()
{
   Clear();
   if( fPartons ) delete fPartons;
   fPartons = 0;
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

#if __ROOT_FULLVERSION__ >= 30000
//_____________________________________________________________________________
void JSFSpring::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFSpring.

   Char_t cmdstr[300];
   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v=R__b.ReadVersion(&R__s, &R__c);
     if( R__v < 3 ) { 
	printf("JSFSpring::Streamer .. Old version data.  Can not read.\n");
        return;
     }
     else if(  R__v > 3 ) {
       JSFSpring::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
       if( !fBases ) {
	 sprintf(cmdstr,"bases=new %s(\"%s\",\"%s\") ; \n\
         bases->SetSpring((Int_t)%d);", fBasesClassName.Data(), 
		fBasesObjectName.Data(), fBasesObjectTitle.Data(),(Int_t)this);
	 gROOT->ProcessLine(cmdstr);
       }
       return;
     }
     // process old version data.
     JSFModule::Streamer(R__b);

     Int_t lc;
     Char_t cname[100], name[100], title[100];
     lc=R__b.ReadStaticArray(cname);
     lc=R__b.ReadStaticArray(name);
     lc=R__b.ReadStaticArray(title);
     if( !fBases ) {
       sprintf(cmdstr,"bases=new %s(\"%s\",\"%s\") ; \n\
        bases->SetSpring((Int_t)%d);", cname, name, title,(Int_t)this);
       gROOT->ProcessLine(cmdstr);
     }

     R__b >> fSeed;
     R__b >> fSeedIY;
     R__b.ReadStaticArray(fSeedIV);
     R__b.CheckByteCount(R__s, R__c, JSFSpring::IsA());

     
   } 
   // Write Streamer
   else {
     fBasesClassName=fBases->ClassName();
     fBasesObjectName=fBases->GetName();
     fBasesObjectTitle=fBases->GetTitle();

     JSFSpring::Class()->WriteBuffer(R__b, this);
   }
}

#else
//_____________________________________________________________________________
void JSFSpring::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFSpring.

   Int_t lc;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      if( R__v < 3 ) {
	printf("JSFSpring::Streamer data's version number is not");
	printf(" compatible with streamer.\n");
      }
      JSFModule::Streamer(R__b);

      Char_t cname[100], name[100], title[100],cmdstr[300];
      lc=R__b.ReadStaticArray(cname);
      lc=R__b.ReadStaticArray(name);
      lc=R__b.ReadStaticArray(title);
      if( !fBases ) {
	sprintf(cmdstr,"bases=new %s(\"%s\",\"%s\") ; \n\
        bases->SetSpring((Int_t)%d);", cname, name, title,(Int_t)this);
	gROOT->ProcessLine(cmdstr);
      }

      R__b >> fSeed;
      R__b >> fSeedIY;
      R__b.ReadStaticArray(fSeedIV);

   } else {
      R__b.WriteVersion(JSFSpring::IsA());
      JSFModule::Streamer(R__b);

      lc=strlen(fBases->ClassName())+1;
      R__b.WriteArray(fBases->ClassName(),lc);
      lc=strlen(fBases->GetName())+1;
      R__b.WriteArray(fBases->GetName(),lc);
      lc=strlen(fBases->GetTitle())+1;
      R__b.WriteArray(fBases->GetTitle(),lc);

      R__b << fSeed; 
      R__b << fSeedIY;
      R__b.WriteArray(fSeedIV,NTAB);

   }
}

#endif
