#ifndef __JSFLCFULL__
#define __JSFLCFULL__

///////////////////////////////////////////////////////////////////////
//
//  JSF Interface to execute lcfull modules.
//(function)
//  Execute main part of LCFULL
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TTree.h>
#include <TClonesArray.h>

#ifndef __JSFModule__
#include "JSFModule.h"
#endif

extern "C" {
extern void prinit_root_();
extern void prjobi_();
extern void prjobt_();
extern void prgevt_(int *idrec);
extern void pruhed_(int *idrec);

extern void tbget_(int *id, const char *bankname, int *elem, int *nw, 
		   int data[], int *iret, int len1);
extern void tbnoel_(int *id, const char *bankname, int *nelm, 
		    int  nary[], int len);
extern void tbput_(int *id, const char *bname, int *ne, int *nw,
		   int data[], int *iret, int len);
extern void tbcrte_(int *id, const char *bname, int *isaloc, 
		    int *isext, int *iret, int len);
extern void tbdelb_(int *id, const char *bname, int *iret, int len);
extern void tblbnm_();
extern void tblist_(int *id);
extern void tblsub_();
extern void tblsum_();
};

typedef struct {
  Int_t rtimlm, rcpumx,
        nevmax, ndfreq, ievseq,
        nfstev, nlstev, nfstdb, nlstdb, nfstot, nlstot,
        kllvlr, kllvlp,
        nfstex, nlstex, nfstrn, nlstrn;
} COMMON_PRJCTL;

typedef struct {
  Int_t nrdrec,nwtrec;
  Int_t nstrec[10][6];
  Int_t mrdrec,mwtrec;
  Int_t mstrec[10][6];
} COMMON_PRRSTA;

typedef struct {
  Int_t nctbgn, nctend;
  Int_t nstevt[64][5];
  Int_t mctbgn, mctend;
  Int_t mstevt[64][5];
} COMMON_PRESTA ;

typedef struct {
  Int_t xtword, xerror, xrectp, xexpno, xrunno,
        xevtno, xdate,  xtime,  xtdacu, xruntp,
        xcrew , xfmerr, xevtid, xdummy[3],
        xerrs1, xerrs2;
} COMMON_PRHEAD;

typedef struct {
  Int_t njblvl[3][30], njbdbg[3][30];
} COMMON_PRJFLG;

typedef struct {
  Int_t ninuni, ninfil, iinfil,
        notuni, notfil, iotfil, notdev;
  Float_t xosize, zosize;
} COMMON_PRFCTL;

typedef struct {
  Int_t numgen, iumgen;
} COMMON_PRGENF;

/*
class COMMON_PRGENF {
 public:
  Int_t numgen;   //! Number of events to be generated.
  Int_t iumgen;   //! Number of events generated so far.
};
*/

typedef struct {
  Int_t ndynat, nowexp, nowrun, nxtfil, nowpnt,
        numdyn, nrndyn[50][3],
        nfinfo[50][4], nofall, nofinf[50][5];
} COMMON_PRDYNA;


extern COMMON_PRJCTL prjctl_;
extern COMMON_PRRSTA prrsta_;
extern COMMON_PRESTA presta_;
extern COMMON_PRHEAD prhead_;
extern COMMON_PRJFLG prjflg_;
extern COMMON_PRFCTL prfctl_;
extern COMMON_PRGENF prgenf_;
extern COMMON_PRDYNA prdyna_;


// ------------------------------------------------------------
// Boddy of Class definition is here

class JSFLCFULL : public JSFModule  {

public:
  Int_t fRecid;  //! Record ID.

public:
  JSFLCFULL(const char *name="JSFLCFULL"  , 
	    const char *title="JSFLCFULL");
  virtual ~JSFLCFULL();

  virtual Bool_t Initialize();
  virtual Bool_t Process(Int_t ev);
  virtual Bool_t BeginRun(Int_t nrun);
//virtual void Clear(Option_t *option="");
//virtual void Terminate();

//  COMMON_PRGENF *PRGENF();
//  COMMON_PRDYNA *PRDYNA();

  void SetNumgen(Int_t ng){ 
    prgenf_.numgen=ng ; prgenf_.iumgen=0 ; 
    prdyna_.ndynat=-1 ; return ; }

  void TBGET( Int_t idrec, const Char_t *bankname, Int_t elem,
	      Int_t &nw, Int_t data[], Int_t &iret);
  void TBNOEL( Int_t idrec, const Char_t *bankname, Int_t &nelm,
	      Int_t neary[]);
  void TBCRTE( Int_t id, const Char_t *bname, Int_t isaloc, 
	       Int_t isext, Int_t &iret);
  void TBPUT( Int_t id, const Char_t *bname, Int_t ne, Int_t nw,
	       Int_t data[], Int_t &iret);
  void TBDELB( Int_t id, const Char_t *bname, Int_t &iret);

  void MakeBranch(TTree *tree){}
  void SetBranch(TTree *tree){}

  void TBINFO();
  void TBINFO(const Char_t * option);

  ClassDef(JSFLCFULL, 1)  // JSF LCFULL interface

};

EXTERN JSFLCFULL *gJSFLCFULL;

#endif


