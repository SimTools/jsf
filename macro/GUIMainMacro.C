//*************************************************
//* Main Macro for JSF execution through GUI
//*
//*(List of functions ) 
//* 
//* int Initialize()    // Initialize
//* void InitGenSim()   // Initialize Generator parameters
//* Bool_t GetEvent(Int_t ev) // Process one event ( Read or generate)
//* Bool_t GetNext()          // Read next or Generate one event
//* Bool_t GetPrevious()      // Read previous event
//* void JobEnd()             // Does job end task
//* void GetArguments()       // Get command line arguments
//* void BatchRun()           // Event loop for batch execution
//* void ResetHist()          // Call User Initalize
//* Bool_t SetOptionsForBatch() // Set options specfic to batch execution.
//* 
//* Some of these functions are called by TRoot::ProcessLine() executed
//* in JSFGUIFrame.cxx 
//*
//*(User functions called from this macro)
//*  Initialize() calls  UserSetOptions() and UserInitialize()
//*  GetEvent(..) calls  UserAnalysis()
//*  JobEnd()     calls  UserTerminate()
//*  ResetHist()  calls  UserInitialize()
//* 
//*  These user functions are defined in a macro file, UserAnalysis.C 
//* 
//*(Author)
//*  Akiya Miyamoto, KEK, 8-March-1999  
//*  Akiya Miyamoto, KEK, 21-April-1999  
//*                  Modified to run both batch and Interaactive
//*  Akiya Miyamoto, KEK, 24-July-1999  
//*                  Moved to macro directory. 
//*                  Add new options to get random seed from a file.
//* 
//$Id$
//
//*************************************************

  TFile *ofile;
  JSFSteer *jsf;
  JSFLCFULL *full;
  JSFQuickSim *sim;
  JSFReadGenerator *rgen;
  
  Int_t gRunMode;  // Generate event, Read root file, ...
  Char_t *gOutputFileName; // Output file name
  Char_t *gInputFileName;  // Input file name
  Char_t *gMacroFileName;  //  Macro file name for user analysis
  Int_t gEventType; // Pythia event, Debug Generator, Read file
  Float_t gEcm;  // CM energy
  Char_t *gInitPythiaMacro; // Macro to initialize pythia
  Int_t gReturnCode; // Return code from event analysis 
  Int_t gFirstEvent;   // Start event number to process in batch
  Int_t gNAnalizeEvent;  // Number of event to process in batch
  Bool_t gEventDataOff=kFALSE; // When TRUE, no event dta is written.
  enum EventType { kPythia=0, kDebug=1, kReadGen=2};

  Int_t gRunNo;  //  Run number 
  Char_t gLastRunFile[256]; // A file name of last run file, where to get seed.
 
//______________________________________________
int Initialize()
{

  //  Set parameters 
  gRunNo=jsf->Env()->GetValue("MainMacro.RunNo",1);
  sscanf(jsf->Env()->GetValue("MainMacro.LastRunFile","Undefined"), "%s",gLastRunFile);

  if( gui == 0 ) {
    SetOptionsForBatch();
  }
  else {
    // In interactive mode, GetArguments() is called when gui object is created.
    gRunMode=gui->GetRunMode();
    gOutputFileName=gui->GetOutputFileName();
    gInputFileName=gui->GetInputFileName();
    gMacroFileName=gui->GetMacroFileName();
    gEventType=gui->GetEventType();
    gEcm=gui->GetEcm();
    gInitPythiaMacro=gui->GetInitPythiaMacro();

  }

  GetArguments();
  gROOT->LoadMacro(gMacroFileName);

  UserSetOptions();

  switch(gRunMode){
    case 1:
      ofile= new TFile(gOutputFileName,"RECREATE");
      jsf->SetIOFiles();
      InitGenSim();
      break;
    case 2:
      ofile= new TFile(gOutputFileName,"RECREATE");
      file = new TFile(gInputFileName);  // Input file
      jsf->SetIOFiles();
      jsf->SetOutput(*ofile);
      simdst = new JSFSIMDST();
      simdst->SetFile(ofile);
      simdst->NoReadWrite(); // Does not output SIMDST data
      break;
    case 3:
      ofile= new TFile(gOutputFileName,"RECREATE");
      jsf->SetIOFiles();
      simdst = new JSFSIMDST();
      simdst->ReadData();
      simdst->SetDataFileName(gInputFileName);
      Char_t parf[256];
      strcpy(parf,gInputFileName);
      Int_t lparf=strlen(parf);
      strcpy(&parf[lparf-3],"param");
      JSFQuickSimParam *par=new JSFQuickSimParam();
      par->ReadParamDetector(parf) ;// Read detector parameter
      simdst->SetQuickSimParam(par);
      break;
    default:
      printf("Run mode %d is not supported\n",gRunMode);
      return -1;
  }

  jsf->Initialize();

  if( gRunMode == 2 ) {
    sim=(JSFQuickSim*)jsf->FindModule("JSFQuickSim");
    simdst->SetQuickSimParam(sim->Param());
  }  

  if( strcmp(gLastRunFile,"Undefined") != 0 ) {
    TFile *flast=new TFile(gLastRunFile,"READ");
    jsf->GetLastRunInfo(flast);       // Get seed of last run. 
    flast->Close(); 
  }
  
  UserInitialize();

  jsf->BeginRun(gRunNo);   // Set run number.
}

