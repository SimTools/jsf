// #include "geant321/gcbank.inc"
// #include "geant321/gcflag.inc"
// #include "geant321/gcunit.inc"
// #include "geant321/gctime.inc"

const Int_t kwbank=69000;

struct typedef {
  Int_t nzebra;
  Float_t gversn,zversn;
  Int_t ixstor,ixdiv,ixcons;
  Float_t fendq[16];
  Int_t lmain,lr1;
  Int_t ws[kwbank];
} COMMON_GCBANK;

extern COMMON_GCBANK gcbank_;

struct typedef {
  Int_t jdigi ,jdraw ,jhead ,jhits ,jkine ,jmate ,jpart;
  Int_t jrotm ,jrung ,jset  ,jstak ,jgstat,jtmed ,jtrack,jvertx;
  Int_t jvolum,jxyz  ,jgpar ,jgpar2,jsklt 
} COMMON_GCLINK;

extern COMMON_GCLINK gclink_;

struct typedef {
  Int_t idebug,idemin,idemax,itest,idrun,idevt,ieorun;
  Int_t ieotri,ievent,iswit[10],ifinit[20],nevent,nrndm[2];
} COMMON_GCFLAG;

struct typedef {
  Bool_t batch, nolog;
} COMMON_GCFLAX ;

extern COMMON_GCFLAX gcflax_;
extern COMMON_GCFLAG gcflag_;  


struct typedef {
  Int_t lin,lout,nunits,lunits[5];
} COMMON_GCUNIT;

extern COMMON_GCUNIT gcunit_;

struct typedef {
  Char_t chmail[132];
} COMMON_GCMAIN;
extern COMMON_GCMAIN gcmain_;

struct typedef {
  Float_t timint,timend;
  Int_t itime,igdate,igtime;
} COMMON_GCTIME;

extern COMMON_GCTIME gctime_;




