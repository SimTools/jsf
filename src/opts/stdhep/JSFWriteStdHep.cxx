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

ClassImp(JSFWriteStdHep)

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

  //   fDoXWInit=gJSF->Env()->GetValue("JSFWriteStdHep.DoXWInit",kTRUE);
  fConvertJetset=gJSF->Env()->GetValue("JSFWriteStdHep.ConvertJetset",kTRUE);
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

//  int flag=0;

  if( fConvertJetset ) {
    TPythia6 *py6=TPythia6::Instance();
    Int_t mconv=1; // Convert from JETSET to HEPEVT
    py6->Pyhepc(mconv);
  }
  
  Int_t ldlb=1;
  stdxwrt_(&ldlb, &fOutStream, &lok);

  if( lok != 0 ) {
    std::cerr << " WriteStdHep::Process  Error to output event " 
	      << nev << std::endl;
    return kFALSE;
  }

#if 0
  // Fill HEPEVT common block from JSFGenerator

  Float_t data[20];
  Int_t i;
  for(i=0;i<20;i++){ data[i]=0.0;}
  JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *gevt=(JSFGeneratorBuf*)gen->EventBuf();
  TClonesArray *pa=gevt->GetParticles();
  Int_t j;
  for(j=0;j<gevt->GetNparticles();j++){
    JSFGeneratorParticle *p=(JSFGeneratorParticle*)pa->At(j);
#if 0
    p->ls("all");
#endif
    data[0]=p->fSer;
    data[1]=p->fID;
    data[2]=p->fMass;
    data[3]=p->fCharge;
    data[4]=p->fP[1];
    data[5]=p->fP[2];
    data[6]=p->fP[3];
    data[7]=p->fP[0];
    data[8]=p->fX[1];
    data[9]=p->fX[2];
    data[10]=p->fX[3];
    data[11]=p->fNdaughter;
    data[12]=p->fFirstDaughter;
    data[13]=p->fMother;
    data[14]=p->fX[0];
    data[15]=p->fLifeTime;
    data[16]=p->fDecayLength;
    Int_t ielm=j+1;
    if( ielm != p->fSer ){
      printf("Warning JSFQuickSim::TBPUTGeneratorParticles");
      printf("  Particle Serial number(%d) and ",p->fSer);
      printf(" element number(%d) is inconsistent.\n",ielm);
    }
    gJSFLCFULL->TBPUT(1,"Generator:Particle_List",ielm,20,(Int_t*)data,iret);
  }

  for(i=0;i<20;i++){ data[i]=0.0;}
  data[0]=gevt->GetEventNumber();
  data[1]=gevt->GetDate();
  data[2]=gevt->GetTime();
  data[4]=gevt->GetStartSeed();
  data[5]=gevt->GetEcm();
  gJSFLCFULL->TBPUT(1,"Generator:Header",1,20,(Int_t*)data,iret);  


#endif

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