//______________________________________________
void InitGenSim()
{

   Int_t eventtype=gEventType;
   Float_t ecm=gEcm;

   full  = new JSFLCFULL();
   switch (eventtype) {
      case kDebug:
	dbg=new DebugGenerator();
	break;
      case kReadGen:
        rgen=new JSFReadGenerator();
	break;
      default:
	if( gInitPythiaMacro != 0 ) gROOT->LoadMacro(gInitPythiaMacro);
	py    = new PythiaGenerator();
	break;
   }
   sim    = new JSFQuickSim();
   simdst = new JSFSIMDST();

   simdst->NoReadWrite();
   simdst->SetQuickSimParam(sim->Param());
   // simdst->WriteData();
   // simdst->SetDataFileName("simdst.dat");
   // simdst->SetParamFileName("simdst.param");

   // If these comments are removed, corresponding information
   // is not saved in the ROOT tree.
   //

   if( gEventDataOff ) {
     full->SetMakeBranch(kFALSE);   // suppress output of EventBuf 
     py->SetMakeBranch(kFALSE);     // suppress output of EventBuf 
     sim->SetMakeBranch(kFALSE);    // suppress output of EventBuf
     simdst->SetMakeBranch(kFALSE); // suppress output of EventBuf
   }
 
   if( eventtype == kPythia ) { 
     py->SetEcm(ecm);             // Center of mass energy (GeV)
     InitPythia();         // Set Pythia parameters.
   }

}

//______________________________________________
Bool_t GetEvent(Int_t ev)
{
  //  
    if( gRunMode==2 && !jsf->GetEvent(ev) ) {
      gReturnCode=-2 ;
      if( gui != 0 ) gui->SetReturnCode(gReturnCode);
      return kFALSE;
    }      
    if( !jsf->Process(ev)) {
      gReturnCode=-1;
      if( gui != 0 ) gui->SetReturnCode(gReturnCode);
      return kFALSE;
    }
    if( gui != 0 ) gui->DisplayEventData();
    UserAnalysis();
    gReturnCode=0;
    if( gui != 0 ) {
      gui->DrawHist();
      gui->SetReturnCode(gReturnCode);
    }
    
    if( gRunMode==1 ) {
      jsf->FillTree();
      jsf->Clear();
    }

    return kTRUE;
}


//_________________________
Bool_t GetNext()
{
  Int_t nevt=jsf->GetEventNumber()+1;
  return GetEvent(nevt);
}

//_________________________
Bool_t GetPrevious()
{
  if( gRunMode != 2 ) {
    printf(" Can not go to previous events.\n");
    return kTRUE;
  }
  else {
    Int_t nevt=jsf->GetEventNumber()-1;
    return GetEvent(nevt);
  }
}

//______________________________________________
void JobEnd()
{
  jsf->Terminate();
  UserTerminate();
  if( ofile ) ofile->Write();

}

//______________________________________________
Bool_t SetOptionsForBatch()
{
  //  Set global parameters for Batch execution.
  //  This macro is called when jsf is executed with -b options.

  gRunMode=jsf->Env()->GetValue("JSFGUI.RunMode",1);
  gMacroFileName=jsf->Env()->GetValue("JSFGUI.MacroFileName","UserAnalysis.C");

  gEventType=jsf->Env()->GetValue("JSFGUI.EventType",kPythia);
  sscanf(jsf->Env()->GetValue("JSFGUI.ECM","300.0"),"%g",&gEcm);
  gInitPythiaMacro=
    jsf->Env()->GetValue("JSFGUI.InitPythiaMacro","InitPythia.C");

  gInputFileName=jsf->Env()->GetValue("JSFGUI.InputFileName","");
  gOutputFileName=jsf->Env()->GetValue("JSFGUI.OutputFileName","jsf.root");
  gFirstEvent=jsf->Env()->GetValue("JSFGUI.FirstEvent",1);
  gNAnalizeEvent= jsf->Env()->GetValue("JSFGUI.NEventsAnalize",10);

}

