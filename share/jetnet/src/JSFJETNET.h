#ifndef __JSFJETNET__
#define __JSFJETNET__

///////////////////////////////////////////////////////////////////////
//
//  Static member functions to call JETNET routines.
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>

// *************************************************************
class JSFJETNET 
{
 public:
  static const Int_t kMAXV;
  static const Int_t kMAXM;
  static const Int_t kMAXI;
  static const Int_t kMAXO;
  static const Int_t kMAXD;

 public:
  JSFJETNET();
  virtual ~JSFJETNET(){}

  void JNTDEC(Int_t method);
  void JNINIT();
  void JNTRAL();
  void JNTEST();

  Int_t GetMAXV(){ return kMAXV; }
  Int_t GetMAXM(){ return kMAXM; }
  Int_t GetMAXI(){ return kMAXI; }
  Int_t GetMAXO(){ return kMAXO; }

  Int_t   GetMSTJN(Int_t i);
  Float_t GetPARJN(Int_t i);
  Int_t   GetMSTJM(Int_t i);
  Float_t GetPARJM(Int_t i);
  Float_t GetOIN(Int_t i);
  Float_t GetOUT(Int_t i);
  Int_t   GetMXNDJM();

  Float_t GetO(Int_t i);
  Float_t GetA(Int_t i);
  Float_t GetD(Int_t i);
  Float_t GetT(Int_t i);
  Float_t GetDT(Int_t i);
  Float_t GetW(Int_t i);
  Float_t GetDW(Int_t i);
  Int_t   GetNSELF(Int_t i);
  Int_t   GetNTSELF(Int_t i);
  Float_t GetG(Int_t i);
  Float_t GetODW(Int_t i);
  Float_t GetODT(Int_t i);
  Float_t GetETAV(Int_t i);
  Int_t   GetM(Int_t i);
  Int_t   GetMV0(Int_t i);
  Int_t   GetMM0(Int_t i);
  Int_t   GetNG(Int_t i);
  Int_t   GetNL();
  Int_t   GetIPOTT();
  Float_t GetER1();
  Float_t GetER2();
  Float_t GetSM(Int_t i);
  Int_t   GetICPON();
  
  void SetMSTJN(Int_t i, Int_t val);
  void SetPARJN(Int_t i, Float_t val);
  void SetMSTJM(Int_t i, Int_t val);
  void SetPARJM(Int_t i, Float_t val);
  void SetOIN(Int_t i, Float_t val);
  void SetOUT(Int_t i, Float_t val);
  void SetMXNDJM(Int_t val);

  void SetO(Int_t i, Float_t val);
  void SetA(Int_t i, Float_t val);
  void SetD(Int_t i, Float_t val);
  void SetT(Int_t i, Float_t val);
  void SetDT(Int_t i, Float_t val);
  void SetW(Int_t i, Float_t val);
  void SetDW(Int_t i, Float_t val);
  void SetNSELF(Int_t i, Int_t val);
  void SetNTSELF(Int_t i, Int_t val);
  void SetG(Int_t i, Float_t val);
  void SetODW(Int_t i, Float_t val);
  void SetODT(Int_t i, Float_t val);
  void SetETAV(Int_t i, Float_t val);
  void SetM(Int_t i, Int_t val);
  void SetMV0(Int_t i, Int_t val);
  void SetMM0(Int_t i, Int_t val);
  void SetNG(Int_t i, Int_t val);
  void SetNL(Int_t val);
  void SetIPOTT(Int_t val);
  void SetER1(Float_t val);
  void SetER2(Float_t val);
  void SetSM(Int_t i, Float_t val);
  void SetICPON(Int_t val);
  
  ClassDef(JSFJETNET,0)  // Interfaces to JETNET routines.
};

#endif


