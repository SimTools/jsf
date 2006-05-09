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

/*
TCanvas *cHist;
TDirectory *cDir;
TH1F *hNCDC;
TH1F *hNVTX;
TH1F *hNGen;
*/

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



//_________________________________________________________
/*
Sample function to define multi-input files.
If UserInputFiles() is defined in UserAnalysis.C, string array 
returned from this function is used to define input files.
Example is attached below.
*/
/*
vector<string> *UserInputFiles()
{
  vector<string> *inp=new vector<string>();

  Char_t *fpref=gROOT->LoadMacro(jsf->Env()->GetValue("MyAnal.FileName.Prefix","../../out/uds91-mar06_2m-500-");
  Int_t iini=gROOT->LoadMacro(jsf->Env()->GetValue("MyAnal.FileName.SeqFirst",1);
  Int_t imax=gROOT->LoadMacro(jsf->Env()->GetValue("MyAnal.FileName.SeqLast",20);

  for(Int_t i=iini;i<=imax;i++){
    stringstream sinp;
    sinp << fpref << i << ".root" << std::ends;
    string tstr;
    tstr=sinp.str();
    std::cerr << tstr << std::endl;
    inp->push_back(tstr);
  }
  return inp;
}

*/


