//*************************************************
//* Main Macro for JSF execution through GUI
//*
//*(Author)
//*  Akiya Miyamoto, KEK, 8-March-1999  
//*************************************************

  TFile *ofile;
  JSFSteer *jsf;
  JSFLCFULL *full;
  JSFQuickSim *sim;
  
  Int_t gRunMode;

  enum EventType { kPythia=0, kDebug=1};


//______________________________________________
int Initialize()
{

  gRunMode=gui->GetRunMode();

  switch(gRunMode){
    case 1:
      ofile= new TFile(gui->GetOutputFileName(),"RECREATE");
      jsf->SetIOFiles();
      InitGenSim();
      break;
    case 2:
      ofile= new TFile(gui->GetOutputFileName(),"RECREATE");
      file = new TFile(gui->GetInputFileName());  // Input file
      jsf->SetIOFiles();
      simdst = new JSFSIMDST();
      simdst->SetFile(ofile);
      simdst->NoReadWrite(); // Does not output SIMDST data
      break;
    case 3:
      ofile= new TFile(gui->GetOutputFileName(),"RECREATE");
      jsf->SetIOFiles();
      simdst = new JSFSIMDST();
      simdst->ReadData();
      simdst->SetDataFileName(gui->GetInputFileName());
      Char_t parf[256];
      strcpy(parf,gui->GetInputFileName());
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
  
  gROOT->LoadMacro(gui->GetMacroFileName());

  UserInitialize();

  jsf->BeginRun(1);              // Set run number to 1.
}

//______________________________________________
void InitGenSim()
{

   Int_t eventtype=gui->GetEventType();
   Float_t ecm=gui->GetEcm();

   full  = new JSFLCFULL();
   switch (eventtype) {
      case kDebug:
	dbg=new DebugGenerator();
	break;
      default:
        gROOT->LoadMacro(gui->GetInitPythiaMacro());
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
  
   //  full->SetMakeBranch(kFALSE);   // suppress output of EventBuf 
   //  py->SetMakeBranch(kFALSE);     // suppress output of EventBuf 
   //  sim->SetMakeBranch(kFALSE);    // suppress output of EventBuf
   //  simdst->SetMakeBranch(kTRUE);     // suppress output of EventBuf
 
   if( eventtype != kDebug ) { 
     py->SetEcm(ecm);             // Center of mass energy (GeV)
     InitPythia();         // Set Pythia parameters.
   }

}

//______________________________________________
Bool_t GetEvent(Int_t ev)
{
    if( gRunMode==2 && !jsf->GetEvent(ev) ) {
      gui->SetReturnCode(-2);
      return kFALSE;
    }      
    if( !jsf->Process(ev)) {
      gui->SetReturnCode(-1);
      return kFALSE;
    }
    gui->DisplayEventData();
    UserAnalysis();
    gui->DrawHist();
    gui->SetReturnCode(0);
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
  if( ofile ) ofile->Write();

}
