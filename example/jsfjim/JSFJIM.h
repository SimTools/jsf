#ifndef __JSFJIM__
#define __JSFJIM__

///////////////////////////////////////////////////////////////////////
//
//  A class to run JIM as JSF module
//
//$Id$ 
//
//////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include "JSFReadJIMBank.h"
#include "JSFModule.h"

// *************************************************************
class JSFJIM : public JSFSIMDST {
protected:
  Char_t fInputFile[256]; //! File name where to read JIM parameter.
  Int_t fUnit; //! Fortran logical unit number to read JIM input
public:
  JSFJIM(const char *name="JSFJIM", 
		  const char *title="Run JIM in JSF",
                 Bool_t constbuf=kTRUE);
  virtual ~JSFJIM(){}

  virtual Bool_t Initialize();
  virtual Bool_t Process(Int_t ev=1);
  virtual Bool_t BeginRun(Int_t nrun=1);
  virtual Bool_t EndRun();
  virtual Bool_t Terminate();

  void SetInput(Char_t *file){ strcpy(fInputFile,file); }

  // Interface functions to get JIM result
  void KZGET(Char_t *cname, Int_t iseg, Int_t &leng, Int_t idat[]);  

//  virtual JSFJIMParam *Param(){ return fParam; }
//  JSFQuickSimParam *Param(){ return (JSFQuickSimPara.cxxm*)fParam;}
//  void SetJIMParam(JSFJIMParam *par){ fParam=par; }

  ClassDef(JSFJIM, 1)  // Run JIM

};

#endif


