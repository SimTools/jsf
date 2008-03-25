
//////////////////////////////////////////////////////////////////////////////
//
//  JLC Study Frame
//
//  Main class to control JLC Study 
//
//  Three types of jobs are supported in this framework,  a) Bases, 
//  b)  Read Bases data->Spring->Hadronize->QuickSim, c) read quick sim data.
//
//  Typical macro files are as follows.
//  a) Bases
//       
//       {
//       // An example of Bases. 
//       // Calculate cross section of e+e- --> gamma* --> f+f- 
//         gROOT->Reset(); 
//         TFile file("bases.root","RECREATE");  // Output file.
//         bases = new LLbarBases();
//         bases->SetEcm(300.0);            // Set ECM
//         bases->SetParton(1, 2.0/3.0 );   // Set Parton
//         bases->DoBases();
//         bases->Write();
//         file->Write();
//       }
//      
//     To run in batch mode, do
//       jsf -q -b llbar.C
//     where llbar.C is the name of the macro file.
//
//   b) Spring->Hadronize->QuickSim
//       
//       {
//       // An example to run Spring, Hadronizer, and QuickSim.
//       // In this example, LLbar event is created.
//         gROOT->Reset();
//         TFile file("jsf.root","RECREATE");  // Output file
//       
//         jsf    = new JSFSteer();             
//         full   = new JSFLCFULL();
//         spring = new LLbarSpring();         
//         spring->ReadBases("bases.root");
//         hdr=new JSFHadronizer();
//         sim=new JSFQuickSim();
//
//         Int_t maxevt=10;
//         jsf->Initialize();
//         jsf->BeginRun(30);                  // Set Run Number
//         for(Int_t ev=1;ev<=maxevt;ev++){
//           printf(" start event %d\n",ev);
//           jsf->Process(ev);
//           jsf->FillTree();
//           jsf->Clear();
//         }
//         jsf->Terminate();
//     //     file->Write(); // file->Write() is not necessary since JSF version 2
//     //                    //  because it is done in JSFSteer::Terminate();
//       }
//
//    c) Read QuickSim data.
//         
//      {
//      //  Example to read QuickSim data.
//        TFile f("jsf.root","READ");
//        jsf = new JSFSteer();
//        jsf->Initialize();
//
//        Int_t maxevt=3;
//
//        for(Int_t i=1;i<=maxevt;i++){
//          jsf->GetEvent(i);
//          if( !jsf->Process(i) ) break ;
//          printf(" read Run %d Event %d\n",jsf->GetRunNumber(), jsf->GetEventNumber());
//        }
//        jsf->Terminate();
//      }
//
//$Id$
//(Update)
//  6-Jan-2001  A.Miyamoto Use Root.3.00 IO functions
//                         Class JSFSteerConf was modified.
// 22-Jan-2005  A.Miyamoto Allow multiple-file output, use of TChain 
// 23-Feb-2005  A.Miyamoto Default Write mode is "Overwrite".
//
/////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

#include <TROOT.h>
#include <TSystem.h>
#include <TChain.h>
#include <TTree.h>
#include <TBrowser.h>
#include <TClonesArray.h>
#include <TApplication.h>
#include <TFile.h>
#include <TKey.h>
#include <TString.h>
#include <TBranchObject.h>

#include "JSFSteer.h"
#include "JSFConfig.h"
#include "JSFModule.h"
#include <TBenchmark.h>

#if __GNUC__ >= 5 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)
#include <cstdlib>
#endif

// #include "JSFQuickSim.h"

JSFSteer *JSFSteer::myself=0;
JSFSteer *gJSF=0;
Int_t     JSFSteer::fWriteMode=TObject::kOverwrite;

ClassImp(JSFSteer)
ClassImp(JSFSteerConf)
ClassImp(JSFSteerBuf)

static TList *gJSFEventList=0;
static TKey  *gJSFEventKey=0;
static TBranch *gJSFBranch=0;

TStopwatch  *gStopwatch=0;

//---------------------------------------------------------------------------
JSFSteerBuf::JSFSteerBuf(const char *name, const char *title) 
        : TNamed(name, title)
{
  fVersion    = __JSF_VERSION__  ;  // JSFSteer version number
  fVersionDate  = __JSF_VERSIONDATE__ ; // version date.
  fMinorVersion = __JSF_MINORVERSION__ ; // Minor Version number
  fPatchLevel   = __JSF_PATCHLEVEL__   ; // Patch level
  fRun          = 0 ;
  fEvent 	= 0 ;
  TDatime dtime;
  fDate		= dtime.GetDate() ;
  fTime		= dtime.GetTime() ;

}

//---------------------------------------------------------------------------
JSFSteerBuf::~JSFSteerBuf()
{
}

//----------------------------------------
JSFSteer *JSFSteer::Instance()
{
  if (!myself ) {
	myself=new JSFSteer();
  }
  if( ! gJSF ) {
    gJSF=myself;  // Just for safety
  }
  return myself;
}


