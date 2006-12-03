#ifndef __JSFLCFULL__
#define __JSFLCFULL__

///////////////////////////////////////////////////////////////////////
//
//  JSF Interface to execute lcfull modules.
//(function)
//  Execute main part of LCFULL
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TTree.h>
#include <TClonesArray.h>

#include "JSFConfig.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif

// ------------------------------------------------------------
// Boddy of Class definition is here

class JSFLCFULL : public JSFModule  {

public:
  Int_t fRecid;  //! Record ID.
#ifdef __LCLIBRAN_USE_RANMAR__
  UInt_t fRandomSeed[208]; // Seed of random variables
#endif
public:
  JSFLCFULL(const char *name="JSFLCFULL"  , 
	    const char *title="JSFLCFULL");
  virtual ~JSFLCFULL();

  virtual Bool_t Initialize();
  virtual Bool_t Process(Int_t ev);
  virtual Bool_t BeginRun(Int_t nrun);
#ifdef __LCLIBRAN_USE_RANMAR__
  virtual Bool_t EndRun();
  virtual Bool_t GetLastRunInfo();
#endif

  void SetNumgen(Int_t ng);

  void TBGET( Int_t idrec, const Char_t *bankname, Int_t elem,
	      Int_t &nw, Int_t data[], Int_t &iret);
  void TBGET( Int_t idrec, const Char_t *bankname, Int_t elem,
	      Int_t &nw, Float_t data[], Int_t &iret);
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
  void TBINIT(Int_t id, Int_t &iret);


  void JCLUST(Int_t numprt, Float_t *prtlst, const Int_t lngdat,
	       const Int_t ipx0, const Int_t maxcut, 
	       Float_t ycut[], Int_t numcls[], Int_t numjet[],
	       Int_t jettbl[], Float_t *jetlst, Int_t &status);

  void SPHER(Int_t numprt, Float_t *prtlst, const Int_t lngdat,
	      const Int_t ipx0, 
	      Float_t &sph, Float_t &apl, Float_t eigvct[3][4]);

  void THRUST(Int_t numprt, Float_t *prtlst, const Int_t lngdat,
	      const Int_t ipx0, 
	      Float_t &thr, Float_t &obl, Float_t eigvct[3][4]);

  void EPROBX(Double_t x, const Int_t ntyp, Double_t &xbeam);


#ifdef __LCLIBRAN_USE_RANMAR__
  void SetRandomSeed(UInt_t rdat[208]);
  void GetRandomSeed(UInt_t rdat[208]);
  void WriteRandomSeed(Char_t *fw="");
  void ReadRandomSeed(Char_t *fr="");
  void PrintRandomSeed(Int_t num=8);  // First num's data are printed.
  ClassDef(JSFLCFULL, 2)  // JSF LCFULL interface
#else
  ClassDef(JSFLCFULL, 1)  // JSF LCFULL interface
#endif

};

R__EXTERN JSFLCFULL *gJSFLCFULL;

#endif


