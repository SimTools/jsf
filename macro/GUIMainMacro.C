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
//* void BatchRun()           // Event loop for batch execution
//* void ResetHist()          // Call User Initalize
//* 
//* Some of these functions are called by TRoot::ProcessLine() executed
//* in JSFGUIFrame.cxx 
//*
//*(User functions called from this macro)
//*  Initialize() calls  UserSetOptions(), UserInitialize(), UserModuleDefine()
//*                      UserAnotherModules()
//*  GetEvent(..) calls  UserAnalysis()
//*  JobEnd()     calls  UserTerminate()
//*  ResetHist()  calls  UserInitialize()
//*
//*************************************************
//*  These user functions are defined in a macro file, UserAnalysis.C. 
//*  UserSetOptions() and UserTerminate() is not called when they are defined.
//*  UserModuleDefine() is called only when RunMode=0.
//*  UserAnotherModules() is to define aditional modules after standard module definition.
//* 
//*(Author)
//*  Akiya Miyamoto, KEK, 8-March-1999  
//*  Akiya Miyamoto, KEK, 21-April-1999  
//*                  Modified to run both batch and Interaactive
//*  Akiya Miyamoto, KEK, 24-July-1999  
//*                  Moved to macro directory. 
//*                  Add new options to get random seed from a file.
//*  Akiya Miyamoto, KEK, 25-January-2000
//*                  Include the use of JSFGUIMergeEvent
//*  Akiya Miyamoto, KEK, 14-July-2000
//*                  Include a call UserAnotherModules
//* 
//$Id$
//
//*************************************************

class JSFJ4;

  TFile *ofile;
  JSFSteer *jsf;
  JSFLCFULL *full;
  JSFQuickSim *sim=0;
  JSFSpring *spring=0;
  JSFSIMDST  *simdst=0;
  JSFModule  *gen=0;
  JSFHadronizer *hdr=0;
  JSFMergeEvent *merge=0;
  JSFJ4         *jsfj4=0;
  JSFJ4         *gJSFJ4=0;

  Int_t gReturnCode; // Return code from event analysis 

  enum EJSFGUIEventType { kPythia=0, kDebug=1, kBasesSpring=2,
                        kReadParton=3, kReadHepevt=4, kNoGenerator=5  , kHerwig=6 };

  Bool_t gHasUserMonitor=kFALSE;

//______________________________________________
int Initialize()
{
  //
  // Initialization of JSF modules.
  //
  gROOT->LoadMacro(jsf->Env()->GetValue("JSFGUI.MacroFileName",
					"UserAnalysis.C"));

  if( gROOT->GetGlobalFunction("UserSetOptions",0,kTRUE) ) UserSetOptions();

  if( gROOT->GetGlobalFunction("UserMonitor",0,kTRUE) ) {
    gHasUserMonitor=kTRUE;
  }

  Char_t *inputFileName=jsf->Env()->GetValue("JSFGUI.InputFileName","");
  Char_t *outputFileName=jsf->Env()->GetValue("JSFGUI.OutputFileName",
					      "jsf.root");
  Int_t irunmode=jsf->Env()->GetValue("JSFGUI.RunMode",1);

  switch(irunmode){
    case 0:
      UserModuleDefine();
      break;
    case 1:
      ofile= new TFile(outputFileName,"RECREATE");
      jsf->SetIOFiles();
      InitGenSim();
      break;
    case 2:
      ofile= new TFile(outputFileName,"RECREATE");
      file = new TFile(inputFileName);  // Input file
      jsf->SetIOFiles();
      jsf->SetOutput(*ofile);
      if( jsf->Env()->GetValue("JSFGUI.SIMDSTConversion",1) == 1 ) {
         simdst = new JSFSIMDST();
         simdst->SetFile(ofile);
         simdst->NoReadWrite(); // Does not output SIMDST data
      }
      break;
    case 3:
      ofile= new TFile(outputFileName,"RECREATE");
      jsf->SetIOFiles();
      simdst = new JSFSIMDST();
      simdst->ReadData();
      simdst->SetDataFileName(inputFileName);
      Char_t parf[256];
      strcpy(parf,inputFileName);
      Int_t lparf=strlen(parf);
      strcpy(&parf[lparf-3],"param");
      JSFQuickSimParam *par=new JSFQuickSimParam();
      par->ReadParamDetector(parf) ;// Read detector parameter
      simdst->SetQuickSimParam(par);
      break;
    case 4:
      ofile = new TFile(outputFileName,"RECREATE");
      jsf->SetIOFiles();
      simdst= new JSFReadJIMBank();
      simdst->SetDataFileName(inputFileName);  // Input file
      break;
    case 5:
      ofile = new TFile(outputFileName,"RECREATE");
      jsf->SetIOFiles();
      simdst = new JSFJIM();
      break;
    default:
      printf("Run mode %d is not supported\n",irunmode);
      return -1;
  }

  if( gROOT->GetGlobalFunction("UserAnotherModules",0,kTRUE) ) UserAnotherModules();

  // ****************************
  // Does Initialize of each JSF Modules.
  // ****************************
  jsf->Initialize();

  if( irunmode == 2  && jsf->Env()->GetValue("JSFGUI.SIMDSTConversion",1) == 1 ) {
    sim=(JSFQuickSim*)jsf->FindModule("JSFQuickSim");
    if( sim ) {
        simdst->SetQuickSimParam(sim->Param());
    }
  }  

  if( spring ) {  spring->ReadBases(
		jsf->Env()->GetValue("JSFGUI.Spring.BasesFile","bases.root"));
  }

  if( jsf->Env()->GetValue("JSFGUI.LastRun",0)) {
    TFile *flast=new TFile(jsf->Env()->GetValue("JSFGUI.LastRunFile",""),
			   "READ");
    jsf->GetLastRunInfo(flast);       // Get seed of last run. 
    flast->Close(); 
  }

  if( jsf->GetOutput() ) { jsf->GetOutput()->cd(); }
  UserInitialize();

  // ****************************
  // Does BerunRun of JSFModules
  // ****************************
  Int_t irun=jsf->Env()->GetValue("JSFGUI.RunNo",1);
  jsf->BeginRun(irun);

}