//---------------------------------------------------------------------------
JSFSteer::JSFSteer(const char *name, const char *title) 
        : TNamed(name, title)
{

  //  Default constructor of JLC study framework
  fModules = 0;
  fConf    = 0;
  fReadin  = 0;

  fIsInitialized = kFALSE ;
  fIsTerminated  = kFALSE ;
  fLastRun       = 0 ;
//  fRun           = 0 ;
  fRunEnded      = kFALSE;
  fEventBuf    = 0; 

  fITree       = 0;
  fOTree       = 0;
  fChain       = 0;
  fModules = 0;
  fConf    = new JSFSteerConf("JSFSteerConf", "JSF Configuration");
  fEvents  = 0 ;
  fEventBuf = 0;

  TTree::SetBranchStyle(0);

  if( strcmp(name,"JSF") == 0 ) {
    if( gJSF ) return ;
    gJSF=this;

    // To Get parameters through TEnv
    // In the command line, do jsf -conf=file_name
    Char_t *envfile="jsf.conf";
    TApplication *ap=gROOT->GetApplication();
    for(Int_t i=0;i<ap->Argc();i++){
      if( strncmp(ap->Argv(i),"-conf=",6) == 0 ){
        Int_t lstr=strlen(ap->Argv(i));
        Char_t *nenvfile=new Char_t[lstr];
        strcpy(nenvfile,(ap->Argv(i)+6));
        envfile=nenvfile;    
      }
    }
    fEnv=new JSFEnv(envfile);  // Create pointer to env file.
    fEventTrees=new TObjArray(); // Remember defined trees.
    fEvents = new TObjArray();   // Collection of EventBuf;
    fModules = new TList();
    fEventBuf = new JSFSteerBuf("JSFSteerBuf", "JSF Event Header");
 
    if( fEnv->GetValue("JSF.Benchmark",1) == 1 ) {
      printf("TBenchmark is initialized\n");
      for(Int_t i=0;i<30;i++) { fCPUTime[i]=0;  fRealTime[i]=0; }
      gStopwatch=new TStopwatch();
      fCPUTime[0]=0;
      fRealTime[0]=0;
      gStopwatch->Reset();
      gStopwatch->Start();
    }
  
    gROOT->LoadMacro(fEnv->GetValue("JSF.LoadLibraryMacro","LoadLibraries.C"));

    char cmd[2048];
    sprintf(cmd,"{long env__ptr=0x%x; LoadLibraries((JSFEnv*)env__ptr);}",(UInt_t)fEnv);
    gROOT->ProcessLine(cmd);
 
    SetIOFiles();

    LoadSharedLibraries();

  }
}

//---------------------------------------------------------------------------
void JSFSteer::LoadSharedLibraries()
{
// Load shared libraries specified in JSF.SharedLibraries   environment variable.
//  

   Char_t shlibs[2056];

  sscanf(fEnv->GetValue("JSF.SharedLibraries","undef"),"%s",shlibs);
  if( strcmp("undef",shlibs) == 0 ) return ;
  Int_t lsh=strlen(shlibs);
  Char_t fn[256];
  Int_t  j=0;
  for(Int_t i=0;i<lsh+1;i++){
    if( shlibs[i] == ' ' || shlibs[i] == '\t' ) {
      continue;
    }
    else if( shlibs[i] != ':' && shlibs[i] != 0) {
      fn[j++]=shlibs[i];
      fn[j]=0;
    }
    else {
      Int_t lfn=strlen(fn);
      if( lfn > 3 ) {
	gSystem->Load(fn);
	printf("JSFSteer::LoadSharedLibraries sucessfully loaded %s\n",fn);
      }
      j=0;
      fn[0]=0;
    }
  }
}

//---------------------------------------------------------------------------
JSFSteer::~JSFSteer()
{
//  Terminate JSF, if fIsTerminated=kFALSE, then delete fModules
//  

  if (fModules) { 
    if( !fIsTerminated ) Terminate(); 
    TIter next(fModules);
    JSFModule *module; 
    while( (module = (JSFModule*)next()) ) { 
      delete module; }
  }
  if (fConf) { delete fConf; }
  if (fEventTrees) { delete fEventTrees; }
  if ( fEvents ) { delete fEvents; }
  if ( fEventBuf ) { delete fEventBuf; }
  if ( fReadin ) { delete fReadin; }

}



//---------------------------------------------------------------------------
void JSFSteer::SetIOFiles()
{
  //  Set Flags for file IO, should be called after Input/Output file is assigned.
  //  This function is from the constructor of JSFSteer.
  //  If file is opened ( checked by gFile->IsOpen() ), variables fIFile and 
  //  fOFile are set according to the open mode.  
  //  fIFile=gFile, if TFile option is READ, fOFile=gFile, is CREATED,
  //  and fIFile=fOFile=gFile, if UPDATE.
  //  After JSFSteer object is created, fIFile and fOFile can be reset by using
  //  SetInput and SetOutput member function of JSFSteer class.
  //
  //  JSFModule class has fFile data member.  fFile=fIFile, if the module is 
  //  executed in input mode. fFile=fOFile, if the module is executed output mode.
  //  If the JSF module object is created in a user macro, it is in output mode.
  //  If the JSF is executed in read-root-file mode, the JSF module objects are
  //  created by JSFSteer though it is created explicitly in the user macro.
  //  This is necessary to store data in a file to memory.  Such objects are 
  //  shown in the job lists with a title "Readin module".  The Readin modules
  //  are executed in input mode. 
  //
  //  When Initialize/BeginRun/EndRun/Terminate function of user module is 
  //  called, corresponding functions in JSFSteer change directory to 
  //  conf/init, conf/beginNNNNN, conf/endNNNNN, conf/term, respectively.
  //  If the module is Readin Module, conf directory is in fIfile.
  //  If not, it is in fOFile.  The process function of JSFSteer class does not
  //  call process function of the Readin module.
 
    // Set fIFile, fOFile according to the status of gFile
    fIFile=0;
    fOFile=0;
    
    if( gFile && gFile->IsOpen() ) {
      if( strcmp(gFile->GetOption(),"READ")==0 ) fIFile=gFile;
      else if ( strcmp(gFile->GetOption(),"CREATE")==0 ) fOFile=gFile;
      else if ( strcmp(gFile->GetOption(),"UPDATE")==0) { 
	fIFile=gFile ; fOFile=gFile ;
      }
    }
    if ( fIFile && Env()->GetValue("JSF.EnvParameter.GetFromInput",kTRUE) ) {
      TDirectory *lastdir=gDirectory;
      fIFile->cd("conf");
      JSFSteerConf rdconf("JSFSteerConfReadin", "JSF Configuration");
      rdconf.Read("JSF");
      Bool_t replace=Env()->GetValue("JSF.EnvParameter.Replace",kFALSE);
      fEnv->Add(rdconf.GetEnv(), replace); // Replace, if not defined in file.
      std::cerr << "JSFSteer::SetIOFiles load EnvParameter values from ";
      std::cerr << fIFile->GetName() << endl;
      lastdir->cd();
    }	
}

