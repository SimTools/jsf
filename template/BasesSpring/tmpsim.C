{
  gROOT->Reset();
  TFile file("jsf.root","RECREATE");  // Output file

  jsf    = new JSFSteer();
  full   = new JSFLCFULL();
  spring = new <<Spring>>();
  spring->ReadBases("bases.root");
  hdr=new JSFHadronizer();
  sim=new JSFQuickSim();

  Int_t maxevt=10;      // Number of event is 10.
  jsf->Initialize();

  jsf->BeginRun(30);      // Set run number to 30.
  for(Int_t ev=1;ev<=maxevt;ev++){
    printf(" start event %d\n",ev);
    jsf->Process(ev);
    jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();

}
