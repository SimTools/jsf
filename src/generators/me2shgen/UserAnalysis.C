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


TFile *ofile;
JSFSteer *jsf;
JSFLCFULL *full;
JSFQuickSim *sim=0;
JSFSpring *spring=0;
JSFSIMDST  *simdst=0;
JSFModule  *gen=0;
JSFHadronizer *hdr=0;
JSFMergeEvent *merge=0;
JSFJ4         *jsfj4=0;
JSFJ4         *gJSFJ4=0;

JSFMEGenerator *mygen;

JSFGenerator *pyhad;

//-----------------------------------------------
void UserModuleDefine()
{

 // Define modules to run Pythia and MakeJIMInput class only.

  //  enum EJSFEventType { kPythia=0, kBasesSpring=2, kReadParton=3};

  Char_t *outputFileName=jsf->Env()->GetValue("JSFGUI.OutputFileName","jsf.root");
  ofile=new TFile(outputFileName,"RECREATE");
  jsf->SetIOFiles();

  full  = new JSFLCFULL();

  mygen=(JSFMEGenerator*)(new JSFReadMEGenerator());

  pyhad=new JSFPythiaHadronizer();
  // pyhad=new JSFSHGenerator();

  if( jsf->Env()->GetValue("JSFGUI.SimulationType",1) == 1 ) {
    sim    = new JSFQuickSim();

    if( jsf->Env()->GetValue("JSFGUI.MergeEvent",0 )) {
      merge=new JSFMergeEvent();
      merge->SetDataFileName(
           jsf->Env()->GetValue("JSFMergeEvent.DataFile","mergeevent.root"));
    }

    simdst = new JSFSIMDST();
    simdst->SetQuickSimParam(sim->Param());
 }

  if( jsf->Env()->GetValue("JSFGUI.SIMDST.Output",0) == 0 && simdst )
    simdst->NoReadWrite();


  if( jsf->Env()->GetValue("JSFGUI.OutputEventData",0) == 0  ) {
    full->SetMakeBranch(kFALSE);
    if(sim) sim->SetMakeBranch(kFALSE);
    if(simdst) simdst->SetMakeBranch(kFALSE);
    if( gen ) gen->SetMakeBranch(kFALSE);
    if( hdr ) hdr->SetMakeBranch(kFALSE);
  }



}


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
  cDir=gDirectory;
  */
}

//_________________________________________________________
void UserAnalysis()
{
  // This function is called when the processing of one event is completed.
  // Any data processing of the event can be performed in this function.
  // 

  /*
  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();

  //  Accumulate information in the histogram
  hNCDC->Fill((Float_t)sdb->GetNCDCTracks());
  hNVTX->Fill((Float_t)sdb->GetNVTXHits());
  hNGen->Fill((Float_t)sdb->GetNGeneratorParticles());
  */

  /*
  cout << mygen->GetName() << endl;

  JSFMEGenerator *megen=(JSFMEGenerator*)mygen;
  JSFMEGeneratorBuf *mebuf=(JSFMEGeneratorBuf*)megen->EventBuf();

  JSFHEPRUP &heprup=*(megen->GetJSFHEPRUP());
  heprup.Print();

  cout << "Run " << mebuf->GetRunNumber() ;
  cout << "Event " << mebuf->GetEventNumber() << endl;
  JSFHEPEUP &hepeup=*(mebuf->GetJSFHEPEUP());
  hepeup.Print();

  cout << "End of UserAnalysis " << endl;
  cout << endl << endl;

  JSFPythiaHadronizer *jsfpy=(JSFPythiaHadronizer*)pyhad;
  TPythia6 *py=jsfpy->GetPythia();
  py->Pylist(2);

  */


  /*
  JSFReadMEGeneratorBuf *mgb=(JSFReadMEGeneratorBuf*)mygen->EventBuf();
  JSFHEPEUP *hepeup=mgb->GetHEPEUP();
  hepeup->Print();
  */

  /*
  **  If these comments are removed, generator particle information
  **  are printed.
  printf(" # Generator Particle is %d\n",sdb->GetNGeneratorParticles());
  TClonesArray *gen=sdb->GetGeneratorParticles();
  for(Int_t i=0;i<sdb->GetNGeneratorParticles();i++){
    JSFGeneratorParticle *g=gen->UncheckedAt(i);
    Int_t ndau=g->GetNDaughter();
    if( ndau != 0 ) continue;
    // printf(" ndau=%d\n",ndau);
    g->ls();
  }
  */

}

//_________________________________________________________
void DrawHist()
{
  //  This function is called to draw histograms during the interactive 
  //  session.  Thus you can see the accumulation of the histogram
  //  interactively.  

  /*
  TDirectory *old=gDirectory;
  if( !cHist ) {
    cHist=new TCanvas("cHist","Canvas 1",100, 100, 400, 400);
  } 
  else {
    cHist->cd();
  }
  hNCDC->Draw();

  old->cd();
  */
}





