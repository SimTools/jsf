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
  JSFQuickSim *sim=0;
  JSFSpring *spring=0;
  JSFSIMDST  *simdst=0;
  JSFModule  *gen=0;
  JSFHadronizer *hdr=0;
  

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
 

enum EJSFGUIEventType { kPythia=0, kDebug=1, kBasesSpring=2,
                        kReadParton=3, kReadHepevt=4 };

//______________________________________________
int Initialize()
{

  //  Set parameters 

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
    case 4:
      break;
    default:
      printf("Run mode %d is not supported\n",gRunMode);
      return -1;
  }

  printf(" Start Initialize \n");

  jsf->Initialize();

  printf(" End of initialize.\n");

  if( gRunMode == 2 ) {
    sim=(JSFQuickSim*)jsf->FindModule("JSFQuickSim");
    simdst->SetQuickSimParam(sim->Param());
  }  


  printf(" spring is %x\n",(Int_t)spring);

  if( spring ) {  spring->ReadBases(
		jsf->Env()->GetValue("JSFGUI.Spring.BasesFile","bases.root"));
  printf("bases file is obtained.\n");
  }

  if( jsf->Env()->GetValue("JSFGUI.LastRun",0)) {
    TFile *flast=new TFile(jsf->Env()->GetValue("JSFGUI.LastRunFile",""),
			   "READ");
    jsf->GetLastRunInfo(flast);       // Get seed of last run. 
    flast->Close(); 
  }

  if( jsf->GetOutput() ) { jsf->GetOutput()->cd(); }
  UserInitialize();

  jsf->BeginRun(jsf->Env()->GetValue("JSFGUI.RunNo",1));

}

//______________________________________________
void InitGenSim()
{
  // To initialize modules for event generation and simulation
  // 
  Char_t wrkstr[256], spname[64];
  Int_t eventtype=gEventType;
  Float_t ecm=gEcm;
  JSFModule *genmod;

  full  = new JSFLCFULL();
  switch (eventtype) {
    case kPythia:
      py  = new PythiaGenerator();
      gen = py;
      break;
    case kDebug:
      gen=new DebugGenerator();
      break;
    case kBasesSpring:
      gSystem->Load(jsf->Env()->GetValue("JSFGUI.Spring.SharedLibrary",
			 "$JSFROOT/example/FFbarSpring/libFFbarSpring.sl"));
      sprintf(spname,"%s",jsf->Env()->GetValue("JSFGUI.Spring.ModuleName",
					       "FFbarSpring"));
      sprintf(wrkstr,"%s *sp=new %s();",spname, spname);
      gROOT->ProcessLine(wrkstr);
      spring = (JSFSpring*)jsf->FindModule(spname);
      hdr    = new JSFHadronizer();
      gen=spring;
      break;
    case kReadPartin:
      gen =new JSFReadParton();
      hdr =new JSFHadronizer();
      break;
    case kReadHepevt:
      gen=new JSFReadGenerator();
      break;
    default;
      break;
  }

  if( jsf->Env()->GetValue("JSFGUI.SimulationType",1) == 1 ) 
    sim    = new JSFQuickSim();
  else sim = new JSFJIM();

  simdst = new JSFSIMDST();

  if( jsf->Env()->GetValue("JSFGUI.SIMDST.Output",0) == 0 ) 
    simdst->NoReadWrite();

  simdst->SetQuickSimParam(sim->Param());

  if( jsf->Env()->GetValue("JSFGUI.OutputEventData",0) == 0  ) {
    full->SetMakeBranch(kFALSE);  
    sim->SetMakeBranch(kFALSE);   
    simdst->SetMakeBranch(kFALSE);
    gen->SetMakeBranch(kFALSE); 
    if( hdr ) hdr->SetMakeBranch(kFALSE);
  }
 
  if( eventtype == kPythia ) { 
    gROOT->LoadMacro(
	     jsf->Env()->GetValue("JSFGUI.InitPythiaMacro","InitPythia.C"));
    py->SetEcm(jsf->Env()->GetValue("JSFGUI.ECM",300.0));
    InitPythia();         // Set Pythia parameters.
  }

}

//______________________________________________
Bool_t GetEvent(Int_t ev)
{
  //  disp=0 so as not to draw event and histogram.

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
  if( gROOT->GetGlobalFunction("UserTerminate",0,kTRUE) ) UserTerminate();
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
  if( jsf->GetOutput() ) { jsf->GetOutput()->cd(); }
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




