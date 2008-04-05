#ifndef __JSFStdHepStdCM__
#define __JSFStdHepStdCM__

///////////////////////////////////////////////////////////////////////
//
//  JSFStdHepStdCM
//
//  STdHep BeginRun/EndRun information
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

#include "TObject.h"

class JSFStdHepStdCM: public TObject
{
 protected:
  Float_t fStdECom;   //  STDECOM  - center-of-mass energy 
  Float_t fStdXSec;   //  STDXSEC  - cross-section 
  Double_t fStdSeed1; //  STDSEED1 - random number seed 
  Double_t fStdSeed2; //  STDSEED2 - random number seed
  Int_t    fNEvtReq;  //  NEVTREQ  - number of events to be generated 
  Int_t    fNEvtGen;  //  NEVTGEN  - number of events actually generated 
  Int_t    fNEvtWrt;  //  NEVTWRT  - number of events written to output file 
  Int_t    fNEvtLH;   //  NEVTLH  - number of Les Houches events written to output file
  Char_t   fGeneratorName[20]; //  name of Monte Carlo generator 
  Char_t   fPDFName[20];       //  name of PDF method used
 
 public:
  JSFStdHepStdCM(): fStdECom(0.0), fStdXSec(0.0), fStdSeed1(0.0), fStdSeed2(0.0),
    fNEvtReq(0), fNEvtGen(0), fNEvtWrt(0), fNEvtLH(0)
    { fGeneratorName[0]=(Char_t)0 ; fPDFName[0]=(Char_t)0; }

  ~JSFStdHepStdCM(){}

  inline Float_t GetStdECom(){ return fStdECom; }
  inline Float_t GetStdXSec(){ return fStdXSec; }
  inline Float_t GetStdSeed1(){ return fStdSeed1;}
  inline Float_t GetStdSeed2(){ return fStdSeed2;}
  inline Int_t   GetNEvtReq(){ return fNEvtReq; }
  inline Int_t   GetNEvtGen(){ return fNEvtGen; }
  inline Int_t   GetNEvtWrt(){ return fNEvtWrt;}
  inline Int_t   GetNEvtLH(){ return fNEvtLH;}
  inline Char_t *GetGenratorName(){ return fGeneratorName; }
  inline Char_t *GetPDFName(){ return fPDFName;}

  void LoadStdHepCM();
  void SetStdHepCM(Float_t ecom, Float_t xsec, Double_t seed1, 
		   Double_t seed2, Int_t nevtreq, Int_t nevtgen,
		   Int_t nevtqrt, Int_t nevtlh, 
		   Char_t *gname, Char_t *pdfname);

  void SetStdECom(Float_t val);
  void SetStdXSec(Float_t val);
  void SetStdSeed1(Double_t val);
  void SetStdSeed2(Double_t val);
  void SetNEvtReq(Int_t val);
  void SetNEvtGen(Int_t val);
  void SetNEvtWrt(Int_t val);
  void SetNEvtLH(Int_t val);
  void SetGeneratorName(Char_t *val);
  void SetPDFName(Char_t *val);

  void Print();

  ClassDef(JSFStdHepStdCM,1)  // JSFStdHep BeginRun/EndRun Header
};

#endif