//____________________________________________________________________________
void JSFSteer::CheckFilePointers()
{
  if ( fIFile==0 && fOFile == 0 ) SetIOFiles();
}

//_____________________________________________________________________________
void JSFSteer::InitializeMessage()
{
//  Print message after jsf initialization

  printf("****  JSF Initialization *******************************************\n");
  printf("      Version %d : Version Date %d \n",GetVersion(),GetVersionDate());
  TDatime dtime;
  printf("      Current date and time : %s\n",dtime.AsString());

  printf("====  List of defined modules ======================================\n");
  TIter next(fModules);
  JSFModule *module;
  printf("      Module name          Tile           Writing? \n");
  printf("--------------------------------------------------------------------\n");
  while (( module = (JSFModule*)next())) {
    printf("      %-20s %-20s %5d\n",
	   module->GetName(),module->GetTitle(),module->IsWritable());
  }
  printf("********************************** end of JSF initialize message ****\n");
}  


//_____________________________________________________________________________
Bool_t JSFSteer::Process(Int_t i)
{
//  Executes JSFModule::Process(Int_t event).
//  If not initialized yet, execute Initialize()
//  If begin-run is not processed yet, execute beginrun() ( and endrun() if
//  necessary.
//  

// Initialize

  if( !fIsInitialized ) {  
    if( !Initialize() ) { fReturnCode=fReturnCode|kJSFQuit|kJSFFALSE ; return kFALSE; } }

  if( !fIsGetEvent ) SetEventNumber(i);

  fReturnCode=kJSFOK;

// BeginRun
  if( fLastRun == 0 ) { 
    if( GetRunNumber() == 0 ) {
      Warning("Process","Run number is undefined. Set default run number 1");
      SetRunNumber(1);
    } 
    if( !BeginRun(GetRunNumber()) ) { fReturnCode+=kJSFTerminate|kJSFFALSE; return kFALSE; }
  }
  else if( fLastRun != GetRunNumber() ) {
    if( !EndRun() ) { fReturnCode+=kJSFTerminate|kJSFFALSE; return kFALSE; }
    if( !BeginRun(GetRunNumber()) ) { fReturnCode+=kJSFTerminate|kJSFFALSE; return kFALSE; }
  }

// Loop over all module to process event data

   if( fOFile ) {
     fOFile=fOTree->GetCurrentFile();
     fOFile->cd();
   }
   TIter next(fModules);
   JSFModule *module;
   Int_t iloop=0;
   while (( module = (JSFModule*)next())) {
//
     if( module->GetFile() != fIFile ) { module->SetFile(fOFile); }
     module->SetModuleStatus(kEventLoop);
     if( module->IsWritable() ) {
        Double_t cputime=0;
	Double_t realtime=0;
	if( gStopwatch ) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,3,0)
	  gStopwatch->Stop();
	  cputime=gStopwatch->CpuTime();
	  realtime=gStopwatch->RealTime();
	  gStopwatch->Continue();
#else
	  cputime=gStopwatch->GetCPUTime();
	  realtime=gStopwatch->GetRealTime();
#endif
	}
	Bool_t rcode=module->Process(i);
	if( gStopwatch ) {
	  iloop++;
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,3,0)
	  gStopwatch->Stop();
	  fCPUTime[iloop] += gStopwatch->CpuTime()-cputime;
	  fRealTime[iloop] += gStopwatch->RealTime()-realtime;
	  gStopwatch->Continue();
#else
	  fCPUTime[iloop] += gStopwatch->GetCPUTime()-cputime;
	  fRealTime[iloop] += gStopwatch->GetRealTime()-realtime;
#endif
	}

        if( !rcode ) { fReturnCode+=kJSFFALSE; return kFALSE; }

	if( fReturnCode&kJSFSkipRestModules ) break;
     }
   }

   fIsGetEvent=kFALSE;

   return kTRUE;
}



