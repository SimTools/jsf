#ifndef __<<Analysis>>__
#define __<<Analysis>>__

///////////////////////////////////////////////////////////////////////
//
//  <<Analysis>>
//
//  <<Comment>>
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include "JSFModule.h"

class <<Analysis>>Buf : public JSFEventBuf 
{
protected:

public:
  <<Analysis>>Buf(const char *name="<<Analysis>>Buf", 
	     const char *title="<<Analysis>> buffer",
		  JSFModule *module=NULL);
  virtual ~<<Analysis>>Buf();

  // add your own function here

  ClassDef(<<Analysis>>Buf,1)  // <<Analysis>> event data buffer
};



// *******************************************************
class <<Analysis>> : public JSFModule 
{
 protected:
  Int_t   fFlag;      // An example of Int_t parameter
  Float_t fParameter; // An example of Float_t parameter

 public:
  <<Analysis>>(const char *name="<<Analysis>>", 
		  const char *title="<<Analysis>>");
  virtual ~<<Analysis>>();

  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t runno=1);
  virtual Bool_t EndRun();
  virtual Bool_t Terminate();
  virtual Bool_t Process(Int_t ev=1);
  
  ClassDef(<<Analysis>>, 1)  // <<Analysis>> class

};

#endif










