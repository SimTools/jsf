//*LastUpdate :  jsf-1-8  19-April-1999  By Akiya Miyamoto
//*LastUpdate :  jsf-1-5  20-Feburary-1999  By Akiya Miyamoto
//*-- Author  : A.Miyamoto  20-September-1998

/*
  19-Apr-1999 A.Miyamoto  A bug in JSFGUIFrame::ToRelativePath is fixed.
  1-May-1999 A.Miyamoto   Add Start borwser and Open JIM file menu
  16-May-1999 A.Miyamoto  Does not call GetArguments macro.
                          It is now called from MainMacro.C
*/

///////////////////////////////////////////////////////////////////
//
// JSFGUIFrame
//
// GUI Class to control JSF execution, mainly for event display.
//
// Following macros are called 
//   Initialize() when Initialize button is pressed.
//   GetNext()    when Next button is pressed.
//   GetPrevious() when Previous button is pressed.
//   GetEvent(Int_t event) when Goto event button is pressed.
//   
//$Id$
//
///////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include <TROOT.h>
#include <TApplication.h>
#include <TGXW.h>

#include <TBrowser.h>

#include <TGListBox.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGWidget.h>
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

#include "JSFSteer.h"
#include "JSFGUIFrame.h"
#include "InputDialog.h"

TBrowser *gbrows;

enum ETestCommandIdentifiers {
   M_FILE_GENEVENT=101,
   M_FILE_READROOT=102,
   M_FILE_READSIMDST=103,
   M_FILE_READJIM=104,

   M_FILE_OPEN=105,
   M_FILE_OPENOUTPUT=106,
   M_FILE_SAVE=107,
   M_FILE_SAVEAS=108,

   M_FILE_STARTBROWSER=150,
   M_FILE_EXIT=151,
 
   M_HELP_CONTENTS=200,
   M_HELP_SEARCH=201,
   M_HELP_ABOUT=203,
   M_HELP_EVENTDISP=204,

   M_DISP_REDRAW=300,
   M_DISP_MOMENTUM=301,
   M_DISP_ALL=302,
   M_DISP_VTX=303,
   M_DISP_EVENT=310,

   M_ONOFF_GEOMETRY=320,
   M_ONOFF_GENNEUTRAL=321,
   M_ONOFF_GENTRACK=322,
   M_ONOFF_LTKCLTRACK=323,
   M_ONOFF_CDCTRACK=324,
   M_ONOFF_VTXHIT=325,
   M_ONOFF_EMCHIT=326,
   M_ONOFF_HDCHIT=327,


   M_ANAL_SHOWHIST=501,
   M_ANAL_SHOWFREQ=502,
   M_ANAL_RESETHIST=503,
   M_ANAL_DRAWHIST=504,
   M_ANAL_RELOAD=505,
   M_ANAL_MACRONAME=506,

   M_GEN_EVENTTYPE=701,
   M_GEN_ECM=702,
   M_GEN_INITPYTHIA=703,
   
   M_GEN_PYTHIA=721,
   M_GEN_DEBUG=722, 

   B_BEGIN=1,
   B_NEXT_EVENT=2,
   B_PREVIOUS_EVENT=3,
   B_GOTO_EVENT=4,

   B_START_ANAL=5,

   T_GOTO_EVENT,
   T_ANAL_START,
   T_ANAL_END

};

Int_t mb_button_id[9] = { kMBYes, kMBNo, kMBOk, kMBApply,
                          kMBRetry, kMBIgnore, kMBCancel,
                          kMBClose, kMBDismiss };

EMsgBoxIcon mb_icon[4] = { kMBIconStop, kMBIconQuestion,
                           kMBIconExclamation, kMBIconAsterisk };

const char *filetypes[] = { "ROOT files",    "*.root",
                            "SIMDST files",     "*.dat",
                            "JIM bnk",     "*.bnk",
                            "All files",     "*",
                            0,               0 };

///////////////////////////////////////////////////////////////////////

