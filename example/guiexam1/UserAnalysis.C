//****************************************************
//*
//*  Sample UserAnalysis Script 
//*  
//*$Id$
//*
//****************************************************

TCanvas *cHist;
TDirectory *cDir;
TH1F *hNCDC;
TH1F *hNVTX;
TH1F *hNGen;

//_________________________________________________________
void UserInitialize()
{
  //  This function is called at the begining of the job or when
  //  "reset hist" action is selected in the gui menu.
  //  This is used to define/reset histograms.

  if( !hNCDC ) delete hNCDC; 
  if( !hNVTX ) delete hNVTX; 
  if( !hNGen ) delete hNGen; 
  hNCDC=new TH1F("hNCDC","Number of CDC Tracks",10,0,100);
  hNVTX=new TH1F("hNVTX","Number of VTX Hits",100,0,200);
  hNGen=new TH1F("hNGen","Number of Generator Tracks",100,0,200);
  cDir=gDirectory;
}

//_________________________________________________________
void UserAnalysis()
{
  // This function is called when the processing of one event is completed.
  // Any data processing of the event can be performed in this function.
  // 

  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();

  //  Accumulate information in the histogram
  hNCDC->Fill((Float_t)sdb->GetNCDCTracks());
  hNVTX->Fill((Float_t)sdb->GetNVTXHits());
  hNGen->Fill((Float_t)sdb->GetNGeneratorParticles());

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

  TDirectory *old=gDirectory;
  if( !cHist ) {
    cHist=new TCanvas("cHist","Canvas 1",100, 100, 400, 400);
  } 
  else {
    cHist->cd();
  }
  hNCDC->Draw();

  old->cd();
}

//_________________________________________________________
void UserSetOptions()
{
  // This function is called only once, soon after jsf is started.
  // This function can be used to define parameters which is not 
  // defined in jsf.conf file.

}

//_________________________________________________________
void UserTerminate()
{
  // This function is called at the end of job.

}





