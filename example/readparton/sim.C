//***************************************************************
//*  sim.C
//*  An example of how to use JSFReadParton.
//*  Bases data, bases.root, is prepared by bases.C script in the
//*  same directory.
//*  
//*  24-July-1999 Akiya Miyamoto 
//*$Id$
//***************************************************************

TFile *file;

Int_t sim()
{
  gROOT->Reset();
  file=new TFile("sim.root","RECREATE");  // Output file

  jsf    = new JSFSteer();
  full   = new JSFLCFULL();
  rdparton = new JSFReadParton(); 
  hdr=new JSFHadronizer();
  sim=new JSFQuickSim();
  simdst=new JSFSIMDST();

  rdparton->SetDataFileName("sample.dat") ; // Input Parton data file.
  rdparton->SetType(1) ; // Input data format (0=binary, 1=asc)
  simdst->NoReadWrite() ; // Suppress output of SIMDST fortran binary file.
  Int_t maxevt=10;      // Number of event is 10.

  jsf->Initialize();

  jsf->BeginRun(30);      // Set run number to 30.

  //  Start event loop.
  for(Int_t ev=1;ev<=maxevt;ev++){
    printf(" start event %d\n ",ev);
    if( !jsf->Process(ev)) break;

    /*
    //  Example to print Generator Particle and 1st CDC track parameter
    Int_t ngen=((JSFSIMDSTBuf*)(simdst->EventBuf()))->GetNGeneratorParticles();
    printf(" ngen=%d\n",ngen);
    TClonesArray *gen=((JSFSIMDSTBuf*)(simdst->EventBuf()))->GetGeneratorParticles();
    for(Int_t i=0;i<ngen;i++){
      JSFGeneratorParticle  *gt=(JSFGeneratorParticle*)gen->UncheckedAt(i);
      gt->ls();
    }

    TClonesArray *cdc=((JSFSIMDSTBuf*)(simdst->EventBuf()))->GetCDCTracks();
    JSFCDCTrack  *t=(JSFCDCTrack*)cdc->UncheckedAt(0);
    t->Print();
    */

    jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();
}


