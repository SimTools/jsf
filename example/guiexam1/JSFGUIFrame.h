//*LastUpdate:  jsf-1-13 25-January-2000  by Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  09/03/1999

//////////////////////////////////////////////////////////////////
//
//  JSFGUIFrame
//
//  For JSF Control Panel GUI
//
//$Id$
//
//////////////////////////////////////////////////////////////////

#ifndef __JSFGUIFrame__
#define __JSFGUIFrame__

#include "JSFConfig.h"

#include <stdlib.h>

#include <TROOT.h>
#include <TApplication.h>
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
#include <TGTab.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>
#include <TRandom.h>
#include <TSystem.h>

#ifndef __JSFEnv__
#include "JSFEnv.h"
#endif

#ifndef __JSFSteer__
#include "JSFSteer.h"
#endif


#ifndef __JSFDemoDisplay__
#include "JSFDemoDisplay.h"
#endif

#ifndef __JSFEventDisplay__
#include "JSFEventDisplay.h"
#endif

#ifndef __JSFGUIAnalysis__
#include "JSFGUIAnalysis.h"
#endif

enum EJSFGUIEventType { kPythia=0, kDebug=1, kBasesSpring=2,
                        kReadParton=3, kReadHepevt=4 };

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

   TGPopupMenu        *fMenuRunMode, *fMenuControl;
   TGPopupMenu        *fMenuGen, *fMenuGenType, *fMenuSimType;
   TGPopupMenu        *fMenuGInfo[6], *fMenuGenPara;
   // Int_t fRunMode; // =1, Gen Event, =2 Read File, =3 Read SIMDST
   TGPopupMenu        *fMenuUser;
   TGPopupMenu        *fPythiaEvtype;

   TGCompositeFrame   *fStartAnal;
   TGTextEntry        *fTFromEvent, *fTToEvent;
   TGTextButton       *fBStartAnal;
   TGLabel            *fLSAFromEvent, *fLSAToEvent;

   JSFEventDisplay *fED;
   JSFDemoDisplay  *fDemo;
   JSFGUIAnalysis  *fAnal;
   Bool_t           fInitialized;

   Char_t fMacroInit[64];
   Char_t fMacroGetNext[64];
   Char_t fMacroGetPrev[64];

   Bool_t fShowHist;
   Int_t  fShowHistFrequency;
   Int_t  fReturnCode;
   Int_t  fNoOfAnalizedEvents;
   Bool_t fTextEntryStatus[3];

   TGTextBuffer *fTB1;

   void DoButtonAction(Long_t parm1);
   void DoFileMenuAction(Long_t parm1, Bool_t prompt=kTRUE);
   void DoDispMenuAction(Long_t parm1, Bool_t prompt=kTRUE);
   void DoAnalMenuAction(Long_t parm1, Bool_t prompt=kTRUE);
   void DoRunmodeMenuAction(Long_t parm1, Bool_t prompt=kTRUE);

public:
   JSFGUIFrame(const TGWindow *p, UInt_t w, UInt_t h, Bool_t demo=kFALSE,
	       JSFEventDisplay *disp=NULL);
   virtual ~JSFGUIFrame();
   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
   void ToRelativePath(const Char_t *fnin,const Char_t *dirnow, Char_t *fnout);

   void Update();

   void DisplayEventData();
   void DrawHist();

   void GotoEventAction();
   void AnalizeEventAction();
   void RunDemo();

   JSFEventDisplay *GetEventDisplay(){ return fED; }

   TGPopupMenu *GetUserMenu(){ return fMenuUser; }

   inline const Char_t *GetInputFileName(){ 
     return gJSF->Env()->GetValue("JSFGUI.InputFileName",""); }
   inline const Char_t *GetOutputFileName(){ 
     return gJSF->Env()->GetValue("JSFGUI.OutputFileName","jsf.root"); }
   inline Int_t GetRunMode(){ 
     return gJSF->Env()->GetValue("JSFGUI.RunMode",1);}
   inline Int_t GetEventType(){ 
     return gJSF->Env()->GetValue("JSFGUI.EventType",kPythia);}
   inline Float_t GetEcm(){ 
     return atof(gJSF->Env()->GetValue("JSFGUI.ECM","300.0")); }
   inline const Char_t *GetInitPythiaMacro(){ 
     return gJSF->Env()->GetValue("JSFGUI.InitPythiaMacro","InitPythia.C");}
   inline const Char_t *GetMacroFileName(){ 
     return gJSF->Env()->GetValue("JSFGUI.MacroFileName","UserAnalysis.C");}
   inline Int_t GetFirstEvent(){ 
     return gJSF->Env()->GetValue("JSFGUI.FirstEvent",1);}
   inline Int_t GetNEventsAnalize(){ 
     return gJSF->Env()->GetValue("JSFGUI.NEventsAnalize",10);}

   void SetEcm(Float_t ecm){ 
     Char_t str[24];       sprintf(str,"%g",ecm);
     gJSF->Env()->SetValue("JSFGUI.ECM",str);}
   void SetRunMode(Int_t runmode){ 
     Char_t str[12];       sprintf(str,"%d",runmode);
     gJSF->Env()->SetValue("JSFGUI.RunMode",str);}
   void SetEventType(Int_t eventtype){ 
     Char_t str[12];       sprintf(str,"%d",eventtype);
     gJSF->Env()->SetValue("JSFGUI.EventType",str);}
   void SetFirstEvent(Int_t n){ 
     Char_t str[12];       sprintf(str,"%d",n);
     gJSF->Env()->SetValue("JSFGUI.FirstEvent",str);}
   void SetNEventsAnalize(Int_t n){ 
     Char_t str[12];       sprintf(str,"%d",n);
     gJSF->Env()->SetValue("JSFGUI.NEventsAnalize",str);}
   void SetReturnCode(Int_t ir){ fReturnCode=ir;}

   Bool_t GetShowHist(){ return fShowHist; }
   Int_t  GetShowHistFrequency(){ return fShowHistFrequency; }
   Int_t  GetNoOfAnalizedEvents(){ return fNoOfAnalizedEvents; }

   ClassDef(JSFGUIFrame, 0)   // A class to GUI
   
};


#endif

