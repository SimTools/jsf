//  
// rawph.C
//
// Read raw data, subtruct pedestal and display pulse height 
// interacctively.  Only histogram is written to the file, jsf.root. 
//
TFile *file;

int rawph()
{
  gROOT->Reset();
  file=new TFile("jsf.root","RECREATE");  // Output file
 
  jsf    = new JSFSteer();
  ccd   = new CCDRawData();
  dark = new CCDCorrectDark();
  rawhist = new CCDRawHist();

  Int_t maxevt=jsf->Env()->GetValue("JSFSteer.Nevent",10);  
  Int_t maxevt=5;

  ccd->SetMakeBranch(kFALSE);  // suppress output of CCD Raw data
  jsf->Initialize();

  jsf->BeginRun(1651);      // Set run number to 1651
  for(Int_t ev=1;ev<=maxevt;ev++){
    if( ! jsf->Process(ev) ) break ;
//     jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();

}

