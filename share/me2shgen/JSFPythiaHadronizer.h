#ifndef __JSFPythiaHadronizer__
#define __JSFPythiaHadronizer__

//////////////////////////////////////////////////////////////////////////
//
// JSFPythiaHadronizer
//
// Hadronizer using Pythia,using /HEPRUP/, /HEPEUP/ common
//
//$Id$
//////////////////////////////////////////////////////////////////////////

#ifndef __JSFSHGenerator__
#include "JSFSHGenerator.h"
#endif
#ifndef PYTHIA_TPythia6
#include "TPythia6.h"
#endif


class JSFPythiaHadronizer;

// ---------------------------------------------
class JSFPythiaHadronizerBuf : public JSFSHGeneratorBuf 
{
 protected:

 public:
  JSFPythiaHadronizerBuf(){}
  JSFPythiaHadronizerBuf(const char *name,
      const char *title="JSF Pythia Shower/Hadronizer Generator Event data",
			 JSFPythiaHadronizer *module=0);
  virtual ~JSFPythiaHadronizerBuf(){}

  void SetParticles(TPythia6 *fpythia);

  ClassDef(JSFPythiaHadronizerBuf,1)  // Pythia Shower/Hadronizer
};

//*************************************************************
class JSFPythiaHadronizer : public JSFSHGenerator 
{
 protected:
   Int_t    fMRPY[6];    // random seed for Jetset
   Float_t  fRRPY[100];  //
   TPythia6 *fPythia;    //!
   Int_t    fHEPEUPFormat; // > 0 when Non-standard HEPEUP format is used.
 public:
  JSFPythiaHadronizer(const char *name="JSFPythiaHadronizer", 
		      const char *title="JSF Pythia Shower/Hadronizer");

  virtual ~JSFPythiaHadronizer();

  virtual Bool_t Initialize();
  virtual Bool_t Process(Int_t event);
  virtual Bool_t BeginRun(Int_t nrun);
  virtual Bool_t EndRun();
  
  inline TPythia6 *GetPythia(){ return fPythia; }
  inline Int_t GetHEPEUPFormat(){ return fHEPEUPFormat; }

  ClassDef(JSFPythiaHadronizer,1) // JSF Pythia Shower/Hadronizer

};

#endif

