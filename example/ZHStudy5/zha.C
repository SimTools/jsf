// *************************************************
// zha.C
//  
// Read the simulated data of the e+e- -> ZH process 
// and select Z->nu+bar(nu) and h-> 2jets decay mode,
// and plots the invariant mass of 2 jets system
// If this script is executed interactively, opens a 
// canvas to draw histograms while analizing the data.
//
// *************************************************

Int_t maxevt=20000;             // Number of event to simulate
Char_t *outputfile="anal.root";  // A file to output histograms
Char_t *inputfile="zhsim.root";  // Input simulator file.

Float_t xEvis=170.0 ;         // Maximum visible energy
Float_t xPtmiss=20.0 ;        // Ptmiss minimum;
Int_t   xNumjet=2;            // Number of jets
Float_t xLeptonP=2.0;         // Lepton P maximum
Float_t xCosth=0.85;          // jet Costh maximum
Float_t xHmass1=100.0;        // Higgs mass range to count number of        
Float_t xHmass2=116.0;        //     candidates
Float_t fitMin=104.0;         // Hmass fit range(minimum)
Float_t fitMax=114.0;          // Hmass fit range(maximum)

static Int_t nGcall=0;         // Number of good event;
Char_t selName[20][100];       // Character array to save selection name

class JSFSteer;

TFile *file;
TCanvas *c1;
JSFSteer *jsf;
TH1F *hHmass, *hMissmass, *hPtmiss, *hStat, *hEvis, *hNjet; 


//*************************************************
//* Initialize histogram
//*************************************************
Int_t InitHist()
{
  file->cd();
  c1=new TCanvas("c1","Canvas 1",100, 100, 800, 800);

  hStat     = new TH1F("hStat", "Selection Statistics",20,0,20);
  hEvis     = new TH1F("hEvis","Visible energy",100,0.0, 500.0);
  hMissmass = new TH1F("hMissmass","Missing Mass",100,-50.0, 250.0);
  hPtmiss = new TH1F("hPtmiss","Missing Pt",100,0.0, 100.0);
  hHmass  = new TH1F("hHmass", "Higgs mass", 100, 60.0, 160.0);
  hNjet   = new TH1F("hNjet", "Number of jets",20,0.0,20.0);
  c1->Divide(2,2);
  return 0;
}

// *****************************************
// Draw Histogram during event analysis
// *****************************************
void HDraw()
{
  file->cd();
  c1->cd(1);
  hStat->Draw();
  c1->cd(2);
  hEvis->Draw();
  c1->cd(3);
  hMissmass->Draw();
  c1->cd(4);
  hHmass->Draw();
  c1->Update();
}  

// *****************************************
// Accumulate hisstogram
// *****************************************
Bool_t HAccumulate()
{
  Char_t msg[60];
  MyAnalysis *anal=(MyAnalysis*)jsf->FindModule("MyAnalysis");
  MyAnalysisBuf *buf=(MyAnalysisBuf*)anal->EventBuf();

  Float_t selid=-0.5;
  hStat->Fill(++selid);
  if( nGcall == 0 ) strcpy(&selName[(Int_t)selid][0],"Total event\00");

  Double_t *event4V=buf->GetEvent4V();
  hEvis->Fill(event4V[0]);

  // Cut by visible energy
  if( event4V[0] > xEvis )       return kFALSE; 
  hStat->Fill(++selid);
  if( nGcall == 0 ) {
    sprintf(msg,"Visible Energy < %g (GeV)\00", xEvis);
    strcpy(&selName[(Int_t)selid][0],msg);
  }
  hMissmass->Fill(buf->GetMissmass());
  hPtmiss->Fill(buf->GetPtmiss());

  // Cut by missing Pt
  if( buf->GetPtmiss() < xPtmiss )  return kFALSE ;
  hStat->Fill(++selid);
  if( nGcall == 0 ) {
    sprintf(msg,"Missing Pt > %g (GeV)\00", xPtmiss);
    strcpy(&selName[(Int_t)selid][0],msg);
  }

  // Cut by number of jet
  hNjet->Fill(buf->GetNumjet());
  if( buf->GetNumjet() != xNumjet )    return kFALSE ;
  hStat->Fill(++selid);
  if( nGcall == 0 ) {
    sprintf(msg,"# jets = 2 (ycut=%g)\00", anal->GetYcut());
    strcpy(&selName[(Int_t)selid][0],msg);
  }

  // Electron/Muon P < 2.0 
  if( buf->GetMaxEP() > xLeptonP )    return kFALSE ;
  hStat->Fill(++selid);
  if( nGcall == 0 ) {
    sprintf(msg,"Electron Momentum < %g (GeV)\00", xLeptonP);
    strcpy(&selName[(Int_t)selid][0],msg);
  }
  if( buf->GetMaxMuP() > xLeptonP )   return kFALSE ;
  hStat->Fill(++selid);
  if( nGcall == 0 ) {
    sprintf(msg,"Muon Momentum < %g (GeV)\00", xLeptonP);
    strcpy(&selName[(Int_t)selid][0],msg);
  }

  // Costh of jets
  if( TMath::Abs(buf->GetCos1()) > xCosth ) return kFALSE;
  hStat->Fill(++selid);
  if( nGcall == 0 ) {
    sprintf(msg,"|Costh-jet1| < %g\00",xCosth);
    strcpy(&selName[(Int_t)selid][0],msg);
  }
  if( TMath::Abs(buf->GetCos2()) > xCosth ) return kFALSE;
  hStat->Fill(++selid);
  if( nGcall == 0 ) {
    sprintf(msg,"|Costh-jet2| < %g\00",xCosth);
    strcpy(&selName[(Int_t)selid][0],msg);
  }

  // Count number of candidates in the suitable mass range.
  hHmass->Fill((Float_t)buf->GetHmass());
  if( buf->GetHmass() < xHmass1 || buf->GetHmass() > xHmass2 ) return kFALSE;
  hStat->Fill(++selid);
  if( nGcall == 0 ) {
    sprintf(msg,"Higgs mass from %g to %g\00",xHmass1, xHmass2);
    strcpy(&selName[(Int_t)selid][0],msg);
    strcpy(&selName[(Int_t)++selid][0],"END\00");
  }

  nGcall++;

  return kTRUE;
}  

