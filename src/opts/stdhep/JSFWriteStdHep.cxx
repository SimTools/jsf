///////////////////////////////////////////////////////////////////
//
//  JSFWriteStdHep
//
//  Interface to StdHep data.  
//  
//  (29-Nov-2007: Interface using CLHEP library is removed.)
//
//$Id$ 
//  
//////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

#include "JSFSteer.h"
#include "JSFWriteStdHep.h"
#include "JSFReadStdHep.h"
#include "JSFParticleData.h"
#include "stdhep.h"
#include "TPythia6.h"
#include "hepev4.h"

ClassImp(JSFWriteStdHep)
THEPEV4 *JSFWriteStdHep::fHEPEV4=0;

extern "C" {
  extern void stdxwinit_(const char *filename, const char *title, 
			 int *ntries, int *istream, int *iok,
			 int lfilename, int ltitle);
  extern void stdxwopen_(const char *filename, const char *title, 
			 int *ntries, int *istream, int *iok,
			 int lfilename, int ltitle);
  extern void stdxwrt_(int *ilbl, int *istream, int *lok);
  extern void jsf_mcfio_init_();
  extern void stdflpyxsec_(int *ntries);
  extern void stdxend_(int *istream);
};

//_____________________________________________________________________________
JSFWriteStdHep::JSFWriteStdHep(const char *name, const char *title )
       : JSFModule(name,title)
{
  fEventBuf=0;
  
  fOutFileName=std::string(gJSF->Env()->GetValue("JSFWriteStdHep.OutputFileName","file.stdhep"));
  fOutTitle=std::string(gJSF->Env()->GetValue("JSFWriteStdHep.OutputTitle","sample"));

  fNTries=gJSF->Env()->GetValue("JSFWriteStdHep.NTries",10);
  fOutStream=0;
  fDebugLevel = gJSF->Env()->GetValue("JSFWriteStdHep.DebugLevel",0);
  fEventSource=gJSF->Env()->GetValue("JSFWriteStdHep.EventSource",1);
  fProcessID=gJSF->Env()->GetValue("JSFWriteStdHep.ProcessID",0);
  fWriteBeginRun=kTRUE;
  fWriteEndRun=kTRUE;
}

//_____________________________________________________________________________
JSFWriteStdHep::~JSFWriteStdHep()
{

}

//_____________________________________________________________________________
Bool_t JSFWriteStdHep::Initialize()
{


  // Initialize
  //  if ( IsWritable() ) {
  //    jsf_mcfio_init_(); 
  //  }

  std::cout << "JSFWriteStdHep::Initialize === runParameter " << std::endl;
  std::cout << "  Output File Name : " << fOutFileName << std::endl;
  std::cout << "  Output File Title: " << fOutTitle << std::endl;
  std::cout << "  NTries           : " << fNTries << std::endl;
  std::cout << "  Debug Level      : " << fDebugLevel << std::endl;
  std::cout << "  Event Source     : " << fEventSource << std::endl;
  std::cout << "    =0 /HEPEVT/, =1 /JETSET/, =2 JSFGenerator " << std::endl;
  std::cout << "  ProcessID        : " << fProcessID << std::endl;
  std::cout << "==============================================" << std::endl;

  return kTRUE;
}
//_____________________________________________________________________________
Bool_t JSFWriteStdHep::BeginRun(Int_t nrun)
{

  const Char_t *filename=fOutFileName.data();
  Int_t lfilename=fOutFileName.length();
  const Char_t *title=fOutTitle.data();
  Int_t ltitle=fOutTitle.length();
  
  Int_t iok=0;

  if ( JSFReadStdHep::IsInitialized() ) {
    stdxwopen_(filename, title, &fNTries, &fOutStream, &iok, lfilename, ltitle);
  }
  else {
    stdxwinit_(filename, title, &fNTries, &fOutStream, &iok, lfilename, ltitle);
  }
  if( iok != 0 ) {
    std::cerr << "Fatal error to open StdHep output file. LOK=" << iok << std::endl;
    std::cerr << "  Output file name : " << fOutFileName << std::endl;
    std::cerr << "  Output title     : " << fOutTitle << std::endl;
    std::cerr << "  NTries           : " << fNTries << std::endl;
    exit(0);
  }
  std::cerr << "StdHep Output file was opened. File is " <<fOutFileName 
	    << " Title is " << fOutTitle << std::endl;

  fWriteBeginRun=kTRUE;

  return kTRUE;
} 



