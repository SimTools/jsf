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
class JSFMEGenerator;

  TFile *ofile; // Output file
  JSFSteer *jsf;
  JSFLCFULL *full;
  JSFQuickSim *sim=0;
  JSFSpring *spring=0;
  JSFSIMDST  *simdst=0;
  JSFModule  *gen=0;
  JSFModule  *hdr=0;
  JSFMergeEvent *merge=0;
  JSFJ4         *jsfj4=0;
  JSFJ4         *gJSFJ4=0;
  JSFMEGenerator *megen=0;

  Int_t gReturnCode; // Return code from event analysis 

  enum EJSFGUIEventType { kPythia=0, kDebug=1, kBasesSpring=2,
                        kReadParton=3, kReadHepevt=4, kNoGenerator=6  , kHerwig=5 };

  Bool_t gHasUserMonitor=kFALSE;

//______________________________________________
int Initialize(Char_t *fin="undef")
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
 
  Char_t *inputFileName="";
  if ( strcmp(fin,"undef") == 0 ) {
    inputFileName=jsf->Env()->GetValue("JSFGUI.InputFileName","");
  }
  else { inputFileName=fin;
  }
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
  if( gentype == 1 ) {
    eventtype=kNoGenerator;
  }
  else {
    full  = new JSFLCFULL();
  }

  Float_t ecm;
  sscanf(jsf->Env()->GetValue("JSFGUI.ECM","300.0"),"%g",&ecm);
  JSFModule *genmod;
  Char_t classname[128];
  Char_t cmdstr[128];
  Char_t hadronizer[128];

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
      sprintf(classname,"%s",jsf->Env()->GetValue("JSFGUI.MEGenerator","No"));
      sprintf(hadronizer,"%s",jsf->Env()->GetValue("JSFGUI.Hadronizer",
						   "JSFHadronizer"));
      if( strcmp(classname,"No") == 0 ) {
	gSystem->Load(jsf->Env()->GetValue("JSFGUI.Spring.SharedLibrary",
			 "../FFbarSpring/libFFbarSpring.sl"));
	sprintf(spname,"%s",jsf->Env()->GetValue("JSFGUI.Spring.ModuleName",
						 "FFbarSpring"));
	sprintf(wrkstr,"%s *sp=new %s();",spname, spname);
	gROOT->ProcessLine(wrkstr);
	spring = (JSFSpring*)jsf->FindModule(spname);
	gen=spring;
      }
      else {
	sprintf(wrkstr,"%s *sp=new %s();",classname, classname);
	gROOT->ProcessLine(wrkstr);
	megen = (JSFMEGenerator*)jsf->FindModule(classname);
	gen=megen;
      }

      if( strcmp(hadronizer,"JSFHadronizer") == 0 ) {
	hdr    = (JSFModule*)new JSFHadronizer();
      }
      else {
	sprintf(cmdstr,"new %s();",hadronizer);
	gROOT->ProcessLine(cmdstr);
	hdr=(JSFModule*)gROOT->FindObject(hadronizer);
      }
      break;
    case kReadParton:
      gen =new JSFReadParton();
      hdr =(JSFModule*)new JSFHadronizer();
      break;
    case kReadHepevt:
      sprintf(classname,"%s",jsf->Env()->GetValue("JSFGUI.JSFReadGenerator.ClassName",
					       "JSFReadGenerator"));
      sprintf(cmdstr,"new %s();",classname);
      printf(" classname is %s\n",classname);
      gROOT->ProcessLine(cmdstr);
      gen=gROOT->FindObject(classname);
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

  if( jsf->Env()->GetValue("JSFGUI.OptModules",0) != 0 ) {
     gROOT->LoadMacro(
             jsf->Env()->GetValue("JSFGUI.OptFunctions","OptFunctions.C"));
     InitOptionalModules();
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
  Bool_t flag=gROOT->GetGlobalFunction("UserTerminate",0,kTRUE) ;
  jsf->Terminate();
  if( flag ) {  UserTerminate(); }
  if( ofile ) ofile->Write(0,jsf->GetWriteMode());
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
  if ( strcmp(jsf->Env()->
	      GetValue("JSFGUI.InputFileName.F1","undef"),"undef")==0 ) {
    Batch_MultiRun();
  }
  else {
    Batch_MultiInputs();
  }

}

