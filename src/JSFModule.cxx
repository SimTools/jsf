//*LastUpdate:  jsf-1-11 23-July-1999  by Akiya Miyamoto
//*LastUpdate:  jsf-1-4 14-Feburary-1999  by Akiya Miyamoto
//*LastUpdate:  v0.3.08 09/29/1998  by A.Miyamoto
//*-- Author :  Akiya Miyamoto  09/22/1998

//////////////////////////////////////////////////////////////////
//
//  JSFModule
//
//  Basic unit of data analysis
//
//  User Analysis class should be derived from this class.
//  Basic member functions for data analysis are,
//      Initialize()
//        BeginRun(Int_t RunNumber)
//          Process(Int_t EventNumber)
//        EndRun()
//      Terminate()
//  User class should override these functions.
//
//  fFile is a pointer data member to the file.  It is set by JSFSteer
//  and can be READ/WRITE/UPDATE. (UPDATE may not work properly yet.)
//  Event data is stored in the tree, Event, in the "fFile".  Other 
//  configuration data are stored as an Keyed file in the "fFile".
//  Directory and key names are,
//       Initialize()  --> /conf/init/[modulename]
//       BeginRun(nnn) --> /conf/beginNNNNNN/[modulename]
//       EndRun(nnn)   --> /conf/endNNNNN/[modulename]
//       Terminate()   --> /conf/term/[modulename]
//  Note that key names have been changed since JSF-1-11, to
//  simplfy the directory structure.
//  The member function, ConfDirectory(), creates and cd to proper 
//  directory.  How to write/read data is depend on the implementaion
//  of the class.  
//  Process is called only when the "fFile" is writable, while other
//  functions are called evenif fFile is not writable.  If Initialize,
//  BeginRun, EndRun, or Terminate is called when fFile is not writable,
//  they should prepare data member of the class properly.
//
//  Event data is saved in the object pointed by fEventBuf, which is 
//  the JSFEventBuf object.  JSFModule does not create JSFEventBuf.
//  User class dereived from JSFModule should define fEventBuf.
//
//  The member function, MakeBranch() and SetBranch() is called from
//  JSFSteer::Initialize(). MakeBranch() defines the tree branch to 
//  store event data, while SetBranch() set addresses to read tree data.
//  Usually, user read event data by tree->Fill(), which readin all branches
//  in the tree. If the branch address is not set properly, the program will
//  crash.  When reading the root data, JSFSteer  read the header and creats 
//  the objetcs required to get tree data.
//  
//$Id$
//
//////////////////////////////////////////////////////////////////
//  
#include <TSystem.h>
#include <TDirectory.h>
#include <TKey.h>

#include "JSFSteer.h"
#include "JSFModule.h"

ClassImp(JSFModule)
ClassImp(JSFEventBuf)

//_____________________________________________________________________________
JSFModule::JSFModule(const char *name, const char *title, Option_t *opt)
       :TNamed(name,title)
{
// Default constructor of JSF module.
// If opt != "", the module is not included in the module list of JSFSteer.

  fVersion  =104;
  fStat     = kConstruct;
  if( strcmp(opt,"")==0 )gJSF->Modules()->Add(this);
  fEventBuf=0;
  fHistograms = 0;
  fBufferSize=1000;
  fSplit=0;

  fMakeBranch=kTRUE;

  if( strcmp(opt,"") == 0 ) {
    if( strcmp(title,"Readin module") == 0 ) fFile = gJSF->GetInput();
    else fFile = gJSF->GetOutput() ;
  }

}

//____________________________________________________________________________
JSFModule::~JSFModule()
{
//  Destrutor of JSF module.
  Clear();  
  TObject *obj=gJSF->Modules()->FindObject(this);

  //  if( !obj ) gJSF->Modules()->Remove(this);
  //  if( !fEventBuf )   delete fEventBuf;
  if( obj ) gJSF->Modules()->Remove(this);
  if( fEventBuf )   delete fEventBuf;
}

//____________________________________________________________________________
void JSFModule::PrintInfo()
{
  Warning("PrintInfo","No information available for this module");
}

