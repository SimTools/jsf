//*LastUpdate:  jsf-1-9 28-May-1999  by A.Miyamoto
//*LastUpdate:  jsf-1-7-2 16-April-1999  by A.Miyamoto
//*LastUpdate:  jsf-1-5 21-Feburary-1999  by A.Miyamoto
//*LastUpdate:  jsf-1-4 14-Feburary-1999  by A.Miyamoto
//*LastUpdate:  v0.3.08 09/29/1998  by A.Miyamoto
//*-- Author :  Akiya Miyamoto  09/24/1998

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
//         file->Write();
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
//
/////////////////////////////////////////////////////////////////////////////


#include <TROOT.h>
#include <TSystem.h>
#include <TChain.h>
#include <TTree.h>
#include <TBrowser.h>
#include <TClonesArray.h>
#include <TApplication.h>
#include <TFile.h>
#include <TKey.h>

#include "JSFSteer.h"
#include "JSFModule.h"

JSFSteer *gJSF=kFALSE;

ClassImp(JSFSteer)
ClassImp(JSFSteerConf)

static TList *gJSFEventList=0;
static TKey  *gJSFEventKey=0;
static TBranch *gJSFBranch=0;


//---------------------------------------------------------------------------
JSFSteer::JSFSteer(const char *name, const char *title) 
        : TNamed(name, title)
{
  //  Default constructor of JLC study framework
  fModules = 0;
  fConf    = 0;
  fReadin  = 0;
  fVersion    = 111  ;  // JSFSteer version number
  fVersionDate  = 19990723 ; // version date.
  fIsInitialized = kFALSE ;
  fIsTerminated  = kFALSE ;
  fLastRun       = 0 ;
  fRun           = 0 ;
  fRunEnded      = kFALSE;

  fITree       = 0;
  fOTree       = 0;
  fModules = new TList();
  fConf    = new JSFSteerConf("JSFSteerConf", "JSF Configuration");

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
    fEnv=new TEnv(envfile);  // Create pointer to env file.

    SetIOFiles();

  }
}

//---------------------------------------------------------------------------
JSFSteer::~JSFSteer()
{
//  Terminate JSF, if fIsTerminated=kFALSE, then delete fModules
//  
  if (!fModules) { 
    if( !fIsTerminated ) Terminate(); 
    TIter next(fModules);
    JSFModule *module; 
    while( (module = (JSFModule*)next()) ) { delete module; }
  }
  if (!fConf) delete fConf;
}

//---------------------------------------------------------------------------
void JSFSteer::SetIOFiles()
{
//  Set Flags for file IO, should be called after Input/Output file is assigned.
//  

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

}


