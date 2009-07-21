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

JSFReadStdHep *stdhep;

//______
void UserModuleDefine()
{
  TFile *file=new TFile("jsf.root","RECREATE");
  cerr << "UserModuleDefine was called." <<endl;
  stdhep=new JSFReadStdHep();
  jsf->SetIOFiles();

}

//_________________________________________________________
void UserInitialize()
{

}

//_________________________________________________________
void UserAnalysis()
{
  std::cout << "Event No. " << gJSF->GetEventNumber() << std::endl;
  if( gJSF->GetEventNumber() < 10) {
    stdhep=(JSFReadStdHep*)gJSF->FindModule("JSFReadStdHep");
    JSFReadStdHepBuf *buf=(JSFReadStdHepBuf*)stdhep->EventBuf();
    buf->GetHEPEV4().Print();
  }


//  if ( gJSF->GetEventNumber() < 5 ) {
    JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
    JSFGeneratorBuf *gbuf=(JSFGeneratorBuf*)gen->EventBuf();
    std::cerr << " GeneratorParticles=" << gbuf->GetNParticles() << std::endl;
 
    TClonesArray *gp=gbuf->GetParticles();
    JSFGeneratorParticle *p=0;
    for( Int_t i=0;i<gbuf->GetNParticles();i++) {
      p=(JSFGeneratorParticle*)gp->UncheckedAt(i);
      p->ls();
    }
//  }

}

//_________________________________________________________
void DrawHist()
{

}





