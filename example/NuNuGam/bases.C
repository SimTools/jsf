TFile *file;

Int_t bases()
{ 
  // Macro example for bases calculation.

  gROOT->Reset();

  // gSystem->Load("libNuNuGam.so");

  // Allocate a file to output bases result.
  file = new TFile("bases.root","RECREATE");

  jsf = new JSFSteer();      // Parameters are obtained from jsf.conf
  bs  = new NuNuGamBases();

    int    ncall = 5000;
    int    itr1  =  10;
    double acc1  = 0.1;
    int    itr2  = 100;
    double acc2  = 0.05;
    double tune  = 1.5;

//    bs1.param( ncall, tune, itr1, acc1, itr2, acc2 );

    bs->SetNoOfSample( ncall );
    bs->SetTuneValue ( tune );
    bs->SetIteration1( acc1, itr1 );
    bs->SetIteration2( acc2, itr2 );


  bs->Bases();
  bs->Bh_plot();
  printf(" Scalls is %d\n",bs->GetScalls());
  bs->Write();  // Write bases object
  file->Write();
 
}

