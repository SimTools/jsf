//  Macro to run Pythia generator
TFile *file;

int big()
{
  gROOT->Reset();
  file=new TFile("/:/w/30/miyamoto/ccd.root","RECREATE");  // Output file

  jsf      = new JSFSteer();
  ccd      = new CCDRawData();
  dark     = new CCDCorrectDark();
//  rawhist  = new CCDRawHist();
  cls      = new CCD2x2Cluster();

  Int_t maxevt=jsf->Env()->GetValue("JSFSteer.Nevent",10);  
  Int_t maxevt=500;
  jsf->Initialize();

  jsf->BeginRun(30);      // Set run number to 30.
  for(Int_t ev=1;ev<=maxevt;ev++){
//    jsf->Process(ev);
    if( ! ccd->Process(ev) ) break ;
    dark->Process(ev);
//    rawhist->Process(ev);
    cls->Process(ev);
    cls->FillBranch();
//    jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();

}

