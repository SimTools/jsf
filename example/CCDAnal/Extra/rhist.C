//  Macro to run Pythia generator
// CCDRawData *ccdraw;
JSFSteer *jsf;

TH1F *hPh0, *hPh1, *hPh2, *hPh3;
TCanvas *c1;

int anal(Int_t id)
{
  if( id == 0 ) { 
    TDirectory *last=gDirectory;
    gFile->cd("/");
    hPh0=new TH1F("hPh0","CCD0 pulse height ",100, 2030.0, 2130.0);
    hPh1=new TH1F("hPh1","CCD1 pulse height ",100, 2150.0, 2250.0);
    hPh2=new TH1F("hPh2","CCD2 pulse height ",100, 1900.0, 2000.0);
    hPh3=new TH1F("hPh3","CCD3 pulse height ",100, 2000.0, 2100.0);

    c1=new TCanvas("c1","Canvas 1",20, 20, 800, 800);
    c1->Divide(2,2);
    last->cd();
    return 0;
  }
  else { 
    TDirectory *last=gDirectory;
    gFile->cd("/");
    CCDRawData *ccdraw=jsf->FindModule("CCDRawData");    
    CCDEnvironmentBuf *env=ccdraw->GetEnvironment();
    CCDRawDataBuf *buf=ccdraw->EventBuf();
    for(Int_t ic=0;ic<4;ic++){
      Int_t nx=buf->GetNx(ic);
      Int_t ny=buf->GetNy(ic);
      Float_t *adc=buf->ADC(ic);
      printf(" adc is %x\n",(Int_t)adc);
      for(Int_t iy=100;iy<300;iy++){
        for(Int_t ix=100;ix<300;ix++){
	  c1->cd(ic+1);
	  if( ic == 0 ) hPh0->Fill(adc[nx*iy+ix]);
	  else if( ic == 1 ) hPh1->Fill(adc[nx*iy+ix]);
	  else if( ic == 2 ) hPh2->Fill(adc[nx*iy+ix]);
	  else if( ic == 3 ) hPh3->Fill(adc[nx*iy+ix]);
        }
      }
      c1->cd(ic+1);
      if( ic == 0 ) hPh0->Draw();
      else if( ic == 1 ) hPh1->Draw();
      else if( ic == 2 ) hPh2->Draw();
      else if( ic == 3 ) hPh3->Draw();
      c1->Update();
    }
    last->cd();
  }
}


int rhist()
{
  gROOT->Reset();
  TFile file("jsf.root");  // Output file

  jsf    = new JSFSteer();
  // ccdraw   = new CCDRawData();

  Int_t maxevt=jsf->Env()->GetValue("JSFSteer.Nevent",10);  
  Int_t maxevt=5;
  jsf->Initialize();

  anal(0);

  jsf->BeginRun(1651);      // Set run number to 30.
  for(Int_t ev=1;ev<=maxevt;ev++){
    jsf->GetEvent(ev);
    jsf->Process(ev);
    anal(ev);
//    jsf->FillTree();
//    jsf->Clear();
  }

  jsf->Terminate();
//  file->Write();

}

