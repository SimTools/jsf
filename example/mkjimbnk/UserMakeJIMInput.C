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

MakeJIMInput  *anal=0;
PythiaGenerator *py=0;
TFile *ofile=0;

/*
//_________________________________________________________
void UserAnotherModules()
{

  amal=new MakeJIMInput();  // define MakeJIMInput

}
*/

//_________________________________________________________
void UserModuleDefine()
{
  // Define modules to run Pythia and MakeJIMInput class only.

  Char_t *outputFileName=jsf->Env()->GetValue("JSFGUI.OutputFileName",
                                              "jsf.root");

  ofile=new TFile(outputFileName,"RECREATE");
  jsf->SetIOFiles();

  py=new PythiaGenerator();
  anal=new MakeJIMInput();


  py->SetMakeBranch(kFALSE);

  gROOT->LoadMacro(jsf->Env()->GetValue("JSFGUI.InitPythiaMacro","InitPythia.C"));
  py->SetEcm(jsf->Env()->GetValue("JSFGUI.ECM",300.0));
  InitPythia();         // Set Pythia parameters.


}

//_________________________________________________________
void UserInitialize()
{
}

//_________________________________________________________
void UserAnalysis()
{

  if( jsf->GetEventNumber()%10 == 1 ) {
    printf(" Event #%d\n",jsf->GetEventNumber());
  }

}

//_________________________________________________________
void DrawHist()
{
}





