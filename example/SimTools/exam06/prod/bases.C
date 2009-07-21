{
// Macro example for bases calculation.

  gROOT->Reset();
  TFile file("bases.root","RECREATE");

  jsf = new JSFSteer();    // required to read parameter from jsf.conf
  
  bases = new WWBases();


//  bases->SetNoOfSample(10000);
//  bases->SetIteration1( 0.2, 10);
//  bases->SetIteration2( 0.1, 10);
  bases->Bases();
  bases->Bh_plot();
  bases->Userout();
  bases->Write();

  file->Write();
}
