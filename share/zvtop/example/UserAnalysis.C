//****************************************************
//*
//*  Dummy UserAnalysis Script to be used with gui.C 
//*  Functions in this scripts are called by GUIMainMacro.C  
//*  
//*  In this script, global functions, UserInitialize() nad UserAnalysis()
//*  Must be defined.  UserInitialize() is called at job initialization 
//*  to initialize histograms, etc.  It is also called when reset historgam
//*  menu is selected from JSF Control Panel.  UserAnalysis() is called 
//*  during the event loop after executing Process functions of JSF modules
//*  and display event daia but before display histogram.  See GetEvent()
//*  function in GUIMainMacro.C, where UserAnalysis() is called.
//*
//*  UserSetOptions() and UserTerminate() may be defined in this file.
//*  UserSetOptions() are called before declaration of JSF modules.
//*  It can be used to set parameters optional for user analsis.
//*  UserTerminate() is called during the JSF termination process.
//*
//*  When runmode=4, UserModuleDefine() must be defined in this file.
//*  It is used to define JSF modules specific to user analysis.  
//*  
//*$Id$
//*
//****************************************************

TCanvas *cHist=0;
TDirectory *cDir=0;

//_________________________________________________________
void UserInitialize()
{
  //  This function is called at the begining of the job or when
  //  "reset hist" action is selected in the gui menu.
  //  This is used to define/reset histograms.
  /*
  if( !hNCDC ) delete hNCDC; 
  if( !hNVTX ) delete hNVTX; 
  if( !hNGen ) delete hNGen; 
  hNCDC=new TH1F("hNCDC","Number of CDC Tracks",10,0,100);
  hNVTX=new TH1F("hNVTX","Number of VTX Hits",100,0,200);
  hNGen=new TH1F("hNGen","Number of Generator Tracks",100,0,200);
  */
  cDir=gDirectory;
}

//_________________________________________________________
void UserAnalysis()
{
  // This function is called when the processing of one event is completed.
  // Any data processing of the event can be performed in this function.
  // 


  Int_t ievt=jsf->GetEventNumber();
  /*
  PythiaGenerator *pythia=(PythiaGenerator*)jsf->FindModule("PythiaGenerator");
  Int_t ivers=pythia->GetVersionNumber();
  if( ivers <= 5 ) {
    TPythia *py=pythia->GetPythia();
  }
  else {
    TPythia6 *py=pyythia->GetPythia();
  }
  */
  //  if( ievt > 90 ) { printf(" Event#=%d\n",ievt); }

  if( ievt%10 == 1 ) {
    printf(" Event#%d",ievt);
    printf("\n");
  }
}

//_________________________________________________________
void DrawHist()
{
  //  This function is called to draw histograms during the interactive 
  //  session.  Thus you can see the accumulation of the histogram
  //  interactively.  

  printf(" DrawHist is called.\n");
  TDirectory *old=gDirectory;
  if( !cHist ) {
    cHist=new TCanvas("cHist","Canvas 1",100, 100, 800, 800);
    cHist->Divide(2,2);
  } 
  else {
    cHist->cd();
  }

  // cHist->cd(1);  ((TH1F*)gROOT->FindObject("hEvis"))->Draw();
  //  cHist->cd(2);  ((TH1F*)gROOT->FindObject("hNpart"))->Draw();
  // cHist->cd(3);  ((TH1F*)gROOT->FindObject("hNjets"))->Draw();
  //  cHist->cd(4);  ((TH1F*)gROOT->FindObject("hJetCosTheta"))->Draw();
  cHist->cd(1);  ((TH1F*)gROOT->FindObject("hNVertices"))->Draw();
  cHist->cd(2);  ((TH1F*)gROOT->FindObject("hVmass"))->Draw();
  cHist->cd(3);  ((TH1F*)gROOT->FindObject("hDecayLength"))->Draw();
  cHist->cd(4);  ((TH1F*)gROOT->FindObject("hMSPTM"))->Draw();

  cHist->cd();
  cHist->Update();
  old->cd();

}

JSFVTXAnalysis *vtxanal=0;

//***********************************************************
void UserAnotherModules()
{
  vtxanal=new JSFVTXAnalysis();
}
  


