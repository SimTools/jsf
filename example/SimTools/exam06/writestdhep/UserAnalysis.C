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

JSFWriteStdHep *stdhep;

//______
void UserAnotherModules()
{
  cerr << "UserModuleDefine was called." <<endl;
  stdhep=new JSFWriteStdHep();

}

//_________________________________________________________
void UserInitialize()
{
}

//_________________________________________________________
void UserAnalysis()
{
  std::cout << "Event No. " << gJSF->GetEventNumber() << std::endl;

  if( gJSF->GetEventNumber() < 5 ) {
    Double_t sesum=0.0;
    Double_t spxsum=0.0;
    Double_t spysum=0.0;
    Double_t spzsum=0.0;
    Int_t spncall=0;
    JSFSpring *spr=(JSFSpring*)gJSF->FindModule("JSFSpring");
    JSFSpringBuf *sbuf=(JSFSpringBuf*)spr->EventBuf();
    TClonesArray *spa=sbuf->GetPartons();
    JSFSpringParton *sp=0;
    for(Int_t i=0;i<sbuf->GetNpartons() ; i++) {
	sp=(JSFSpringParton*)spa->UncheckedAt(i);
        if( sp->GetNDaughter() == 0 ) {
          sesum+=sp->GetE();
	  spxsum+=sp->GetPx();
	  spysum+=sp->GetPy();
	  spzsum+=sp->GetPz();
          spncall++;
        }
    }
    std::cerr << " Spring ncall=" << spncall 
	<< "Esum=" << sesum << " Pxsum=" << spxsum
	<< " Pysum=" << spysum << " Pzsum=" << spzsum
	<< std::endl;


    JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
    JSFGeneratorBuf *gbuf=(JSFGeneratorBuf*)gen->EventBuf();
    std::cerr << " # of GeneratorParticles=" << gbuf->GetNParticles() << std::endl;

    Double_t esum=0.0;
    Double_t pxsum=0.0;
    Double_t pysum=0.0;
    Double_t pzsum=0.0;
    Int_t nsum=0;
    TClonesArray *gp=gbuf->GetParticles();
    JSFGeneratorParticle *p=0;
    for( Int_t i=0;i<gbuf->GetNParticles();i++) {
      p=(JSFGeneratorParticle*)gp->UncheckedAt(i);
      if( p->GetNDaughter() == 0 ) {
//       if( p->GetStatus()==1 ) {
	nsum++;
        esum+=p->GetE();
        pxsum+=p->GetPx();
        pysum+=p->GetPy();
        pzsum+=p->GetPz();      
        p->ls();
      }

   }
   std::cerr << " nsum=" << nsum << "Sum E=" << esum << " SumPx=" << pxsum 
	<< " SumPy=" << pysum << " SumPz=" << pzsum << std::endl;

 }

}

//_________________________________________________________
void DrawHist()
{

}





