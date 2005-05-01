#ifndef __MyAnalysis__
#define __MyAnalysis__

///////////////////////////////////////////////////////////////////////
//
//  MyAnalysis
//
//  Sample program
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include "JSFModule.h"

#include "TNtuple.h"

// *******************************************************
class MyAnalysis : public JSFModule 
{
 protected:
  Int_t   fFlag;      // An example of Int_t parameter
  Float_t fParameter; // An example of Float_t parameter
  TNtuple *fNtuple;   //

 public:
  MyAnalysis(const char *name="MyAnalysis", 
		  const char *title="MyAnalysis");
  virtual ~MyAnalysis();

  virtual Bool_t Initialize();
//  virtual Bool_t BeginRun(Int_t runno=1);
//  virtual Bool_t EndRun();
//  virtual Bool_t Terminate();
  virtual Bool_t Process(Int_t ev=1);
  
  ClassDef(MyAnalysis, 1)  // MyAnalysis class

};

#endif










