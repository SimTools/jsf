//
// Collection of optional macros to be used in GUIMainMacro
//

void DefineOptionalModules()
{
  if( strlen(gSystem->Getenv("SATELLITESROOT")) > 4 ) {
    ConstructS4Modules();  // This macro is defined in $SATELLITESROOT/macros/S4Macros.C
  }
}
