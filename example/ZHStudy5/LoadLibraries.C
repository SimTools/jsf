Int_t LoadLibraries()
{
  gSystem->Load("libJSF.so");
//  gSystem->Load("libPythia6.so");
  gSystem->Load("libJSFGenerator.so");
  gSystem->Load("libBasesSpring.so");
  gSystem->Load("libJSFBeamGeneration.so");
  gSystem->Load("libJSFTools.so");
  gSystem->Load("libJSFQuickSim.so");

//  gSystem->Load("libEG.so");
//  gSystem->Load("libEGPythia6.so");
//  gSystem->Load("libJSFPythia6.so");
  gSystem->Load("libEG.so");
  gSystem->Load("libJSFPythia5.so");
  gSystem->Load("libEGPythia.so");

  printf("Load libraries\n");

}
