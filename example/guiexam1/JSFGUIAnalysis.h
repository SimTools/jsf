//*LastUpdate:  jsf-1-5 23-Feburary-1999  by Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  22/02/1999

//////////////////////////////////////////////////////////////////
//
//  JSFGUIAnalysis
//
//  GUI Interface to user analysis
//
//$Id$
//  
//////////////////////////////////////////////////////////////////

#ifndef __JSFGUIAnalysis__
#define __JSFGUIAnalysis__

#include "JSFConfig.h"

#include <TROOT.h>
#include <TCanvas.h>
#include <TView.h>
#if __ROOT_FULLVERSION__ >= 22300
#include <TVirtualX.h>
#else
#include <TGXW.h>
#endif

#include <TGListBox.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>


// **************************************************************
class JSFGUIAnalysis : public TGTransientFrame {
public: 

  TGVerticalFrame *fFrame;
  TGHorizontalFrame *fBottomFrame;
  TGButton         *fButtonDismiss, *fButtonRedraw;
  TGCheckButton    *fCheckDraw;
  TGLayoutHints    *fLHBDismiss;

public:
  JSFGUIAnalysis(const TGWindow *p, const TGWindow *main, UInt_t w=400, UInt_t h=200,
               UInt_t options = kMainFrame | kVerticalFrame);
   virtual ~JSFGUIAnalysis(){};
//   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

};



#endif
