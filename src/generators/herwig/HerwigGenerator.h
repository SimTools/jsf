#ifndef __HerwigGenerator__
#define __HerwigGenerator__

///////////////////////////////////////////////////////////////////////
//
//  JSF Herwig generator
//
//$Id$
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TTree.h>
#include <TClonesArray.h>

#include "JSFConfig.h"
#include "JSFGenerator.h"
#include "THerwig.h"
#include "JSFBeamGeneration.h"

class HerwigGenerator : public JSFGenerator {
private:
  static THerwig *fHerwig; //!
  Double_t fEcm        ;   // Center of mass energy (GeV)
  Int_t    fIPROC      ;   // Process number 
  Int_t    fIBRN[2]    ;   // 


  // For beamstrahlung
  TFile             *fBSFile; //! Beamstrahlung file
  JSFBeamGenerationCain  *fBS;     //  Beamstrahlung spectrum
  Double_t           fEcmMax; //! Abolutely max ECM energy
  TString            fBSname; //  Name of BS spectrum 
  Double_t           fBSwidth; //  half-width of BS spectrum

  Int_t     fPrintStat; //! call PYSTAT at the end run if not equal 0.				    
  Float_t   fEventWeight ; //! Weight of current event.    
  Int_t     fBeamStrahlung ; //  > 0 to include beam strahlung.

  Double_t           fEMinus; //!
  Double_t           fEPlus;  //!
  Int_t              fNBSGen; //
  Int_t              fNBSGood; //
  Double_t           fBSThreshold; // 


public:
  HerwigGenerator(const Char_t *name="HerwigGenerator", 
		  const Char_t *title="Herwig Event Generator",
		  const Option_t *opt="");
  virtual ~HerwigGenerator();

  inline Double_t GetEMinus(){ return fEMinus; }
  inline Double_t GetEPlus(){ return fEPlus; }

  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t nrun);
  virtual Bool_t Process(Int_t ev=1);
  virtual Bool_t EndRun();
  virtual Bool_t Terminate();
//  virtual Bool_t GetLastRunInfo();

  virtual Int_t HepevtToGeneratorParticles(COMMON_HEPEVT_t *hepevt);
  virtual void GetChargeCtau(Int_t pdgcode, Float_t &charge, Float_t &ctau);

  inline const JSFBeamGenerationCain *GetBS(){ return fBS; }
  inline const TString &GetBSName(){ return fBSname; }

  inline void SetIPROC(Int_t i){ fIPROC=i; }
  inline Int_t GetIPROC(){ return fIPROC; }

  inline void     SetEcm(Double_t ecm){fEcm=ecm; }
  inline Double_t GetEcm(){ return fEcm;}
  inline THerwig *GetHerwig(){ return fHerwig; }
 
  inline Int_t GetBeamStrahlung(){  return fBeamStrahlung; }
  inline void  SetBeamStrahlung(Int_t ibs){ fBeamStrahlung=ibs; }

  void    SetEventWeight(Float_t weight){ fEventWeight=weight; }
  Float_t GetEventWeight(){ return fEventWeight; }

  ClassDef(HerwigGenerator, 1)  // Herwig Generator

};

#endif


