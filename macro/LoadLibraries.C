Int_t LoadLibraries()
{
  const Int_t pyversion=6;  // Set 5 to use Pythia5 

//   gSystem->Load("libJSF.so");
  gSystem->Load("libJSFGenerator.so");
  gSystem->Load("libBasesSpring.so");
  gSystem->Load("libJSFBeamGeneration.so");
  gSystem->Load("libJSFTools.so");
  gSystem->Load("libJSFQuickSim.so");

  if ( pyversion == 6 ) {
    gSystem->Load("libPythia6.so");
    gSystem->Load("libJSFPythia6.so");
    gSystem->Load("libEG.so");
    gSystem->Load("libEGPythia6.so");
  }
  else {
    gSystem->Load("libJSFPythia5.so");
    gSystem->Load("libEG.so");
    gSystem->Load("libEGPythia.so");
  }

  printf("Libraries loaded\n");

}