//_____________________________________________________________
void JSFWriteStdHep::StdXWINIT(std::string ifilename, std::string ititle,
			 Int_t ntry, Int_t &iostream, Int_t &iok)
{
  const Char_t *filename=ifilename.data();
  Int_t lfilename=ifilename.length();
  const Char_t *title=ititle.data();
  Int_t ltitle=ititle.length();
  
  stdxwinit_(filename, title, &ntry, &iostream, &iok, lfilename, ltitle);  

}

//_____________________________________________________________
void JSFWriteStdHep::StdXWOPEN(std::string ifilename, std::string ititle,
			 Int_t ntry, Int_t &iostream, Int_t &iok)
{
  const Char_t *filename=ifilename.data();
  Int_t lfilename=ifilename.length();
  const Char_t *title=ititle.data();
  Int_t ltitle=ititle.length();
  
  stdxwopen_(filename, title, &ntry, &iostream, &iok, lfilename, ltitle);  

}

//_____________________________________________________________
void JSFWriteStdHep::StdXWRT(Int_t ilbl, Int_t ostream, Int_t &iok)
{
  stdxwrt_(&ilbl, &ostream, &iok);

}

//_____________________________________________________________
void JSFWriteStdHep::StdFLPYXSEC(Int_t ntry)
{
  Int_t itry=ntry;
  stdflpyxsec_(&itry);
}

//_____________________________________________________________________________
void JSFWriteStdHep::StdXEND(Int_t iostream)
{
  stdxend_(&iostream);
}

