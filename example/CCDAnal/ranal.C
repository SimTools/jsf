//  
// ranal.C
//
// read formatted data and does dark subtraction, 2x2 clustering 
// and histograming of event #100 to 200.
// 

int ranal()
{
  gROOT->Reset();
  
  TFile fout("jsf.root","RECREATE"); //  Output file.

  TFile file("/:/w/30/miyamoto/r1651.root");  // Input file
  jsf    = new JSFSteer();
  jsf->SetInput(file);
  jsf->SetOutput(fout);

  dark = new CCDCorrectDark();
  cls = new CCD2x2Cluster();


  Int_t nevevt=jsf->Env()->GetValue("JSFSteer.Nevent",100);  
  Int_t minevt=100;
  Int_t maxevt=minevt+nevent;

  jsf->Initialize();
  CCDRawData *raw=(CCDRawData*)jsf->FindModule("CCDRawData");

  jsf->BeginRun(1651);      // Set run number to 30.
  for(Int_t ev=minevt;ev<=maxevt;ev++){
    Int_t ib;
    if( ! (ib=jsf->GetEvent(ev)) ) break ;
    ((CCDRawDataBuf*)(raw->EventBuf()))->GetHeader()->Print();
    if( ! jsf->Process(ev) ) break ;
    jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
  fout->Write();

}

