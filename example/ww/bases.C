{
// Macro example for bases calculation.

  gROOT->Reset();
  TFile file("bases.root","RECREATE");

  jsf = new JSFSteer();    // required to read parameter from jsf.conf
  bases = new WWBases();

//  bases->SetNCALL(5000);
  bases->fPrintInfo=kTRUE;
  bases->fPrintHist=kTRUE;
//  bases->SetITMX1(1);
//  bases->SetITMX2(1);

  bases->DoBases();
  bases->Write();

  file->Write();

}