//_____________________________________________________________________________
Bool_t JSFSteer::Initialize()
{
//  Initialize JSF environment, namely
//  (1) Make/SetTree
//      If output file is writable, create conf directory and write
//      JSFSteer class.
//  (2) Print initial message.
//  (3) Calls JSFModule::Initialize()
//   
   if (fIsInitialized) return kTRUE; 
   fIsInitialized=kTRUE;

   // Setup branches for 

   if( fIFile == NULL && fOFile == NULL ) {
     printf("Fatal error in JSFSteer::Initialize() .. Neither input file ");
     printf("nor output file is defined.\n");
     printf("Put the statement as follows before JSFSteer object is created.\n");
     printf("    file=new TFile(\"filename\",\"RECREATE\"); \n");
     printf("This will open output file.\n");
     gSystem->Exit(16);
   }
  cerr << "Output file name is " << fOFile->GetName() << endl;

   // If Input file is opened, setup tree to read
   if( !fChain && fIFile && fIFile->IsOpen() ) { 
     if( !SetupTree() ) return kFALSE; 
   } 

   // If Output file is writable, make tree for them
   if( fOFile && fOFile->IsOpen() && fOFile->IsWritable() ) {
       fOFile->cd("/");
       MakeConfDir();
       if( fEnv->GetValue("JSF.MakeJSFTree",1) == 1 ) {
	 fOFile->cd("/");
	 if( !MakeTree() ) return kFALSE; 
       }
   }

   InitializeMessage();
   TDirectory *last=gDirectory;
   TListIter next(fModules);
   JSFModule *module;
   Int_t iloop=0;
   while ((module = (JSFModule*)next())) {
     module->SetModuleStatus(kInitialize);
     module->GetFile()->cd();

     Double_t cputime=0;
     Double_t realtime=0;
     if( gStopwatch ) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,3,0)
       gStopwatch->Stop();
       cputime=gStopwatch->CpuTime();
       realtime=gStopwatch->RealTime();
       gStopwatch->Continue();
#else
       cputime=gStopwatch->GetCPUTime();
       realtime=gStopwatch->GetRealTime();
#endif
     }
     Bool_t rcode=module->Initialize();
     if( gStopwatch ) {
       iloop++;
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,3,0)
       gStopwatch->Stop();
       fCPUTime[iloop] += gStopwatch->CpuTime()-cputime;
       fRealTime[iloop] += gStopwatch->RealTime()-realtime;
       gStopwatch->Continue();
#else
       fCPUTime[iloop] += gStopwatch->GetCPUTime()-cputime;
       fRealTime[iloop] += gStopwatch->GetRealTime()-realtime;
#endif
     }
     if( !rcode ) return kFALSE;
   }

   last->cd();

   return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFSteer::BeginRun(Int_t nrun)
{
  // 
  //  Calls JSFModule::BeginRun
  // 
  //  If fIFile == NULL, input, nrun, is used for run number.
  //  If fIFile != NULL, run number is obtained from a file, fIFile.
  //  If conf directory in fIFile contains more than two run, 
  //  run number of smallest run number is selected.

  // TDirectory *curdir=gDirectory;

  Char_t keyname[32];
  if( fIFile != NULL ) {
    fIFile->cd("/conf");
    Int_t lrun=99999999;
    TList *dlist=gDirectory->GetListOfKeys();
    TListIter nkey(dlist);
    TKey  *key;
    Int_t irun;
    while ((key = (TKey*)nkey())) {
       Char_t tname[20];
       strncpy(tname,key->GetName(),strlen(key->GetName()));
       if( strncmp(tname,"begin",5) == 0 ) {
	  sscanf(tname+5,"%d",&irun);
	  if( irun < lrun ) { lrun = irun; }
       }
    }
    SetRunNumber(lrun);
  }
  else {
    SetRunNumber(nrun);
  }

  fLastRun = GetRunNumber();
  fRunEnded = kFALSE ;
//  Make a begin-Run directory in the output file

  sprintf(keyname,"begin%5.5d",GetRunNumber());

   // If Output file is writable, make tree for them
   if( fOFile && fOFile->IsOpen() && fOFile->IsWritable() ) {
      fOFile->cd("/conf");
      if(!gDirectory->FindKey(keyname) ) gDirectory->mkdir(keyname);
   }

  sprintf(keyname,"/conf/begin%5.5d",GetRunNumber());
  TIter next(fModules);
  JSFModule *module;
  Int_t iloop=0;
  while (( module = (JSFModule*)next())) {
    module->SetModuleStatus(kBeginRun);
    module->SetRunNumber(GetRunNumber());
    module->GetFile()->cd(keyname);

    Double_t cputime=0;
    Double_t realtime=0;
    if( gStopwatch ) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,3,0)
      gStopwatch->Stop();
       cputime=gStopwatch->CpuTime();
       realtime=gStopwatch->RealTime();
      gStopwatch->Continue();
#else
       cputime=gStopwatch->GetCPUTime();
       realtime=gStopwatch->GetRealTime();
#endif
    }
    Bool_t rcode=module->BeginRun(GetRunNumber());
    if( gStopwatch ) {
      iloop++;
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,3,0)
      gStopwatch->Stop();
      fCPUTime[iloop] += gStopwatch->CpuTime()-cputime;
      fRealTime[iloop] += gStopwatch->RealTime()-realtime;
      gStopwatch->Continue();
#else
      fCPUTime[iloop] += gStopwatch->GetCPUTime()-cputime;
      fRealTime[iloop] += gStopwatch->GetRealTime()-realtime;
#endif
    }

    if( !rcode ) return kFALSE;

  }
  if( fOFile && fOFile->IsOpen() && fOFile->IsWritable() ) {
       fOFile->Write(0, GetWriteMode());
  }


  if( fIFile ) fIFile->cd("/");
  return kTRUE;
}


