Int_t LoadLibraries()
{
  const Int_t pyversion=6;  // Set 5 to use Pythia5 

//   gSystem->Load("libJSF.so");
  gSystem->Load("libJSFGenerator.so");
  gSystem->Load("libBasesSpring.so");
  gSystem->Load("libJSFBeamGeneration.so");
  gSystem->Load("libJSFTools.so");
  gSystem->Load("libJSFQuickSim.so");

  gSystem->Load("libEG.so");
  if ( pyversion == 6 ) {
    gSystem->Load("libPythia6.so");
    gSystem->Load("libEGPythia6.so");
    gSystem->Load("libJSFPythia6.so");
  }
  else {
    gSystem->Load("libEGPythia.so");
    gSystem->Load("libJSFPythia5.so");
  }

  printf("Libraries loaded\n");

}
