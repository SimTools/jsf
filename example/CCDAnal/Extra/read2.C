//  Macro to run Pythia generator

int read2()
{
  gROOT->Reset();
  TFile file("/:/w/30/miyamoto/r1651.root");  // Output file

  jsf    = new JSFSteer();

  Int_t maxevt=jsf->Env()->GetValue("JSFSteer.Nevent",10);  
  Int_t maxevt=1000;
  jsf->Initialize();

  jsf->BeginRun(1651);      // Set run number to 30.
  for(Int_t ev=1;ev<=maxevt;ev++){

    printf(" start event loop...\n");
    
    Int_t ib;
    if( ! (ib=jsf->GetEvent(ev)) ) { 
      printf(" GetEvent for ev=%d..is %d\n",ev,ib) ; break ;
    }

    printf(" .. Got event .. Run=%d Event %d\n",jsf->GetRunNumber(),jsf->GetEventNumber());

    if( ! jsf->Process(ev) ) break ;
//    jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
//  file->Write();

}

