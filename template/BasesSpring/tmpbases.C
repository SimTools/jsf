{
// Macro example for bases calculation.

  gROOT->Reset();
  TFile file("bases.root","RECREATE");

  jsf = new JSFSteer();    // required to read parameter from jsf.conf
  bases = new <<Bases>>();
  bases->Bases();
  bases->Write();

  file->Write();

}