//_____________________________________________________________________________
Bool_t JSFSteer::EndRun()
{
//
//  Calls JSFModule::EndRun
//

    fRunEnded = kTRUE;
    fReturnCode = kJSFOK;

    Char_t keyname[32];
    sprintf(keyname,"end%5.5d",GetRunNumber());
   // If Output file is writable, make endrun directory
   if( fOFile && fOFile->IsOpen() && fOFile->IsWritable() ) {
      fOFile->cd("/conf");
      if( !gDirectory->FindKey(keyname) ) gDirectory->mkdir(keyname);
   }

    sprintf(keyname,"/conf/end%5.5d",GetRunNumber());

    TIter next(fModules);
    JSFModule *module;
    Int_t iloop=0;
    while (( module = (JSFModule*)next())) {
        module->SetModuleStatus(kEndRun);
	fOFile->cd(keyname);
//	module->GetFile()->cd(keyname);

	Double_t cputime=0;
	Double_t realtime=0;
	if( gStopwatch ) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,3,0)
	  gStopwatch->Stop();
	  cputime=gStopwatch->CpuTime();
	  realtime=gStopwatch->RealTime();
	  gStopwatch->Continue();
#else
	  cputime=gStopwatch->GetCPUTime();
	  realtime=gStopwatch->GetRealTime();
#endif
	}
	Bool_t rcode=module->EndRun();
	if( gStopwatch ) {
	  iloop++;
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,3,0)
	  gStopwatch->Stop();
	  fCPUTime[iloop] += gStopwatch->CpuTime()-cputime;
	  fRealTime[iloop] += gStopwatch->RealTime()-realtime;
	  gStopwatch->Continue();
#else
	  fCPUTime[iloop] += gStopwatch->GetCPUTime()-cputime;
	  fRealTime[iloop] += gStopwatch->GetRealTime()-realtime;
#endif
	}

	if( !rcode ) { fReturnCode+=kFALSE ; return kFALSE; }
    }
    
    if( fOFile && fOFile->IsOpen() && fOFile->IsWritable() ) {
          fOFile->Write(0, GetWriteMode());
    }
    if( fOFile ) fOFile->cd("/");
    return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFSteer::GetLastRunInfo(TFile *file, Int_t lastrun)
{
//
//  Calls JSFModule::GetLastRunInfo(TFile *file, Int_t lastrun)
//  Purpose of this function is to read seed of randam number, etc
//  from conf/endNNNNN directory.
//  If lastrun < 0, last run number in conf/[classname] is used.
//

  cout << "JSFSteer::GetLastRunInfo... File=" << file->GetName() << endl;

  TDirectory *curdir=gDirectory;

  file->cd("/conf");
  Char_t keyname[32];
  Int_t lrun=lastrun;
  if( lrun < 0 ) {
    TList *dlist=gDirectory->GetListOfKeys();
    TListIter nkey(dlist);
    TKey  *key;
    Int_t irun;
    while ((key = (TKey*)nkey())) {
       Char_t tname[20];
       strncpy(tname,key->GetName(),9);
       if( tname[0] == 'e' && tname[1]=='n' && tname[2]=='d' ) {
	  sscanf(&tname[3],"%d",&irun);
	  if( irun > lrun ) { lrun = irun; }
       }
    }
  }

  if( lrun < 0 ) {
    printf("Error in JSFSteer::GetLastRunInfo() \n");
    printf(" No endrun directory was found in conf directory ");
    printf(" of file %s\n",file->GetName());
    return kFALSE;
  }

  sprintf(keyname,"/conf/end%5.5d",lrun);
  if( !file->cd(keyname) ) {
    printf("Error in JSFSteer::GetLastRunInfo() \n");
    printf(" Unable to change directory to %s\n",keyname);
    return kFALSE;
  }

  printf("JSFSteer::GetLastRunInfo(...)");
  printf(" Get Last Run information from a file %s\n",file->GetName());
  //
  TIter next(fModules);
  JSFModule *module;
  while (( module = (JSFModule*)next())) {
    Bool_t rc=module->GetLastRunInfo();
    if( !rc ) return kFALSE;
  }

  curdir->cd();
  return kTRUE;

}

//_____________________________________________________________________________
Int_t JSFSteer::GetEvent(Int_t nevt)
{
// Readin event data.
// nevt is the number from 1 to n, Note the difference with 
// the event number of root, which ranges from 0 to n-1.

// Remember EventBuf name and module addresses.
  
   TIter  next(fModules);
   vector<JSFModule*> modadr;
   vector<string>     bufnames;

   JSFModule *module;
   while( (module=(JSFModule*)next()) ){
     if ( !module->IsWritable() ) {
        if ( module->EventBuf() ) {
          modadr.push_back(module);
          string strname(module->EventBuf()->GetName());
          bufnames.push_back(strname);
        }
      }
   }

   Int_t nb=fITree->GetEntry(nevt-1, 0);

   Int_t numbuf=bufnames.size();
   for(Int_t i=0;i<numbuf;i++){
     JSFEventBuf *buf=gJSF->FindEventBuf(bufnames[i].data());
     JSFModule   *mod=modadr[i];
     mod->SetEventBufAddress(buf);
   }

   SetRunNumber(fReadin->GetRunNumber());
   SetEventNumber(fReadin->GetEventNumber()); 
   fIsGetEvent=kTRUE;
   fReturnCode=kJSFOK;


   if( GetRunNumber() == 0 ) { 
	cerr << " Run number is 0 .. Funny!!! " << endl;
        exit(0);
   }

   if( !nb ) { fReturnCode = kJSFEOF|kJSFFALSE; }

   return nb; 
}

