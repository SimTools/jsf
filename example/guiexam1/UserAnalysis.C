//****************************************************
//*
//*  Sample UserAnalysis Script 
//*  
//****************************************************

TH1F *hNCDC;
TH1F *hNVTX;
TH1F *hNGen;
TCanvas *cHist;
TDirectory *cDir;

//_________________________________________________________
void UserInitialize()
{
  if( !hNCDC ) delete hNCDC; 
  if( !hNVTX ) delete hNVTX; 
  if( !hNGen ) delete hNGen; 
  hNCDC=new TH1F("hNCDC","Number of CDC Tracks",10,0,100);
  hNVTX=new TH1F("hNVTX","Number of VTX Hits",100,0,200);
  hNGen=new TH1F("hNGen","Number of Generator Tracks",100,0,200);
  cDir=gDirectory;
}

//_________________________________________________________
void ResetHist()
{
  UserInitialize();
}


//_________________________________________________________
void UserAnalysis()
{
  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();

  hNCDC->Fill((Float_t)sdb->GetNCDCTracks());
  hNVTX->Fill((Float_t)sdb->GetNVTXHits());
  hNGen->Fill((Float_t)sdb->GetNGeneratorParticles());

}


//_________________________________________________________
void DrawHist()
{

  TDirectory *old=gDirectory;
  if( !cHist ) {
    cHist=new TCanvas("cHist","Canvas 1",100, 100, 400, 400);
  } 
  else {
    cHist->cd();
  }
  // hNCDC->Draw();
  // hNVTX->Draw();
  hNGen->Draw();
  cHist->Update();

  old->cd();

}












