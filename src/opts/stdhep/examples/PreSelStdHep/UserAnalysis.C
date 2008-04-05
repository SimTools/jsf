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

MySelection  *anal=0;
JSFReadStdHep *stdhepr=0;
JSFWriteStdHep *stdhepw=0;
JSFLCFULL *lcfull=0;
Int_t nread=0;

//_________________________________________________________
void UserModuleDefine()
{

  TFile *file=new TFile("jsf.root","RECREATE");
  jsf->SetIOFiles();
//  lcfull=new JSFLCFULL();
  stdhepr=new JSFReadStdHep();  // ReadStdHep events

  anal=new MySelection();  // define MyAnalysis

  stdhepw=new JSFWriteStdHep();  // Output events in StdHep format.
  stdhepw->DoConvertJetset(kFALSE);

}


//_________________________________________________________
void UserInitialize()
{
}

//_________________________________________________________
void UserAnalysis()
{
  nread++;
  std::cout << "Event Number is " << gJSF->GetEventNumber() 
   << " Nread=" << nread << std::endl;
}

//_________________________________________________________
void DrawHist()
{
}