JSFGUIFrame::JSFGUIFrame(const TGWindow *p, UInt_t w, UInt_t h)
      : TGMainFrame(p, w, h)
{
   // Create test main frame. A TGMainFrame is a top level window.

   fED=new JSFEventDisplay();
   fInitialized=kFALSE;
   fNoOfAnalizedEvents=-999;
   gbrows=NULL;

   // Create menubar and popup menus. The hint objects are used to place
   // and group the different menu widgets with respect to eachother.
   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                                      0, 0, 1, 1);
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);

   fMenuFile = new TGPopupMenu(fClient->GetRoot());
   fMenuFile->AddLabel("Run Type and Files");
   fMenuFile->AddEntry("&Generate Event", M_FILE_GENEVENT);
   fMenuFile->AddEntry("&Read Root File", M_FILE_READROOT);
   fMenuFile->AddEntry("&Read SIMDST File", M_FILE_READSIMDST);
   fMenuFile->AddEntry("&Read JIM Bank File", M_FILE_READJIM);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("&Open Input...", M_FILE_OPEN);
   fMenuFile->AddEntry("&Open Output...", M_FILE_OPENOUTPUT);
   fMenuFile->AddEntry("&Start Browser...", M_FILE_STARTBROWSER);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("E&xit", M_FILE_EXIT);

   fRunMode=gJSF->Env()->GetValue("JSFGUI.RunMode",1);
   fMenuFile->CheckEntry(M_FILE_GENEVENT+fRunMode-1);

   fMenuOnOffSignals = new TGPopupMenu(fClient->GetRoot());
   fMenuOnOffSignals->AddLabel("On/Off ...");
   fMenuOnOffSignals->AddEntry("Geometry",M_ONOFF_GEOMETRY);
   fMenuOnOffSignals->AddEntry("Gen. Neutral particle",M_ONOFF_GENNEUTRAL);
   fMenuOnOffSignals->AddEntry("Gen. Charged particle",M_ONOFF_GENTRACK);
   fMenuOnOffSignals->AddEntry("LTKCL Tracks",M_ONOFF_LTKCLTRACK);
   fMenuOnOffSignals->AddEntry("CDC Tracks",M_ONOFF_CDCTRACK);
   fMenuOnOffSignals->AddEntry("VTX Hits",M_ONOFF_VTXHIT);
   fMenuOnOffSignals->AddEntry("EMC Hits",M_ONOFF_EMCHIT);
   fMenuOnOffSignals->AddEntry("HDC Hits",M_ONOFF_HDCHIT);

   if( fED->fDrawGeometry ) fMenuOnOffSignals->CheckEntry(M_ONOFF_GEOMETRY);
   if( fED->fGenNeutral->fShow ) fMenuOnOffSignals->CheckEntry(M_ONOFF_GENNEUTRAL);
   if( fED->fGenCharged->fShow ) fMenuOnOffSignals->CheckEntry(M_ONOFF_GENTRACK);
   if( fED->fLTKCLTrack->fShow ) fMenuOnOffSignals->CheckEntry(M_ONOFF_LTKCLTRACK);
   if( fED->fCDCTrack->fShow ) fMenuOnOffSignals->CheckEntry(M_ONOFF_CDCTRACK);
   if( fED->fVTXHit->fShow ) fMenuOnOffSignals->CheckEntry(M_ONOFF_VTXHIT);
   if( fED->fEMCHit->fShow ) fMenuOnOffSignals->CheckEntry(M_ONOFF_EMCHIT);
   if( fED->fHDCHit->fShow ) fMenuOnOffSignals->CheckEntry(M_ONOFF_HDCHIT);

   fMenuDisplay = new TGPopupMenu(fClient->GetRoot());
   fMenuDisplay->AddLabel("Event Display...");
   fMenuDisplay->AddSeparator();
   fMenuDisplay->AddEntry("Show Event", M_DISP_EVENT);
   fMenuDisplay->AddEntry("ReDraw", M_DISP_REDRAW);
   fMenuDisplay->AddSeparator();
   fMenuDisplay->AddEntry("Momentum View", M_DISP_MOMENTUM);
   fMenuDisplay->AddEntry("ALL View", M_DISP_ALL);
   fMenuDisplay->AddEntry("VTX View", M_DISP_VTX);
   fMenuDisplay->AddSeparator();
   fMenuDisplay->AddPopup("Show Signal...", fMenuOnOffSignals);

   if( fED->fDrawAtNewEvent) fMenuDisplay->CheckEntry(M_DISP_EVENT);
   fMenuDisplay->CheckEntry(fED->fDisplayType+301);
   
   // Analysis 
   fMenuAnal = new TGPopupMenu(fClient->GetRoot());
   fMenuAnal->AddLabel("User Analysis...");
   fMenuAnal->AddSeparator();
   fMenuAnal->AddEntry("Show Hist",M_ANAL_SHOWHIST);
   fMenuAnal->AddEntry("Show Frequency...",M_ANAL_SHOWFREQ);
   fMenuAnal->AddEntry("Draw Hist",M_ANAL_DRAWHIST);
   fMenuAnal->AddEntry("Reset Hist",M_ANAL_RESETHIST);
   fMenuAnal->AddSeparator();
   fMenuAnal->AddEntry("Reload Macro",M_ANAL_RELOAD);
   fMenuAnal->AddEntry("Macro Name...",M_ANAL_MACRONAME);

   sscanf(gJSF->Env()->GetValue("JSFGUI.MacroFileName","UserAnalysis.C"),
	  "%s",fMacroFileName);
   fShowHist=gJSF->Env()->GetValue("JSFGUI.ShowHist",1);
   fShowHistFrequency=gJSF->Env()->GetValue("JSFGUI.ShowHistFrequency",1);
   fFirstEvent=gJSF->Env()->GetValue("JSFGUI.FirstEvent",1);
   fNEventsAnalize=gJSF->Env()->GetValue("JSFGUI.NEventAnalize",10);
   if( fShowHist ) fMenuAnal->CheckEntry(M_ANAL_SHOWHIST);


   // Help menu
   fMenuHelp = new TGPopupMenu(fClient->GetRoot());
   fMenuHelp->AddEntry("&About", M_HELP_ABOUT);
   fMenuAnal->AddSeparator();
   fMenuHelp->AddEntry("&Event Display", M_HELP_EVENTDISP);

   
   fEventType=gJSF->Env()->GetValue("JSFGUI.EventType",kPythia);
   sscanf(gJSF->Env()->GetValue("JSFGUI.ECM","300.0"),"%g",&fEcm);
   sscanf(gJSF->Env()->GetValue("JSFGUI.InitPythiaMacro","InitPythia.C"),
	  "%s",fInitPythiaMacro);
   

   //  Generator Control menu
   fMenuGenType=new TGPopupMenu(fClient->GetRoot());
   fMenuGenType->AddLabel("Generator Type..");
   fMenuGenType->AddEntry("Pythia",M_GEN_PYTHIA);
   fMenuGenType->AddEntry("Debug",M_GEN_DEBUG);
   fMenuGenType->CheckEntry(M_GEN_PYTHIA+fEventType);

   fMenuGen=new TGPopupMenu(fClient->GetRoot());
   fMenuGen->AddLabel("Generator Conttrol");
   fMenuGen->AddPopup("Event Type",fMenuGenType);
   fMenuGen->AddEntry("CM Energy",M_GEN_ECM);
   fMenuGen->AddEntry("InitPythiaMacro",M_GEN_INITPYTHIA);

   // Menu button messages are handled by the main frame (i.e. "this")
   // ProcessMessage() method.
   fMenuFile->Associate(this);
   fMenuGen->Associate(this);
   fMenuDisplay->Associate(this);
   fMenuAnal->Associate(this);
   fMenuHelp->Associate(this);


   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
   // fMenuBar->AddPopup("&Options", fMenuOption, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Generator", fMenuGen, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Analysis", fMenuAnal, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Event Display", fMenuDisplay, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);

   AddFrame(fMenuBar, fMenuBarLayout);

   // Create frame to display file information.
   fFileFrame = new TGCompositeFrame(this, 2, 2, kHorizontalFrame);
   fLFile = new TGLabel(fFileFrame," Input File: ");
   fFileFrame->AddFrame(fLFile, 
			 new TGLayoutHints(kLHintsTop|kLHintsLeft,2,0,2,2));

   sscanf(gJSF->Env()->GetValue("JSFGUI.InputFileName",""),
	  "%s",fInputFileName);
   if( strlen(fInputFileName) >= 60 ){
     printf("Input file name, %s, is too long.\n",fInputFileName);
     printf("It is trancated when displayed.\n");
   }
   Char_t infn[101], infn2[101];
   Int_t  lfn2=strlen(fInputFileName);
   Int_t  lfn=TMath::Min(100,lfn2);
   
   strncpy(infn,fInputFileName,lfn);
   infn[lfn]=0;
   sprintf(infn2,"%-100s",infn);
   fLFileN = new TGLabel(fFileFrame,infn2);
   fLFileN->SetWidth(400);
   if( lfn2 <= 0 ) {
     fMenuFile->DisableEntry(M_FILE_READROOT);
     fMenuFile->DisableEntry(M_FILE_READSIMDST);
     fMenuFile->DisableEntry(M_FILE_READJIM);
   }

   fFileFrame->AddFrame(fLFileN, 
			 new TGLayoutHints(kLHintsTop|kLHintsLeft,2,0,2,2));
   AddFrame(fFileFrame, new TGLayoutHints(kLHintsTop|kLHintsExpandX,
					   2, 2, 1, 0) );


   // To display output file name

   fFileOFrame = new TGCompositeFrame(this, 2, 2, kHorizontalFrame);
   fLOFile = new TGLabel(fFileOFrame," Output File: ");
   fFileOFrame->AddFrame(fLOFile, 
			 new TGLayoutHints(kLHintsTop|kLHintsLeft,2,0,2,2));

   sscanf(gJSF->Env()->GetValue("JSFGUI.OutputFileName","jsf.root"),
	  "%s",fOutputFileName);
   if( strlen(fOutputFileName) >= 60 ){
     printf("Output file name, %s, is too long.\n",fOutputFileName);
     printf("It is trancated when displayed.\n");
   }
   lfn2=strlen(fOutputFileName);
   lfn=TMath::Min(100,lfn2);
   
   strncpy(infn,fOutputFileName,lfn);
   infn[lfn]=0;
   sprintf(infn2,"%-100s",infn);
   fLOFileN = new TGLabel(fFileOFrame,infn2);
   fLOFileN->SetWidth(400);
   //   if( lfn2 <= 0 ) {
   //  fMenuFile->DisableEntry(M_FILE_READROOT);
   //  fMenuFile->DisableEntry(M_FILE_READSIMDST);
   // }

   fFileOFrame->AddFrame(fLOFileN, 
			 new TGLayoutHints(kLHintsTop|kLHintsLeft,2,0,2,2));
   AddFrame(fFileOFrame, new TGLayoutHints(kLHintsTop|kLHintsExpandX,
					   2, 2, 1, 0) );

   //
   fBeginFrame = new TGCompositeFrame(this, 10, 10, kHorizontalFrame);
   fBBegin = new TGTextButton(fBeginFrame,"Initialize",B_BEGIN);
   fBBegin->Associate(this);
   fBeginFrame->AddFrame(fBBegin,
			 new TGLayoutHints(kLHintsTop|kLHintsLeft,2,0,2,2));
   AddFrame(fBeginFrame, new TGLayoutHints(kLHintsTop|kLHintsExpandX,
					   10, 10, 1, 0) );

   //
   fEventFrame = new TGCompositeFrame(this, 10, 10, kHorizontalFrame );

   fBNextEvent = new TGTextButton(fEventFrame, "Next Event", B_NEXT_EVENT);
   fBNextEvent->Associate(this);
   fEventFrame->AddFrame(fBNextEvent, 
			 new TGLayoutHints(kLHintsTop|kLHintsRight,2,0,2,2));

   fBPreviousEvent = new TGTextButton(fEventFrame, "Previous Event",
				                   B_PREVIOUS_EVENT);
   fBPreviousEvent->Associate(this);
   fEventFrame->AddFrame(fBPreviousEvent, 
			 new TGLayoutHints(kLHintsTop|kLHintsLeft,2,0,2,2));

   fLEventNumber = new TGLabel(fEventFrame, "  Event Number: 0         ");
   //   fLEventNumber->SetWidth(200);
   //   fLEventNumber->SetHeight(20);
   //   fLEventNumber->SetBackgroundColor(1);
   fEventFrame->AddFrame(fLEventNumber, 
			 new TGLayoutHints(kLHintsTop|kLHintsLeft,2,2,2,2));

   AddFrame(fEventFrame, new TGLayoutHints(kLHintsTop|kLHintsExpandX,
					   10, 10, 1, 0) );

   // Jump to the event
   fEventGoto  = new TGCompositeFrame(this, 60, 20, kHorizontalFrame);
   fBGotoEvent = new TGTextButton(fEventGoto,"Jump to Event No. ",B_GOTO_EVENT);
   fEventGoto->AddFrame(fBGotoEvent, 
         new TGLayoutHints(kLHintsTop|kLHintsLeft, 4, 4, 2, 2) );
   fBGotoEvent->Associate(this);

   fTGotoEvent = new TGTextEntry(fEventGoto, new TGTextBuffer(20),T_GOTO_EVENT);
   fTGotoEvent->Resize(50, fTGotoEvent->GetDefaultHeight());
   fEventGoto->AddFrame(fTGotoEvent, 
	new TGLayoutHints(kLHintsTop | kLHintsLeft, 4, 4, 2, 2));
   fTGotoEvent->Associate(this);
   AddFrame(fEventGoto, new TGLayoutHints(kLHintsTop | kLHintsLeft,
            10, 10, 1, 0));

   // Scan event
   fStartAnal = new TGCompositeFrame(this, 60, 20, kHorizontalFrame);
   fBStartAnal= new TGTextButton(fStartAnal, "Start analize  ",B_START_ANAL);
   fStartAnal->AddFrame(fBStartAnal, 
         new TGLayoutHints(kLHintsTop|kLHintsLeft, 4, 4, 2, 2) );
   fBStartAnal->Associate(this);

   Char_t str[20];
   sprintf(str,"%d",fNEventsAnalize);
   TGTextBuffer *tb2=new TGTextBuffer(20);
   tb2->AddText(0,str);
   fTToEvent = new TGTextEntry(fStartAnal, tb2, T_ANAL_END);
   fTToEvent->Resize(50, fTToEvent->GetDefaultHeight());
   fStartAnal->AddFrame(fTToEvent, 
	new TGLayoutHints(kLHintsTop | kLHintsLeft, 4, 4, 2, 2));
   fTToEvent->Associate(this);

   fLSAFromEvent = new TGLabel(fStartAnal,"Events   from Event No. ");
   fStartAnal->AddFrame(fLSAFromEvent, 
			 new TGLayoutHints(kLHintsTop|kLHintsLeft,2,0,2,2));

   sprintf(str,"%d",fFirstEvent);
   TGTextBuffer *tb=new TGTextBuffer(20);
   tb->AddText(0,str);
   fTFromEvent = new TGTextEntry(fStartAnal, tb ,T_ANAL_START);
   fTFromEvent->Resize(50, fTFromEvent->GetDefaultHeight());
   fStartAnal->AddFrame(fTFromEvent, 
	new TGLayoutHints(kLHintsTop | kLHintsLeft, 4, 4, 2, 2));
   fTFromEvent->Associate(this);

   //   fLSAToEvent = new TGLabel(fStartAnal,"   No. of Events to analize: ");
   // fStartAnal->AddFrame(fLSAToEvent, 
   //			 new TGLayoutHints(kLHintsTop|kLHintsLeft,2,0,2,2));

   AddFrame(fStartAnal, new TGLayoutHints(kLHintsTop | kLHintsLeft,
            10, 10, 1, 0));

   SetWindowName("JSF Control Panel");
   MapSubwindows();

   // we need to use GetDefault...() to initialize the layout algorithm...
   // Resize(GetDefaultSize());
   Resize(400,200);
   Layout();
   MapWindow();


}

