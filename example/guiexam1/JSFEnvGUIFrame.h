//*LastUpdate:  jsf-1-13 25-January-2000  by Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  26/08/1999

//////////////////////////////////////////////////////////////////
//
//  JSFEnvGUIFrame
//
//  To Get Env prameter from GUI panel.
//
//$Id$
//
//////////////////////////////////////////////////////////////////

#ifndef __JSFEnvGUIFrame__
#define __JSFEnvGUIFrame__

#include "JSFConfig.h"

#include <stdlib.h>

#include <TROOT.h>
#include <TApplication.h>
#if __ROOT_VERSION__ >= 2 && __ROOT_MINORVERSION__ >= 23
#include <TVirtualX.h>
#else
#include <TGXW.h>
#endif

#include <TGButton.h>
#include <TGListBox.h>
#include "JSFEnv.h"


//_______________________________________________
class JSFEnvGUIFrame : public TGMainFrame {

private:
   TGCompositeFrame *fCFBottom;
   TGTextButton *fTBBottom[3];
   TOrdCollection *fParam;

   TGCompositeFrame *fCFSelect;
   TGTextButton  *fTBSelect[2];

   TGCompositeFrame *fListFrame;
   TGListBox *fListBox;
   TGLayoutHints *fL3;
   
   TList *fLLabels; // List of parameter name label.
   TList *fLMenus;  // List of parameter menu.

public:
   JSFEnvGUIFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~JSFEnvGUIFrame();
   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);

   ClassDef(JSFEnvGUIFrame,1) // JSFEnv GUI class

};


#endif

