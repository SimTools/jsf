{
  TFile f("jsf.root","READ");

  jsf = new JSFSteer();

  jsf->Initialize();

  Int_t maxevt=3;

  for(Int_t i=1;i<=maxevt;i++){
    jsf->GetEvent(i);
    if( !jsf->Process(i) ) break ;
    printf(" read Run %d Event %d\n",jsf->GetRunNumber(), jsf->GetEventNumber());
    //    jsf->FillTree();
    //    jsf->Clear();
  }

  jsf->Terminate();

// f->Write();

}