//____________________________________________________________________
JSFGUIFrame::~JSFGUIFrame()
{
   // Delete all created widgets.

   delete fMenuBarLayout;
   delete fMenuBarItemLayout;
   delete fMenuBarHelpLayout;

   delete fMenuFile;
   delete fMenuDisplay;
   delete fMenuAnal;
   delete fMenuHelp;
}

//__________________________________________________________________
void JSFGUIFrame::CloseWindow()
{
   // Got close message for this MainFrame. Calls parent CloseWindow()
   // (which destroys the window) and terminate the application.
   // The close message is generated by the window manager when its close
   // window menu item is selected.

   TGMainFrame::CloseWindow();
   gApplication->Terminate(0);
}

//_________________________________________________________________
Bool_t JSFGUIFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle messages send to the TestMainFrame object. E.g. all menu button
   // messages.


   EMsgBoxIcon icontype = kMBIconExclamation;
   Int_t retval;
   Int_t buttons=0;
   Char_t evtmsg[40];
   Char_t workstr[256];
   TGString *tgmsg;
   InputDialog *dialog;
   Int_t i;
   Char_t defval[20], retstr[120];
   switch (GET_MSG(msg)) {

      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {

            case kCM_BUTTON:
               //printf("Button was pressed, id = %ld\n", parm1);
	      switch (parm1) {
	        case B_BEGIN: 
		  if( fInitialized ) {
  	            new TGMsgBox(fClient->GetRoot(), this,  "JSF Message", 
				 "JSF is already initialized",
                                  icontype, buttons, &retval);
		    break;
                  } 
                  fMenuFile->DisableEntry(M_FILE_OPEN);
                  fMenuFile->DisableEntry(M_FILE_OPENOUTPUT);
		  fInitialized=kTRUE;
		  gROOT->ProcessLine("Initialize();");
		  break;

	        case B_NEXT_EVENT: 
		  if( !fInitialized ) {
		    fMenuFile->DisableEntry(M_FILE_OPEN);
                    fMenuFile->DisableEntry(M_FILE_OPENOUTPUT);
		    fInitialized=kTRUE;
		    gROOT->ProcessLine("Initialize();");
                  }
		  gROOT->ProcessLine("GetNext();");
		  sprintf(evtmsg,"  Event Number: %d\n",gJSF->GetEventNumber());
		  tgmsg=new TGString(evtmsg);	
		  fLEventNumber->SetText(tgmsg);
		  break;

	        case B_PREVIOUS_EVENT: 
		  if( !fInitialized ) {
  	            new TGMsgBox(fClient->GetRoot(), this,  "JSF Message", 
			 "JSF is not initialized\nPress initialize button first",
                                  icontype, buttons, &retval);
		    break;
                  }
		  gROOT->ProcessLine("GetPrevious();");
		  sprintf(evtmsg,"  Event Number: %d\n",gJSF->GetEventNumber());
		  tgmsg=new TGString(evtmsg);
		  fLEventNumber->SetText(tgmsg);
		  break;
	        case B_GOTO_EVENT: 
		  GotoEventAction();
		  break;
	        case B_START_ANAL: 
		  AnalizeEventAction();
		  break;
	      }
	      break;

            case kCM_MENUSELECT:
               //printf("Pointer over menu entry, id=%ld\n", parm1);
               break;

            case kCM_MENU:
               switch (parm1) {
	         case M_FILE_GENEVENT: 
	         case M_FILE_READROOT: 
	         case M_FILE_READSIMDST: 
	         case M_FILE_READJIM: 
		   for(i=M_FILE_GENEVENT;i<=M_FILE_READJIM;i++){
                     fMenuFile->UnCheckEntry(i);
		   }
		   fMenuFile->CheckEntry(parm1);
		   fRunMode=parm1-M_FILE_GENEVENT+1;
		   break;

                  case M_FILE_OPEN:
                     {
                        TGFileInfo fi;
                        fi.fFileTypes = (char **)filetypes;
			Char_t dirnam[256];
			strcpy(dirnam,gSystem->WorkingDirectory());
			new TGFileDialog(fClient->GetRoot(), this, kFDOpen,&fi);
			sprintf(workstr,"%s/%s",
				gSystem->WorkingDirectory(),fi.fFilename);
			ToRelativePath(workstr, dirnam, fInputFileName);
			tgmsg=new TGString(fInputFileName);
			fLFileN->SetText(tgmsg);
			fLFileN->SetTextJustify(kTextLeft);
			gSystem->ChangeDirectory(dirnam);
			fMenuFile->EnableEntry(M_FILE_READROOT);
			fMenuFile->EnableEntry(M_FILE_READSIMDST);
			fMenuFile->EnableEntry(M_FILE_READJIM);

			Int_t lfn=strlen(fInputFileName);
			if( strcmp(&fInputFileName[lfn-4],"root") == 0 ){
			  fMenuFile->CheckEntry(M_FILE_READROOT);
			  fMenuFile->UnCheckEntry(M_FILE_READJIM);
			  fMenuFile->UnCheckEntry(M_FILE_READSIMDST);
			  fMenuFile->UnCheckEntry(M_FILE_GENEVENT);
			  fRunMode=2;
			}
			else if( strcmp(&fInputFileName[lfn-3],"dat") == 0 ){
			  fMenuFile->CheckEntry(M_FILE_READSIMDST);
			  fMenuFile->UnCheckEntry(M_FILE_READROOT);
			  fMenuFile->UnCheckEntry(M_FILE_READJIM);
			  fMenuFile->UnCheckEntry(M_FILE_GENEVENT);
			  fRunMode=3;
			}
			else if( strcmp(&fInputFileName[lfn-3],"bnk") == 0 ){
			  fMenuFile->CheckEntry(M_FILE_READJIM);
			  fMenuFile->UnCheckEntry(M_FILE_READROOT);
			  fMenuFile->UnCheckEntry(M_FILE_READSIMDST);
			  fMenuFile->UnCheckEntry(M_FILE_GENEVENT);
			  fRunMode=4;
			}


			break;

                     }
                     break;

                  case M_FILE_OPENOUTPUT:
                     {
                        TGFileInfo fi;
                        fi.fFileTypes = (char **)filetypes;
			Char_t dirnam[256];
			strcpy(dirnam,gSystem->WorkingDirectory());
			new TGFileDialog(fClient->GetRoot(), this, kFDSave,&fi);
			sprintf(workstr,"%s/%s",
				gSystem->WorkingDirectory(),fi.fFilename);
			ToRelativePath(workstr,dirnam,fOutputFileName);
			tgmsg=new TGString(fOutputFileName);
			fLOFileN->SetText(tgmsg);
			fLOFileN->SetTextJustify(kTextLeft);
			gSystem->ChangeDirectory(dirnam);
                     }
                     break;

                  case M_FILE_STARTBROWSER:
		    if( !gbrows ) gbrows=new TBrowser("Browser");
                    break;

                  case M_FILE_EXIT:
		    gROOT->ProcessLine("JobEnd();");
		    CloseWindow();   // this also terminates theApp
                    break;

                  case M_DISP_REDRAW:
		     fED->DisplayEventData();
                     break;

                  case M_DISP_MOMENTUM:
                  case M_DISP_ALL:
                  case M_DISP_VTX:
		     fMenuDisplay->UnCheckEntry(fED->fDisplayType+301);
		     fMenuDisplay->CheckEntry(parm1);
		     fED->fDisplayType=parm1-301;
                     break;

	          case M_DISP_EVENT:
		     if( fED->fDrawAtNewEvent ) {  
		       fMenuDisplay->UnCheckEntry(M_DISP_EVENT);
		       fED->fDrawAtNewEvent=kFALSE;
                     }
		     else {
		       fMenuDisplay->CheckEntry(M_DISP_EVENT);
		       fED->fDrawAtNewEvent=kTRUE;
                     }


  	          case M_ONOFF_GEOMETRY:
		    if( fED->fDrawGeometry ) {
		      fMenuOnOffSignals->UnCheckEntry(parm1);
		      fED->fDrawGeometry=kFALSE;
		    } 
		    else {
		      fMenuOnOffSignals->CheckEntry(parm1);
		      fED->fDrawGeometry=kTRUE;
		    }
		    break;


  	          case M_ONOFF_GENNEUTRAL:
		    if( fED->fGenNeutral->fShow ) {
		      fMenuOnOffSignals->UnCheckEntry(parm1);
		      fED->fGenNeutral->fShow=kFALSE;
		    } 
		    else {
		      fMenuOnOffSignals->CheckEntry(parm1);
		      fED->fGenNeutral->fShow=kTRUE;
		    }
		    break;


  	          case M_ONOFF_GENTRACK:
		    if( fED->fGenCharged->fShow ) {
		      fMenuOnOffSignals->UnCheckEntry(parm1);
		      fED->fGenCharged->fShow=kFALSE;
		    } 
		    else {
		      fMenuOnOffSignals->CheckEntry(parm1);
		      fED->fGenCharged->fShow=kTRUE;
		    }
		    break;

  	          case M_ONOFF_LTKCLTRACK:
		    if( fED->fLTKCLTrack->fShow ) {
		      fMenuOnOffSignals->UnCheckEntry(parm1);
		      fED->fLTKCLTrack->fShow=kFALSE;
		    } 
		    else {
		      fMenuOnOffSignals->CheckEntry(parm1);
		      fED->fLTKCLTrack->fShow=kTRUE;
		    }
		    break;

  	          case M_ONOFF_CDCTRACK:
		    if( fED->fCDCTrack->fShow ) {
		      fMenuOnOffSignals->UnCheckEntry(parm1);
		      fED->fCDCTrack->fShow=kFALSE;
		    } 
		    else {
		      fMenuOnOffSignals->CheckEntry(parm1);
		      fED->fCDCTrack->fShow=kTRUE;
		    }
		    break;

  	          case M_ONOFF_VTXHIT:
		    if( fED->fVTXHit->fShow ) {
		      fMenuOnOffSignals->UnCheckEntry(parm1);
		      fED->fVTXHit->fShow=kFALSE;
		    } 
		    else {
		      fMenuOnOffSignals->CheckEntry(parm1);
		      fED->fVTXHit->fShow=kTRUE;
		    }
		    break;

  	          case M_ONOFF_EMCHIT:
		    if( fED->fEMCHit->fShow ) {
		      fMenuOnOffSignals->UnCheckEntry(parm1);
		      fED->fEMCHit->fShow=kFALSE;
		    } 
		    else {
		      fMenuOnOffSignals->CheckEntry(parm1);
		      fED->fEMCHit->fShow=kTRUE;
		    }
		    break;

  	          case M_ONOFF_HDCHIT:
		    if( fED->fHDCHit->fShow ) {
		      fMenuOnOffSignals->UnCheckEntry(parm1);
		      fED->fHDCHit->fShow=kFALSE;
		    } 
		    else {
		      fMenuOnOffSignals->CheckEntry(parm1);
		      fED->fHDCHit->fShow=kTRUE;
		    }
		    break;


	          case M_ANAL_SHOWHIST:
		    if( fShowHist ) {
		      fMenuAnal->UnCheckEntry(M_ANAL_SHOWHIST);
		      fShowHist=kFALSE;
		    }
		    else {
		      fMenuAnal->CheckEntry(M_ANAL_SHOWHIST);
		      fShowHist=kTRUE;
		    }
		    break;

	          case M_ANAL_SHOWFREQ:
		    sprintf(defval,"%d",fShowHistFrequency);
		    dialog = new InputDialog("Histgram dispplay rate",defval,retstr);
		    sscanf(retstr,"%d",&fShowHistFrequency);
		    break;

	          case M_ANAL_DRAWHIST:
 		    gROOT->ProcessLine("DrawHist();");
		    break;

	          case M_ANAL_RESETHIST:
 		    gROOT->ProcessLine("ResetHist();");
		    break;

	          case M_ANAL_MACRONAME:
		    dialog= new InputDialog("User Analysis Macro file name", 
					    fMacroFileName, fMacroFileName);
		    break;

 	          case M_ANAL_RELOAD:
		    gROOT->LoadMacro(fMacroFileName);
		    break;

	          case M_GEN_ECM:
		    sprintf(defval,"%g",fEcm);
		    dialog = new InputDialog("CM Energy(GeV)",defval,retstr);
		    sscanf(retstr,"%g",&fEcm);
		    break;

	         case M_GEN_INITPYTHIA:
		    dialog = new InputDialog("InitPythia Macro file name",
					     fInitPythiaMacro,retstr);
		    sscanf(retstr,"%s",fInitPythiaMacro);
		    break;

	          case M_GEN_PYTHIA:
	          case M_GEN_DEBUG:
		     fMenuGenType->UnCheckEntry(M_GEN_PYTHIA+fEventType);
		     fMenuGenType->CheckEntry(parm1);
		     fEventType=parm1-M_GEN_PYTHIA;

		     printf(" new Event type is %d\n",fEventType);

                     break;

	          case M_HELP_ABOUT:
		     new TGMsgBox(fClient->GetRoot(), this,
                                  "About JSF Control Panel", 
"Welcome to JSF Control Panel.\nYou can display event from this panel,\nor start event simulation.",
                                  icontype, buttons, &retval);
	          case M_HELP_EVENTDISP:
		     new TGMsgBox(fClient->GetRoot(), this,
                                  "About Event Display", 
	  "Momentum View shows LTKCL track momentum. \nTrack color shows track type.\nIn All view,  red beam pipe is +Z, \nblue is -Z, cyan is +X\nand purple is +Y directions",
                                  icontype, buttons, &retval);
		     break;

                  default:
                     break;
               }
            default:
               break;
         }
	 break;

      case kC_TEXTENTRY:

	switch (GET_SUBMSG(msg)) {
	  case kTE_ENTER:
	    if( parm1 == T_GOTO_EVENT )  GotoEventAction();
	    break;
 	  default:
	    break;
	}
        break;

      default:
         break;
   }
   return kTRUE;
}



