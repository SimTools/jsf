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
#include "PythiaGenerator.h"


#include "JSFDemoDisplay.h"
#include "JSFSIMDST.h"
#include "JSFGUIFrame.h"

ClassImp(JSFDemoDisplay)

extern JSFSIMDST *simdst;


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
    printf(" Initialize of demo canvas in progress.\n");
    gROOT->ProcessLine("SoundMessage(-1)");
    fMain = new TGMainFrame(gClient->GetRoot(),600, 600, kHorizontalFrame);
    fMain->SetWindowName("JSF Demo");

    fDisp = new TRootEmbeddedCanvas("EventDisplay",fMain, 550, 550);
    TGLayoutHints *fL3 = new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandY, 5, 5, 5, 5);
    TGLayoutHints *fL4 = new TGLayoutHints(kLHintsCenterX, 5, 5, 5, 5);
    TGLayoutHints *fLDisp = new TGLayoutHints(kLHintsTop | kLHintsLeft ,
                        5, 5, 5, 5);
    TGLayoutHints *lhTL= 
      new TGLayoutHints(kLHintsTop|kLHintsLeft|kLHintsExpandX,5,5,5,5);

    fMain->AddFrame(fDisp,fLDisp);

    fCont = new TGCompositeFrame(fMain,200,750,kVerticalFrame);

    fLogo=new TGIcon(fCont,gClient->GetPicture("JLClogo98a.xpm"),200,67);
    fCont->AddFrame(fLogo,fL3);

    fHist1 = new TRootEmbeddedCanvas("Hist1",fCont,200,120);
    fCont->AddFrame(fHist1,fL4);

    fHist2 = new TRootEmbeddedCanvas("Hist2",fCont,200,120);
    fCont->AddFrame(fHist2,fL4);

    fHist3 = new TRootEmbeddedCanvas("Hist3",fCont,200,120);
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
  //  printf("Start DisplayDemo ... fCanvas =%x\n",(Int_fCanvas);

   if( !fMain ) {
    InitializeDemo();
  }
  else {
    fDisp->GetCanvas()->cd();
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

   Char_t msg[256]="                                                                     ";
   sprintf(msg,"#sqrt{300}GeV : Evt %d : %s",gJSF->GetEventNumber(),
	   GetEventTypeString());
   TLatex *txt=new TLatex(-0.9, -0.9, msg);
   fSignals->Add(txt);

   txt->SetTextSize(0.05);
   txt->SetTextColor(50);

   txt->Draw();

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
  PythiaGenerator *py=(PythiaGenerator*)gJSF->FindModule("PythiaGenerator");
  Int_t itype=py->GetPythia()->GetMSTI(1);

  static  Char_t *processname[400]={0};
  processname[1]="e^{+}e^{-} #rightarrow #gamma/Z^{0} ";
  processname[22]="e^{+}e^{-} #rightarrow Z^{0} Z^{0}";
  processname[24]="e^{+}e^{-} #rightarrow Z^{0} H^{0}";
  processname[25]="e^{+}e^{-} #rightarrow W^{+} W^{-}";
  processname[36]="e^{+}e^{-} #rightarrow e #nu W";
  processname[103]="e^{+}e^{-} #rightarrow e^{+}e{-} H^{0} (#gamma #gamma #rightarrow H^{0}";
  processname[123]="e^{+}e^{-} #rightarrow e^{+}e^{-} H0 (ZZ fusion)";
  processname[124]="e^{+}e^{-} #rightarrow #nu#bar{#nu} H0 (WW fusion)";

  if( processname[itype] != 0 ) return processname[itype];

  static Char_t rtstr[64];
  sprintf(rtstr,"Process id=%d",itype);
  return rtstr;
  
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
"JSF Demo
<<Type of event>>
Standard model processes are generated using Pythia and 
Quick Simulator. Event rates are mixed by proper weight.
<<Event Display>>
Green helixes: CDC tracks
Blue boxes: Elemag calorimeters
Purple boxes: Hadron calorimeters
Central red bar: e+ incident side
Central blue bar: e- incident side
<<Histograms>>
Current data are added to data analized previously.  
[Reset] button clears first two entries.
Higgs mass: ZH->(nu ~nu)+(b ~b) are selected.
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