//*************************************************
//* Event Analysis
//*************************************************
void Analize(Int_t nev)
{
  if( HAccumulate() ) { 
    if( nev > 10 ) HDraw();
  }
}

// **************************************************************
// End of the job.   Printout some fit results, 
// Fit Higgs mass distribution in the mass region from 104 GeV 114 GeV
// **************************************************************
void Terminate()
{

  TF1 *gf=new TF1("fitf","gaus", fitMin, fitMax);
  hHmass->Fit("fitf","R");

  Double_t n0  =gf->GetParameter(0);
  Double_t avr =gf->GetParameter(1);
  Double_t sd  =gf->GetParameter(2);
  Double_t mass1 = avr - 3.0*sd ; Double_t mass2=avr+3.0*sd ;
  Int_t ib1=hHmass->GetXaxis()->FindBin(mass1);
  Int_t ib2=hHmass->GetXaxis()->FindBin(mass2);
  printf(" Gaussian fit to the invariant mass of higgs candidate\n");
  printf(" Fit range : from %g to %g (GeV)\n",fitMin, fitMax);
  printf(" Fit results : ");
  for(Int_t i=0;i<3;i++){
    printf("  %s = %g :",gf->GetParName(i),gf->GetParameter(i));
  }
  printf("\n");
  
  Stat_t ncand=hHmass->Integral(ib1,ib2);
  printf(" Number of candidate from mass %g to %g GeV is %d\n",
	 mass1, mass2, ncand);

  // Summary of event selection.
  Char_t bl[100];
  printf("\n Summary of event selection .\n");
  for(Int_t i=1;i<hStat->GetNbinsX();i++){
    if( strcmp(&selName[i-1][0],"END")==0 ) break; 
    printf("%40.40s : %7d \n",&selName[i-1][0],hStat->GetBinContent(i));
  }

}

//*************************************************
//* Main part to generate qqbar event
//*************************************************
int zha()
{
  gSystem->Load("libJSF.so");
  gROOT->Macro("LoadLibraries.C");
  gSystem->Load("libMyAnalysis.so");
  
  file = new TFile(outputfile,"RECREATE");  // Output file
  fin  = new TFile(inputfile);            // Input simulator data
  jsf  = JSFSteer::Instance();
  jsf->SetInput(*fin);
  jsf->SetOutput(*file);

  InitHist();                    // Initialize histogram

  jsf->Initialize();             // JSF Module initialization.

  jsf->BeginRun(1);      // Set run number to 1.
  for(Int_t ev=1;ev<=maxevt;ev++){
    Int_t ib;
    if( ! (ib=jsf->GetEvent(ev)) ) break ;
    if( ! jsf->Process(ev) ) break ;
    Analize(ev);        // Event selection and do histogram
  }

  jsf->Terminate();
  Terminate();           // Terminate process specific to this analysis

  file->Write();
}


