//  Sample script to run debug generator.
//

DebugGenerator *dbg;
JSFQuickSim    *sim;

TH1F *hPx;
TCanvas *c1;

int anal(Int_t id)
{

  if( id == 0 ) { 
    TDirectory *last=gDirectory;
    gFile->cd("/");
    hPx=new TH1F("hPx","Px distribution",100,-100,100);
    c1=new TCanvas("c1","Canvas 1",200, 10, 400, 380);
    hPx->Draw();
    c1->Update();
    last->cd();
    return 0;
  }
  else { 
    TDirectory *last=gDirectory;
    gFile->cd("/");
    JSFQuickSimBuf *buf=sim->EventBuf();
    Int_t nt=buf->GetNtracks();
    TClonesArray *ts=buf->GetTracks();
    for(Int_t i=0;i<nt;i++) {
      JSFLTKCLTrack *t=(JSFLTKCLTrack*)ts->At(i);
      Float_t px=t->GetPx();
      hPx->Fill(px);
    }
    c1->cd();
    hPx->Draw();
    c1->Update();
    last->cd();
  }
}

int sdebug()
{
  gROOT->Reset();
  TFile file("jsf.root","RECREATE");  // Output file

  jsf    = new JSFSteer();
  full   = new JSFLCFULL();
  dbg=new DebugGenerator();
  sim=new JSFQuickSim();

// Number of event to generate, default=10

  Int_t maxevt=jsf->Env()->GetValue("JSFSteer.Nevent",10);  
  Int_t maxevt=20;
  jsf->Initialize();

  anal(0);
  // dbg->PrintParam();

  jsf->BeginRun(30);      // Set run number to 30.
  for(Int_t ev=1;ev<=maxevt;ev++){
    //    printf(" start event %d\n",ev);
    jsf->Process(ev);

    anal(ev);

    jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();

}

