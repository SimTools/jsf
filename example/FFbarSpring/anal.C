//*******************************************************************
// Sample macro to read QuickSim data and show histogram online.
// Displays Partons costh distribution and Combined_Hadron_Track energy 
// Distribution.  Only Energy distribution of the Combined_hadron_track
// is displayed while in the interactive session.
// Event data is not saved in the output data.
//
//$Id$
//*******************************************************************

class JSFSpring;
class JSFQuickSim;

TFile *file;
TH1F *hCosp, *hE;
TCanvas *c1;
JSFSpring *spring;
JSFQuickSim *sim;

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
    c1=new TCanvas("c1","Canvas 1",10, 10, 800, 400);
    c1->Divide(2,1);
    c1->Update();
    last->cd();
    return 0;
  }
  else { 
    TDirectory *last=gDirectory;
    gFile->cd("/");

    // Histogram Spring-parton costh
    Int_t i;
    FFbarSpringBuf *sbuf=(FFbarSpringBuf*)spring->EventBuf();
    TClonesArray *pa=sbuf->GetPartons();
    for(i=0;i<sbuf->GetNpartons();i++){
       JSFSpringParton *p=pa->At(i);
       hCosp->Fill(p->GetCosth());
    }

    // Histogram COmbined_Hadron_Track's momentum
    JSFQuickSimBuf *qbuf=sim->EventBuf();
    TClonesArray *ta=qbuf->GetTracks();
    for(i=0;i<qbuf->GetNTracks();i++){
      JSFLTKCLTrack *t=ta->At(i);
      hE->Fill(t->GetE());
    }

    // Show hCosp histogram
    c1->cd();
    Int_t Ihist = 0;
    c1->cd(++Ihist); hCosp->Draw();
    c1->cd(++Ihist); hE->Draw();
    c1->Update();
    last->cd();
  }
}

//////////////////////////////////////////////////////////////////
// Main 
//////////////////////////////////////////////////////////////////
int anal()
{
  file=new TFile("anal.root","RECREATE");  // Output file

  jsf    = new JSFSteer();
  full   = new JSFLCFULL();
  spring = new FFbarSpring();
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
    // jsf->FillTree();  // do not output event tree
                         // Since not event data is written, the file, jsf.root,
                         // can not read by read.C macro.
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();

}