//_________________________________________________________________
void JSFGUIFrame::DisplayEventData()
{
   fED->DisplayEventData();
}

//_________________________________________________________________
void JSFGUIFrame::DrawHist()
{

   if( !fShowHist )  return;
   if( fNoOfAnalizedEvents < 0 ) gROOT->ProcessLine("DrawHist();");
   else if( fNoOfAnalizedEvents%fShowHistFrequency  == 0 ) { 
            gROOT->ProcessLine("DrawHist();");
   }
}

//_________________________________________________________________
void JSFGUIFrame::GotoEventAction()
{
  Char_t evtmsg[40];
  TGString *tgmsg;
  Char_t cmd[256];

  if( !fInitialized ) {
       fMenuFile->DisableEntry(M_FILE_OPEN);
       fMenuFile->DisableEntry(M_FILE_OPENOUTPUT);
       fInitialized=kTRUE;
       gROOT->ProcessLine("Initialize();");
  }
  sprintf(cmd,"GetEvent(%s);",
	 fTGotoEvent->GetBuffer()->GetString());
  gROOT->ProcessLine(cmd);
  sprintf(evtmsg,"  Event Number: %d\n",gJSF->GetEventNumber());
  tgmsg=new TGString(evtmsg);
  fLEventNumber->SetText(tgmsg);
}


