#ifndef __JSFGeneratedProcessType__
#define __JSFGeneratedProcessType__

///////////////////////////////////////////////////////////////////////
//
//  JSFGeneratedProcessType
//  Utility class for easy access to the event type.
//
//$Id$
///////////////////////////////////////////////////////////////////////

#include <TObject.h>

#include "JSFConfig.h"
#include "JSFGenerator.h"

class JSFGeneratedProcessType : public TObject {
 private:
  Int_t    fNd; // Number of final state d-quark
  Int_t    fNu; // Number of final state u-quark
  Int_t    fNs; // Number of final state s-quark
  Int_t    fNc; // Number of final state c-quark
  Int_t    fNb; // Number of final state c-quark
  Int_t    fNt; // Number of final state c-quark
  Int_t    fNe;  // Number of final state electron
  Int_t    fNne; // Number of final state neutrino-electron 
  Int_t    fNmu;  // Number of final state muon
  Int_t    fNnmu; // Number of final state neutrino-muon
  Int_t    fNtau;  // Number of final state tau
  Int_t    fNntau;  // Number of final state neutrino-tau

  Int_t    fNgamma;  // Number of final state gamma
  Int_t    fNz;  // Number of generated z
  Int_t    fNw;  // Number of generated w
  Int_t    fNh;  // Number of generated higgs
  
  TClonesArray  *fGen; // TClonesArray of generator particle information

  void Reset();

public:
  JSFGeneratedProcessType(TClonesArray *gen=NULL);
  virtual ~JSFGeneratedProcessType(){};

  void AnalizeEvent(TClonesArray *gen=NULL);

  inline Int_t GetNd(){ return fNd; }
  inline Int_t GetNu(){ return fNu; }
  inline Int_t GetNs(){ return fNs; }
  inline Int_t GetNc(){ return fNc; }
  inline Int_t GetNb(){ return fNb; }
  inline Int_t GetNt(){ return fNt; }

  inline Int_t GetNe(){ return fNe; }
  inline Int_t GetNn(){ return fNne; }
  inline Int_t GetNmu(){ return fNmu; }
  inline Int_t GetNnmu(){ return fNnmu; }
  inline Int_t GetNtau(){ return fNtau; }
  inline Int_t GetNntau(){ return fNntau; }

  inline Int_t GetNumberOfdquarks(){ return fNd+fNs+fNb; }
  inline Int_t GetNumberOfuquarks(){ return fNu+fNc+fNt; }
  inline Int_t GetNumberOfquarks(){ return fNd+fNu+fNs+fNs+fNb+fNt; }
  inline Int_t GetNumberOfChargedLeptons(){ return fNe+fNmu+fNtau; }
  inline Int_t GetNumberOfNeutrinos(){ return fNne+fNnmu+fNntau; }
  inline Int_t GetNumberOfLeptons(){ return fNe+fNmu+fNtau+fNne+fNnmu+fNntau; }
  inline Int_t GetNumberOfFermions(){ return GetNumberOfquarks() +
					GetNumberOfLeptons(); }

  inline Bool_t IsFourFermionProcess(Int_t nquark, Int_t ncharged_lepton, 
			    Int_t nneutrino){
    return ((  GetNumberOfFermions()==4 &&
	      GetNumberOfquarks()==nquark && 
	      GetNumberOfLeptons()==ncharged_lepton &&
	      GetNumberOfNeutrinos()==nneutrino ) ? kTRUE : kFALSE) ; 
  }

  inline Bool_t HasQuarks(Int_t d, Int_t u, Int_t s,
			Int_t c, Int_t b, Int_t t){
  return ( (fNd==d && fNu==u && fNs==s && fNc==c && fNb==b && fNt==t) ?
	   kTRUE : kFALSE ) ; }

  inline Bool_t HasLeptons(Int_t e, Int_t ne, Int_t m,
			Int_t nm, Int_t t, Int_t nt){
  return ( (fNe==e && fNne==ne && fNmu==m && 
	    fNnmu==nm && fNtau==t && fNntau==nt) ?
	   kTRUE : kFALSE ) ; }

  inline Bool_t IsQQQQ(){  return IsFourFermionProcess(4,0,0); }
  inline Bool_t IsQQLN(){  return IsFourFermionProcess(2,1,1); }
  inline Bool_t IsQQNN(){  return IsFourFermionProcess(2,0,2); }
  inline Bool_t IsQQLL(){  return IsFourFermionProcess(2,2,0); }
  inline Bool_t IsLNLN(){  return IsFourFermionProcess(0,2,2); }

  inline Bool_t IsQQEN(){ return  ((  
         GetNumberOfFermions()==4 && GetNumberOfquarks()==2 &&  
	 fNne==1 &&  fNe==1) ? kTRUE : kFALSE) ; 
  }
  inline Bool_t IsQQMN(){ return  ((
         GetNumberOfFermions()==4 && GetNumberOfquarks()==2 &&  
	 fNnmu==1 &&  fNmu==1) ? kTRUE : kFALSE) ; 
  }
  inline Bool_t IsQQTN(){ return  ((
         GetNumberOfFermions()==4 && GetNumberOfquarks()==2 &&  
	 fNntau==1 &&  fNtau==1) ? kTRUE : kFALSE) ; 
  }
  inline Bool_t IsQQEE(){ return  ((
         GetNumberOfFermions()==4 && GetNumberOfquarks()==2 &&  
	 fNne==2 &&  fNe==2) ? kTRUE : kFALSE) ; 
  }
  inline Bool_t IsQQMM(){ return  ((
         GetNumberOfFermions()==4 && GetNumberOfquarks()==2 &&  
	 fNnmu==1 &&  fNmu==1) ? kTRUE : kFALSE) ; 
  }
  inline Bool_t IsQQTT(){ return  ((
         GetNumberOfFermions()==4 && GetNumberOfquarks()==2 &&  
	 fNntau==1 &&  fNtau==1) ? kTRUE : kFALSE ) ; 
  }


  ClassDef(JSFGeneratedProcessType, 1)  // Get ProcessType

};

#endif


