#ifndef __JSFGUIFrame__
#define __JSFGUIFrame__
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
#include <TEnv.h>

#ifndef __JSFEventDisplay__
#include "JSFEventDisplay.h"
#endif

#ifndef __JSFGUIAnalysis__
#include "JSFGUIAnalysis.h"
#endif

enum EventType { kPythia=0, kDebug=1};


class JSFGUIFrame : public TGMainFrame {

private:

   TGCompositeFrame  *fBeginFrame;
   TGButton          *fBBegin;

   TGCompositeFrame   *fEventGoto;
   TGTextEntry        *fTGotoEvent;
   TGTextButton       *fBGotoEvent;

   TGCompositeFrame   *fFileFrame,  *fFileOFrame;
   TGLabel            *fLFile, *fLFileN, *fLOFile, *fLOFileN;

   TGCompositeFrame   *fEventFrame;
   TGButton           *fBNextEvent, *fBPreviousEvent;
   TGLabel            *fLEventNumber;
   
   TGCompositeFrame   *fEventOptions;
   TGCheckButton      *fCheckDisplay;

   TGMenuBar          *fMenuBar;
   TGPopupMenu        *fMenuFile, *fMenuHelp;
   TGPopupMenu        *fMenuDisplay, *fMenuAnal;
   TGPopupMenu        *fMenuOnOffSignals;
   TGLayoutHints      *fMenuBarLayout, *fMenuBarItemLayout, *fMenuBarHelpLayout;

   TGPopupMenu        *fMenuGen, *fMenuGenType;

   Int_t fRunMode; // =1, Gen Event, =2 Read File, =3 Read SIMDST


   TGCompositeFrame   *fStartAnal;
   TGTextEntry        *fTFromEvent, *fTToEvent;
   TGTextButton       *fBStartAnal;
   TGLabel            *fLSAFromEvent, *fLSAToEvent;

   Char_t fInputFileName[256];
   Char_t fOutputFileName[256];

   JSFEventDisplay *fED;
   JSFGUIAnalysis  *fAnal;
   Bool_t           fInitialized;

   Char_t fMacroInit[64];
   Char_t fMacroGetNext[64];
   Char_t fMacroGetPrev[64];

   Char_t fMacroFileName[256];
   Bool_t fShowHist;
   Int_t  fShowHistFrequency;
   Int_t  fFirstEvent;
   Int_t  fNEventsAnalize;
   Int_t  fReturnCode;
   Int_t  fNoOfAnalizedEvents;


   Char_t fInitPythiaMacro[256];
   Float_t fEcm;
   Int_t   fEventType;

public:
   JSFGUIFrame(const TGWindow *p, UInt_t w, UInt_t h);
   virtual ~JSFGUIFrame();
   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
   void ToRelativePath(const Char_t *fnin,const Char_t *dirnow, Char_t *fnout);

   Char_t *GetInputFileName(){ return fInputFileName; }
   Char_t *GetOutputFileName(){ return fOutputFileName; }
   Int_t GetRunMode(){ return fRunMode; }
   Int_t GetEventType(){ return fEventType; }
   Float_t GetEcm(){ return fEcm;}
   Char_t *GetInitPythiaMacro(){ return fInitPythiaMacro; }
   


   void DisplayEventData();
   void DrawHist();
   JSFEventDisplay *GetEventDisplay(){ return fED; }

   void GotoEventAction();
   void AnalizeEventAction();

   Char_t *GetMacroFileName(){ return fMacroFileName; }
   void SetFirstEvent(Int_t n){ fFirstEvent=n; }
   void SetNEventsAnalize(Int_t n){ fNEventsAnalize=n; }
   Int_t GetFirstEvent(){ return fFirstEvent; }
   Int_t GetNEventsAnalize(){ return fNEventsAnalize; }
   void SetReturnCode(Int_t ir){ fReturnCode=ir;}

};


#endif