//_________________________________________________________________
void JSFGUIFrame::AnalizeEventAction()
{
  Char_t evtmsg[40];
  TGString *tgmsg;
  Char_t cmd[256];

  EMsgBoxIcon icontype = kMBIconExclamation;
  Int_t retval;
  Int_t buttons=0;

  if( !fInitialized ) {
       fMenuFile->DisableEntry(M_FILE_OPEN);
       fMenuFile->DisableEntry(M_FILE_OPENOUTPUT);
       fInitialized=kTRUE;
       gROOT->ProcessLine("Initialize();");
  }
  Int_t i;
  sscanf(fTFromEvent->GetBuffer()->GetString(),"%d",&fFirstEvent);
  sscanf(fTToEvent->GetBuffer()->GetString(),"%d",&fNEventsAnalize);
  fNoOfAnalizedEvents=0;
  Int_t last=fFirstEvent+fNEventsAnalize-1;
  for(i=fFirstEvent;i<=last;i++){
    fNoOfAnalizedEvents++;
    sprintf(cmd,"GetEvent(%d);",i);
    gROOT->ProcessLine(cmd);
    switch (fReturnCode) {
      case -2:
         new TGMsgBox(fClient->GetRoot(), this, "JSF Message", 
	     "Reached end-of-file\n or read error occured.",
             icontype, buttons, &retval);
	     break;
 
     case -1:
         new TGMsgBox(fClient->GetRoot(), this, "JSF Message", 
	     "Analysis routine returned error.",
             icontype, buttons, &retval);
	     break;
    }
    sprintf(evtmsg,"  Event Number: %d\n",gJSF->GetEventNumber());
    tgmsg=new TGString(evtmsg);
    fLEventNumber->SetText(tgmsg);
    if( fReturnCode != 0 ) break;
  }
  fNoOfAnalizedEvents=-999;  
  DrawHist();
}

