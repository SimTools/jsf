// --------------------------------------------------------
void LoadLibraryWithMessage(Char_t *name)
{
  gSystem->Load(name);
  cerr << "Shared library " << name << " was loaded " << endl;
}

// --------------------------------------------------------
Int_t LoadLibraries(JSFEnv *env=0)
{

  const Int_t pyversion=6;  // Set 5 to use Pythia5 

  LoadLibraryWithMessage("libJSFGenerator.so");
  LoadLibraryWithMessage("libBasesSpring.so");
  LoadLibraryWithMessage("libJSFBeamGeneration.so");
  LoadLibraryWithMessage("libJSFTools.so");
  LoadLibraryWithMessage("libJSFQuickSim.so");

  if ( pyversion == 6 ) {
    LoadLibraryWithMessage("libPythia6.so");
    LoadLibraryWithMessage("libJSFPythia6.so");
    LoadLibraryWithMessage("libEG.so");
  }
  else {
    LoadLibraryWithMessage("libTAUOLA.so");
    LoadLibraryWithMessage("libJSFPythia5.so");
  }


// ** Load Satellites libraries, when SATELLITESROOT environment variable is defined.

  if( strlen(gSystem->Getenv("SATELLITESROOT") ) > 3 ) {
    FileStat_t filestat;
    Char_t *s4mdef=gSystem->ConcatFileName(gSystem->Getenv("SATELLITESROOT"),"macros/S4Macros.C");
    Char_t *s4m=env->GetValue("JSFJ4.S4MacroFileName",s4mdef);

    Int_t ierr=gSystem->GetPathInfo(s4m, filestat);
    if ( ierr == 0 ) ) {
      gROOT->LoadMacro(s4m);
      cerr << "Macro " << s4m << " was loaded." << endl;
      LoadS4Libraries(env);
    }
  }

  printf("LoadLibraries() completed\n");

}

