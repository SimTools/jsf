///////////////////////////////////////////////////////////////////
//
// JSFJETNET
//
// A collection of Interface routines to use JETNET routines
//
//$Id$
//
//////////////////////////////////////////////////////////////////

#include "JSFJETNET.h"

ClassImp(JSFJETNET)

const Int_t JSFJETNET::kMAXV=2000;
const Int_t JSFJETNET::kMAXM=150000;
const Int_t JSFJETNET::kMAXI=1000;
const Int_t JSFJETNET::kMAXO=1000;
const Int_t JSFJETNET::kMAXD=2;

extern "C" {
  extern void jntdec_(Int_t *method);
  extern void jninit_();
  extern void jntral_();
  extern void jntest_();
};

typedef struct {
  Int_t mstjn[40];
  Float_t parjn[40];
  Int_t mstjm[20];
  Float_t parjm[20];
  Float_t oin[JSFJETNET::kMAXI];
  Float_t out[JSFJETNET::kMAXO];
  Int_t mxndjm;
} COMMON_JNDAT1 ;

typedef struct {
  Float_t o[JSFJETNET::kMAXV];
  Float_t a[JSFJETNET::kMAXV];
  Float_t d[JSFJETNET::kMAXV];
  Float_t t[JSFJETNET::kMAXV];
  Float_t dt[JSFJETNET::kMAXV];
  Float_t w[JSFJETNET::kMAXM];
  Float_t dw[JSFJETNET::kMAXM];
  Int_t nself[JSFJETNET::kMAXM];
  Int_t ntself[JSFJETNET::kMAXV];
  Float_t g[JSFJETNET::kMAXV+JSFJETNET::kMAXM];
  Float_t odw[JSFJETNET::kMAXM];
  Float_t odt[JSFJETNET::kMAXV];
  Float_t etav[JSFJETNET::kMAXV+JSFJETNET::kMAXV];
} COMMON_JNINT1 ;

typedef struct {
  Int_t m[11];
  Int_t mv0[11];
  Int_t mm0[11];
  Int_t ng[10];
  Int_t nl;
  Int_t ipott;
  Float_t er1, er2, sm[10];
  Int_t icpon;
} COMMON_JNINT2;

typedef struct {
  Float_t tinv[10];
  Int_t igfn[10];
  Float_t etal[10];
  Float_t widl[10];
  Float_t satm[10];
} COMMON_JNDAT2;

typedef struct {
  Int_t ndim;
  Int_t isw[10];
  Int_t nodes[JSFJETNET::kMAXD+2];
  Int_t nbo;
} COMMON_JMINT1;


typedef struct {
  Int_t ilimon,ilinon,nc;
  Float_t g2;
  Int_t  nit;
  Float_t errln[4], derrln, stepln[4], stepmn,errmn;
  Int_t   ieval,isucc,icurve,nsc;
  Float_t gvec2;
} COMMON_JNINT4;

typedef struct {
  Int_t iset;
  Float_t gasdev;
} COMMON_JNGAUS;

typedef struct {
  Int_t mrjn[5];
  Float_t prjn[100];
} COMMON_JNDATR;

extern COMMON_JNDAT2 jndat2_;
extern COMMON_JNDAT1 jndat1_; 
extern COMMON_JNINT1 jnint1_; 
extern COMMON_JNINT2 jnint2_; 
extern COMMON_JNINT4 jnint4_; 
extern COMMON_JMINT1 jmint1_;
extern COMMON_JNGAUS jngaus_;
extern COMMON_JNDATR jndatr_;



// ---------------------------------------------------------------
JSFJETNET::JSFJETNET()
{
}

// ---------------------------------------------------------------
void JSFJETNET::JNTDEC(Int_t method){ jntdec_(&method); }
void JSFJETNET::JNINIT(){ jninit_(); }
void JSFJETNET::JNTRAL(){ jntral_(); }
void JSFJETNET::JNTEST(){ jntest_(); }

// ---------------------------------------------------------------
Int_t    JSFJETNET::GetMSTJN(Int_t i){ return jndat1_.mstjn[i-1]; }
Float_t  JSFJETNET::GetPARJN(Int_t i){ return jndat1_.parjn[i-1]; }
Int_t    JSFJETNET::GetMSTJM(Int_t i){ return jndat1_.mstjm[i-1]; }
Float_t  JSFJETNET::GetPARJM(Int_t i){ return jndat1_.parjm[i-1]; }
Float_t  JSFJETNET::GetOIN(Int_t i){ return jndat1_.oin[i-1]; }
Float_t  JSFJETNET::GetOUT(Int_t i){ return jndat1_.out[i-1]; }
Int_t    JSFJETNET::GetMXNDJM(){ return jndat1_.mxndjm; }