//_________________________________________________________
void Batch_MultiRun()
{
  Initialize();
  Int_t firstrun=jsf->Env()->GetValue("JSFGUI.RunNo",1);
  Int_t lastrun=jsf->Env()->GetValue("JSFGUI.LastRun",-1);
  string fnbegin(jsf->GetOutput()->GetName());

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
    string fnend(jsf->GetOutput()->GetName());

    if ( strcmp(fnbegin.data(), fnend.data()) != 0 ) {
      jsf->OTree()->GetCurrentFile()->Write();
      ofile=new TFile(fnbegin.data(),"UPDATE");
      jsf->SetOutput(*ofile);
    }

      flag=jsf->EndRun();
      if( flag & (jsf->kJSFTerminate|jsf->kJSFQuit) ) break;
  }

  JobEnd();

}

//_________________________________________________________
void Batch_MultiInputs()
{
  // Script to process multiple input files as a single input file
  vector<string> *inputs=SetInputFiles();
  string fn=(*inputs)[0];

  Initialize(fn.data());
  Int_t irun=jsf->Env()->GetValue("JSFGUI.RunNo",1);
  Int_t ninputs=inputs->size();
  Int_t ifile=0;

  Int_t irun;
  Bool_t flag=kTRUE;
  JSFSteer::EJSFReturnCode iret=jsf->kJSFOK;
  jsf->BeginRun(irun);

  Int_t nread=0;
  Int_t ievt=jsf->Env()->GetValue("JSFGUI.FirstEvent",1);
  while( ievt < jsf->Env()->GetValue("JSFGUI.NEventsAnalize",1000) ) {
      GetEvent(++nread);
      iret=jsf->GetReturnCode();
      if( iret == jsf->kJSFOK ) {
	ievt ++ ;
      }
      else if( iret & jsf->kJSFEOF ) {
        cout << "End-of-file of" << fn << " detected ";
        cout << " after reading " << nread-1 << " events " << endl;
        ifile++;
        if( ifile >= ninputs ) break;
	fn=(*inputs)[ifile];
        TFile *nfile=new TFile(fn.data());
        jsf->SetInput(*nfile);
	nread=0;
      }
      else if( iret & jsf->kJSFFALSE ) {
	printf("End of event loop due to error at event# %d\n",ievt);
	break;
      }
      else if( iret & (jsf->kJSFDoEndRun|jsf->kJSFTerminate|jsf->kJSFQuit)) {
	flag=jsf->EndRun();
	break;
      }
  }

  JobEnd();

  cout << "JSFGUI Batch Multi-File process completed" << endl;
  cout << "Read " << ievt-1 << " events ";
  cout << " from " << ifile << " files " << endl;

}


//_________________________________________________________
vector<string> *SetInputFiles()
{
  Int_t maxinp=jsf->Env()->GetValue("JSFGUI.MaxInputFiles",3000);
  vector<string> *inp=new vector<string>();
  Char_t     wrkstr[128];
  Int_t nundef=0;
  for(Int_t i=0;i<maxinp;i++){
    sprintf(wrkstr,"JSFGUI.InputFileName.F%d",i+1);
    Char_t *fn=jsf->Env()->GetValue(wrkstr,"undef");
    if( strcmp(fn,"undef") != 0 ) {
      inp->push_back(string(fn));
      nundef=0;
    }
    else {
      nundef++;
      if ( nundef > 10 ) break;
    }
  }
  return inp;
}

  

