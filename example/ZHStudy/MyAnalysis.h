#ifndef __MyAnalysis__
#define __MyAnalysis__

///////////////////////////////////////////////////////////////////////
//
//  My Analysis
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TTree.h>
#include <TClonesArray.h>
#include <TH1.h>
#include "JSFModule.h"


class MyAnalysisBuf : public JSFEventBuf {
protected:
  Double_t fEcm;        // CM energy (GeV)
  Int_t    fNcmulti;    // Charged track multiplicity
  Double_t fMaxEP;      // Maximum electron momentum (GeV)
  Double_t fMaxMuP;     // Maximum muon momentum (GeV)			     

  Int_t    fNumjet;     // Number of jets.
  Double_t fEvent4V[4]; // Sum of 4 vector of all tracks.
  Double_t fJet[5][4];  // Jet four momentum ( up to 5 jets )
  Double_t fPtmiss;     // Missing Pt of the event	
  Double_t fMissmass;   // Missing mass (GeV)
  Double_t fCos1 ;      // Costh of 1st jet
  Double_t fCos2 ;      // Costh of 2nd jet
  Double_t fHmass ;     // Higgs mass (GeV)

  void CalculateVariables(Int_t njet, Float_t jetlst[][4]);
  void SetEcm(Float_t ecm){fEcm=ecm;}
  void SetLeptonMaxP(Double_t maxEP, Double_t maxMuP)
                          { fMaxEP=maxEP; fMaxMuP = maxMuP; }
  void SetNcharged(Int_t nt){ fNcmulti = nt; }
  void SetEvent4V(Double_t e, Double_t px, Double_t py, Double_t pz){
    fEvent4V[0]=e;  fEvent4V[1]=px;  fEvent4V[2]=py; fEvent4V[3]=pz; }
 
public:
  MyAnalysisBuf(const char *name="MyAnalysisBuf", 
	     const char *title="MyAnalysis event buffer",
		JSFModule *module=NULL)
    : JSFEventBuf(name, title, module){};
  virtual ~MyAnalysisBuf(){}

  friend class MyAnalysis;

  Double_t GetEcm(){ return fEcm; }
  Double_t GetNcmulti(){ return fNcmulti; }
  Double_t GetMaxEP(){ return fMaxEP; }
  Double_t GetMaxMuP(){ return fMaxMuP; }
  Int_t    GetNumjet(){ return fNumjet; }
  Double_t *GetEvent4V(){ return fEvent4V; }
  Double_t *GetJet(){ return &fJet[0][0]; }
  Double_t GetPtmiss(){ return fPtmiss; }
  Double_t GetMissmass(){ return fMissmass; }
  Double_t GetCos1(){ return fCos1 ; }
  Double_t GetCos2(){ return fCos2 ; }
  Double_t GetHmass(){ return fHmass ; }
  
  ClassDef(MyAnalysisBuf,1)  // MyAnalysis event buffer
};



class MyAnalysis : public JSFModule {
protected:
  Float_t fYcut ; // ycut value for JCLUST
public:
  MyAnalysis(const char *name="MyAnalysis", 
		  const char *title="My Analysis example");
  virtual ~MyAnalysis(){}

  virtual Bool_t Initialize();
  virtual Bool_t Process(Int_t ev=1);

  friend class MyAnalysisBuf;
  Float_t GetYcut(){ return fYcut;}
  void SetYcut(Float_t ycut){ fYcut=ycut; }

  ClassDef(MyAnalysis, 1)  // My Analysis class

};

#endif


