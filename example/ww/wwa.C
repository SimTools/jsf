// *************************************************
// wwa.C
//  
// Make a histogram, reading the simulated results of 
// the process e+e- ->W+W-
//
// *************************************************

Int_t maxevt=20000;             // Number of event to simulate
Char_t *outputfile="anal.root";  // A file to output histograms
Char_t *inputfile="wwsim.root";  // Input simulator file.

TFile *file;
TCanvas *c1;
JSFSteer *jsf;
TH1F *hEvis; 


//*************************************************
//* Initialize histogram
//*************************************************
Int_t InitHist()
{
  file->cd();
  c1=new TCanvas("c1","Canvas 1",100, 100, 400, 400);
  //  JSFSpring *spr=(JSFSpring*)jsf->FindModule("WWBases");
  //printf(" Ebeam is %g \n",spr->GetEbeam());
  //Float_t evismax=spr->GetEbeam()*2*1.5;
  Float_t evismax = 700.0;
  // hStat     = new TH1F("hStat", "Selection Statistics",20,0,20);
  hEvis     = new TH1F("hEvis","Visible energy",100,0.0, evismax);
  //  c1->Divide(2,2);
  return 0;
}

// *****************************************
// Draw Histogram during event analysis
// *****************************************
void HDraw()
{
  file->cd();
  hEvis->Draw();
  c1->Update();
}  

// *****************************************
// Accumulate hisstogram
// *****************************************
Bool_t HAccumulate()
{
   JSFQuickSim *sim=(JSFQuickSim*)jsf->FindModule("JSFQuickSim");
   JSFQuickSimBuf *evt=(JSFQuickSimBuf*)sim->EventBuf();
   Int_t nt=evt->GetNTracks();
   TClonesArray *tracks=evt->GetTracks();

   TVector ssum(4);
   TVector st(4);
   for(Int_t i=0;i<nt;i++){
     JSFLTKCLTrack *t=(JSFLTKCLTrack*)tracks->UncheckedAt(i);
     st=t->GetPV();
     ssum += st;  // Sum 4 momentum
   }
   hEvis->Fill(ssum(0));
  
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

//*************************************************
//* Main part to generate qqbar event
//*************************************************
int wwa()
{
  
  file = new TFile(outputfile,"RECREATE");  // Output file
  fin  = new TFile(inputfile);            // Input simulator data
  jsf  = new JSFSteer();
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
  // Terminate();           // Terminate process specific to this analysis

  file->Write();
}

