{
// ************************************************************
// simdst.C
//  
// To create SIMDST data.
// 
//
//  Macro : simdst.C
//  This macro is to create SIMDST data.  To run this macro, do
//
//     jsf [options] simdst.C 
//
//  Valid options are.
//   --help : display help information
//   --maxevt=N  : Number of event is set to N 
//   --type=TYPE : Set type of event to generate.
//     TYPE=zh300 to generatoe e+e- -> Zh event at Ecm=300GeV, mh=110GeV.
//     TYPE=eez91 to generate e+e- -> Z -> ffbar event at Ecm=91GeV
//         only single flavour type is generated.
//     TYPE=FILENAME : read a HEPEVT data from a file.
//   --flavour=F   : F is a flavour to generate.
//     F=d, u, s, c, b, e, mu, tau
//   --destdir=DIRNAME   : Directory name where data is written.
//
//(Author)
//  Akiya Miyamoto   10-Feburary-1999
//  E-Mail: akiya.miyamoto@kek.jp
//(Update)
//  A.Miyamoto  8-Apr-1999  Add option to read HEPEVT data
// ************************************************************

// ************************************************************
//  Set Parameters
// ************************************************************

  TFile *file;
  JSFSteer *jsf;
  JSFLCFULL *full;
  JSFQuickSim *sim;
  JSFReadGenerator *rgen;

  enum EDecayMode {ddbar=156, uubar=157, ssbar=158, 
		   ccbar=159, bbbar=160, 
		   ee=164, mumu=166, tautau=168} ;

// *******************************************
//  Set default parameters
// *******************************************

  Int_t maxevt=10;              // Number of events to generate
  Char_t fndir[256];  strcpy(fndir,"./");
  Char_t fnreadgen[256]; strcpy(fnreadgen,"");
  Int_t fIDC=bbbar;
  Float_t fEcm=91.0;
  Float_t fHiggsmass=110.0;
  enum EventType { kzh300=0, keez91=1, kReadGen=2 };
  Char_t *eventTypeName[3]={"e+e- -> zh @ 300GeV", "e+e- -> Z@91GeV -> ffbar",
                   "Read Generator data from a file"};
  Int_t fEventType=kzh300;
  Char_t fnpref[256];    strcpy(fnpref,"zh300gev");
  Char_t *zffbname[16]={"ddbar","uubar","ssbar","ccbar",
			"bbbar","ttbar","LLbar","HHbar",
			"ee","nuenue","mumu","numunumu","tautau","nutaunutau",
			"chichi","nuchinuchi"};

// *******************************************
//  Parse command line options
// *******************************************

  TApplication *ap=gROOT->GetApplication();
  Int_t i;
  Char_t str[256];
  for(i=0;i<ap->Argc();i++){
    if( strncmp(ap->Argv(i),"--maxevt=",9) == 0 ){
      strcpy(str,(ap->Argv(i)+9));
      sscanf(str,"%d",&maxevt);
      printf(" str=%s maxevt=%d\n",str,maxevt);
    } 
    elseif( strcmp(ap->Argv(i),"--help") ==0 ) {
      printf("Macro : simdst.C\n");
      printf("  This macro is to create SIMDST data.");
      printf("  To run this macro, do\n");
      printf("\n       jsf [options] simdst.C \n\n");
      printf("  Valid options are.\n");
      printf("   --help : display help information\n");
      printf("   --maxevt=N  : Number of event is set to N \n");
      printf("   --type=TYPE : Set type of event to generate.\n");
      printf("     TYPE=zh300 to generatoe e+e- -> Zh event at Ecm=300GeV, mh=110GeV.\n");
      printf("     TYPE=eez91 to generate e+e- -> Z -> ffbar event at Ecm=91GeV\n");
      printf("         only single flavour type is generated.\n");
      printf("     TYPE=FILENAME : read a generator data file, FILENAME.\n");
      printf("         only single flavour type is generated.\n");
      printf("   --flavour=F   : F is a flavour to generate.\n");
      printf("     F=d, u, s, c, b, e, mu, tau\n");
      printf("   --destdir=DIRNAME   : Directory name where data is written.\n");
      return;
    }
    elseif( strncmp(ap->Argv(i),"--type=",7) == 0 ){
      strcpy(str,(ap->Argv(i)+7));
      if( strcmp(str,"zh300") == 0 )   fEventType=kzh300;
      elseif( strcmp(str,"eez91") == 0 ) {
	fEventType=keez91;
	fIDC=bbbar;
	sprintf(fnpref,"eez91-%s",zffbname[fIDC-156]);
      }
      else {
	fEventType=kReadGen;
	strcpy(fnpref,"readgen");
	strcpy(fnreadgen,str);
      }
      printf("Event type requested by command argument is %s\n",
	     eventTypeName[fEventType]);
    } 
    elseif( strncmp(ap->Argv(i),"--flavour=",10) == 0 ){
      strcpy(str,(ap->Argv(i)+10));
      if( strcmp(str,"d")==0 ) fIDC=ddbar;
      elseif( strcmp(str,"u")==0 ) fIDC=uubar;
      elseif( strcmp(str,"s")==0 ) fIDC=ssbar;
      elseif( strcmp(str,"c")==0 ) fIDC=ccbar;
      elseif( strcmp(str,"b")==0 ) fIDC=bbbar;
      elseif( strcmp(str,"e")==0 ) fIDC=ee;
      elseif( strcmp(str,"mu")==0 ) fIDC=mumu;
      elseif( strcmp(str,"tau")==0 ) fIDC=tautau;
      else {
	printf("Flavour name (%s) is invalid.\n",str);
	return;
      }
     sprintf(fnpref,"eez91-%s",zffbname[fIDC-156]);
    } 
    elseif( strncmp(ap->Argv(i),"--destdir=",10) == 0 ){
      strcpy(fndir,(ap->Argv(i)+10));
      printf("Output data is written in the directory %s\n",fndir);
    } 

  }
  printf(" Maximum event number is %d\n",maxevt);

//********************************************
//*  Set Event type.
//********************************************

  printf(" Event type is %s\n",eventTypeName[fEventType]);
  Char_t macroname[256];
  switch (fEventType) {
    case keez91:
      fEcm=91.0;              // Center of mass energy
      sprintf(macroname,"%s/macro/PythiaZ2ffb.C",gSystem->Getenv("JSFROOT"));
      gROOT->LoadMacro(macroname); // Initialize Pythia for ee->Z event
      break;
    case kzh300:
      fEcm=300.0;              // Center of mass energy
      fHiggsmass=110.0;        // Higgs Mass
      sprintf(macroname,"%s/macro/PythiaZH.C",gSystem->Getenv("JSFROOT"));
      gROOT->LoadMacro(macroname); // Initialize Pythia for ZH event.
      strcpy(fnpref,"zh300gev");
      break;
    case kReadGen:
      break;
    default:
      printf(" Event Type (%d) is no recognized\n",fEventType);
  }

//********************************************
//*  Start simulation
//********************************************

  sprintf(macroname,"%s/macro/MainSimdst.C",gSystem->Getenv("JSFROOT"));
  gROOT->LoadMacro(macroname);
  MainSimdst(maxevt);

}
