#ifndef __JSFEnvGUIFrame__
#define __JSFEnvGUIFrame__
//
//$Id$
//
#include <stdlib.h>

#include <TROOT.h>
#include <TApplication.h>
#include <TGXW.h>

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
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>
#include <TSystem.h>

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

