#ifndef __JSFJIMEventDisplay__
#define __JSFJIMEventDisplay__
 
//////////////////////////////////////////////////////////////////////////
//
// JSFJIMEventDisplay
//
// Event Display for JIM data
//
//$Id:
//
//////////////////////////////////////////////////////////////////////////

#ifndef __JSFEventDisplay__
#include "JSFEventDisplay.h"
#endif

// ******* JSFJIMEventDisplay *************************************
class JSFJIMEventDisplay : public JSFEventDisplay
{
 public:
  JSFJIMEventDisplay(JSFGUIFrame *gui=NULL){}
  virtual ~JSFJIMEventDisplay(){}

  virtual void DisplayEMCHits();
  virtual void DisplayHDCHits();

  ClassDef(JSFJIMEventDisplay, 1) // JSFJIMEventDisplay
};
 
#endif

