//****************************************************
//*
/*
  Simple UserAnalysis Script.
  This example creates a ntuple, which contains
    (1) Number of generatorparticles(ngen)
    (2) Total energies of charged tracks(ech), gammas(egam), and neutral hadrons(enh)
        from generator particle information
    (3) Total observed energies(evis) and missing Pt(ptmis).

  To run this program interactively, do
    $ jsf gui.C
  The JSF control panel will pops up and you will set various parameters interactively.
  Select [File] -> [Start Browser ..], then ROOT's Browser pops up and ntuples can be 
  accessed during the interactive execution.

  To run this program as a batch job, do
    $ jsf -b -q --maxevt=1000 gui.C
  It will run 1000 events and results are written in a file, jsf.root.

*/
//*     
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


TNtuple *ntuple;

//_________________________________________________________
void UserInitialize()
{
  //  This function is called at the begining of the job or when
  //  "reset hist" action is selected in the gui menu.
  //  This is used to define/reset histograms.

  ntuple = new TNtuple("ntuple","Event Information",
        "ngen:ech:egam:enh:evis:ptmis");
}

//_________________________________________________________
void UserAnalysis()
{
  // This function is called when the processing of one event is completed.
  // Any data processing of the event can be performed in this function.
  // 

  if ( gJSF->GetEventNumber()%100 == 0 ) {
    cerr << "Passed " << gJSF->GetEventNumber() << endl;
  }

  JSFSIMDST    *simdst=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();

  Double_t ech=0;
  Double_t egam=0;
  Double_t enh=0;
  Double_t ngen=sdb->GetNGeneratorParticles();

// Accumulate ech, egam, enh
  TClonesArray *gen=sdb->GetGeneratorParticles();
  TIter nextgp(gen);
  JSFGeneratorParticle *gp=0;
  while( (gp=(JSFGeneratorParticle*)nextgp() ) ) {
    if ( gp->GetNDaughter() > 0 ) continue;
    Int_t apid=TMath::Abs(gp->GetID());
    if ( apid == 12 || apid == 14 || apid == 16 ) continue;
    if ( gp->GetID() == 22 ) {
       egam += gp->GetE();
    }
    else if ( TMath::Abs(gp->GetCharge()) > 0.1 ) {
       ech += gp->GetE();
    }
    else {
       enh += gp->GetE();
    }
  }

// Use QuickSim results
  TObjArray *pcols=sdb->GetLTKCLTracks();
  TIter nextsp(pcols);
  JSFLTKCLTrack *sp=0;
  ANL4DVector   psum(0.0, 0.0, 0.0, 0.0);
  while ( (sp=(JSFLTKCLTrack*)nextsp() ) ) {
    ANL4DVector p(sp->GetE(), sp->GetPx(), sp->GetPy(), sp->GetPz());
    psum += p;
  }
  Double_t evis = psum.E();
  Double_t ptmis = psum.GetPt();

  ntuple->Fill(ngen,ech,egam,enh,evis,ptmis);


}

//_________________________________________________________
void DrawHist()
{
  //  This function is called to draw histograms during the interactive 
  //  session.  Thus you can see the accumulation of the histogram
  //  interactively.  


}





