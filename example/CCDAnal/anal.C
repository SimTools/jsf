//  
// anal.C
//
// This macro reads CCD Beamtest raw data,  subtruct dark signal,
// does 2x2 clustering and histogram.
//
// cluster class and histogram is written to the file, jsf.root.

TFile *file;

int anal()
{
  gROOT->Reset();
  file=new TFile("jsf.root","RECREATE");  // Output file
 
  jsf    = new JSFSteer();
  ccd   = new CCDRawData();
  dark = new CCDCorrectDark();
  cls = new CCD2x2Cluster();

  Int_t maxevt=500;

  ccd->SetMakeBranch(kFALSE);  // suppress output of CCD Raw data
  jsf->Initialize();

  jsf->BeginRun(1651);      // Set run number to 1651
  for(Int_t ev=1;ev<=maxevt;ev++){
    if( ! jsf->Process(ev) ) break ;
    jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();

}

