//*LastUpdate :  jsf-1-12 3-September-1999  By Akiya Miyamoto
//*LastUpdate :  jsf-1-9  16-May-1999  By Akiya Miyamoto
//*LastUpdate :  jsf-1-8  19-April-1999  By Akiya Miyamoto
//*LastUpdate :  jsf-1-5  20-Feburary-1999  By Akiya Miyamoto
//*-- Author  : A.Miyamoto  20-September-1998

/*
  19-Apr-1999 A.Miyamoto  A bug in JSFGUIFrame::ToRelativePath is fixed.
  1-May-1999 A.Miyamoto   Add Start borwser and Open JIM file menu
  16-May-1999 A.Miyamoto  Does not call GetArguments macro.
                          It is now called from MainMacro.C
  31-July-1999 A.Miyamoto Add comments.
  3-September-1999 A.Miyamoto A lot of changes to use JSFEnv class and JSFDemoDisplay
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
// Parameters ;  name: default #  description 
// ~~~~~~~~~~~~
//  JSFGUI.RunMode: 1
//                 # 1 = Generate event.
//                 # 2 = Read root file.
//                 # 3 = Read SIMDST fortran binary file.
//                 # 4 = Read JIM data.
//  JSFGUI.MacroFileName: UserAnalysis.C
//                 # File name for UserAnalysis macro
//  JSFGUI.ShowHist: 0 
//                 # 0 = Does not draw histogram at event event.
//                 # 1 = Draw histogram.  A function, DrawHist() is called.
//                 #     It must be defined in the UserAnalysis macro.
//  JSFGUI.ShowHistFrequency: 1
//                 # At every ShowHistFrequency events, histogram is drown.
//  JSFGUI.FirstEvent: 1
//                 # Event number where to start analysis.  This number is shown in 
//                 # "From event no." field of JSF control pannel.
//  JSFGUI.NEventsAnalize: 10
//                 # Number of event to analize.  The number is shown in the JSF control
//                 # pannel. 
//  JSFGUI.EventType: 1
//                 # Valid when run mode=1 and specifies the type of event generator.
//                 # 0 = Pythia Generator
//                 # 1 = Debug generator
//                 # 2 = Bases/Spring generator          ( not available yet)
//                 # 3 = Read parton data and simulate.  ( not available yet)
//                 # 4 = Read hepevt data                ( not available yet)
//  JSFUI.SimulationType : 1                             ( not available yet)
//                 # 1 = Quick Simulator                
//                 # 2 = JLCSIM/JIM Simulator
//  JSFGUI.ECM: 300
//                 # Center of mass system for PythiaGenerator in GeV unit.
//  JSFGUI.InitPythia: InitPythia.C
//                 # Name of a macro filewhich is used to initialize Pythia.
//  JSFGUI.InputFileName: 
//                 # Input file name
//  JSFGUI.OutputFileName: jsf.root
//                 # Output file name,
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

#include "JSFSteer.h"
#include "JSFGUIFrame.h"
#include "InputDialog.h"
#include "JSFEnvGUIFrame.h"
#include "JSFDemoDisplay.h"

TBrowser *gbrows;

enum EJSFGUICommandIdentifiers {
   M__FILE_BEGIN=100,
   M_FILE_OPEN=125,
   M_FILE_OPENOUTPUT=126,
   M_FILE_SAVE=127,
   M_FILE_SAVEAS=128,
   M_FILE_SETPARAM=129,

   M_FILE_STARTBROWSER=150,
   M_FILE_EXIT=151,

   M__FILE_END=160,

   M_HELP_CONTENTS=200,
   M_HELP_SEARCH=201,
   M_HELP_ABOUT=203,
   M_HELP_EVENTDISP=204,

   M__DISP_BEGIN=299,
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
   M__DISP_END=330,

   M__ANAL_BEGIN=500,
   M_ANAL_SHOWHIST=501,
   M_ANAL_SHOWFREQ=502,
   M_ANAL_RESETHIST=503,
   M_ANAL_DRAWHIST=504,
   M_ANAL_RELOAD=505,
   M_ANAL_MACRONAME=506,
   M__ANAL_END=507,

   M__CONT_BEGIN=600,

   M_GEN_PYEV_ZH=621,
   M_GEN_PYEV_GGH=622,
   M_GEN_PYEV_EEH=623,
   M_GEN_PYEV_NNH=624,
   M_GEN_PYEV_GAMMAZ=625,
   M_GEN_PYEV_ZZ=626,
   M_GEN_PYEV_WW=627,
   M_GEN_PYEV_ENW=628,
   M_GEN_PYEV_TWOPHOTON=629,
   M_GEN_PYEV_DECAY_Z=630,

   M_CONT_RT_USERDEFINE=660,
   M_CONT_RT_GENEVENT=661,
   M_CONT_RT_READROOT=662,
   M_CONT_RT_READSIMDST=663,
   M_CONT_RT_READJIM=664,
   M_CONT_RT_JSFJIM=665,
   M_CONT_RT_LAST=666,
   
   M_CONT_PARAM_SAVE=683,
   M_CONT_PARAM_SAVEAS=684,
   M_CONT_PARAM_EDIT=685,

   M_GEN_EVENTTYPE=701,
   M_GEN_ECM=702,
   M_GEN_INITPYTHIA=703,
   M_GEN_PYTHIA_PRSTAT=704,
   M_GEN_PYTHIA_HMASS=705,
   
   M_GEN_PYTHIA=721,
   M_GEN_DEBUG=722, 
   M_GEN_SPRING=723,
   M_GEN_RPARTON=724,
   M_GEN_RHEPEVT=725,
   M_GEN_MERGE=726,
   M_GEN_LASTRUN=727,
   M_GEN_LAST=728,

   M_GEN_LASTRUNFILE=729,

   M_GEN_DEBUG_RANDOM=741,
   M_GEN_DEBUG_NPARTICLE=742,
   M_GEN_DEBUG_PRANGE=743,
   M_GEN_DEBUG_COSTHRANGE=744,
   M_GEN_DEBUG_AZIMUTHRANGE=745,
   M_GEN_DEBUG_VTXRRANGE=746,
   M_GEN_DEBUG_VTXPHIRANGE=747,
   M_GEN_DEBUG_VTXZRANGE=748,
   M_GEN_DEBUG_NSPECIE=749,
   M_GEN_DEBUG_SPECIES1=750,
   M_GEN_DEBUG_SPECIES2=751,
   M_GEN_DEBUG_SPECIES3=753,

   M_GEN_SPRING_SO=761,
   M_GEN_SPRING_MODULE_NAME=762,
   M_GEN_BASES_FILE=763,

   M_GEN_RPARTON_DATAFILE=781,
   M_GEN_RPARTON_FORMAT=782,

   M_GEN_RDGEN_DATAFILE=801,
   M_GEN_RDGEN_FORMAT=802,

   M_GEN_MERGE_DATAFILE=821,
   M_GEN_MERGE_LUMPERTRAIN=822,
   M_GEN_MERGE_SEED=823,

   M_SIM_QUICKSIM=841,
   M_SIM_JIM=842,
   M_SIM_QUICKSIM_PARAM=843,

   M_GUI_RUNNO=861,
   M_GUI_SIMDSTOUT=862,
   M_GUI_OUTEVENTDATA=863,

   M__CONT_END=880,

   B_BEGIN=1,
   B_NEXT_EVENT=2,
   B_PREVIOUS_EVENT=3,
   B_GOTO_EVENT=4,

   B_START_ANAL=5,

   T_GOTO_EVENT,
   T_ANAL_START,
   T_ANAL_NEVENTS

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

// ClassImp(JSFGUIFrame)

///////////////////////////////////////////////////////////////////////

JSFGUIFrame::JSFGUIFrame(const TGWindow *p, UInt_t w, UInt_t h, Bool_t demo)
      : TGMainFrame(p, w, h)
{
   // Create test main frame. A TGMainFrame is a top level window.

  if( demo ) {
    fDemo=new JSFDemoDisplay(this);
    fED=fDemo;
  }
  else {
    fED=new JSFEventDisplay(this);
    fDemo=0;
  }

   fInitialized=kFALSE;
   fNoOfAnalizedEvents=-999;
   gbrows=NULL;
   Int_t i;

   // Create menubar and popup menus. The hint objects are used to place
   // and group the different menu widgets with respect to eachother.
   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                                      0, 0, 1, 1);
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);

   fMenuFile = new TGPopupMenu(fClient->GetRoot());
   fMenuFile->AddLabel("Files");
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("&Open Input...", M_FILE_OPEN);
   fMenuFile->AddEntry("&Open Output...", M_FILE_OPENOUTPUT);
   fMenuFile->AddEntry("&Start Browser...", M_FILE_STARTBROWSER);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("E&xit", M_FILE_EXIT);

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


   // Help menu
   fMenuHelp = new TGPopupMenu(fClient->GetRoot());
   fMenuHelp->AddEntry("&About", M_HELP_ABOUT);
   fMenuHelp->AddSeparator();
   fMenuHelp->AddEntry("&Event Display", M_HELP_EVENTDISP);

   fMenuRunMode = new TGPopupMenu(fClient->GetRoot());
   fMenuRunMode->AddLabel("Run Mode");
   fMenuRunMode->AddSeparator();
   fMenuRunMode->AddEntry("&Generate Event", M_CONT_RT_GENEVENT);
   fMenuRunMode->AddEntry("&Run JIM simulation", M_CONT_RT_JSFJIM);
   fMenuRunMode->AddSeparator();
   fMenuRunMode->AddEntry("&Read Root File", M_CONT_RT_READROOT);
   fMenuRunMode->AddEntry("&Read SIMDST File", M_CONT_RT_READSIMDST);
   fMenuRunMode->AddEntry("&Read JIM Bank File", M_CONT_RT_READJIM);
   fMenuRunMode->AddEntry("&User defined", M_CONT_RT_USERDEFINE);
   

   //  Generator Control menu
   fPythiaEvtype=new TGPopupMenu(fClient->GetRoot());
   fPythiaEvtype->AddLabel("Pythia Event Type");
   fPythiaEvtype->AddSeparator();
   fPythiaEvtype->AddEntry("ZH",M_GEN_PYEV_ZH);
   fPythiaEvtype->AddEntry("gamma_gamma_H",M_GEN_PYEV_GGH);
   fPythiaEvtype->AddEntry("eeH",M_GEN_PYEV_EEH);
   fPythiaEvtype->AddEntry("nnH",M_GEN_PYEV_NNH);
   fPythiaEvtype->AddEntry("gamma/Z",M_GEN_PYEV_GAMMAZ);
   fPythiaEvtype->AddEntry("ZZ",M_GEN_PYEV_ZZ);
   fPythiaEvtype->AddEntry("WW",M_GEN_PYEV_WW);
   fPythiaEvtype->AddEntry("enW",M_GEN_PYEV_WW);
   fPythiaEvtype->AddEntry("twophoton",M_GEN_PYEV_TWOPHOTON);
   fPythiaEvtype->AddSeparator();
   fPythiaEvtype->AddEntry("Z decay mode",M_GEN_PYEV_DECAY_Z);
   
   fMenuGenType=new TGPopupMenu(fClient->GetRoot());
   for(i=0;i<M_GEN_LAST-M_GEN_PYTHIA;i++) {
     fMenuGInfo[i]=new TGPopupMenu(fClient->GetRoot());
   }

   fMenuGInfo[0]->AddLabel("Parameters");
   fMenuGInfo[0]->AddSeparator();
   fMenuGInfo[0]->AddEntry("CM Energy",M_GEN_ECM);
   fMenuGInfo[0]->AddEntry("InitPythiaMacro",M_GEN_INITPYTHIA);
   fMenuGInfo[0]->AddEntry("PRSTAT argument",M_GEN_PYTHIA_PRSTAT);
   fMenuGInfo[0]->AddPopup("Event type ...",fPythiaEvtype);
   fMenuGInfo[0]->AddEntry("Mass of Higgs",M_GEN_PYTHIA_HMASS);

   fMenuGInfo[1]->AddLabel("Parameters");
   fMenuGInfo[1]->AddSeparator();
   fMenuGInfo[1]->AddEntry("Random Seed",M_GEN_DEBUG_RANDOM);
   fMenuGInfo[1]->AddEntry("# of particles",M_GEN_DEBUG_NPARTICLE);
   fMenuGInfo[1]->AddEntry("Momentum",M_GEN_DEBUG_PRANGE);
   fMenuGInfo[1]->AddEntry("Costh",M_GEN_DEBUG_COSTHRANGE);
   fMenuGInfo[1]->AddEntry("Azimuthal angle",M_GEN_DEBUG_AZIMUTHRANGE);
   fMenuGInfo[1]->AddEntry("Vertex radius",M_GEN_DEBUG_VTXRRANGE);
   fMenuGInfo[1]->AddEntry("Vertex phi",M_GEN_DEBUG_VTXPHIRANGE);
   fMenuGInfo[1]->AddEntry("Vertex Z",M_GEN_DEBUG_VTXZRANGE);
   fMenuGInfo[1]->AddEntry("# of species",M_GEN_DEBUG_NSPECIE);
   fMenuGInfo[1]->AddEntry("Property of 1st specie",M_GEN_DEBUG_SPECIES1);
   fMenuGInfo[1]->AddEntry("Property of 2nd specie",M_GEN_DEBUG_SPECIES2);
   fMenuGInfo[1]->AddEntry("Property of 3rd specie",M_GEN_DEBUG_SPECIES3);


   fMenuGInfo[2]->AddLabel("Parameters");
   fMenuGInfo[2]->AddSeparator();
   fMenuGInfo[2]->AddEntry("Spring Module name",M_GEN_SPRING_MODULE_NAME);
   fMenuGInfo[2]->AddEntry("Shared Library name",M_GEN_SPRING_SO);
   fMenuGInfo[2]->AddEntry("Bases File name",M_GEN_BASES_FILE);

   fMenuGInfo[3]->AddLabel("Parameters");
   fMenuGInfo[3]->AddSeparator();
   fMenuGInfo[3]->AddEntry("Input data file",M_GEN_RPARTON_DATAFILE);
   fMenuGInfo[3]->AddEntry("Format",M_GEN_RPARTON_FORMAT);

   fMenuGInfo[4]->AddLabel("Parameters");
   fMenuGInfo[4]->AddSeparator();
   fMenuGInfo[4]->AddEntry("Input data file",M_GEN_RDGEN_DATAFILE);
   fMenuGInfo[4]->AddEntry("Format",M_GEN_RDGEN_FORMAT);

   fMenuGInfo[5]->AddLabel("Parameters");
   fMenuGInfo[5]->AddSeparator();
   fMenuGInfo[5]->AddEntry("Data file",M_GEN_MERGE_DATAFILE);
   fMenuGInfo[5]->AddEntry("Luminosity/Train",M_GEN_MERGE_LUMPERTRAIN);
   fMenuGInfo[5]->AddEntry("Random seed",M_GEN_MERGE_SEED);

   fMenuGenType->AddLabel("Generator Parameters");
   fMenuGenType->AddSeparator();
   fMenuGenType->AddPopup("Pythia ...",fMenuGInfo[0]);
   fMenuGenType->AddPopup("Debug ...",fMenuGInfo[1]);
   fMenuGenType->AddPopup("Bases/Spring ...",fMenuGInfo[2]);
   fMenuGenType->AddPopup("Read parton  ...",fMenuGInfo[3]);
   fMenuGenType->AddPopup("Read HEPEVT ...",fMenuGInfo[4]);
   fMenuGenType->AddSeparator();
   fMenuGenType->AddPopup("Event Merge ...",fMenuGInfo[5]);
   fMenuGenType->AddEntry("Last run file",M_GEN_LASTRUNFILE);
   fMenuGenType->AddSeparator();
   fMenuGenType->AddEntry("Run number",M_GUI_RUNNO);
   fMenuGenType->AddEntry("Output SIMDST data",M_GUI_SIMDSTOUT);
   fMenuGenType->AddEntry("Output Event data",M_GUI_OUTEVENTDATA);
   
   fMenuGen=new TGPopupMenu(fClient->GetRoot());
   fMenuGen->AddLabel("Type of Generator");
   fMenuGen->AddSeparator();
   fMenuGen->AddEntry("Pythia",M_GEN_PYTHIA);
   fMenuGen->AddEntry("Debug",M_GEN_DEBUG);
   fMenuGen->AddEntry("Bases/Spring",M_GEN_SPRING);
   fMenuGen->AddEntry("Read Parton Data",M_GEN_RPARTON);
   fMenuGen->AddEntry("Read HEPEVT Data",M_GEN_RHEPEVT);
   fMenuGen->AddSeparator();
   fMenuGen->AddEntry("Merge Event",M_GEN_MERGE);
   fMenuGen->AddEntry("Use last run seed",M_GEN_LASTRUN);

   fMenuSimType=new TGPopupMenu(fClient->GetRoot());
   fMenuSimType->AddLabel("Simulator type and Parameters");
   fMenuSimType->AddSeparator();
   fMenuSimType->AddEntry("Use QuickSim",M_SIM_QUICKSIM);
   fMenuSimType->AddEntry("Use JIM Sim.",M_SIM_JIM);
   fMenuSimType->AddSeparator();
   fMenuSimType->AddEntry("QuickSim Parameter file...",M_SIM_QUICKSIM_PARAM);


   fMenuControl=new TGPopupMenu(fClient->GetRoot());
   fMenuControl->AddLabel("Parameter control");
   fMenuControl->AddSeparator();
   fMenuControl->AddPopup("Run Mode ...",fMenuRunMode);
   fMenuControl->AddPopup("Generator type ...",fMenuGen);
   fMenuControl->AddPopup("Generator parameters ...",fMenuGenType);
   fMenuControl->AddSeparator();
   fMenuControl->AddPopup("Simulator type ...",fMenuSimType);
   fMenuControl->AddSeparator();
   fMenuControl->AddEntry("Save parameters",M_CONT_PARAM_SAVE);
   fMenuControl->AddEntry("Save as  ...",M_CONT_PARAM_SAVEAS);
   fMenuControl->AddEntry("Other parameters ...",M_CONT_PARAM_EDIT);
   
   fMenuFile->Associate(this);
   fMenuControl->Associate(this);
   fMenuDisplay->Associate(this);
   fMenuAnal->Associate(this);
   fMenuHelp->Associate(this);

   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);

   fMenuBar->AddPopup("&Controls", fMenuControl, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Analysis", fMenuAnal, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Event Display", fMenuDisplay, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);

   // Add user Menu
   if( gJSF->Env()->GetValue("JSFGUI.UserMenu",0) ) {
     gROOT->LoadMacro(gJSF->Env()->GetValue("JSFGUI.UserMenuMacro","UserMenuMacro.C"));
     fMenuUser = new TGPopupMenu(fClient->GetRoot());
     Char_t cmd[256];
     sprintf(cmd,"UserMenu((TGPopupMenu*)0x%x);",(UInt_t)fMenuUser);
     gROOT->ProcessLine(cmd);
     fMenuUser->Associate(this);
     fMenuBar->AddPopup("UserMenu", fMenuUser, fMenuBarItemLayout);
   }

   AddFrame(fMenuBar, fMenuBarLayout);

   // Create frame to display file information.
   TGLayoutHints *lhTopLeft=new TGLayoutHints(kLHintsTop|kLHintsLeft,2,0,2,2);
   TGLayoutHints *lhTopExpandX=new TGLayoutHints(kLHintsTop|kLHintsExpandX,2,0,2,2);

   Char_t blank[110];
   sprintf(blank,"%-100s","  ");
   fFileFrame = new TGCompositeFrame(this, 2, 2, kHorizontalFrame);
   fLFile = new TGLabel(fFileFrame," Input File: ");
   fFileFrame->AddFrame(fLFile, lhTopLeft);
   fLFileN = new TGLabel(fFileFrame,blank);
   fLFileN->SetWidth(400);
   fFileFrame->AddFrame(fLFileN, lhTopExpandX); 
   AddFrame(fFileFrame, lhTopExpandX);

   // To display output file name

   fFileOFrame = new TGCompositeFrame(this, 2, 2, kHorizontalFrame);
   fLOFile = new TGLabel(fFileOFrame," Output File : ");
   fFileOFrame->AddFrame(fLOFile, lhTopLeft);
   fLOFileN = new TGLabel(fFileOFrame,blank);
   fLOFileN->SetWidth(400);
   fFileOFrame->AddFrame(fLOFileN, lhTopExpandX);
   AddFrame(fFileOFrame, lhTopExpandX);

   //
   fBeginFrame = new TGCompositeFrame(this, 10, 10, kHorizontalFrame);
   fBBegin = new TGTextButton(fBeginFrame,"Initialize",B_BEGIN);
   fBBegin->Associate(this);
   fBeginFrame->AddFrame(fBBegin, lhTopLeft);
   AddFrame(fBeginFrame, lhTopExpandX);

   //
   TGLayoutHints *lhTopRight= new TGLayoutHints(kLHintsTop|kLHintsRight,2,0,2,2);
   fEventFrame = new TGCompositeFrame(this, 10, 10, kHorizontalFrame );
   fBNextEvent = new TGTextButton(fEventFrame, "Next Event", B_NEXT_EVENT);
   fBNextEvent->Associate(this);
   fEventFrame->AddFrame(fBNextEvent, lhTopRight);

   fBPreviousEvent = new TGTextButton(fEventFrame, "Previous Event", B_PREVIOUS_EVENT);
   fBPreviousEvent->Associate(this);
   fEventFrame->AddFrame(fBPreviousEvent, lhTopLeft);

   fLEventNumber = new TGLabel(fEventFrame, "  Event Number: 0         ");
   fEventFrame->AddFrame(fLEventNumber, lhTopLeft);
   AddFrame(fEventFrame, lhTopExpandX);

   // Jump to the event
   fEventGoto  = new TGCompositeFrame(this, 60, 20, kHorizontalFrame);
   fBGotoEvent = new TGTextButton(fEventGoto,"Jump to Event No. ",B_GOTO_EVENT);
   fEventGoto->AddFrame(fBGotoEvent, lhTopLeft);
   fBGotoEvent->Associate(this);

   fTGotoEvent = new TGTextEntry(fEventGoto, new TGTextBuffer(20),T_GOTO_EVENT);
   fTGotoEvent->Resize(50, fTGotoEvent->GetDefaultHeight());
   fEventGoto->AddFrame(fTGotoEvent, lhTopLeft);
   fTGotoEvent->Associate(this);
   AddFrame(fEventGoto, lhTopLeft);

   // Scan event
   fStartAnal = new TGCompositeFrame(this, 60, 20, kHorizontalFrame);
   fBStartAnal= new TGTextButton(fStartAnal, "Start analize  ",B_START_ANAL);
   fStartAnal->AddFrame(fBStartAnal, lhTopLeft);
   fBStartAnal->Associate(this);

   TGTextBuffer *tb1=new TGTextBuffer(20);
   tb1->AddText(0," ");
   fTToEvent = new TGTextEntry(fStartAnal, tb1, T_ANAL_NEVENTS);
   fTToEvent->Resize(50, fTToEvent->GetDefaultHeight());
   fStartAnal->AddFrame(fTToEvent, lhTopLeft);
   fTToEvent->Associate(this);

   fLSAFromEvent = new TGLabel(fStartAnal,"Events   from Event No. ");
   fStartAnal->AddFrame(fLSAFromEvent, lhTopLeft);

   TGTextBuffer *tb2=new TGTextBuffer(20);
   tb2->AddText(0," ");
   fTFromEvent = new TGTextEntry(fStartAnal, tb2,T_ANAL_START);
   fTFromEvent->Resize(50, fTFromEvent->GetDefaultHeight());
   fStartAnal->AddFrame(fTFromEvent, lhTopLeft);
   fTFromEvent->Associate(this);

   for(i=0;i<3;i++){ fTextEntryStatus[i]=kFALSE;}

   AddFrame(fStartAnal, lhTopLeft);

   Update();

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

  if( fMenuUser ) {
    gROOT->ProcessLine("UserMenuDelete();");
    delete fMenuUser;
  }
  
  delete fMenuBarLayout;
  delete fMenuBarItemLayout;
  delete fMenuBarHelpLayout;

  delete fMenuFile;
  delete fMenuDisplay;
  delete fMenuAnal;
  delete fMenuHelp;
}
 
//__________________________________________________________________
void JSFGUIFrame::Update()
{
  // Update parameters of JSF Control Panel

  Char_t str[30];
  Int_t i;

  //*****************
  Int_t firstEvent=0;
  if( fTextEntryStatus[1] ) {
    sscanf(fTFromEvent->GetBuffer()->GetString(),"%d",&firstEvent);
    SetFirstEvent(firstEvent);
    fTextEntryStatus[1]=kFALSE;
  }
  else {
    firstEvent=GetFirstEvent();
    sprintf(str,"%d",firstEvent);
    TGTextBuffer *tb=fTFromEvent->GetBuffer();
    tb->Clear();
    tb->AddText(0,str);
    fTFromEvent->UnmapWindow();  fTFromEvent->MapWindow();
  }

  //*****************
  Int_t nEventsAnalize=0;
  if( fTextEntryStatus[2] ) {
    sscanf(fTToEvent->GetBuffer()->GetString(),"%d",&nEventsAnalize);
    SetNEventsAnalize(nEventsAnalize);
    fTextEntryStatus[2]=kFALSE;
  }
  else {
    nEventsAnalize=GetNEventsAnalize();
    sprintf(str,"%d",nEventsAnalize);
    TGTextBuffer *tb=fTToEvent->GetBuffer();
    tb->Clear();  tb->AddText(0,str);
    fTToEvent->UnmapWindow();  fTToEvent->MapWindow();
  }

  //*****************
  fED->Update();
  if( fED->fDrawGeometry ) fMenuOnOffSignals->CheckEntry(M_ONOFF_GEOMETRY);
  else fMenuOnOffSignals->UnCheckEntry(M_ONOFF_GEOMETRY);

  if( fED->fDrawAtNewEvent) fMenuDisplay->CheckEntry(M_DISP_EVENT);
  else fMenuDisplay->UnCheckEntry(M_DISP_EVENT);

  if( fED->fDrawGeometry) fMenuDisplay->CheckEntry(M_ONOFF_GEOMETRY);
  else fMenuDisplay->UnCheckEntry(M_ONOFF_GEOMETRY);

  for(i=M_DISP_MOMENTUM;i<=M_DISP_VTX;i++){ fMenuDisplay->UnCheckEntry(i); }
  fMenuDisplay->CheckEntry(fED->fDisplayType+M_DISP_MOMENTUM);

  fED->fGenNeutral->Update(fMenuOnOffSignals, M_ONOFF_GENNEUTRAL);
  fED->fGenCharged->Update(fMenuOnOffSignals, M_ONOFF_GENTRACK);
  fED->fLTKCLTrack->Update(fMenuOnOffSignals, M_ONOFF_LTKCLTRACK);
  fED->fCDCTrack->Update(fMenuOnOffSignals, M_ONOFF_CDCTRACK);
  fED->fVTXHit->Update(fMenuOnOffSignals, M_ONOFF_VTXHIT);
  fED->fEMCHit->Update(fMenuOnOffSignals, M_ONOFF_EMCHIT);
  fED->fHDCHit->Update(fMenuOnOffSignals, M_ONOFF_HDCHIT);


  //**************************************************
  fShowHist = gJSF->Env()->GetValue("JSFGUI.ShowHist",1); 
  if( fShowHist ) fMenuAnal->CheckEntry(M_ANAL_SHOWHIST);
  else fMenuAnal->UnCheckEntry(M_ANAL_SHOWHIST);
  fShowHistFrequency=gJSF->Env()->GetValue("JSFGUI.ShowHistFrequency",10);

  //**************************************************
  // Control Parameters
  //**************************************************

  Int_t irunmode=GetRunMode();
  for(i=M_CONT_RT_USERDEFINE;i<M_CONT_RT_LAST;i++){
    if( i != M_CONT_RT_GENEVENT+irunmode-1 ) {
      fMenuRunMode->UnCheckEntry(i);
      if(fInitialized) fMenuRunMode->DisableEntry(i);
    }
    else {
      fMenuRunMode->CheckEntry(i);
      fMenuRunMode->EnableEntry(i);
    }
  }

  for(i=M_GEN_PYTHIA;i<M_GEN_LAST;i++) {
    if( irunmode == 1 && !fInitialized ) fMenuGen->EnableEntry(i);
    else fMenuGen->DisableEntry(i);
    fMenuGen->UnCheckEntry(i);
  }
  fMenuGen->CheckEntry(M_GEN_PYTHIA+GetEventType());
  if( fInitialized ) fMenuGen->EnableEntry(M_GEN_PYTHIA+GetEventType());

  if( gJSF->Env()->GetValue("JSFGUI.LastRun",0) == 0 ) {
    fMenuGen->UnCheckEntry(M_GEN_LASTRUN);
    fMenuGenType->DisableEntry(M_GEN_LASTRUNFILE);
  }
  else {
    fMenuGen->CheckEntry(M_GEN_LASTRUN);
    fMenuGenType->EnableEntry(M_GEN_LASTRUNFILE);
  }


  static const Int_t nproc=9;
  Char_t *pname[nproc]={"ZH","gammagammaH", "eeH","nnH",
			"gammaZ", "ZZ","WW","enW","twophoton"};
  Char_t wrkstr[64];
  for(i=M_GEN_PYEV_ZH;i<=M_GEN_PYEV_TWOPHOTON;i++){
    sprintf(wrkstr,"JSFGUI.Pythia.Process.%s",pname[i-M_GEN_PYEV_ZH]);
    if( gJSF->Env()->GetValue(wrkstr,0) ) fPythiaEvtype->CheckEntry(i);
    else  fPythiaEvtype->UnCheckEntry(i);
  }
   
  Int_t imrg=gJSF->Env()->GetValue("JSFGUI.MergeEvent",0);
  if( imrg ) {
    fMenuGen->CheckEntry(M_GEN_MERGE);
    fMenuGenType->EnableEntry(M_GEN_MERGE_DATAFILE);
  }
  else {
    fMenuGen->UnCheckEntry(M_GEN_MERGE);
    fMenuGenType->DisableEntry(M_GEN_MERGE_DATAFILE);
  }

  if( gJSF->Env()->GetValue("JSFGUI.SIMDST.Output",0) ) 
    fMenuGenType->CheckEntry(M_GUI_SIMDSTOUT);
  else     fMenuGenType->UnCheckEntry(M_GUI_SIMDSTOUT);

  if( gJSF->Env()->GetValue("JSFGUI.OutputEventData",0) ) 
    fMenuGenType->CheckEntry(M_GUI_OUTEVENTDATA);
  else     fMenuGenType->UnCheckEntry(M_GUI_OUTEVENTDATA);


  fMenuSimType->UnCheckEntry(M_SIM_QUICKSIM);
  fMenuSimType->UnCheckEntry(M_SIM_JIM);
  fMenuSimType->DisableEntry(M_SIM_JIM);
  Int_t isim=gJSF->Env()->GetValue("JSFGUI.SimulationType",1);
  fMenuSimType->CheckEntry(M_SIM_QUICKSIM+isim-1);


  //***********************
  Char_t wrk[120];
  strncpy(wrk,GetInputFileName(),100);
  if( strcmp(fLFileN->GetText()->GetString(), wrk) != 0 ) {
    fLFileN->SetText(new TGString(wrk));
    fLFileN->SetTextJustify(kTextLeft);
    fLFileN->SetWidth(400);
  }

  strncpy(wrk,GetOutputFileName(),100);
  if( strcmp(fLOFileN->GetText()->GetString(), wrk) != 0 ) {
    fLOFileN->SetText(new TGString(wrk));
    fLOFileN->SetTextJustify(kTextLeft);
    fLOFileN->SetWidth(400);
  }


  //***********************
  if( fMenuUser ) {
    sprintf(wrk,"UserMenuUpdate((TGPopupMenu*)0x%x);",(UInt_t)fMenuUser);
    gROOT->ProcessLine(wrk);
  }
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

   Char_t wrkstr[256];
   EMsgBoxIcon icontype = kMBIconExclamation;
   Int_t retval;
   Int_t buttons=0;

   switch (GET_MSG(msg)) {

      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {

            case kCM_BUTTON:
		DoButtonAction(parm1);
		if( parm1 >= 8000 ) fDemo->ProcessButton(parm1);
		break;

            case kCM_MENUSELECT:
                break;

            case kCM_MENU:
	      if( parm1 > M__FILE_BEGIN && parm1 < M__FILE_END ) {
		   DoFileMenuAction(parm1);  
	      }
	      else if( parm1 > M__CONT_BEGIN && parm1 < M__CONT_END ) {
		   DoRunmodeMenuAction(parm1);
	      }
	      else if( parm1 > M__ANAL_BEGIN && parm1 < M__ANAL_END ) {
		   DoAnalMenuAction(parm1);
	      }
	      else if( parm1 > M__DISP_BEGIN && parm1 < M__DISP_END ) {
		   DoDispMenuAction(parm1);  
	      }
	      else if( parm1 == M_HELP_ABOUT ) {
		     new TGMsgBox(fClient->GetRoot(),this,"About JSF Control Panel", 
"Welcome to JSF Control Panel.
You can display event from this panel,
or start event simulation.",      icontype, buttons, &retval);
	      }
	      else if( parm1 == M_HELP_EVENTDISP) {
		     new TGMsgBox(fClient->GetRoot(), this, "About Event Display", 
"Momentum View shows LTKCL track momentum. 
Track color shows track type.In All view,  
red beam pipe is +Z, blue is -Z, 
cyan is +Xand purple is +Y directions",    icontype, buttons, &retval);
	      }
	      else {
		if( fMenuUser ) {
		  sprintf(wrkstr,"UserMenuProcessMessage((TGPopupMenu*)0x%x,%ld);",
			  (UInt_t)fMenuUser, parm1);
		  gROOT->ProcessLine(wrkstr);
		}
	      }
              break;
            default:
               break;
         }
	 break;

      case kC_TEXTENTRY:

	switch(parm1) {
	  case T_GOTO_EVENT:   fTextEntryStatus[0]=kTRUE; break;
	  case T_ANAL_START:   fTextEntryStatus[1]=kTRUE; break;
	  case T_ANAL_NEVENTS: fTextEntryStatus[2]=kTRUE; break;
	}

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
  Char_t cmd[256];

  if( !fInitialized ) {
       fMenuFile->DisableEntry(M_FILE_OPEN);
       fMenuFile->DisableEntry(M_FILE_OPENOUTPUT);
       fInitialized=kTRUE;
       gROOT->ProcessLine("Initialize();");
       Update();
  }
  sprintf(cmd,"GetEvent(%s);", fTGotoEvent->GetBuffer()->GetString());
  gROOT->ProcessLine(cmd);
  sprintf(evtmsg,"  Event Number: %d\n",gJSF->GetEventNumber());
  fLEventNumber->SetText(new TGString(evtmsg));
}


//_________________________________________________________________
void JSFGUIFrame::AnalizeEventAction()
{
  Char_t evtmsg[40];
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
  Update();
  Int_t i;
  Int_t iFirstEvent=GetFirstEvent();
  Int_t iNEventsAnalize=GetNEventsAnalize();
  fNoOfAnalizedEvents=0;
  Int_t last=iFirstEvent+iNEventsAnalize-1;
  for(i=iFirstEvent;i<=last;i++){
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
    fLEventNumber->SetText(new TGString(evtmsg));

    if( fReturnCode != 0 ) break;
  }
  fNoOfAnalizedEvents=-999;  
  DrawHist();
}


//_________________________________________________________________
void JSFGUIFrame::RunDemo()
{

  if( fDemo == 0 ) fDemo=new JSFDemoDisplay(this); 

  if( !fInitialized ) {
       fMenuFile->DisableEntry(M_FILE_OPEN);
       fMenuFile->DisableEntry(M_FILE_OPENOUTPUT);
       fInitialized=kTRUE;
       gROOT->ProcessLine("Initialize();");
       Update();
  }

  fDemo->SetTimer();

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


//_________________________________________________________________
void JSFGUIFrame::DoButtonAction(Long_t parm1)
{
   // Process Button press event.

   EMsgBoxIcon icontype = kMBIconExclamation;
   Int_t retval;
   Int_t buttons=0;
   Char_t evtmsg[40];
   TGString *tgmsg;
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
       Update();
       break;

     case B_NEXT_EVENT: 
       if( !fInitialized ) {
	 fMenuFile->DisableEntry(M_FILE_OPEN);
	 fMenuFile->DisableEntry(M_FILE_OPENOUTPUT);
	 fInitialized=kTRUE;
	 gROOT->ProcessLine("Initialize();");
	 Update();
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

}


//_________________________________________________________________
void JSFGUIFrame::DoFileMenuAction(Long_t parm1, Bool_t prompt)
{
   // Process Button press event.

   Char_t workstr[256];
   Char_t retstr[120];
   Char_t dirnam[256];
   switch(parm1) {
     case M_FILE_OPEN:
       {
	 strcpy(retstr,GetInputFileName());
	 if( prompt ) {
 	   TGFileInfo fi;
	   fi.fFileTypes = (char **)filetypes;
	   Char_t dirnam[256];
	   strcpy(dirnam,gSystem->WorkingDirectory());
	   new TGFileDialog(fClient->GetRoot(), this, kFDOpen,&fi);
	   if( fi.fFilename != 0 ) {
	     sprintf(workstr,"%s/%s",
		 gSystem->WorkingDirectory(),fi.fFilename);
	     ToRelativePath(workstr, dirnam, retstr);
	     gJSF->Env()->SetValue("JSFGUI.InputFileName",retstr);
	   }
	 }
	 gSystem->ChangeDirectory(dirnam);
	 Int_t lfn=strlen(retstr);
	 if( strcmp(&retstr[lfn-4],"root") == 0 ) SetRunMode(2);
	 else if( strcmp(&retstr[lfn-3],"dat") == 0 ) SetRunMode(3);
	 else if( strcmp(&retstr[lfn-3],"bnk") == 0 ) SetRunMode(4);
       }
       break;

     case M_FILE_OPENOUTPUT:
       strcpy(retstr,GetOutputFileName());
       {
	 TGFileInfo fi;
	 fi.fFileTypes = (char **)filetypes;
	 Char_t dirnam[256];
	 strcpy(dirnam,gSystem->WorkingDirectory());
	 new TGFileDialog(fClient->GetRoot(), this, kFDSave,&fi);
	 if( fi.fFilename != 0 ) {
	   sprintf(workstr,"%s/%s",
		 gSystem->WorkingDirectory(),fi.fFilename);
	   ToRelativePath(workstr,dirnam,retstr);
	   gSystem->ChangeDirectory(dirnam);
	   gJSF->Env()->SetValue("JSFGUI.OutputFileName",retstr);
	 }
       }
       break;

     case M_FILE_STARTBROWSER:
       if( !gbrows ) gbrows=new TBrowser("Browser");
       break;

     case M_FILE_EXIT:
       gROOT->ProcessLine("JobEnd();");
       CloseWindow();   // this also terminates theApp
       break;

   }
   Update();

}



//_________________________________________________________________
void JSFGUIFrame::DoDispMenuAction(Long_t parm1, Bool_t prompt)
{

   Char_t wrk[128];
   switch(parm1) {
     case M_DISP_REDRAW:
       fED->DisplayEventData();
       break;

     case M_DISP_MOMENTUM:
     case M_DISP_ALL:
     case M_DISP_VTX:
       fED->fDisplayType=parm1-M_DISP_MOMENTUM;
       sprintf(wrk,"%d",fED->fDisplayType);
       gJSF->Env()->SetValue("JSFEventDisplay.DisplayType",wrk);
       break;

     case M_DISP_EVENT:
       if( fED->fDrawAtNewEvent ) { fED->fDrawAtNewEvent=kFALSE; }
       else  fED->fDrawAtNewEvent=kTRUE;
       sprintf(wrk,"%d",(Int_t)fED->fDrawAtNewEvent);
       gJSF->Env()->SetValue("JSFEventDisplay.DrawAtNewEvent",wrk);
       break;

     case M_ONOFF_GEOMETRY:
       if( fED->fDrawGeometry ) { fED->fDrawGeometry=kFALSE; }
       else { fED->fDrawGeometry=kTRUE; }
       sprintf(wrk,"%d",(Int_t)fED->fDrawAtNewEvent);
       gJSF->Env()->SetValue("JSFEventDisplay.DrawGeometry",wrk);
       break;

     case M_ONOFF_GENNEUTRAL:
       fED->fGenNeutral->ToggleShow();
       break;

     case M_ONOFF_GENTRACK:
       fED->fGenCharged->ToggleShow();
       break;

     case M_ONOFF_LTKCLTRACK:
       fED->fLTKCLTrack->ToggleShow();
       break;

     case M_ONOFF_CDCTRACK:
       fED->fCDCTrack->ToggleShow();
       break;

     case M_ONOFF_VTXHIT:
       fED->fVTXHit->ToggleShow();
       break;
       
     case M_ONOFF_EMCHIT:
       fED->fEMCHit->ToggleShow();
       break;

     case M_ONOFF_HDCHIT:
       fED->fHDCHit->ToggleShow();
       break;

   }
   Update();

}


//_________________________________________________________________
void JSFGUIFrame::DoAnalMenuAction(Long_t parm1, Bool_t prompt)
{
  //

   Char_t wrkstr[256];
   InputDialog *dialog;

   switch(parm1) {
     case M_ANAL_SHOWHIST:
       if( fShowHist ) gJSF->Env()->SetValue("JSFGUI.ShowHist","0");
       else gJSF->Env()->SetValue("JSFGUI.ShowHist","1");
       break;

     case M_ANAL_SHOWFREQ:
       if( prompt ) {
	 sprintf(wrkstr,"%d",fShowHistFrequency);
	 dialog = new InputDialog("Histgram dispplay rate",wrkstr,wrkstr);
	 if( wrkstr[0]!= 0 )gJSF->Env()->SetValue("JSFGUI.ShowHistFrequency",wrkstr);
       }
       break;

     case M_ANAL_DRAWHIST:
       gROOT->ProcessLine("DrawHist();");
       break;

     case M_ANAL_RESETHIST:
       gROOT->ProcessLine("ResetHist();");
       break;

     case M_ANAL_MACRONAME:
       strcpy(wrkstr,GetMacroFileName());
       dialog= new InputDialog("User Analysis Macro file name", 
			       wrkstr, wrkstr);
       if( wrkstr[0]!=0 ) gJSF->Env()->SetValue("JSFGUI.MacroFileName",wrkstr);
       break;

     case M_ANAL_RELOAD:
       gROOT->LoadMacro(GetMacroFileName());
       break;

   }
   Update();

}

//_________________________________________________________________
void JSFGUIFrame::DoRunmodeMenuAction(Long_t parm1, Bool_t prompt)
{
  //


  Char_t wrkstr[256], retstr[256];
  InputDialog *dialog;
  Int_t retval=0;
  Int_t buttons=0;

  static const Int_t maxmenu=29;
  Char_t *value[maxmenu][3]={
    {"JSFGUI.ECM","300",
"JSFGUI.ECM:
Center of mass energy (GeV) for Pythia Generator."},  // 1
    {"JSFGUI.InitPythiaMacro","InitPythia.C",
"JSFGUI.InitPythiaMacro:
A name of a macro file, which defines a function to initialize Pythia.
A function name must be void InitPythia()"},  // 2
    {"PythiaGenerator.PrintStat","1",
"PythiaGenerator.PrintStat:
NPYSTAT, the argument of Subroutine PYSTAT.
If not 0, PYSTAT is called at the end of run as
CALL PYSTAT(NPYSTAT)."},  // 3

    {"DebugGenerator.RandomSeed","12345",
       "DebugGenerator.RandomSeed:\nSeed of random number"}, // 4 -2 
    {"DebugGenerator.Nparticles","4",
       "DebugGenerator.Nparticles:\nNumber of particles per event."}, //5-2
    {"DebugGenerator.RangeP","0.1  10.0",
       "DebugGenerator.RangeP\nMinimum and maximum of particle's momentum in GeV."}, //6-2
    {"DebugGenerator.RangeCosth","-1.0 1.0",
       "DebugGenerator.RangeCosth:
Minimum and maximum of Cosine of particle production angle."}, //7-2
    {"DebugGenerator.RangeAzimuth","0.0, 360.0",
     "DebugGenerator.RangeAzimuth:
Minimum and maximum of particle's azimuthal angle."}, //8-2
    {"DebugGenerator.RangeVTXR","0.0 0.0",
     "DebugGenerator.RangeVTXR:
Minimum and maximum of radius of particle's 
vertex points in unit of cm."}, // 9- 3 
    {"DebugGenerator.RangeVTXPhi","0.0, 360.0",
"DebugGenerator.RangeVTXPhi:
Minimum and maximum of azimuthal angle of particle's 
vertex coordinate. Note that coordinate of vertex point 
is given by cylindrical coordinate system."}, //10-4
    {"DebugGenerator.RangeVTXZ","0.0, 0.0",
"DebugGenerator.RangeVTXZ:
Minimum and maximum of Z coordinate of vertex coordinate."}, //11-2
    {"DebugGenerator.Nspecies","2",
"DebugGenerator.Nspecies:
Number of species of generated particles"}, //12-2
    {"DebugGenerator.Species1","13 0.1  1.0",
"DebugGenerator.Species1:
ID, mass and charge of first species."}, //13-2
    {"DebugGenerator.Species2","13 0.1  -1.0",
"DebugGenerator.Species2:
ID, mass and charge of first species."}, //14-2
    {"DebugGenerator.Species3","13 0.1  -1.0",
"DebugGenerator.Species3:
ID, mass and charge of first species."}, //15-2
    {"JSFGUI.Spring.SharedLibrary:","../FFbarSpring/libFFbarSpring.so",
"JSFGUI.Spring.SharedLibrary:
A name of shared library for event generation 
by Bases/Spring."}, //16-3
    {"JSFGUI.Spring.ModuleName","FFbarSpring",
"JSFGUI.Spring.ModuleName:
A module name of Spring.  It must be inherited
from a class, JSFModule and defined in the
shared library specified by JSFGUI.Spring.SharedLibrary"}, //17-4
    {"JSFGUI.Spring.BasesFile","../FFbarSpring/bases.root",
"JSFGUI.Spring.BasesFile:
A file name of bases data."}, //18-2
    {"JSFReadParton.DataFile","parton.dat",
"JSFReadParton.DataFile:
Input data file name of JSFReadParton class."}, //19-2
    {"JSFReadParton.Format","1",
"JSFReadParton.Format:
Format of input data. 1 = ASC file, 0=Fortran binary."}, //20-2
    {"JSFReadGenerator.DataFile","genevent.dat",
"JSFReadGenerator.DataFile:
Input file name for JSFReadGenerator class. This 
class read generator data of HEPEVT format."}, //21-3
    {"JSFReadGenerator.Format","HEPEVT",
"JSFReadGenerator.Format:
Data format of generator data.  Value other than HEPEVT
is not recognized, since it is only supported now."}, //22-3
    {"JSFMergeEvent.DataFile","simdst.root",
"JSFMergeEvent.DataFile:
A file name of background data which is used by 
JSFMergeEvent class."}, //23-3
    {"JSFMergeEvent.LumPerTrain","0.06",
"JSFMergeEvent.LumPerTrain:
Luminosity per bunch train in unit of  1/nb 
(nano barn invers).  Default value of 0.06 corresponds 
to collider luminosity of 9x10^{33} / cm^2 sec operated 
at RF pulse frequency of 150 Hz."}, //24-5
      {"JSFMergeEvent.RandomSeed","1990729",
"JSFMergeEvent.RandomSeed:
Seed of random number for JSFMergeEvent class.  Since 
this class uses gRandom variable, it 
will be mixed up, if gRandom is used at another place."}, //25-4
    {"JSFQuickSim.ParameterFile","Undefined",
"JSFQuickSim.ParameterFile:
A file name for Quick simulator's detector parameter.
If it's \"Undefined\" ot not specified, a file,
$LCLIBROOT/simjlc/parm/detect7.com is used."}, //26-4
    {"JSFGUI.RunNo","1",
"JSFGUI.RunNo:
Run number "}, // 27-2
    {"JSFGUI.Pythia.Decay.Z","0",
"JSFGUI.Pythia.Decay.Z:
Decay mode of Z
-1 = Disable all Z decay
 0 = Enable all Z decay
 n = Enable only one decay node of Z,
   1=d-quark, 2=u-quark, 3=s-quark, 4=c-quark, 5=b-quark,
  11=e, 12=nu_e, 13=mu, 14=nu_mu, 15=tau, 16=nu_tau"},  // 28-7
    {"JSFGUI.Pythia.Higgsmass","120.0",
"JSFGUI.Pythia.Higgsmass:
Mass of Higgs used by Pythia.
(note) Branching ratio of Higgs is not correct."}//29-3 						
};


  Int_t nline[maxmenu]={2,3,4, //  3
			  2,2,2,2,2, 3,4,2,2,2,  2,2,  // 12
			  3,4,2,2,2, 3,3,3,5,4, 4,2,7,3 }; //13

  Int_t id[maxmenu]={M_GEN_ECM,M_GEN_INITPYTHIA,
   M_GEN_PYTHIA_PRSTAT, M_GEN_DEBUG_RANDOM,
   M_GEN_DEBUG_NPARTICLE,M_GEN_DEBUG_PRANGE,M_GEN_DEBUG_COSTHRANGE,
   M_GEN_DEBUG_AZIMUTHRANGE,M_GEN_DEBUG_VTXRRANGE,M_GEN_DEBUG_VTXPHIRANGE,
   M_GEN_DEBUG_VTXZRANGE,M_GEN_DEBUG_NSPECIE,M_GEN_DEBUG_SPECIES1,
   M_GEN_DEBUG_SPECIES2,M_GEN_DEBUG_SPECIES3,
   M_GEN_SPRING_SO,M_GEN_SPRING_MODULE_NAME,M_GEN_BASES_FILE,
   M_GEN_RPARTON_DATAFILE,M_GEN_RPARTON_FORMAT,
   M_GEN_RDGEN_DATAFILE,M_GEN_RDGEN_FORMAT,
   M_GEN_MERGE_DATAFILE,M_GEN_MERGE_LUMPERTRAIN,M_GEN_MERGE_SEED,
   M_SIM_QUICKSIM_PARAM, M_GUI_RUNNO, M_GEN_PYEV_DECAY_Z,M_GEN_PYTHIA_HMASS};
   



  Int_t i;

  const char *ftypes1[] = { "ROOT files",    "*.root",
                            0,               0 };
  const char *ftypes2[] = { "JSF Configuration files",    "*.conf",
				"All files","*",
				0,               0 };

  static const Int_t nproc=9;
  Char_t *pname[nproc]={"ZH","gammagammaH", "eeH","nnH",
			"gammaZ", "ZZ","WW","enW","twophoton"};

  switch(parm1) {

    case M_CONT_RT_GENEVENT: 
    case M_CONT_RT_READROOT: 
    case M_CONT_RT_READSIMDST: 
    case M_CONT_RT_READJIM: 
    case M_CONT_RT_USERDEFINE: 
    case M_CONT_RT_JSFJIM: 
       SetRunMode(parm1-M_CONT_RT_GENEVENT+1);
       break;

    case M_GEN_PYTHIA:
    case M_GEN_DEBUG:
    case M_GEN_SPRING:
    case M_GEN_RPARTON:
    case M_GEN_RHEPEVT:
      SetEventType(parm1-M_GEN_PYTHIA);
      break;

    case M_GEN_LASTRUNFILE:
      if( prompt ) {
 	TGFileInfo fi;
	fi.fFileTypes = (char **)ftypes1;
        fi.fFilename = (Char_t*)gJSF->Env()->GetValue("JSFGUI.LastRunFile","");
	Char_t dirnam[256];
	strcpy(dirnam,gSystem->WorkingDirectory());
	new TGFileDialog(fClient->GetRoot(), this, kFDOpen,&fi);
	sprintf(wrkstr,"%s/%s",gSystem->WorkingDirectory(),fi.fFilename);
	ToRelativePath(wrkstr, dirnam, retstr);
	if( strlen(retstr) > 0 ) gJSF->Env()->SetValue("JSFGUI.LastRunFile",retstr);
	gSystem->ChangeDirectory(dirnam);
      }
      break;

    case M_CONT_PARAM_SAVE:
      gJSF->Env()->WriteFile();
      sprintf(wrkstr,
      "New parameters are saved in a file %s.\nPrevious file is renamed as %s.bak.",
      gJSF->Env()->GetEnvFileName(),gJSF->Env()->GetEnvFileName());
      new TGMsgBox(fClient->GetRoot(), this, "JSF Message",
	  wrkstr, kMBIconExclamation, buttons, &retval);
      break;

    case M_CONT_PARAM_SAVEAS:
      TGFileInfo fi;
      fi.fFileTypes = (char **)ftypes2;
      Char_t dirnam[256];
      strcpy(dirnam,gSystem->WorkingDirectory());
      new TGFileDialog(fClient->GetRoot(), this, kFDSave,&fi);
      sprintf(wrkstr,"%s/%s", gSystem->WorkingDirectory(),fi.fFilename);
      if( gJSF->Env()->WriteFile(wrkstr) ) {
        sprintf(retstr,"Current parameters are saved in the file\n%s",wrkstr);
        new TGMsgBox(fClient->GetRoot(), this, "JSF Message",
	    retstr, kMBIconExclamation, buttons, &retval);
      }
      else { 
	sprintf(retstr,"Unable to save parameters in the file \n%s",wrkstr);
        new TGMsgBox(fClient->GetRoot(), this, "JSF Message",
	    retstr, kMBIconExclamation, buttons, &retval);
      }      
      break;

   case M_GEN_MERGE:
      if( fMenuGen->IsEntryChecked(M_GEN_MERGE) ) {
        gJSF->Env()->SetValue("JSFGUI.MergeEvent","0");
      }
      else {
        gJSF->Env()->SetValue("JSFGUI.MergeEvent","1");
      }
      break;

   case M_GEN_LASTRUN:
      if( fMenuGen->IsEntryChecked(M_GEN_LASTRUN) ) {
        gJSF->Env()->SetValue("JSFGUI.LastRun","0");
      }
      else {
        gJSF->Env()->SetValue("JSFGUI.LastRun","1");
      }
      break;

   case M_GUI_SIMDSTOUT:
      if( fMenuGen->IsEntryChecked(M_GUI_SIMDSTOUT) ) {
        gJSF->Env()->SetValue("JSFGUI.SIMDST.Output","0");
      }
      else {
        gJSF->Env()->SetValue("JSFGUI.SIMDST.Output","1");
      }
      break;


   case M_GUI_OUTEVENTDATA:
      if( fMenuGen->IsEntryChecked(M_GUI_OUTEVENTDATA) ) {
        gJSF->Env()->SetValue("JSFGUI.OutputEventData","0");
      }
      else {
        gJSF->Env()->SetValue("JSFGUI.OutputEventData","1");
      }
      break;

   case M_SIM_QUICKSIM:
      gJSF->Env()->SetValue("JSFGUI.SimulationType","1");
      break;

   case M_SIM_JIM:
      gJSF->Env()->SetValue("JSFGUI.SimulationType","2");
      break;

    case M_CONT_PARAM_EDIT:
       new JSFEnvGUIFrame(gClient->GetRoot(), 400, 200);
       break;

    case M_GEN_PYEV_ZH:
    case M_GEN_PYEV_GGH:
    case M_GEN_PYEV_EEH:
    case M_GEN_PYEV_NNH:
    case M_GEN_PYEV_GAMMAZ:
    case M_GEN_PYEV_ZZ:
    case M_GEN_PYEV_WW:
    case M_GEN_PYEV_ENW:
    case M_GEN_PYEV_TWOPHOTON:
      sprintf(wrkstr,"JSFGUI.Pythia.Process.%s",pname[parm1-M_GEN_PYEV_ZH]);
      if( fPythiaEvtype->IsEntryChecked(parm1) ) {
        gJSF->Env()->SetValue(wrkstr,"0");
      }
      else {
        gJSF->Env()->SetValue(wrkstr,"1");
      }
      break;

    default:
      if( prompt ) {
	for(i=0;i<maxmenu;i++){
	  if( id[i] == parm1 ) {
	    strcpy(wrkstr,gJSF->Env()->GetValue(value[i][0],value[i][1]));
	    dialog = new InputDialog(value[i][2],wrkstr,wrkstr,nline[i]);
	    if(wrkstr[0]!=0) gJSF->Env()->SetValue(value[i][0],wrkstr);
	    break;
	  }
	}
      }
      break;

  } 
  Update();

}
