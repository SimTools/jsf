{
  gROOT->Reset();
  TFile file("jsf.root","RECREATE");  // Output file

  jsf    = new JSFSteer();
  jim    = new JSFJIM();

  Int_t maxevt=500;      // Number of event is 10.
  jim->SetInput("input.dat");  // Set a file name where to read jim parameters
  jsf->Initialize();

  jsf->BeginRun(30);      // Set run number to 30.
  for(Int_t ev=1;ev<=maxevt;ev++){
    printf(" start event %d\n",ev);
    if( !jsf->Process(ev)) break;
    jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();
}