//_____________________________________________________________________________
Bool_t JSFSteer::Terminate()
{
//  Calls JSFModule::Terminate()

  if( !fRunEnded ) EndRun() ;

  if( fOFile ) { 
    fOFile->cd("/conf");
    gDirectory->mkdir("term");
    //    fConf->Initialize(fModules);
    //    fConf->Write("JSF");
  }


  TIter next(fModules);
  JSFModule *module;
  Int_t iloop=0;
  while (( module = (JSFModule*)next())) {
        module->SetModuleStatus(kTerminate); 
        fOFile->cd("/conf/term");
//	module->GetFile()->cd("/conf/term");

	Double_t cputime=0;
	Double_t realtime=0;
	if( gStopwatch ) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,3,0)
	  gStopwatch->Stop();
	  cputime=gStopwatch->CpuTime();
	  realtime=gStopwatch->RealTime();
	  gStopwatch->Continue();
#else
	  cputime=gStopwatch->GetCPUTime();
	  realtime=gStopwatch->GetRealTime();
#endif
	}
	Bool_t rcode=module->Terminate();
	if( gStopwatch ) {
	  iloop++;
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,3,0)
	  gStopwatch->Stop();
	  fCPUTime[iloop] += gStopwatch->CpuTime()-cputime;
	  fRealTime[iloop] += gStopwatch->RealTime()-realtime;
	  gStopwatch->Continue();
#else
	  fCPUTime[iloop] += gStopwatch->GetCPUTime()-cputime;
	  fRealTime[iloop] += gStopwatch->GetRealTime()-realtime;
#endif
	}
        if( !rcode )  return kFALSE; 

  }
  if( fOFile ) { 
    fOFile->cd("/conf");
    fConf->Initialize(fModules);
    fConf->Write("JSF");
    fOFile->cd("/");
    fOFile->Write(0, GetWriteMode());
  }

  TDatime *dtime=new TDatime();
  printf("JSF Terminated at date and time : %s\n",dtime->AsString());

  if( gStopwatch ) {
    printf("****** Summary of CPU/Real time in sec  ****************\n");
    Double_t cpusum=0;
    Double_t realsum=0;
    next.Reset();
    Int_t loop=0;
    while (( module = (JSFModule*)next())) {
      loop++;
      printf("%s ",module->GetName());
      printf(" CPU time = %g ",fCPUTime[loop]);
      printf(" Real time = %g ",fRealTime[loop]);
      printf("\n");
      cpusum  += fCPUTime[loop];
      realsum += fRealTime[loop];
    }
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,3,0)
    gStopwatch->Stop();
    printf("JSFSteer     CPU time = %g",gStopwatch->CpuTime()-fCPUTime[0]-cpusum);
    printf("  Real time = %g \n",gStopwatch->RealTime()-fRealTime[0]-realsum);
    printf(" ----------------------------------------------------\n");
    printf("Total time ");
    printf(" CPU time = %g ",gStopwatch->CpuTime()-fCPUTime[0]);
    printf(" Real time = %g ",gStopwatch->RealTime()-fRealTime[0]);
    printf("\n");
#else
    printf("JSFSteer     CPU time = %g",gStopwatch->GetCPUTime()-fCPUTime[0]-cpusum);
    printf("  Real time = %g \n",gStopwatch->GetRealTime()-fRealTime[0]-realsum);
    printf(" ----------------------------------------------------\n");
    printf("Total time ");
    printf(" CPU time = %g ",gStopwatch->GetCPUTime()-fCPUTime[0]);
    printf(" Real time = %g ",gStopwatch->GetRealTime()-fRealTime[0]);
    printf("\n");
#endif

  }

  fIsTerminated = kTRUE;

  return kTRUE;
}


//_____________________________________________________________________________
void JSFSteer::PrintInfo()
{
//     Gives information about versions etc.
   printf("\n\n");
   printf("**************************************************************\n");
   printf("*             JSFSteer version:%3d released at %6d         *\n",GetVersion(), GetVersionDate());
   printf("*                pre-pre release                             *\n");
   printf("**************************************************************\n");

   printf("\n\n");
//     Print info for all defined Makers
   TIter next(fModules);
   JSFModule *module;
   while ((module = (JSFModule*)next())) {
      module->PrintInfo();
   }
}

