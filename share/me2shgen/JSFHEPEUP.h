#ifndef __JSFHEPEUP__
#define __JSFHEPEUP__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFHEPEUP                                                            //
//
// A class to store HEPEUP event information
//                                                                      //
//$Id$
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

//----------------------------------------------------------------
class JSFHEPEUP : public TObject
{
 protected:
  Int_t fNUP;
  Int_t fIDPRUP;
  Double_t fXWGTUP;
  Double_t fSCALUP;
  Double_t fAQEDUP;
  Double_t fAQCDUP;
  Int_t  fNUP2;
  Int_t  fNUP5;
  Int_t *fIDUP    ;  //[fNUP]
  Int_t *fISTUP   ;  //[fNUP]
  Int_t *fMOTHUP  ;  //[fNUP2]
  Int_t *fICOLUP  ;  //[fNUP2]
  Double_t *fPUP  ;  //[fNUP5]
  Double_t *fVTIMUP ; //[fNUP]
  Double_t *fSPINUP ; //[fNUP]

 public:
  JSFHEPEUP(Int_t nup=0);
  virtual ~JSFHEPEUP();
  JSFHEPEUP(const JSFHEPEUP &hepeup);
  JSFHEPEUP &operator=(const JSFHEPEUP &rhs);

  
  inline Int_t GetNUP(){ return fNUP; }
  inline Int_t GetIDPRUP(){ return fIDPRUP; }
  inline Double_t GetXWGTUP(){ return fXWGTUP; }
  inline Double_t GetSCALUP(){ return fSCALUP; }
  inline Double_t GetAQEDUP(){ return fAQEDUP; }
  inline Double_t GetAQCDUP(){ return fAQCDUP; }
  inline Int_t GetIDUP(const Int_t i){ return fIDUP[i-1]; }
  inline Int_t GetISTUP(const Int_t i){ return fISTUP[i-1]; }
  inline Int_t GetMOTHUP(const Int_t i, const Int_t j){ 
    return fMOTHUP[2*(j-1)+(i-1)]; }
  inline Int_t GetMOTHUP1(const Int_t i){return fMOTHUP[2*(i-1)]; }
  inline Int_t GetMOTHUP2(const Int_t i){return fMOTHUP[2*(i-1)+1]; }
  inline Int_t GetICOLUP(const Int_t i, const Int_t j){ 
    return fICOLUP[2*(j-1)+(i-1)]; }
  inline Int_t GetICOLUP1(const Int_t i){return fICOLUP[2*(i-1)]; }
  inline Int_t GetICOLUP2(const Int_t i){return fICOLUP[2*(i-1)+1]; }

  inline Double_t GetPx(const Int_t i){ return fPUP[5*(i-1)]; }
  inline Double_t GetPy(const Int_t i){ return fPUP[5*(i-1)+1]; }
  inline Double_t GetPz(const Int_t i){ return fPUP[5*(i-1)+2]; }
  inline Double_t GetE(const Int_t i){ return fPUP[5*(i-1)+3]; }
  inline Double_t GetMass(const Int_t i){ return fPUP[5*(i-1)+4]; }
  inline Double_t *GetPUP(const Int_t i){ return &fPUP[5*(i-1)]; }
  
  inline Double_t GetVTIMUP(const Int_t i){ return fVTIMUP[i-1]; }
  inline Double_t GetSPINUP(const Int_t i){ return fSPINUP[i-1]; }

  Bool_t GetParticle(Int_t i, Int_t &idup, Int_t &istup, Int_t mother[2],
		     Int_t colup[2], Double_t p[5], Double_t &vtimup,
		     Double_t &spinup);

  Bool_t SetParticle(Int_t i, Int_t idup, Int_t istup, Int_t mother[2],
		   Int_t colup[2], Double_t p[5], Double_t vtimup,
		   Double_t spinup);
  inline void SetHeaders(Int_t idprup, Double_t xwgtup, 
	   Double_t scalup, Double_t aqedup, Double_t aqcdup){
    fIDPRUP = idprup;
    fXWGTUP = xwgtup;
    fSCALUP = scalup;
    fAQEDUP = aqedup;
    fAQCDUP = aqcdup;
  }
		   
  void Load(void *address);   // Read from Common/HEPEUP/
  void Save(void *address);   // Write to  Common/HEPEUP/
  void Print();  // Print contents 

  Bool_t ReadFile(ifstream &in);

  ClassDef(JSFHEPEUP,1) // HEPEUP event data

};

#endif
