{
  TFile f("jsf.root","READ");

  jsf = new JSFSteer();

  jsf->Initialize();

  Int_t maxevt=10;

  JSFQuickSim *sim=jsf->FindModule("JSFQuickSim");
  JSFQuickSimBuf *sbuf=(JSFQuickSimBuf*)sim->EventBuf();
  
  JSFHadronizer *had=(JSFHadronizer*)jsf->FindModule("JSFHadronizer");
  FFbarSpring *spr=(FFbarSpring*)jsf->FindModule("FFbarSpring");

  for(Int_t i=1;i<=maxevt;i++){
    //    if( !jsf->GetEvent(i) ) break;
    //    if( !spr->GetBranch()->GetEvent(i) ) break;
    sbuf->GetEventTracks(i);
    sim->GetBranch()->GetEvent(i);

    printf(" Get event #%d\n",i);
    printf(" sbuf->Ntrack is %d\n",sbuf->GetNTracks());
    
    //    jsf->ITree()->Print();
    //    if( !jsf->Process(i) ) break ;
    //  printf(" read Run %d Event %d\n",jsf->GetRunNumber(), jsf->GetEventNumber());
    //    jsf->FillTree();
    //    jsf->Clear();
  }

  jsf->Terminate();

// f->Write();

}