//_____________________________________________________________________________
Bool_t JSFSteer::MakeTree()
{
  // Create trees for output.  Executed only when fOFile is define.
  // If fTree is not defined, create Tree in the output file.
  // If fTree is already defined, keep fTree, but JSF branch is 
  // writeen to fOFile when fOFile != fIFile.
  // Then calls JSFModule::MakeBranch. 

  fOFile->cd("/");

  if( !fOTree ) { 
    fOTree=new TTree("Event","JSF event tree");
    AddTree(fOTree);
  }


  // Define JSF in the tree.
  Int_t split=1;
  Int_t bsize=4000;
  fBrJSF=fOTree->Branch(fEventBuf->GetName(), fEventBuf->ClassName(),
	&fEventBuf, bsize, split);

  TIter next(fModules);
  JSFModule *module;
  while ((module = (JSFModule*)next())) {
    if( ! module->IsWritable() ) continue ;
    module->MakeBranch(fOTree);
  }

  if( fIFile ) fIFile->cd("/");

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFSteer::SetupTree()
{
  // Setup addresses to read tree data
  // Read the Key infile:/conf/JSF and find modules to read tree,
  // then execute SetupBranch() method of each modules.

  if( !fIFile->cd("conf") ) {
    Fatal("SetupTree","%s/conf does not exists.",fIFile->GetName());
    return kFALSE;
  }

  gJSFEventList = fIFile->GetListOfKeys();

// Get all event key directory
  gJSFEventKey  = (TKey*)gJSFEventList->Last();
  if( gJSFEventKey == NULL ) { 
     Error("SetupTree","Event directory is not found in the input file.");
     return kFALSE;
  }

  fITree=new TChain("Event");
  fITree->AddFile(fIFile->GetName());
  cerr << "JSFSteer::SetupTree() input file is " << fIFile->GetName() << endl;
  if( Env()->GetValue("JSF.ChainFiles",1) ) {
    TString fnin(fIFile->GetName());
    TString fnref=fnin.Remove(fnin.Sizeof()-6,6);
    for(Int_t i=1;i<10000;i++) {
      TString fnadd=fnref;
      fnadd+="_";
      fnadd+=i;
      fnadd+=".root";
      if ( gSystem->AccessPathName(fnadd) ) break;
      fITree->AddFile(fnadd);
      cerr << fnadd << " added to the input chains" << endl;
    }
  }
     
  cerr << "Entries in Chain is " << fITree->GetEntries() << endl;

  AddTree(fITree);

//  fReadin=new JSFSteer("ReadinJSF");
//  fReadin->fConf->Read("JSF");
  fReadin = new JSFSteerBuf("ReadinJSF");

  JSFSteerConf rdconf("JSFSteerConfReadin", "JSF Configuration");
  rdconf.Read("JSF");
  if ( rdconf.fNmodule < 1 ) {
    std::cerr << "Fatal error in JSFSteer::SetupTree " ;
    std::cerr << " No input event data found " << std::endl;
    std::cerr << " Make sure JSFGUI.OutputEventFile is Yes in jsf.conf file " <<std::endl;
    exit(-1) ;
  }
  if ( Env()->GetValue("JSF.EnvParameter.GetFromInput",kTRUE) ) {
    Bool_t replace=Env()->GetValue("JSF.EnvParameter.Replace",kFALSE);
    fEnv->Add(rdconf.GetEnv(), replace); // Replace, if not defined in file.
  }


// Prepare pointer for JSF obtained from a file.
  fITree->SetBranchStatus("*");
  fITree->SetBranchAddress("JSFSteerBuf", &fReadin);
  
  TObject *first=fModules->First();
  Char_t temp[200];
  JSFEventBuf **rdinbuf=new JSFEventBuf*[rdconf.fNmodule];
  for(Int_t i=0;i<rdconf.fNmodule;i++){
    sprintf(temp,"%s mod%d(\"%s\",\"Readin module\")",
	    rdconf.fClasses[i].Data(),i,rdconf.fNames[i].Data());
    gROOT->ProcessLine(temp);

    JSFModule *module = FindModule(rdconf.fClasses[i].Data());
    if ( module->EventBuf() ) {
      rdinbuf[i]=module->EventBuf();
      Int_t lc=strlen(module->GetName());
      Char_t *name = new Char_t [lc+10];
      if( strcmp(module->GetName(), "JSFQuickSim") == 0 ) {
	strcpy(name,rdinbuf[i]->GetName());

      }
      else {
        sprintf(name,"%s-EventBuf",module->GetName());
      }
      fITree->SetBranchAddress(name, &rdinbuf[i]);

      delete name;
     }
    
    if( first ) {  // Put readin-module to the first.
      fModules->Remove(module);
      fModules->AddBefore(first, module);
    }

  }

  return kTRUE;
}


//_____________________________________________________________________________
void JSFSteer::SetInput(TFile &file)
{
  // Setup addresses to read tree data
  // Read the Key infile:/conf/JSF and find modules to read tree,
  // then execute SetupBranch() method of each modules.
  // for multiple file input

  if( fReadin == 0 ) { 
    fIFile = &file; 
    return ;
  }

  //  delete fReadin;
  fIFile->Close();


  fIFile=&file;

  if( !fIFile->cd("conf") ) {
    Fatal("SetupTree","%s/conf does not exists.",fIFile->GetName());
    return ;
  }

  gJSFEventList = fIFile->GetListOfKeys();

// Get all event key directory
  gJSFEventKey  = (TKey*)gJSFEventList->Last();
  if( gJSFEventKey == NULL ) { 
     Error("SetupTree","Event directory is not found in the input file.");
     return ;
  }

  Char_t kname[60];
  if( strcmp(gJSFEventKey->GetName(),"Event") == 0 ) { 
    sprintf(kname, "%s;%d",gJSFEventKey->GetName(),gJSFEventKey->GetCycle());
    fITree=(TChain*)fIFile->Get(kname);
  }

  while( (gJSFEventKey=(TKey*)gJSFEventList->Before(gJSFEventKey)) ) {
    if( strcmp(gJSFEventKey->GetName(),"Event") != 0 ) continue ;
    sprintf(kname, "%s;%d",gJSFEventKey->GetName(),gJSFEventKey->GetCycle());
    fITree=(TChain*)fIFile->Get(kname);
  }

  JSFSteerConf rdconf("JSFSteerConfReadin", "JSF Configuration");
  rdconf.Read("JSF");
  if ( Env()->GetValue("JSF.EnvParameter.GetFromInput",kTRUE) ) {
    Bool_t replace=Env()->GetValue("JSF.EnvParameter.Replace",kFALSE);
    fEnv->Add(rdconf.GetEnv(), replace); // Replace, if not defined in file.
  }

// Prepare pointer for JSF obtained from a file.
  gJSFBranch=fITree->GetBranch("JSFSteerBuf");

  gJSFBranch->SetAddress(&fReadin); 

  // Create modules defined in JSFSteerConf direcory
  
  Char_t temp[200];
  for(Int_t i=0;i<rdconf.fNmodule;i++){
    sprintf(temp,"%s mod%d(\"%s\",\"Readin module\")",
	    rdconf.fClasses[i].Data(),i,rdconf.fNames[i].Data());

    JSFModule *module = FindModule(rdconf.fClasses[i].Data());

    module->SetBranch(fITree);
    module->SetFile(fIFile);

  }

  return ;
}



//_____________________________________________________________________________
void JSFSteer::MakeConfDir()
{
  // Create conf directory at the top of output file.

  TFile *file=fOFile;
  TList *dlist=fOFile->GetList();
  if ( dlist->FindObject("conf") == NULL ) { file->mkdir("conf"); }

  file->cd("conf");
  dlist=gDirectory->GetListOfKeys();
  if ( dlist->FindObject("init") == NULL ) { file->mkdir("init"); }

}

//_____________________________________________________________________________
JSFModule *JSFSteer::FindModule(const Text_t *classname, const Char_t *opt)
{
  // Find JSF Module, which is inherited by "classname".

  TIter  next(fModules);
  JSFModule *module;
  while( (module=(JSFModule*)next()) ){
    if( module->InheritsFrom(classname) ){ return module; }
//    if( strcmp(classname, module->ClassName()) == 0 ) { return module; }
  }

  if( strcmp(opt,"quiet") != 0 ) {
    Error("FindModule","A class %s is not found",classname);
  }
  return NULL;
}

//_____________________________________________________________________________
JSFEventBuf *JSFSteer::FindEventBuf(const Char_t *classname, const Char_t *opt)
{
// Find JSFEventBuf, which is inherited from "class name"

  TIter  next(fEvents);
  JSFEventBuf *buf;
  while( (buf=(JSFEventBuf*)next()) ){
    if( buf->InheritsFrom(classname) ){ return buf; }
  }

  if( strcmp(opt,"quiet") != 0 ) {
    Error("FindEventBuf","A class %s is not found",classname);
  }
  return NULL;
}

//_____________________________________________________________________________
JSFEventBuf *JSFSteer::FindEventBuf(TBranch *branch, const Char_t *opt)
{
  // Find JSFEventBuf object defined for branch

  TIter  next(fModules);
  JSFModule *module;
  while( (module=(JSFModule*)next()) ){
//    printf(" Looking for event buf for module %s\n",module->GetName());
    JSFEventBuf *buf=module->EventBuf();
    if( buf ) {
//      printf(" EventBuf name is %s\n",buf->GetName());
      if( strcmp(buf->GetName(), branch->GetName()) == 0 ) { return buf; }
    }
  }

  if( strcmp(opt,"quiet") != 0 ) {
    Error("FindEventBuf","A Branch %s is not found",branch->GetName());
  }
  return NULL;
}

//_____________________________________________________________________________
JSFSteerConf::JSFSteerConf(const char *name, const char*title)
  : TNamed (name, title)
{
  //  A class to save JSFSteer information;
  //  class name, name, and title of defined modules are saved.

  fSize=50;
  fClasses=new TString [fSize];
  fNames=new TString [fSize];
  fNmodule = 0;
  fEnv     = 0;
}

//_____________________________________________________________________________
JSFSteerConf::~JSFSteerConf()
{
  if ( fEnv ) delete fEnv;
  delete [] fClasses;
  delete [] fNames;
}

//_____________________________________________________________________________
void JSFSteerConf::Initialize(TList *mlist)
{
// Make fClasses and fNames from fModules list for output

  fNmodule=0;
  TIter next(mlist);
  JSFModule *module;
  while (( module = (JSFModule*)next())){
    if(module->IsWritable() && module->GetMakeBranch())fNmodule++; 
  }

  next.Reset();
  Int_t im=0;
  while (( module = (JSFModule*)next())) {
    if(module->IsWritable() && module->GetMakeBranch()){
      fNames[im]=module->GetName();
      fClasses[im]=module->ClassName();
      im++;
    }
  }
  //  Bool_t localonly=gJSF->Env()->GetValue("JSF.EnvParameter.SaveOnlyLocal",kTRUE);
  Bool_t localonly=gJSF->Env()->GetValue("JSF.EnvParameter.SaveOnlyLocal",kFALSE);
  fEnv=new JSFEnv(*gJSF->Env(), localonly );
  
}

//______________________________________________________________________________
void JSFSteerConf::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFSteerConf.

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v = R__b.ReadVersion(&R__s, &R__c); 
     if( R__v > 1 ) {
       JSFSteerConf::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
       return;
     }
     // Process OLD version data
      TNamed::Streamer(R__b);
      R__b >> fNmodule;
      Char_t temp[200];
      Int_t lc;
      for(Int_t i=0;i<fNmodule;i++){
	lc=R__b.ReadStaticArray(temp);
	fClasses[i]=temp;
	lc=R__b.ReadStaticArray(temp);
	fNames[i]=temp;
      }
      R__b.CheckByteCount(R__s, R__c, JSFSteerConf::IsA());
   } 
   else {
     JSFSteerConf::Class()->WriteBuffer(R__b, this);
   }
}
