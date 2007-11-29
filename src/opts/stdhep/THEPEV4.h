#ifndef __THEPEV4__
#define __THEPEV4__

///////////////////////////////////////////////////////////////////////
//
//  THEPEV4
//
//  Holds HEPEV4 common information
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>

class THEPEV4 : public TObject
{
 protected:
  Double_t fEventWeight;                /* event weight */
  Double_t fAlphaQED;                   /* QED coupling alpha_em */
  Double_t fAlphaQCD;                   /* QCD coupling alpha_s */
  Double_t fScale[10];                  /* Scale Q of the event */
  Int_t    fNHEP;
  Double_t *fSpin0;   //[fNHEP]
  Double_t *fSpin1;   //[fNHEP]
  Double_t *fSpin2;   //[fNHEP]         
  Int_t    *fColorFlow0;  //[fNHEP]     /* spin information */
  Int_t    *fColorFlow1;  //[fNHEP]     /* (Anti-)Colour flow */
  Int_t    fEventID;                       /* ID, as given by LPRUP codes */
 public:
  THEPEV4();
  void SetHEPEV4(Int_t nhep);

  virtual ~THEPEV4();

  inline Double_t GetEventWeight(){ return fEventWeight; }
  inline Double_t GetAlphaQED(){ return fAlphaQED; }
  inline Double_t GetAlphaQCD(){ return fAlphaQCD; }
  inline Double_t *GetScale(){ return fScale;}
  inline Double_t GetScale(Int_t i){ return fScale[i]; }
  inline Int_t GetEventID(){ return fEventID; }
  inline Int_t GetNHEP(){ return fNHEP;}
    
  Double_t GetSpin(Int_t i, Int_t elm);
  Int_t GetColorFlow(Int_t i, Int_t elm);

  void Print();

  ClassDef(THEPEV4,1)  // HEPEV4 class

};

#endif