// ---------------------------------------------------------------
Float_t  JSFJETNET::GetO(Int_t i){ return jnint1_.o[i-1]; }
Float_t  JSFJETNET::GetA(Int_t i){ return jnint1_.a[i-1]; }
Float_t  JSFJETNET::GetD(Int_t i){ return jnint1_.d[i-1]; }
Float_t  JSFJETNET::GetT(Int_t i){ return jnint1_.t[i-1]; }
Float_t  JSFJETNET::GetDT(Int_t i){ return jnint1_.dt[i-1]; }
Float_t  JSFJETNET::GetW(Int_t i){ return jnint1_.w[i-1]; }
Float_t  JSFJETNET::GetDW(Int_t i){ return jnint1_.dw[i-1]; }
Int_t    JSFJETNET::GetNSELF(Int_t i){ return jnint1_.nself[i-1]; }
Int_t    JSFJETNET::GetNTSELF(Int_t i){ return jnint1_.ntself[i-1]; }
Float_t  JSFJETNET::GetG(Int_t i){ return jnint1_.g[i-1]; }
Float_t  JSFJETNET::GetODW(Int_t i){ return jnint1_.odw[i-1]; }
Float_t  JSFJETNET::GetODT(Int_t i){ return jnint1_.odt[i-1]; }
Float_t  JSFJETNET::GetETAV(Int_t i){ return jnint1_.etav[i-1]; }


// ---------------------------------------------------------------
Int_t    JSFJETNET::GetM(Int_t i){ return jnint2_.m[i-1]; }
Int_t    JSFJETNET::GetMV0(Int_t i){ return jnint2_.mv0[i-1]; }
Int_t    JSFJETNET::GetMM0(Int_t i){ return jnint2_.mm0[i-1]; }
Int_t    JSFJETNET::GetNG(Int_t i){ return jnint2_.ng[i-1]; }
Int_t    JSFJETNET::GetNL(){ return jnint2_.nl; }
Int_t    JSFJETNET::GetIPOTT(){ return jnint2_.ipott; }
Float_t  JSFJETNET::GetER1(){ return jnint2_.er1; }
Float_t  JSFJETNET::GetER2(){ return jnint2_.er2; }
Float_t  JSFJETNET::GetSM(Int_t i){ return jnint2_.sm[i-1]; }
Int_t    JSFJETNET::GetICPON(){ return jnint2_.icpon; }

// ---------------------------------------------------------------
void JSFJETNET::SetMSTJN(Int_t i, Int_t val){ jndat1_.mstjn[i-1]=val; }
void JSFJETNET::SetPARJN(Int_t i, Float_t val){ jndat1_.parjn[i-1]=val; }
void JSFJETNET::SetMSTJM(Int_t i, Int_t val){ jndat1_.mstjm[i-1]=val; }
void JSFJETNET::SetPARJM(Int_t i, Float_t val){ jndat1_.parjm[i-1]=val; }
void JSFJETNET::SetOIN(Int_t i, Float_t val){ jndat1_.oin[i-1]=val; }
void JSFJETNET::SetOUT(Int_t i, Float_t val){ jndat1_.out[i-1]=val; }
void JSFJETNET::SetMXNDJM(Int_t val){ jndat1_.mxndjm=val; }

// ---------------------------------------------------------------
void JSFJETNET::SetO(Int_t i, Float_t val){ jnint1_.o[i-1]=val; }
void JSFJETNET::SetA(Int_t i, Float_t val){ jnint1_.a[i-1]=val; }
void JSFJETNET::SetD(Int_t i, Float_t val){ jnint1_.d[i-1]=val; }
void JSFJETNET::SetT(Int_t i, Float_t val){ jnint1_.t[i-1]=val; }
void JSFJETNET::SetDT(Int_t i, Float_t val){ jnint1_.dt[i-1]=val; }
void JSFJETNET::SetW(Int_t i, Float_t val){ jnint1_.w[i-1]=val; }
void JSFJETNET::SetDW(Int_t i, Float_t val){ jnint1_.dw[i-1]=val; }
void JSFJETNET::SetNSELF(Int_t i, Int_t val){ jnint1_.nself[i-1]=val; }
void JSFJETNET::SetNTSELF(Int_t i, Int_t val){ jnint1_.ntself[i-1]=val; }
void JSFJETNET::SetG(Int_t i, Float_t val){ jnint1_.g[i-1]=val; }
void JSFJETNET::SetODW(Int_t i, Float_t val){ jnint1_.odw[i-1]=val; }
void JSFJETNET::SetODT(Int_t i, Float_t val){ jnint1_.odt[i-1]=val; }
void JSFJETNET::SetETAV(Int_t i, Float_t val){ jnint1_.etav[i-1]=val; }

// ---------------------------------------------------------------
void JSFJETNET::SetM(Int_t i, Int_t val){ jnint2_.m[i-1]=val; }
void JSFJETNET::SetMV0(Int_t i, Int_t val){ jnint2_.mv0[i-1]=val; }
void JSFJETNET::SetMM0(Int_t i, Int_t val){ jnint2_.mm0[i-1]=val; }
void JSFJETNET::SetNG(Int_t i, Int_t val){ jnint2_.ng[i-1]=val; }
void JSFJETNET::SetNL(Int_t val){ jnint2_.nl=val; }
void JSFJETNET::SetIPOTT(Int_t val){ jnint2_.ipott=val; }
void JSFJETNET::SetER1(Float_t val){ jnint2_.er1=val; }
void JSFJETNET::SetER2(Float_t val){ jnint2_.er2=val; }
void JSFJETNET::SetSM(Int_t i, Float_t val){ jnint2_.sm[i-1]=val; }
void JSFJETNET::SetICPON(Int_t val){ jnint2_.icpon=val; }
  