//______________________________________________
void InitGenSim()
{
  // To initialize modules for event generation and simulation
  // 
  Char_t wrkstr[256], spname[64];
  Int_t eventtype=jsf->Env()->GetValue("JSFGUI.EventType",kPythia);

  bool gentype=gJSF->Env()->GetValue("JSFJ4.UseJupiterGenerator",0);
  if( gentype == 1 ) eventtype=kNoGenerator;

  Float_t ecm;
  sscanf(jsf->Env()->GetValue("JSFGUI.ECM","300.0"),"%g",&ecm);
  JSFModule *genmod;

  full  = new JSFLCFULL();
  switch (eventtype) {
    case kPythia:
      py  = new PythiaGenerator();
      gen = py;
      break;
    case kHerwig:
      py  = new HerwigGenerator();
      gen = py;
      break;
    case kDebug:
      gen=new DebugGenerator();
      break;
    case kBasesSpring:
      gSystem->Load(jsf->Env()->GetValue("JSFGUI.Spring.SharedLibrary",
			 "../FFbarSpring/libFFbarSpring.sl"));
      sprintf(spname,"%s",jsf->Env()->GetValue("JSFGUI.Spring.ModuleName",
					       "FFbarSpring"));

      Char_t hadronizer[128];
      sprintf(hadronizer,"%s",jsf->Env()->GetValue("JSFGUI.Hadronizer",
						   "JSFHadronizer"));
      sprintf(wrkstr,"%s *sp=new %s();",spname, spname);
      gROOT->ProcessLine(wrkstr);
      spring = (JSFSpring*)jsf->FindModule(spname);
      if( strcmp(hadronizer,"JSFHadronizer") == 0 ) {
	hdr    = new JSFHadronizer();
      }
      else {
	Char_t cmdstr[128];
	sprintf(cmdstr,"new %s();",hadronizer);
	gROOT->ProcessLine(cmdstr);
	hdr=gROOT->FindObject(hadronizer);
      }
      gen=spring;
      break;
    case kReadParton:
      gen =new JSFReadParton();
      hdr =new JSFHadronizer();
      break;
    case kReadHepevt:
      gen=new JSFReadGenerator();
      break;
    case kNoGenerator:
      gen=0;
      break;
    default;
      break;
  }

  if( jsf->Env()->GetValue("JSFGUI.SimulationType",1) == 1 ) {
    sim    = new JSFQuickSim();

    if( jsf->Env()->GetValue("JSFGUI.MergeEvent",0 )) {
      merge=new JSFMergeEvent();
      merge->SetDataFileName(
	   jsf->Env()->GetValue("JSFMergeEvent.DataFile","mergeevent.root"));
    }

    simdst = new JSFSIMDST();
    simdst->SetQuickSimParam(sim->Param());
  }

  // Jupiter simulation
  else if( jsf->Env()->GetValue("JSFGUI.SimulationType",1) == 3 ) {
    jsfj4  = new JSFJ4();
    gJSFJ4 = jsfj4;
  }
  else if( jsf->Env()->GetValue("JSFGUI.SimulationType",1) > 1 ) {
    printf("JSFGUI.SimualtionType > 1 is not supported yet.\n");
  }
  //}
    //else {
    //simdst = new JSFJIM();
    //}



  if( jsf->Env()->GetValue("JSFGUI.SIMDST.Output",0) == 0 && simdst ) 
    simdst->NoReadWrite();


  if( jsf->Env()->GetValue("JSFGUI.OutputEventData",0) == 0  ) {
    full->SetMakeBranch(kFALSE);  
    if(sim) sim->SetMakeBranch(kFALSE);   
    if(simdst) simdst->SetMakeBranch(kFALSE);
    if( gen ) gen->SetMakeBranch(kFALSE); 
    if( hdr ) hdr->SetMakeBranch(kFALSE);
  }
 
  if( eventtype == kPythia ) { 
    gROOT->LoadMacro(
	     jsf->Env()->GetValue("JSFGUI.InitPythiaMacro","InitPythia.C"));
    py->SetEcm(jsf->Env()->GetValue("JSFGUI.ECM",300.0));
    InitPythia();         // Set Pythia parameters.
  }
  if( eventtype == kHerwig ) {
	  gROOT->LoadMacro(
	     jsf->Env()->GetValue("JSFGUI.InitHerwigMacro","InitHerwig.C"));
    InitHerwig();         // Set Herwig parameters.
  }


}

