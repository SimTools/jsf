#ifndef __JSFReadJIMBank__
#define __JSFReadJIMBank__

///////////////////////////////////////////////////////////////////////
//
//  Read JIM's output, bnk file, and put them into JSFSIMDST class
//
//$Id: 
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>

#ifndef __JSFSIMDST__
#include "JSFSIMDST.h"
#endif
#ifndef __JSFJIMParam__
#include "JSFJIMParam.h"
#endif

// *************************************************************
class JSFReadJIMBankBuf : public JSFSIMDSTBuf {

public:
  JSFReadJIMBankBuf(const char *name="JSFReadJIMBankBuf", 
	     const char *title="Read JIM bnk file",
  	     JSFSIMDST *module=NULL);
  virtual ~JSFReadJIMBankBuf();

//  virtual void SetClassData(Int_t nev);       // Set Class Data
//  virtual Bool_t PackDST(Int_t nev); // Pack SIMDST information.
  virtual Bool_t UnpackDST(Int_t nev); // Unpack SIMDST information.
  friend class JSFReadJIMBank;

  ClassDef(JSFReadJIMBankBuf,1)  // Buffer to read JIM bank
};

// *************************************************************
class JSFReadJIMBank : public JSFSIMDST {
protected:
  JSFJIMParam *fParam;
  Bool_t      fDoesParameterInitialized; //!
public:
  JSFReadJIMBank(const char *name="JSFReadJIMBank", 
		  const char *title="Read JIM's *.bnk file",
                 Bool_t constbuf=kTRUE);
  virtual ~JSFReadJIMBank();

  friend class JSFReadJIMBankBuf;

  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t nrun=0);
  virtual Bool_t Process(Int_t ev=1);
  virtual Bool_t EndRun();


  virtual JSFJIMParam *Param(){ return fParam; }
//  JSFQuickSimParam *Param(){ return (JSFQuickSimParam*)fParam;}

  void SetJIMParam(JSFJIMParam *par){ fParam=par; }

  ClassDef(JSFReadJIMBank, 1)  // Read JIM Bank file.

};

#endif


