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

#include "JSFGenerator.h"


class PythiaGenerator : public JSFGenerator {
private:
  Double_t fEcm;     ; // Center of mass energy (GeV)
  TPythia  *fPythia  ; //! Pointer to the pythia class
  Int_t    fMRLU[6];   // Random seed at the begining of event
  Float_t  fRRLU[100]; // 			     
public:
  PythiaGenerator(const char *name="PythiaGenerator", 
		  const char *title="Pythia Event generator");
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
  ClassDef(PythiaGenerator, 1)  // Pythia Generator

};

#endif


