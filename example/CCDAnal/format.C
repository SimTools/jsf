//
// Read raw data of beam test and converted it to the root file.
// 

int format(Int_t run=1651)
{
  gROOT->Reset();

  Char_t fname[64];
  sprinf(fname,"/:/w/30/miyamoto/r%4.4d.root",run);
  TFile file(fname,"RECREATE");  // Output file

  jsf    = new JSFSteer();
  raw   = new CCDRawData();

  Int_t maxevt=jsf->Env()->GetValue("JSFSteer.Nevent",1000);  
  jsf->Initialize();

// Run loop
  jsf->BeginRun(run);     
  for(Int_t ev=1;ev<=maxevt;ev++){
    if( ! jsf->Process(ev) ) break;
    jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();

}

