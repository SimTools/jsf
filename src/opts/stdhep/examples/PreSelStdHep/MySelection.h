#ifndef __MySelection__
#define __MySelection__

///////////////////////////////////////////////////////////////////////
//
//  MySelection
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
class MySelection : public JSFModule 
{
 protected:
  TNtuple *fNtuple;   //
  Bool_t fWriteBeginRun; //!  
 public:
  MySelection(const char *name="MySelection", 
		  const char *title="MySelection");
  virtual ~MySelection();

  virtual Bool_t Initialize();
//  virtual Bool_t BeginRun(Int_t runno=1);
//  virtual Bool_t EndRun();
//  virtual Bool_t Terminate()
  void MakeBeginRunInfo();
  virtual Bool_t Process(Int_t ev=1);
  
  ClassDef(MySelection, 1)  // MySelection class

};

#endif