//_____________________________________________________________________________
void JSFSteer::InitializeMessage()
{
//  Print message after jsf initialization

  printf("****  JSF Initialization *******************************************\n");
  printf("      Version %d : Version Date %d \n",fVersion,fVersionDate);
  TDatime *dtime=new TDatime();
  printf("      Current date and time : %s\n",dtime->AsString());

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
  if( !fIsInitialized ) {  if( !Initialize() ) { return kFALSE; } }

  if( !fIsGetEvent ) fEvent=i;

// BeginRun
  if( fLastRun == 0 ) { 
    if( fRun == 0 ) {
      Warning("Process","Run number is undefined. Set default run number 1");
      fRun = 1;
    } 
    if( !BeginRun(fRun) ) return kFALSE;
  }
  else if( fLastRun != fRun ) {
    if( !EndRun() ) { return kFALSE; }
    if( !BeginRun(fRun) ) { return kFALSE; }
  }

// Loop over all module to process event data

   if( fOFile ) fOFile->cd();
   TIter next(fModules);
   JSFModule *module;
   while (( module = (JSFModule*)next())) {
     module->SetModuleStatus(kEventLoop);
     if( module->IsWritable() ) {
       if( !module->Process(i) ) return kFALSE;
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

   // If Input file is opened, setup tree to read
   if( fIFile && fIFile->IsOpen() ) { if( !SetupTree() ) return kFALSE; } 

   // If Output file is writable, make tree for them
   if( fOFile && fOFile->IsOpen() && fOFile->IsWritable() ) {
       fOFile->cd("/");
       MakeConfDir();
       fOFile->cd("/");
       if( !MakeTree() ) return kFALSE; 
   }

   InitializeMessage();
   TDirectory *last=gDirectory;
   TListIter next(fModules);
   JSFModule *module;
   while ((module = (JSFModule*)next())) {
     module->SetModuleStatus(kInitialize);
     //     module->GetFile()->cd("/conf/init");
     module->GetFile()->cd();
     if( !module->Initialize() )  return kFALSE; 
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

  fRun=nrun;
  fLastRun = fRun;
  fRunEnded = kFALSE ;
//  Make a begin-Run directory in the output file

  Char_t keyname[32];
  sprintf(keyname,"begin%5.5d",fRun);

  printf(" In beginRun .. keyname=%s \n",keyname);

   // If Output file is writable, make tree for them
   if( fOFile && fOFile->IsOpen() && fOFile->IsWritable() ) {
      fOFile->cd("/conf");
      gDirectory->mkdir(keyname);
   }

  sprintf(keyname,"/conf/begin%5.5d",fRun);
  TIter next(fModules);
  JSFModule *module;
  while (( module = (JSFModule*)next())) {
    module->SetModuleStatus(kBeginRun);
    module->SetRunNumber(fRun);
    printf(" module file is %s\n",module->GetFile()->GetName());
    module->GetFile()->cd(keyname);
    if( !module->BeginRun(fRun) ) return kFALSE ;
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

    Char_t keyname[32];
    sprintf(keyname,"end%5.5d",fRun);
   // If Output file is writable, make endrun directory
   if( fOFile && fOFile->IsOpen() && fOFile->IsWritable() ) {
      fOFile->cd("/conf");
      gDirectory->mkdir(keyname);
   }


    sprintf(keyname,"/conf/end%5.5d",fRun);
    TIter next(fModules);
    JSFModule *module;
    while (( module = (JSFModule*)next())) {
        module->SetModuleStatus(kEndRun);
	module->GetFile()->cd(keyname);
        if( !module->EndRun() )  return kFALSE; 
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

    Int_t  nb=fITree->GetEvent(nevt-1); 

    fRun = fReadin->GetRunNumber();
    fEvent = fReadin->GetEventNumber(); 
    fIsGetEvent=kTRUE;
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
    fConf->Initialize(fModules);
    fConf->Write("JSF");
  }

  TIter next(fModules);
  JSFModule *module;
  while (( module = (JSFModule*)next())) {
        module->SetModuleStatus(kTerminate); 
	module->GetFile()->cd("/conf/term");
        if( !module->Terminate() )  return kFALSE; 
  }
  if( fOFile ) fOFile->cd("/");

  TDatime *dtime=new TDatime();
  printf("JSF Terminated at date and time : %s\n",dtime->AsString());

  return kTRUE;
}


//_____________________________________________________________________________
void JSFSteer::PrintInfo()
{
//     Gives information about versions etc.
   printf("\n\n");
   printf("**************************************************************\n");
   printf("*             JSFSteer version:%3d released at %6d         *\n",fVersion, fVersionDate);
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
  }


  // Define JSF in the tree.
     Int_t split=1;
     Int_t bsize=4000;
     fBrJSF=fOTree->Branch(GetName(),ClassName(),&gJSF, bsize,split);  

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
  Char_t kname[60];
  if( strcmp(gJSFEventKey->GetName(),"Event") == 0 ) { 
    sprintf(kname, "%s;%d",gJSFEventKey->GetName(),gJSFEventKey->GetCycle());
    fITree=(TTree*)fIFile->Get(kname);
  }

  while( (gJSFEventKey=(TKey*)gJSFEventList->Before(gJSFEventKey)) ) {
    if( strcmp(gJSFEventKey->GetName(),"Event") != 0 ) continue ;
    sprintf(kname, "%s;%d",gJSFEventKey->GetName(),gJSFEventKey->GetCycle());
    fITree=(TTree*)fIFile->Get(kname);
  }


  fReadin=new JSFSteer("ReadinJSF");
  fReadin->fConf->Read("JSF");

// Prepare pointer for JSF obtained from a file.
  gJSFBranch=fITree->GetBranch("JSF");
  gJSFBranch->SetAddress(&fReadin); 

  // Create modules defined in JSFSteerConf direcory
  
  TObject *first=fModules->First();
  Char_t temp[200];
  for(Int_t i=0;i<fReadin->fConf->fNmodule;i++){
    sprintf(temp,"%s mod%d(\"%s\",\"Readin module\")",
	    fReadin->fConf->fClasses[i],i,fReadin->fConf->fNames[i]);
    printf(" temp=%s\n",temp);
    gROOT->ProcessLine(temp);

    JSFModule *module = FindModule(fReadin->fConf->fClasses[i]);

    module->SetBranch(fITree);

    if( first ) {  // Put readin-module to the first.
      fModules->Remove(module);
      fModules->AddBefore(first, module);
    }

  }

  return kTRUE;
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
JSFModule *JSFSteer::FindModule(Text_t *classname)
{
  // Find JSF Module, which is inherited by "classname".

  TIter  next(fModules);
  JSFModule *module;
  while( (module=(JSFModule*)next()) ){
    if( module->InheritsFrom(classname) ){ return module; }
//    if( strcmp(classname, module->ClassName()) == 0 ) { return module; }
  }

  Error("FindModule","A class %s is not found",classname);
  return NULL;
}


//_____________________________________________________________________________
JSFSteerConf::JSFSteerConf(const char *name, const char*title)
  : TNamed (name, title)
{
  //  A class to save JSFSteer information;
  //  class name, name, and title of defined modules are saved.
  fClasses=0;
  fNames=0;
  fNmodule = 0;
}

//_____________________________________________________________________________
JSFSteerConf::~JSFSteerConf()
{
  if (!fClasses) delete fClasses;
  if (!fNames) delete fNames;
}

//_____________________________________________________________________________
void JSFSteerConf::Initialize(TList *mlist)
{
// Make fClasses and fNames from fModules list for output

  fNmodule=0;
  TIter next(mlist);
  JSFModule *module;
  while (( module = (JSFModule*)next()))
        {if(module->IsWritable() && module->GetMakeBranch())fNmodule++; }

  fClasses = new Char_t* [fNmodule];
  fNames   = new Char_t* [fNmodule];
  next.Reset();
  Int_t lc;
  Int_t im=0;
  while (( module = (JSFModule*)next())) {
    if(module->IsWritable() && module->GetMakeBranch()){
      lc=strlen(module->GetName());
      fNames[im]=new Char_t[lc+1];
      strcpy(fNames[im], module->GetName());
      lc=strlen(module->ClassName());
      fClasses[im]=new Char_t[lc+1];
      strcpy(fClasses[im], module->ClassName());
      im++;
    }
  }

}

//______________________________________________________________________________
void JSFSteerConf::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFSteerConf.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TNamed::Streamer(R__b);
      R__b >> fNmodule;
      fClasses = new Char_t* [fNmodule];
      fNames   = new Char_t* [fNmodule];
      Char_t temp[200];
      Int_t lc;
      for(Int_t i=0;i<fNmodule;i++){
	lc=R__b.ReadStaticArray(temp);
	temp[lc]=(Char_t)NULL;
	fClasses[i]=new Char_t[lc+1];
	strcpy(fClasses[i],temp);
	lc=R__b.ReadStaticArray(temp);
	temp[lc]=(Char_t)NULL;
	fNames[i]=new Char_t[lc+1];
	strcpy(fNames[i],temp);
      }

   } else {
      R__b.WriteVersion(JSFSteerConf::IsA());
      TNamed::Streamer(R__b);
      R__b << fNmodule;
      //  Write class and name information
      Int_t lc;
      for(Int_t i=0;i<fNmodule;i++){
	lc=strlen(fClasses[i])+1;
	R__b.WriteArray(fClasses[i],lc);
	lc=strlen(fNames[i])+1;
	R__b.WriteArray(fNames[i],lc);
      }
   }
}

