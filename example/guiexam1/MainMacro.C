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

enum EventType { kPythia=0, kDebug=1};


//______________________________________________
int Initialize()
{

  Int_t runmode=gui->GetRunMode();

  printf(" runmode = %d\n",runmode);

  switch(runmode){
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
      break;
    default:
      printf("Run mode %d is not supported\n",runmode);
      return -1;
  }

  jsf->Initialize();
  
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
  printf(" Can not go to previous events.\n");
  //  Int_t nevt=jsf->GetEventNumber()-1;
  //  GetEvent(nevt);
}

//______________________________________________
void JobEnd()
{
  jsf->Terminate();
  if( !ofile ) ofile->Write();

}
