// Sample macro to read QuickSim data and show histogram online.
// Displays Partons costh distribution and Combined_Hadron_Track energy 
// Distribution.  Only Energy distribution of the Combined_hadron_track
// is displayed while in the interactive session.
// Event data is not saved in the output data.
//

<<Spring>> *spring;
JSFQuickSim    *sim;

TH1F *hCosp, *hE;
TCanvas *c1;

//////////////////////////////////////////////////////////////////
// Macro for Histograming
//  hist(0) : Initialize
//  hist(n) : Histograming.
//////////////////////////////////////////////////////////////////
int hist(Int_t id)
{

  if( id == 0 ) { 
    TDirectory *last=gDirectory;
    gFile->cd("/");  // Histograms are created at the top directory
    hCosp=new TH1F("hCosp","Costh of parton",20,-1.0, 1.0);
    hE=new TH1F("hPhad","Hadron momentum",100,0.0, 20.0);
    c1=new TCanvas("c1","Canvas 1",200, 10, 400, 380);
    hCosp->Draw();
    c1->Update();
    last->cd();
    return 0;
  }
  else { 
    TDirectory *last=gDirectory;
    gFile->cd("/");

    // Histogram Spring-parton costh
    Int_t i;
    <<Spring>>Buf *sbuf=spring->EventBuf();
    TClonesArray *pa=sbuf->GetPartons();
    for(i=0;i<sbuf->GetNpartons();i++){
       JSFSpringParton *p=pa->At(i);
       hCosp->Fill(p->GetCosth());
    }

    // Histogram COmbined_Hadron_Track's momentum
    JSFQuickSimBuf *qbuf=sim->EventBuf();
    TClonesArray *ta=qbuf->GetTracks();
    for(i=0;i<qbuf->GetNtracks();i++){
      JSFLTKCLTrack *t=ta->At(i);
      hE->Fill(t->GetE());
    }

    // Show hCosp histogram
    c1->cd();
    hCosp->Draw();
    c1->Update();
    last->cd();
  }
}

//////////////////////////////////////////////////////////////////
// Main 
//////////////////////////////////////////////////////////////////
int anal()
{
  gROOT->Reset();

  TFile file("jsf.root","RECREATE");  // Output file

  jsf    = new JSFSteer();
  full   = new JSFLCFULL();
  spring = new <<Spring>>();
  spring->ReadBases("bases.root");
  hdr=new JSFHadronizer();
  sim=new JSFQuickSim();

  Int_t maxevt=100;      // Number of event is 10.
  jsf->Initialize();

  hist(0);

  jsf->BeginRun(30);      // Set run number to 30.
  for(Int_t ev=1;ev<=maxevt;ev++){
    jsf->Process(ev);
    hist(ev);
    // jsf->FillTree();  // don not output event tree
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();

}


