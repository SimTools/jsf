//***************************************************************
//*  spring.C
//*$Id$
//***************************************************************

TFile *file;  // file must be global to browse it at the end.

Int_t spring()
{
  gROOT->Reset();


  gSystem->Load("libNuNuGam.so");

  file=new TFile("jsf.root","RECREATE");  // Output file

  JSFSteer      jsf;
  NuNuGamSpring sp;

  Int_t maxevt=8000;      // Number of event is 10.

  sp.ReadBases("bases.root");  // ReadBases must be called before BeginRun
  //  spring->Bases()->SetSeed(12345);  // Set seed for Spring

  for(Int_t i=0;i<maxevt;i++){ sp.Spring(); }

  sp.GetBases()->Sp_info();
  //  sp.GetBases()->Sh_plot();
  
  JSFBases *bs=sp.GetBases();
  printf(" Cross=%g +- %g\n",bs->GetEstimate(), bs->GetError());

  file->Write();

}    
