//***************************************************************
//*  sim.C
//*$Id$
//***************************************************************

TFile *file;  // file must be global to browse it at the end.

Int_t sim()
{
  gROOT->Reset();

  //  Load library ( dynamic loading is not available in ccjlc system.)
  if( strncmp(gSystem->HostName(),"ccjlc",5)  != 0 ) {
    Char_t *name=gSystem->DynamicPathName("libNuNuGam.so");
    gSystem->Load(name);
  }      

  file=new TFile("jsf.root","RECREATE");  // Output file

  // Define modules.  JSFLCFULL must be declared to use JSFHadronizer and 
  // JSFQuickSim, which uses LCLIB libraries.  Each modules are executed
  // in the order according to the order of definition.
  jsf    = new JSFSteer();
  full   = new JSFLCFULL();
  spring = new NuNuGamSpring(); 
  hdr    = new JSFHadronizer();
  sim    = new JSFQuickSim();

  Int_t maxevt=20;      // Number of event is 10.
  jsf->Initialize();

  // spring->GetBases()->Bases();
  spring->ReadBases("bases.root");  // ReadBases must be called before BeginRun
  //  spring->Bases()->SetSeed(12345);  // Set seed for Spring
  spring->SetPrintInfo(kTRUE);
  // spring->SetPrintHist(kTRUE);


  jsf->BeginRun(30);      // Set run number to 30.

  //  ------------------------------------------------------------
  //  Event loop.
  //  ------------------------------------------------------------
  for(Int_t ev=1;ev<=maxevt;ev++){
    printf(" start event %d ",ev);
    if( !jsf->Process(ev)) break;  

    //  Example to print Px of first particle.
    NuNuGamSpring *spr=(NuNuGamSpring*)jsf->FindModule("NuNuGamSpring");
    NuNuGamSpringBuf *sb=(NuNuGamSpringBuf*)spr->EventBuf();
    TClonesArray *sc=sb->GetPartons();
    JSFSpringParton *p=(JSFSpringParton*)sc->UncheckedAt(0);
    printf(" np=%g",sb->GetNpartons());
    printf(" Px=%g\n",p->GetPx());

    jsf->FillTree();
    jsf->Clear();
  }
  
  //  ------------------------------------------------------------

  jsf->Terminate();
  file->Write();
}


