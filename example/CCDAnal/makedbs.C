//  
// Create database file for CCD Beam test data analysis
// Data base includes,
// (1) runindex for dark-run runnumber info.
// (2) pedestals for sigma and rms of each CCD's ph.
// (3) darkdata
//

TFile *file;

int makedbs()
{
  gROOT->Reset();
  file=new TFile("jsfdbs.root","RECREATE");  // Output file

  jsf = new JSFSteer();

  JSFDBS::SetFile(file);

// Put Run Index   
  Int_t drlist[5]={1650, 0, 0, 0, 0};
  CCDRunIndex *run;
  run=new CCDRunIndex(1231, 1231, 4, "EEV", -15.0, 0.0, 2.0, 1, drlist);
  run=new CCDRunIndex(1651, 1231, 4, "EEV", -15.0, 0.0, 2.0, 1, drlist);
  run=new CCDRunIndex(1691, 1231, 4, "EEV", -15.0, 0.0, 2.0, 1, drlist);
  run=new CCDRunIndex(1660, 1231, 4, "EEV", -15.0, 0.0, 2.0, 1, drlist);
  run=new CCDRunIndex(1760, 1231, 4, "EEV", -15.0, 0.0, 2.0, 1, drlist);

  printf(" Put RunIndex database.\n");

  CCDPedestal *ped=new CCDPedestal[4];
  ped[0]=new CCDPedestal( 0, -0.009309,0.003055,3.427191,0.002072,7.468054,803,  21 );
  ped[1]=new CCDPedestal( 1,  0.023090,  0.002527,  2.890321,  0.001821,  7.758386, 748, 20);
  ped[2]=new CCDPedestal( 2,  0.004312,  0.001910,  2.014642,  0.001371,  5.659824, 450, 19);
  ped[3]=new CCDPedestal( 3,  0.056138,  0.002830,  3.230995,  0.002026, 12.963085, 709, 8);

  CCDPedestals *peds;
  peds=new CCDPedestals(1651, 4, 5, ped );

  CCDDarkData dark;
  dark.LoadFile("/:/w/30/miyamoto/dark01650.dat");
  dark.WriteDBS(1650);

  file->Write();

}