//_____________________________________________________________
Bool_t JSFWriteStdHep::Process(Int_t nev)
{
// 
  int lok=0;

  if( fWriteBeginRun ) {
    fWriteBeginRun=kFALSE;
    stdflpyxsec_(&fNTries);
    Int_t ilbl=100;
    stdxwrt_(&ilbl, &fOutStream, &lok);

    if( lok != 0 ) { 
      std::cerr << "Error  JSFWriteStdHep::BeginRun failed to write BeginRun record ";
      std::cerr << fOutFileName << std::endl;
      return kFALSE;
    }
  }
  
//    Double_t esum=0.0;
//    Double_t pxsum=0.0;
//    Double_t pysum=0.0;
//    Double_t pzsum=0.0;

  // Here we convert JETSET to HEPEVT before Write-Stdhep is called.
  if( fEventSource == 1 ) {
    TPythia6 *py6=TPythia6::Instance();
    Int_t mconv=1; // Convert from JETSET to HEPEVT
    py6->Pyhepc(mconv);
  }
  
  // Fill HEPEVT common block from JSFGenerator
  else if( fEventSource==2 ) {
    JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
    JSFGeneratorBuf *gevt=(JSFGeneratorBuf*)gen->EventBuf();
    TClonesArray *pa=gevt->GetParticles();

    hepevt_.nevhep=nev;
    hepevt_.nhep=gevt->GetNparticles();

    Int_t j;

	std::cerr << " fHEPEV4=" << fHEPEV4 << std::endl;

    hepev4_.eventweightlh = fHEPEV4 ? fHEPEV4->GetEventWeight() : 1.0 ;
    hepev4_.alphaqedlh= fHEPEV4 ? fHEPEV4->GetAlphaQED() : 0.0 ;
    hepev4_.alphaqcdlh= fHEPEV4 ? fHEPEV4->GetAlphaQCD() : 0.0 ;
    if( fHEPEV4 ) {
      for (j=0;j<10;j++) { hepev4_.scalelh[j]=fHEPEV4->GetScale(j); }
    }
    else {
      for (j=0;j<10;j++) { hepev4_.scalelh[j]=0.0; }
    }
    hepev4_.idruplh=fHEPEV4 ? fHEPEV4->GetEventID() : fProcessID ;



    for(j=0;j<gevt->GetNparticles();j++){
      JSFGeneratorParticle *p=(JSFGeneratorParticle*)pa->At(j);
#if 0
//    p->ls("all");
//    p->ls();
#endif
      if( fHEPEV4 ) { 
	hepev4_.spinlh[j][0]=fHEPEV4->GetSpin(j, 0);
      	hepev4_.spinlh[j][1]=fHEPEV4->GetSpin(j, 1);
      	hepev4_.spinlh[j][2]=fHEPEV4->GetSpin(j, 2);
      	hepev4_.icolorflowlh[j][0]=fHEPEV4->GetColorFlow(j, 0);
      	hepev4_.icolorflowlh[j][1]=fHEPEV4->GetColorFlow(j, 1);
      }
      else {
	hepev4_.spinlh[j][0]=0.0;
      	hepev4_.spinlh[j][1]=0.0;
      	hepev4_.spinlh[j][2]=0.0;
      	hepev4_.icolorflowlh[j][0]=0;
      	hepev4_.icolorflowlh[j][1]=0;
      }

      hepevt_.isthep[j]=13;
      hepevt_.idhep[j]=p->GetID();
      hepevt_.jmohep[j][0]=p->GetMother();
      hepevt_.jmohep[j][1]=0;
      hepevt_.jdahep[j][0]=p->GetFirstDaughter();
      hepevt_.jdahep[j][1]=p->GetFirstDaughter()+p->GetNDaughter()-1;


      if (p->GetNDaughter() == 0 ) {
	hepevt_.isthep[j]=1; 
        if( p->GetMother() < 0 ) { hepevt_.jmohep[j][0] = 0 ; }
        hepevt_.jdahep[j][0]=0;
        hepevt_.jdahep[j][1]=0;
      }	
      else if( p->GetMother() < 0 || p->GetNDaughter() > 1000 ) {
        hepevt_.isthep[j]=13;	
        hepevt_.jdahep[j][0]=0;
        hepevt_.jdahep[j][1]=0;
      }
      else if ( p->GetNDaughter() > 0 ) {
        Int_t id=p->GetFirstDaughter();
        JSFGeneratorParticle *pd=(JSFGeneratorParticle*)pa->At(id-1);
	if( pd->GetNDaughter() > 1000 ) {
          hepevt_.isthep[j]=13;
//	  hepevt_.jdahep[j][0]=p->GetNDaughter();
//	  hepevt_.jdahep[j][1]=p->GetFirstDaughter();
	}
	else {
          hepevt_.isthep[j]=2; 
        }
      } 
//      std::cerr << " j=" << j << " isthep=" << hepevt_.isthep[j]
//        << " jdahep=" << hepevt_.jdahep[j][0] << " " 
//	<< hepevt_.jdahep[j][1]
//	<< " ndau=" << p->fNdaughter
//	<< " n1st=" << p->fFirstDaughter
//	<< " mod="  << p->fMother
//	<< std::endl;

      hepevt_.phep[j][0]=p->GetPx();
      hepevt_.phep[j][1]=p->GetPy();
      hepevt_.phep[j][2]=p->GetPz();
      hepevt_.phep[j][3]=p->GetE();
      hepevt_.phep[j][4]=p->GetMass();
      hepevt_.vhep[j][0]=p->GetX();
      hepevt_.vhep[j][1]=p->GetY();
      hepevt_.vhep[j][2]=p->GetZ();
      hepevt_.vhep[j][3]=p->GetT();

//      if ( hepevt_.isthep[j]==1 ) {
//	esum+= hepevt_.phep[j][3];
//        pxsum+=hepevt_.phep[j][0];
//        pysum+=hepevt_.phep[j][1];
//        pzsum+=hepevt_.phep[j][2];
//      }
    }
  }

//  std::cerr << "JSFWriteStdHep::... esum=" << esum 
//	<< " pxsum=" << pxsum << " pysum=" << pysum
//	<< " pzsum=" << pzsum << std::endl;

  Int_t ldlb=1;
  stdxwrt_(&ldlb, &fOutStream, &lok);

  if( lok != 0 ) {
    std::cerr << " WriteStdHep::Process  Error to output event " 
	      << nev << std::endl;
    return kFALSE;
  }

  return kTRUE;
}


//_____________________________________________________________________________
Bool_t JSFWriteStdHep::EndRun()
{
  //  if ( IsWritable() ) {
  Int_t lok=0;
  if( fWriteEndRun ) {
    fWriteEndRun=kFALSE;
    Int_t ilbl=200;
    stdxwrt_(&ilbl, &fOutStream, &lok);
    if( lok != 0 ) { 
      std::cerr << "Error  JSFWriteStdHep::EndRun failed to write EndRun record ";
      std::cerr << fOutFileName << std::endl;
      return kFALSE;
    }
  }
  //  }
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFWriteStdHep::Terminate()
{
  if( fOutStream >= 0 ) {
    stdxend_(&fOutStream);
  }

  return kTRUE;
}

