//********************************************************************
// Common to convert JIM output to SIMDST
// 
//(History)
// 29-May-1999 A.Miyamoto Created
//
//$Id$
//
//********************************************************************
#ifndef __JSFReadJIMCommon__
#define __JSFReadJIMCommon__


const Int_t kJGenMax=700;
const Int_t kJCmbMax=700;
const Int_t kJTrkMax=300;
const Int_t kJEClsMax=500;
const Int_t kJHClsMax=400;
const Int_t kJClsSize=3;
const Int_t kJVTXBufSize=2000;

typedef struct {
  Int_t endian;
  Char_t produc[4];
  Int_t ivers, head[2],ierflg;
  Int_t ngen ; Float_t gen[kJGenMax][kGenSize]; 
               Int_t igen[kJGenMax][kIGenSize];
  Int_t ncmb ; Float_t cmbt[kJCmbMax][kCmbtSize];
  Int_t ntrk ; Float_t trkf[kJTrkMax][kTrkfSize]; 
               Float_t trkd[kJTrkMax][kTrkdSize];
               Int_t nvtx[kJTrkMax];
  Int_t nemh ; Int_t emh[kJEClsMax][kJClsSize];
  Int_t nhdh ; Int_t hdh[kJHClsMax][kJClsSize];
  Int_t nsih ; Float_t vtxd[kJVTXBufSize][kVTXHSize] ; 
  Int_t idvtx[kJVTXBufSize][kVTXIDSize];
} COMMON_TRBUFF;

extern COMMON_TRBUFF trbuff_;

#endif
