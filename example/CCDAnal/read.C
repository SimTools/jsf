// 
//  read.C
//
//  Just read jsf.root file.
// 

int read()
{
  gROOT->Reset();

  TFile file("jsf.root");  // Input file
  jsf    = new JSFSteer();

  Int_t maxevt=jsf->Env()->GetValue("JSFSteer.Nevent",10);  
  Int_t maxevt=10;
  jsf->Initialize();

  jsf->BeginRun(1651);      // Set run number to 1651
  for(Int_t ev=1;ev<=maxevt;ev++){
    Int_t ib;
    if( ! (ib=jsf->GetEvent(ev)) ) break;
    printf(" .. Got event .. Run=%d Event %d\n",jsf->GetRunNumber(),jsf->GetEventNumber());
    if( ! jsf->Process(ev) ) break ;
    jsf->Clear();
  }

  jsf->Terminate();
//  file->Write();

}

