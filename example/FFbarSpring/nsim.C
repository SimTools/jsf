//***************************************************************
//*  nsim.C
//*  An example script to run Spring, Hadronizer and QuickSimulator.
//*  This script reads seeds of random numbers of previous run
//*  from a file, jsf.root.  By this way, user can generate events
//*  with a sequence following previous run.   
//*  
//*  24-July-1999 Akiya Miyamoto 
//*$Id$
//***************************************************************

TFile *file;

Int_t nsim()
{
  gROOT->Reset();

  //  Load library ( dynamic loading is not available in ccjlc system.)
  if( strncmp(gSystem->HostName(),"ccjlc",5)  != 0 ) {
    Char_t *name=gSystem->DynamicPathName("libFFbarSpring");
    gSystem->Load(name);
  }      

  file=new TFile("jsf.run2.root","RECREATE");  // Output file

  jsf    = new JSFSteer();
  full   = new JSFLCFULL();
  spring = new FFbarSpring();
  hdr = new JSFHadronizer();
  sim = new JSFQuickSim();

  Int_t maxevt=10;      // Number of event is 10.
  jsf->Initialize();

  //  Get seeds of prevous run from a file, jsf.root.
  //  This part must be executed prior to the begin run.
  TFile *flast=new TFile("jsf.root","READ");
  jsf->GetLastRunInfo(flast);       // Get seed of last run. 
  flast->Close(); 

  spring->ReadBases("bases.root");  // Bases must be initialized after 
                                    // GetLastRunInfo  

  //  Begin run
  jsf->BeginRun(31);
  for(Int_t ev=1;ev<=maxevt;ev++){
    if( !jsf->Process(ev)) break;

    //  Example to print Px of first particle.
    FFbarSpring *spr=(FFbarSpring*)jsf->FindModule("FFbarSpring");
    FFbarSpringBuf *sb=(FFbarSpringBuf*)spr->EventBuf();
    TClonesArray *sc=sb->GetPartons();
    JSFSpringParton *p=(JSFSpringParton*)sc->UncheckedAt(0);
    printf(" start event %d ",ev);
    printf(" np=%g",sb->GetNpartons());
    printf(" Px=%g\n",p->GetPx());

    jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();

}

