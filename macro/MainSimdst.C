//*************************************************
//* Main part to generate Pythia event
//*  Pythia is initialized by InitPythia function, which should be 
//*  defined by a macro prior to this macro.
//*
//*(Author)
//*  Akiya Miyamoto, KEK, 11-Feburary-1999  
//*************************************************
int MainSimdst(Int_t maxevt)
{
  Char_t outfile[256];
  Char_t ofile[256], oparam[256];
  Char_t ftype[10];
  if( maxevt == 1000 ) {strcpy(ftype,".1kev");}
  elseif( maxevt == 100 ) {strcpy(ftype,".100ev");}
  elseif( maxevt == 10 ) {strcpy(ftype,".10ev");}
  else { strcpy(ftype,"");}
  sprintf(outfile,"%s/%s%s.root",fndir,fnpref,ftype);

  file = new TFile(outfile,"RECREATE");  // Output file
  jsf   = new JSFSteer();
  full  = new JSFLCFULL();
  py    = new PythiaGenerator();
  sim   = new JSFQuickSim();
  simdst = new JSFSIMDST();

  sprintf(ofile,"%s/%s%s.dat",fndir,fnpref,ftype);
  sprintf(oparam,"%s/%s%s.param",fndir,fnpref,ftype);

  simdst->SetDataFileName(ofile);
  simdst->SetParamFileName(oparam);

  
  full->SetMakeBranch(kFALSE);   // suppress output of EventBuf 
  //  py->SetMakeBranch(kFALSE);     // suppress output of EventBuf 
  //  sim->SetMakeBranch(kFALSE);    // suppress output of EventBuf
  simdst->SetMakeBranch(kFALSE);     // suppress output of EventBuf
 
  py->SetEcm(fEcm);             // Center of mass energy (GeV)
  InitPythia();         // Set Pythia parameters.
  jsf->Initialize();

  jsf->BeginRun(1);              // Set run number to 1.
  for(Int_t ev=1;ev<=maxevt;ev++){
    printf(" start event %d \n",ev);
    if( !jsf->Process(ev)) break ;
    jsf->FillTree();
    jsf->Clear();
  }

  printf(" end of job\n");

  jsf->Terminate();
  file->Write();

}


