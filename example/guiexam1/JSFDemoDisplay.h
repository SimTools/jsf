//*LastUpdate:  jsf-1-12 22-August-1999  by Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  22-August-1999

//////////////////////////////////////////////////////////////////
//
//  JSFDemoDisplay
//
//  Display event and histogram for demo
//  
//$Id$
//  
//////////////////////////////////////////////////////////////////

#include <TRootEmbeddedCanvas.h>
#include <TTimer.h>

#ifndef __JSFDemoDisplay__
#define __JSFDemoDisplay__


#ifndef __JSFEventDisplay__
#include "JSFEventDisplay.h"
#endif

// **************************************************************
class JSFDemoDisplay : public JSFEventDisplay {
friend class JSFGUIFrame ;
private:

  TDirectory *fCanvasDirectory;

  TGMainFrame *fMain;
  TRootEmbeddedCanvas *fDisp, *fHist1, *fHist2, *fHist3;
  TGCompositeFrame *fCont, *fUpbuttons, *fDownbuttons;
  TGTextButton *fButtons[8];

  TCanvas *fCanvas;
  TView   *fView;
  TList   *fWidgets;
  TGIcon  *fLogo;

  TTimer *fTimer;
  Int_t fNCall;


public:
  JSFDemoDisplay(JSFGUIFrame *gui);
  virtual ~JSFDemoDisplay();

  void DisplayDemo();
  void InitializeDemo();

  void SetTimer();
  TTimer *GetTimer(){ return fTimer;}
  Bool_t HandleTimer(TTimer *timer=NULL);
  void ProcessButton(Long_t parm1);

  const Char_t *GetEventTypeString();

  ClassDef(JSFDemoDisplay, 1)   // A class to display event

};

#endif