//------------------------------------------------------------
void JSFGUIFrame::ToRelativePath(const Char_t *fnin, 
				 const Char_t *dirnowi, Char_t *fnout)
{
  // Absolute path name is changed to the path relative to 
  // dirnow.

  //  Int_t lfn=strlen(fnin);
  Int_t ldir=strlen(dirnowi);
#ifdef R__ACC
  Char_t dirnow[512];
#else
  Char_t dirnow[ldir+2];
#endif
  strcpy(dirnow, dirnowi);
  if( dirnowi[ldir] != '/' ) { strcat(dirnow, "/"); }
  
  Int_t i;
  for(i=0;i<ldir+1;i++){ if( fnin[i] != dirnow[i] ) break;   }

  if( ldir > 30 && i < ldir/2 ) {
    strcpy(fnout,fnin);
    return ;
  }
  Int_t j;
  for(j=i-1;j>-1;j--){ if( fnin[j] == '/' ) break;  }

  // count number of slash between j and ldir
  Int_t k;  Int_t nslash=0; 
  for(k=j+1;k<=ldir;k++){  
    if( dirnow[k] == '/' ) { nslash++; }
   }

  k=0;
  for(i=0;i<nslash;i++){ sprintf(&fnout[k],"../"); k+=3; }
  sprintf(&fnout[k],"%s",&fnin[j+1]);
}