//______________________________________________
Bool_t GetEvent(Int_t ev)
{

  if( jsf->GetInput() ) {
    if( !jsf->GetEvent(ev) ) { return kFALSE; }
  }

    Bool_t flag=jsf->Process(ev);
    Int_t iret=jsf->GetReturnCode();

    if( iret & jsf->kJSFFALSE ) return kFALSE;

    if( !( iret & ( jsf->kJSFSkipRestModules|
		    jsf->kJSFDoEndRun|jsf->kJSFTerminate|jsf->kJSFQuit) )) {
      UserAnalysis();

      if( !(iret&jsf->kJSFNotShowDisplay) && (gui != 0) ) gui->DisplayEventData();

      if( !(iret&jsf->kJSFNotShowHistogram) && (gui != 0) ) gui->DrawHist();

    }    

    if( gHasUserMonitor ) {
      UserMonitor();
    }

    if( jsf->GetOutput() ) {
      if( !(iret & jsf->kJSFNoOutput) ) jsf->FillTree();
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
  Int_t irunmode=jsf->Env()->GetValue("JSFGUI.RunMode",1);
  if( irunmode != 2 ) {
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
  Int_t firstrun=jsf->Env()->GetValue("JSFGUI.RunNo",1);
  Int_t lastrun=jsf->Env()->GetValue("JSFGUI.LastRun",-1);

  if( lastrun < 0 ) { lastrun=firstrun; }

  Int_t irun;
  Int_t ievt;
  Bool_t flag=kTRUE;
  JSFSteer::EJSFReturnCode iret=jsf->kJSFOK;
  for(irun=firstrun;irun<=lastrun;irun++){
    if( irun != firstrun ) jsf->BeginRun(irun);
    for( ievt=jsf->Env()->GetValue("JSFGUI.FirstEvent",1);
	 ievt<=jsf->Env()->GetValue("JSFGUI.NEventsAnalize",10); ievt++){
      GetEvent(ievt);
      iret=jsf->GetReturnCode();
      if( iret & jsf->kJSFEOF ) {
	printf("End of event loop due to end-of-file at event# %d\n",ievt);
	break;
      }
      else if( iret & jsf->kJSFFALSE ) {
	printf("End of event loop due to error at event# %d\n",ievt);
	break;
      }
      else if( iret & (jsf->kJSFDoEndRun|jsf->kJSFTerminate|jsf->kJSFQuit)) {
	goto EndRun;
      }
    }

    EndRun:
      flag=jsf->EndRun();
      if( flag & (jsf->kJSFTerminate|jsf->kJSFQuit) ) break;
  }
  JobEnd();

}




