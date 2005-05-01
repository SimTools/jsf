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

TH1F *hThrust;
TH1F *hPtSum;
TCanvas *cHist;
//_________________________________________________________
void UserInitialize()
{
  //  This function is called at the begining of the job or when
  //  "reset hist" action is selected in the gui menu.
  //  This is used to define/reset histograms.

  cHist=new TCanvas("cHist","Canvas 1",100, 100, 400, 400);
  cHist->Divide(2,1);
  hThrust=new TH1F("hThrust","Thrust",100, 0.0, 1.0);
  hPtSum=new TH1F("hPtSum","PtSum",100, 0.0, 100.0);

}

//_________________________________________________________
void UserAnalysis()
{

  Int_t nevt=jsf->GetEventNumber();
  if( nevt%100 == 1 ) {
    cout << "Event #=" << nevt << endl;
  }

  JSFGenerator *gen=jsf->FindModule("JSFGenerator");
  JSFGeneratorBuf *buf=(JSFGeneratorBuf*)gen->EventBuf();

  //
  TClonesArray *genp=buf->GetParticles();
  ANL4DVector  sum;
  TObjArray    particles(1000);

  Int_t ntracks=0;
  Int_t nc=0;
  TIter next(genp);
  JSFGeneratorParticle *gp;
  while((gp=(JSFGeneratorParticle*)next())) {
    if( gp->GetNDaughter() > 0 ) continue;
    ANL4DVector *p=new  ANL4DVector(gp->GetE(), gp->GetPx(), 
		  gp->GetPy(), gp->GetPz());
    particles.Add(p);
    
    if( TMath::Abs(gp->GetID())==12 ||  TMath::Abs(gp->GetID())==14 || 
	TMath::Abs(gp->GetID())==16 ) continue; 
    sum+=(*p);   // Sum 4 momentum other than neutrino

  }
 
  //
  ANLEventShape evshape;
  evshape.Initialize(particles);
  
  hThrust->Fill(evshape.GetThrust());
  hPtSum->Fill(sum.GetPt());

  particles.Delete();  // Delete p objects in particles
}

//_________________________________________________________
void DrawHist()
{
  //  This function is called to draw histograms during the interactive 
  //  session.  Thus you can see the accumulation of the histogram
  //  interactively.  

  TDirectory *old=gDirectory;
  cHist->cd(1) ; hThrust->Draw();
  cHist->cd(2) ; hPtSum->Draw();
  cHist->Modified();
  cHist->Update();

  old->cd();

}