//____________________________________________________________________________
Bool_t JSFModule::Process(Int_t ev)
{
//   Called at each events.  This function is not called when 
//   the default file assigned to the class (fFile) is not writable.
  
  ConfDirectory();

  if( !IsWritable() ) return kTRUE;

  if( fEventBuf ) {
    fEventBuf->SetHeader();
  }

  return kTRUE ;
}


//____________________________________________________________________________
Bool_t JSFModule::Initialize()
{
// 
  ConfDirectory();
  return kTRUE;
}


//____________________________________________________________________________
void JSFModule::ConfDirectory()
{
  // cd to conf directory to access configuration information
  // or where event tree is.

  // Is there /conf ?
  
  //fFile->cd("conf");
  //TList *dlist=gDirectory->GetListOfKeys();
  //if ( !dlist->FindObject(GetName()) ){gDirectory->mkdir(GetName());}
  //gDirectory->cd(GetName());

  // Create run status dependant directory
  Char_t name[30];
  switch (fStat) {
    case kInitialize:
      strcpy(name,"/conf/init");
      break;
    case kBeginRun:
      sprintf(name,"/conf/begin%5.5i",fRunNumber);
      break;
    case kEndRun:
      sprintf(name,"/conf/end%5.5i",fRunNumber);
      break;
    case kTerminate:
      strcpy(name,"/conf/term");
      break;
    case kEventLoop:
      strcpy(name,"/");
      break;
    case kConstruct:
    case kDestruct:
    default:
      Fatal("ConfDirectory","Called with invalid status");
   }

// cd to right directory
   if( fStat == kEventLoop ) fFile->cd(name);
   else {
     //     TList *dlist=gDirectory->GetListOfKeys();
     //if ( dlist->FindObject(name) == NULL) gDirectory->mkdir(name);
     // gDirectory->cd(name);
   }
}

//____________________________________________________________________________
Bool_t JSFModule::BeginRun(Int_t nrun)
{
// The BeginRun function is called when new run is started.  
// Each module
// should output run dependant parameter here by object->Write() statement.

  ConfDirectory();
  return kTRUE;
}

//____________________________________________________________________________
Bool_t JSFModule::EndRun()
{
//  End of run.
  ConfDirectory();
  return kTRUE;
}

//____________________________________________________________________________
Bool_t JSFModule::Terminate()
{
//  End of run.
  ConfDirectory();
  return kTRUE;
}

//____________________________________________________________________________
Bool_t JSFModule::GetLastRunInfo()
{
  return kTRUE;
}


//____________________________________________________________________________
void JSFModule::MakeBranch(TTree *tree)
{
//  Make branch of the module
   fTree=tree;
   if( fEventBuf && fMakeBranch ) {
     Int_t split=fSplit;
     Int_t bsize=fBufferSize;
     Int_t lc=strlen(GetName());
     Char_t *name = new Char_t [lc+10];
     sprintf(name,"%s-EventBuf",GetName());
     fBranch = tree->Branch(name, fEventBuf->ClassName() ,&fEventBuf, bsize, split);
     delete name;
   }
}

//____________________________________________________________________________
void JSFModule::SetBranch(TTree *tree)
{
//  Set Branch address for this module


   fTree=tree;
   if( fEventBuf ) {
     Int_t lc=strlen(GetName());
     Char_t *name = new Char_t [lc+10];
     sprintf(name,"%s-EventBuf",GetName());
     fBranch=tree->GetBranch(name);
     fBranch->SetAddress(&fEventBuf);
     delete name;
   }
}


//___________________________________________________________________________
JSFEventBuf::JSFEventBuf(const char *name, const char *title, JSFModule *module)
       :TNamed(name,title)
{
// Default constructor of JSF module.
  fModule=module;
}

//___________________________________________________________________________
JSFEventBuf::JSFEventBuf(JSFModule *module, const char *name, const char *title)
       :TNamed(name,title)
{
// Default constructor of JSF module.
  fModule=module;
}

//___________________________________________________________________________
void JSFEventBuf::SetHeader()
{
  // Set header information of event buffer.
  
  fRunNumber=gJSF->GetRunNumber();
  fEventNumber=gJSF->GetEventNumber();
  TDatime *dtime=new TDatime();
  fDate=dtime->GetDate(); 
  fTime=dtime->GetTime(); 
  delete dtime;
}