//_________________________________________________________
void ResetHist()
{
  UserInitialize();
}

//_________________________________________________________
void BatchRun()
{
  Initialize();

  Int_t i;
  for(i=gFirstEvent;i<=gFirstEvent+gNAnalizeEvent-1;i++){
    GetEvent(i);
    if( gReturnCode == -2 ) {
      printf("End of event loop due to end-of-file at event# %d\n",i);
      break;
    }
    else if( gReturnCode == -1 ) {
      printf("End of event loop due to error at event# %d\n",i);
      break;
    }
  }

  JobEnd();
    
}


//_________________________________________________________
void GetArguments()
{

  TApplication *ap=gROOT->GetApplication();

  Int_t maxevt;
  Int_t i;
  Char_t str[256];
  for(i=0;i<ap->Argc();i++){
    if( strncmp(ap->Argv(i),"--maxevt=",9) == 0 ){
      strcpy(str,(ap->Argv(i)+9)); 
      sscanf(str,"%d",&maxevt);
      gNAnalizeEvent= maxevt;
      if( gui ) gui->SetNEventsAnalize(gNAnalizeEvent);
      printf(" Number of event to process is %d\n",gNAnalizeEvent);
    } 
    elseif( strncmp(ap->Argv(i),"--1stevt=",9) == 0 ){
      strcpy(str,(ap->Argv(i)+9)); 
      sscanf(str,"%d",&gFirstEvent);
      if( gui ) gui->SetFirstEvent(gFirstEvent);
      printf(" Event number to start analysis is %d\n",gFirstEvent);
    } 
    elseif( strncmp(ap->Argv(i),"--ecm=",6) == 0 ){
      strcpy(str,(ap->Argv(i)+6)); 
      sscanf(str,"%g",&gEcm);
      if( gui ) gui->SetEcm(gEcm);
      printf(" Center of mass energy is %g (GeV)\n",gEcm);
    } 
    elseif( strncmp(ap->Argv(i),"--runno=",8) == 0 ){
      strcpy(str,(ap->Argv(i)+8)); 
      sscanf(str,"%d",&gRunNo);
      printf(" Run no is %d \n",gRunNo);
    } 
    elseif( strncmp(ap->Argv(i),"--macro=",8) == 0 ){
      strcpy(str,(ap->Argv(i)+8)); 
      gMacroFileName=new Char_t[strlen(str)+1];
      strcpy(gMacroFileName,str);
      printf(" User Macro filename is %s\n");
    } 
    elseif( strncmp(ap->Argv(i),"--InputFile=",12) == 0 ){
      strcpy(str,(ap->Argv(i)+12)); 
      gInputFileName=new Char_t[strlen(str)+1];
      strcpy(gInputFileName,str);
      printf(" Input file name is %s\n");
    } 
    elseif( strncmp(ap->Argv(i),"--OutputFile=",13) == 0 ){
      strcpy(str,(ap->Argv(i)+13)); 
      gOutputFileName=new Char_t[strlen(str)+1];
      strcpy(gOutputFileName,str);
      printf(" Output file name is %s\n");
    } 
    elseif( strncmp(ap->Argv(i),"--LastRunFile=",14) == 0 ){
      strcpy(gLastRunFile,(ap->Argv(i)+14)); 
      printf(" Seeds of random numbers will be obtained from %s\n");
    } 
    elseif( strcmp(ap->Argv(i),"--EventDataOff") == 0 ){
      gEventDataOff=kTRUE;
      printf(" Event data is not written to the root file.\n");
    } 
    elseif( strcmp(ap->Argv(i),"--help") ==0 ) {
      printf("Macro : gui.C\n");
      printf("  This macro is for batch and/or interactive execution of jsf.\n");
      printf("  To run this macro, do\n");
      printf("\n       jsf [options] gui.C \n\n");
      printf("  Valid options are.\n");
      printf("   --help : display help information\n");
      printf("   --maxevt=N  : Number of event is set to N \n");
      printf("   --1stevt=N  : First event number to analize \n");
      printf("   --ecm=Ecm   : Set center of mass energy.\n");
      printf("   --EventDataOff : Does not output event data to the root file.\n");
      printf("   --macro=USERMACRO : A macro file name for UserAnalysis.\n");
      printf("   --runno=N : Run number (default=1).\n");
      printf("   --LastRunFile=FILENAME : A file name of last run, where to get seed.\n");
      printf("   --InputFile=FILENAME : A file name from where data is read.\n");
      printf("   --OutputFile=FILENAME : A file name to where data is written.\n");
      ap->Terminate();
    }
  }
  return ;
}




