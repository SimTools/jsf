{ 
  // Macro example for bases calculation.

  gROOT->Reset();

  //  Load library ( dynamic loading is not available in ccjlc system.)
  if( strncmp(gSystem->HostName(),"ccjlc",5)  != 0 ) {
    Char_t *name=gSystem->DynamicPathName("libFFbarSpring");
    gSystem->Load(name);
  }      

  // Allocate a file to output bases result.
  TFile file("bases.root","RECREATE");

  jsf = new JSFSteer();    // Parameters are obtained from jsf.conf
  bases = new FFbarBases();
  bases->DoBases();        // Do bases integration
  bases->Write();          // Write result to the file. 
  file->Write();

}

