//*LastUpdate :  jsf-1-12  22-August-1999  By Akiya Miyamoto
//*-- Author  : A.Miyamoto  22-August-1999


///////////////////////////////////////////////////////////////////
//
// JSFDemoDisplay
//
// Display event and histogram for demo
//
//$Id$
//
///////////////////////////////////////////////////////////////////

#include <TSystem.h>
#include <TLatex.h>
#include "JSFSteer.h"
#include "JSFGenerator.h"


#include "JSFDemoDisplay.h"
#include "JSFSIMDST.h"
#include "JSFGUIFrame.h"

ClassImp(JSFDemoDisplay)

extern JSFSIMDST *simdst;

#if 0
static const Int_t mainwindowXsize=600;
static const Int_t mainwindowYsize=600;
static const Int_t evdispXsize=550;
static const Int_t evdispYsize=550;
static const Int_t legodispXsize=200;
static const Int_t legodispYsize=67;
static const Int_t histXsize=200;
static const Int_t histYsize=120;
#else
static const Int_t mainwindowXsize=830;
static const Int_t mainwindowYsize=785;
static const Int_t evdispXsize=780;
static const Int_t evdispYsize=735;
static const Int_t legodispXsize=200;
static const Int_t legodispYsize=150;
static const Int_t histXsize=200;
static const Int_t histYsize=140;
#endif

//---------------------------------------------------------------------------
JSFDemoDisplay::JSFDemoDisplay(JSFGUIFrame *gui): JSFEventDisplay(gui)
{
  fMain=NULL ; 
  fDisp=NULL; 
  fTimer=NULL; 
  fNCall=0;
}

//---------------------------------------------------------------------------
JSFDemoDisplay::~JSFDemoDisplay()
{
  if( fTimer ) delete fTimer;
  if( fHist1 ) delete fHist1;
  if( fHist2 ) delete fHist2;
  if( fHist3 ) delete fHist3;
  if( fDisp  ) delete fDisp ;
  if( fMain  ) delete fMain ;
}



//---------------------------------------------------------------------------
void JSFDemoDisplay::InitializeDemo()
{
  //    fCanvas = new TCanvas("EventDisplay","EventDisplay",fCanvasSize[0],fCanvasSize[1]);
    gROOT->ProcessLine("SoundMessage(-1)");
    fMain = new TGMainFrame(gClient->GetRoot(),
		    mainwindowXsize, mainwindowYsize, kHorizontalFrame);
    fMain->SetWindowName("JSF Demo");

    fDisp = new TRootEmbeddedCanvas("EventDisplay",fMain, 
				    evdispXsize, evdispYsize);
    TGLayoutHints *fL3 = new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandY, 5, 5, 5, 5);
    TGLayoutHints *fL4 = new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5);
    TGLayoutHints *fLDisp = new TGLayoutHints(kLHintsTop | kLHintsLeft ,
                        5, 5, 5, 5);
    TGLayoutHints *lhTL= 
      new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandX,5,5,5,5);

    fMain->AddFrame(fDisp,fLDisp);

    fCont = new TGCompositeFrame(fMain,200,750,kVerticalFrame);

    fLogo=new TGIcon(fCont,gClient->GetPicture(
      gJSF->Env()->GetValue("JSFDEMO.Logofile","JLClogo98a.xpm")),
		     legodispXsize, legodispYsize);
    fCont->AddFrame(fLogo,fL3);

    fHist1 = new TRootEmbeddedCanvas("Hist1",fCont,histXsize, histYsize);
    fCont->AddFrame(fHist1,fL4);

    fHist2 = new TRootEmbeddedCanvas("Hist2",fCont,histXsize, histYsize);
    fCont->AddFrame(fHist2,fL4);

    fHist3 = new TRootEmbeddedCanvas("Hist3",fCont,histXsize, histYsize);
    fCont->AddFrame(fHist3,fL4);


    Int_t i;
    fUpbuttons= new TGCompositeFrame(fCont,10,10,kHorizontalFrame);
    fButtons[0]= new TGTextButton(fUpbuttons, "Step", 8000);
    fButtons[1]= new TGTextButton(fUpbuttons, "X3D", 8001);
    fButtons[2]= new TGTextButton(fUpbuttons, "Reset", 8002);
    fButtons[3]= new TGTextButton(fUpbuttons, "About", 8003);
    for(i=0;i<4;i++){
      fButtons[i]->Associate((TGMainFrame*)GetGUIMain());
      fUpbuttons->AddFrame(fButtons[i],lhTL);
    }
    fDownbuttons= new TGCompositeFrame(fCont,10,10,kHorizontalFrame);
    fButtons[4]= new TGTextButton(fDownbuttons, "Start", 8004);
    fButtons[5]= new TGTextButton(fDownbuttons, "Stop", 8005);
    fButtons[6]= new TGTextButton(fDownbuttons, "    ", 8006);
    fButtons[7]= new TGTextButton(fDownbuttons, "Quit", 8007);
    for(i=4;i<8;i++){
      fButtons[i]->Associate((TGMainFrame*)GetGUIMain());
      fDownbuttons->AddFrame(fButtons[i],lhTL);
    }
    fCont->AddFrame(fUpbuttons,lhTL);
    fCont->AddFrame(fDownbuttons,lhTL);

    fMain->AddFrame(fCont,fL4);

    fDisp->GetCanvas()->cd();

    fDisp->Layout();
    fCont->Layout();
    //    fDisp->GetCanvas()->SetBorderMode(0);
    fCanvasDirectory=gDirectory;

    fMain->MapSubwindows();
    fMain->Resize(fMain->GetDefaultSize());
    fMain->Layout();
    fMain->MapWindow();

    gROOT->ProcessLine("SoundMessage(-2)");

}

