TFile *file;

Int_t bases()
{ 
  // Macro example for bases calculation.

  gROOT->Reset();

  //  Load library ( dynamic loading is not available in ccjlc system.)
  if( strncmp(gSystem->HostName(),"ccjlc",5)  != 0 ) {
    Char_t *name=gSystem->DynamicPathName("libFFbarSpring");
    gSystem->Load(name);
  }      

  // Allocate a file to output bases result.
  file = new TFile("bases.root","RECREATE");

  jsf = new JSFSteer();      // Parameters are obtained from jsf.conf
  bs  = new FFbarBases();
  bs->Bases();
  bs->Bh_plot();
  printf(" Scalls is %d\n",bs->GetScalls());
  bs->Write();
  file->Write();
 
}

