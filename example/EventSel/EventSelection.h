#ifndef __EventSelection__
#define __EventSelection___

///////////////////////////////////////////////////////////////////////
//
//  EventSelection
//
//  Sample program
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include "JSFModule.h"


// *******************************************************
class EventSelection : public JSFModule 
{
 protected:
  Int_t fNcall;
  Int_t fNgood;
 public:
  EventSelection(const char *name="EventSelection", 
		  const char *title="EventSelection");
  virtual ~EventSelection();

  virtual Bool_t Initialize();
  virtual Bool_t Process(Int_t ev=1);
  virtual Bool_t Terminate();
  
  ClassDef(EventSelection, 1)  // EventSelection class

};

#endif