//---------------------------------------------------------------------------
void JSFDemoDisplay::DisplayDemo()
{
  // Display Event
  if( !fMain ) {
     InitializeDemo();
   }
  else {
    // fDisp->GetCanvas()->cd();
    gPad->Clear();
    ///    fMain->Clear();

    fDisp->Clear();
    Clear();
  } 
  TDirectory *olddir=gDirectory;
  fSignals = new TList();

  fCanvasDirectory->cd();

  simdst=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST");

  fDisp->GetCanvas()->cd();
  fDisp->GetCanvas()->Clear();
  fView   = new TView(fViewNo);
  Int_t ierr;
  fView->SetView(fViewAngle[0], fViewAngle[1], fViewAngle[2], ierr);
  fView->SetRange(&fViewRange[fDisplayType][0], &fViewRange[fDisplayType][3]);
  fSignals->Add(fView);

  if( fDrawGeometry ) DrawGeometry(fDisplayType);

   switch (fDisplayType) {
     case 0:
      if( fLTKCLTrack->fShow ) DisplayLTKCLMomentum();
      break;
     case 1:
      if( fEMCHit->fShow )     DisplayEMCHits();
      if( fHDCHit->fShow )     DisplayHDCHits();
      if( fCDCTrack->fShow )   DisplayCDCTracks();
      //      if( fLTKCLTrack->fShow )   DisplayLTKCLTracks();
      if( fVTXHit->fShow )     DisplayVTXHits();
      if( fGenCharged->fShow || fGenNeutral->fShow) 
	DisplayGeneratorParticles();
      break;
     case 2:
      if( fCDCTrack->fShow )   DisplayCDCTracks();
      if( fVTXHit->fShow )     DisplayVTXHits();
      if( fGenCharged->fShow || fGenNeutral->fShow ) DisplayGeneratorParticles();
       break;
   }

   //   printf(" process name is %s\n",GetEventTypeString());
   /*
   Char_t msg[256]="                                                                     ";
   sprintf(msg,"#sqrt{300}GeV : Evt %d : %s",gJSF->GetEventNumber(),
	   GetEventTypeString());
   TLatex *txt=new TLatex(-0.9, -0.9, msg);
   fSignals->Add(txt);

   txt->SetTextSize(0.05);
   txt->SetTextColor(50);

   txt->Draw();
   */


  gROOT->ProcessLine("DrawCanvas()");

  fDisp->GetCanvas()->Update();


  fHist1->GetCanvas()->cd();
  gROOT->ProcessLine("DrawHist(1);");
  fHist1->GetCanvas()->Update();
  fHist2->GetCanvas()->cd();
  gROOT->ProcessLine("DrawHist(2);");
  fHist2->GetCanvas()->Update();
  fHist3->GetCanvas()->cd();
  gROOT->ProcessLine("DrawHist(3);");
  fHist3->GetCanvas()->Update();
 
  if( fCanvasDirectory != olddir ) olddir->cd();

  gROOT->ProcessLine("SoundMessage()");

}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
const Char_t *JSFDemoDisplay::GetEventTypeString()
{
  JSFGenerator *ch=dynamic_cast<JSFGenerator*>(gJSF->FindModule("JSFGenerator"));
  return ch->GetEventTypeString();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void JSFDemoDisplay::SetTimer()
{
  fTimer=new TTimer(this, (Long_t)1000, kTRUE);
  gSystem->AddTimer(fTimer);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Bool_t JSFDemoDisplay::HandleTimer(TTimer *timer)
{
  // 
  if( timer ) timer->Reset();
  fNCall++;
 
  if( gJSF->Process(fNCall)) {

    gROOT->ProcessLine("UserAnalysis();");

    DisplayDemo();

  }
  

  return kTRUE;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void JSFDemoDisplay::ProcessButton(Long_t parm1)
{

   Int_t retval;
   Int_t buttons=0;
   EMsgBoxIcon icontype = kMBIconExclamation;

  switch(parm1) {
    case 8007:  // Quit
      gROOT->ProcessLine("SoundMessage(-99)");
      gROOT->ProcessLine("JobEnd();");
      GetGUIMain()->CloseWindow();   // this also terminates theApp
      break;

    case 8005:  // Stop
      fTimer->TurnOff();
      break;

    case 8004:  // Start
      fTimer->TurnOn();
      break;

    case 8003:  // Help
      new TGMsgBox(gClient->GetRoot(), GetGUIMain(),  "JSF Message", 
"JSF Demo\n\
<<Type of event>>\n\
Standard model processes are generated using Pythia and \n\
Quick Simulator. Event rates are mixed by proper weight.\n\
<<Event Display>>\n\
Green helixes: CDC tracks\n\
Blue boxes: Elemag calorimeters\n\
Purple boxes: Hadron calorimeters\n\
Central red bar: e+ incident side\n\
Central blue bar: e- incident side\n\
<<Histograms>>\n\
Current data are added to data analized previously.  \n\
[Reset] button clears first two entries.\n\
Higgs mass: ZH->(nu ~nu)+(b ~b) are selected.\n\
",
              icontype, buttons, &retval);
      break;

    case 8002:  // Reset
      gROOT->ProcessLine("ResetHist();");
      break;

    case 8001:  // X3D
      fDisp->GetCanvas()->x3d();
      break;

    case 8000:  // Step
      HandleTimer();
      break;
      
  }
}
