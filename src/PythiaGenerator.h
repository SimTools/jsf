#ifndef __PythiaGenerator__
#define __PythiaGenerator__

///////////////////////////////////////////////////////////////////////
//
//  JSF Pythia generator
//
//$Id$
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TPythia.h>
#include <TMCParticle.h>

#include "JSFConfig.h"
#include "JSFGenerator.h"


class PythiaGenerator : public JSFGenerator {
private:
  Double_t fEcm      ; // Center of mass energy (GeV)
  TPythia  *fPythia  ; //! Pointer to the pythia class
  Char_t   fFrame[8]; // Name of beam particle
  Char_t   fBeamParticle[8]; // Name of beam particle
  Char_t   fTargetParticle[8]; // Target particle.
  Int_t    fPrintStat; //! call PYSTAT at the end run if not equal 0.				    
  
  Int_t    fMRLU[6];   // Random seed at the begining of event
  Float_t  fRRLU[100]; // 			     
// Following data are filled only at the end of run, or data is read.
  Int_t    fNUMSUB   ; // Number of defined subprocesses + 1 (for total)
  Int_t    *fISUB    ; //[fNUMSUB] ISUB of subprocesses whose MSUB=1. (0=total, 1 to fNUMSUB )
  Int_t    *fNGEN    ; //[fNUMSUB] Number of generated subprocesses. (0=total, 1 to fNUMSUB )
  Double_t *fXSEC    ; //[fNUMSUB] Integrated cross sections (0=total, 1 to fNUMSUB)
  Float_t   fEventWeight ; //! Weight of current event.    

  Int_t     fBeamStrahlung ; //  > 0 to include beam strahlung.

public:
  PythiaGenerator(const Char_t *name="PythiaGenerator", 
		  const Char_t *title="Pythia Event generator",
		  const Option_t *opt="");
  virtual ~PythiaGenerator();

  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t nrun);
  virtual Bool_t Process(Int_t ev=1);
  virtual Bool_t EndRun();
  virtual Bool_t Terminate();
  virtual Bool_t GetLastRunInfo();
  TPythia  *GetPythia(){ return fPythia ; }

  void SetEcm(Double_t ecm){fEcm=ecm; }
  Double_t GetEcm(){ return fEcm;}

  Int_t GetNUMSUB(){ return fNUMSUB; }
  Int_t GetISUB(Int_t i){ return fISUB[i]; }
  Int_t GetNGEN(Int_t i){ return fNGEN[i]; }
  Double_t GetXSEC(Int_t i){ return fXSEC[i]; }

  inline Int_t GetBeamStrahlung(){  return fBeamStrahlung; }
  inline void SetBeamStrahlung(Int_t ibs){ fBeamStrahlung=ibs; }

  void SetEventWeight(Float_t weight){ fEventWeight=weight; }
  Float_t GetEventWeight(){ return fEventWeight; }

#ifdef __LCLIBRAN_USE_RANMAR__
  void WriteRandomSeed(Char_t *fw="");
  void ReadRandomSeed(Char_t *fr="");
  void PrintRandomSeed(Int_t num=8);  // First num's data are printed
#endif

  ClassDef(PythiaGenerator, 3)  // Pythia Generator

};

#endif


