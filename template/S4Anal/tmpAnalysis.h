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

<<+S4Anal>>
class TNtuple;
<<-S4Anal>>

// *******************************************************
class <<Analysis>> : public JSFModule 
{
 protected:
<<+S4Anal>>
  TNtuple *fNtevent;  //! Event basis ntuple
  TNtuple *fNthits;   //! Hit basis ntuple
<<-S4Anal>>
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










