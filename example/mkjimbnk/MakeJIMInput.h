#ifndef __MakeJIMInput__
#define __MakeJIMInput___

///////////////////////////////////////////////////////////////////////
//
//  MakeJIMInput
//
//  Create ZBS format data for JIM simulator
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include "JSFModule.h"


// *******************************************************
class MakeJIMInput : public JSFModule 
{
 protected:
  Char_t fOutputFile[256];
  Int_t  fLunit;
  Int_t  fFirstEvent;
 public:
  MakeJIMInput(const char *name="MakeJIMInput", 
		  const char *title="MakeJIMInput");
  virtual ~MakeJIMInput();

  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t runno=1);
  virtual Bool_t EndRun();
  virtual Bool_t Terminate();
  virtual Bool_t Process(Int_t ev=1);
  
  ClassDef(MakeJIMInput, 1)  // MakeJIMInput class

};

#endif










