//////////////////////////////////////////////////////////////////
//
//  JSFSteer
//
//  Controls execution of JSF Modules
//  
//$Id$
//////////////////////////////////////////////////////////////////

#ifndef __JSFSteer__
#define __JSFSteer__

#include <TNamed.h>
#include <TTree.h>
#include <TChain.h>
#include <TDatime.h>
#include <TString.h>

#include "JSFEnv.h"
#include "JSFConfig.h"

class JSFModule;
class JSFEventBuf;

class JSFSteerConf : public TNamed {
protected:
  Int_t     fSize;     //! size of fClasses, fNames array
  Int_t     fNmodule;  // Number of modules;
  TString   *fClasses; //[fNmodule]  Names of module Clases 
  TString   *fNames;   //[fNmodule]  Module names.
public:
  JSFSteerConf(const char *name="JSFSteerConf", 
               const char *title="JSF Configuration");
  virtual   ~JSFSteerConf();

  void Initialize(TList *module); // Mkae fClasses, fNames from fModules

  friend class JSFSteer;
  ClassDef(JSFSteerConf,2) // Stores JSF Module info at Initialize()
};

//********************************************************************
class JSFSteerBuf: public TNamed 
{
protected:
// Followings are event-by-event information
  Int_t      fVersion;      // JSFSteer version number
  Int_t      fVersionDate;  // JSFSteer version date
  Int_t      fMinorVersion; // Minor version
  Int_t      fPatchLevel;   // Patch Level

  Int_t      fRun   ;       // Run number
  Int_t      fEvent ;       // Event number (1 to n )
  Int_t      fDate  ;       // Event Date
  Int_t      fTime  ;       // Event time

public:
  JSFSteerBuf(const char *name="JSFSteerBuf", 
		const char *title="JSF Event Header Information");
  virtual   ~JSFSteerBuf();

  virtual void  SetRunNumber(Int_t run=1) { fRun=run;  }
  virtual void  SetEventNumber(Int_t event=1) {fEvent=event;}

  inline Int_t  GetVersion(){ return fVersion;}
  inline Int_t  GetVersionDate() { return fVersionDate;}
  inline Int_t GetMinorVersion(){ return fMinorVersion; }
  inline Int_t GetPatchLevel(){ return fPatchLevel; }

  inline Int_t  GetRunNumber() { return fRun;}
  inline Int_t  GetEventNumber() { return fEvent;}
  inline Int_t  GetDate() { return fDate;}
  inline Int_t  GetTime() { return fTime;}

  ClassDef(JSFSteerBuf, 1)   // Header Information to be saved on the file.
};

//********************************************************************
class JSFSteer  : public TNamed {
protected:

  JSFSteerBuf  *fReadin ; //! Pointer to readin JSFConfiguration info
  JSFSteerBuf  *fEventBuf; //! Event header information of this run.

  Bool_t     fIsInitialized; //! kTRUE when JSF is initialized.
  Int_t      fLastRun;       //! Last Run number, or =0 when no run is not processed yet.
  Bool_t     fIsTerminated;  //! kTRUE when JSF is terminated.
  Bool_t     fRunEnded;      //! kTRUE after EndRun was called.
  Bool_t     fIsGetEvent;    //! kTRUE when GetEvent is called.

  TList      *fModules;     //! List of modules

  TFile      *fIFile;       //! Pointer to input file
  TFile      *fOFile;       //! Pointer to output file
//  TTree      *fITree;        //! Pointer to input event tree
  TChain     *fITree;        //! Pointer to input event tree
  TTree      *fOTree;        //! Pointer to output event tree
  JSFEnv       *fEnv;         //! Pointer to the env data.
  TBranch    *fBrJSF;      //! Branch address of JSFSteer.
  TChain     *fChain;     //! A tree of chained file.
  TObjArray  *fEventTrees;  //! Collection of event trees.
  TObjArray  *fEvents;      //! Collection of EventBuf;

  Double_t    fCPUTime[30];   //!
  Double_t    fRealTime[30];   //!

  Int_t fReturnCode ; //! Return code of JSF Module

private:
  virtual void MakeConfDir();
  void InitializeMessage();
  void LoadSharedLibraries();
  static JSFSteer *myself;  
  static Int_t fWriteMode;
public:
  JSFSteerConf *fConf;       //! Pointer to module information.

  enum  EJSFReturnCode {kJSFOK=0x00, kJSFSkipRestModules=0x01, kJSFNoOutput=0x02,
        kJSFEOF=0x04, kJSFDoEndRun=0x08, kJSFTerminate=0x10,
        kJSFQuit=0x20, kJSFNotShowDisplay=0x40, 
		kJSFNotShowHistogram=0x80, kJSFFALSE=0x8000} ;

  static JSFSteer *Instance();
public:
  JSFSteer(const char *name="JSF", const char *title="The JLC Study Frame");
  virtual   ~JSFSteer();
  
  void SetInput(TFile& file);
  void SetOutput(TFile& file){ fOFile = &file ; return ; }
  TFile *GetOutput(){ return fOFile; }
  TFile *GetInput(){ return fIFile; }
  virtual void SetIOFiles();
  virtual void CheckFilePointers();

  virtual void  SetRunNumber(Int_t run=1) { fEventBuf->SetRunNumber(run); }
  virtual void  SetEventNumber(Int_t event=1) { fEventBuf->SetEventNumber(event); }
  
//  inline TTree     *ITree() {return fITree;}
  inline TChain    *ITree() {return fITree;}
  inline TTree     *OTree() {return fOTree;}
  inline void AddTree(TTree *t){ fEventTrees->Add(t); }
  inline TObjArray *GetListOfTrees(){ return fEventTrees; }

  inline void SetChain(TChain *chain){ fChain=chain; }
  inline TChain *GetChain(){ return fChain; }

  TList     *Modules()  {return fModules;}
  JSFModule *FindModule(const Text_t *classname,const Option_t *opt="");
  JSFEventBuf *FindEventBuf(TBranch *branch, const Option_t *opt="");
  JSFEventBuf *FindEventBuf(const Char_t *classname, const Option_t *opt="");
  JSFEnv    *Env(){ return fEnv;}
  TObjArray *GetEvents(){ return fEvents; }

  Int_t     GetVersion(){ return fEventBuf->GetVersion();}
  Int_t     GetVersionDate() { return fEventBuf->GetVersionDate(); }
  Int_t     GetPatchLevel()  { return fEventBuf->GetPatchLevel(); }

  Int_t     GetRunNumber() { return fEventBuf->GetRunNumber();}
  Int_t     GetEventNumber() { return fEventBuf->GetEventNumber();}
  Int_t     GetDate() { return fEventBuf->GetDate();}
  Int_t     GetTime() { return fEventBuf->GetTime();}


  Int_t     GetNevent(){ return (Int_t)fITree->GetEntries(); }
  Int_t     GetEvent(Int_t nevt);

  Bool_t    MakeTree();   // Create branches for event output
  Bool_t    SetupTree();    // Set branches for readin data

  virtual Bool_t  Initialize();             
  virtual Bool_t  Process(Int_t eventnumber=1); // To be called in each events.
  virtual Bool_t  Terminate();              
  virtual Bool_t  BeginRun(Int_t nrun);
  virtual Bool_t  EndRun();
  virtual Bool_t  GetLastRunInfo(TFile *file, Int_t lastrun=-1);

  virtual void  PrintInfo();
  virtual void  FillTree(){ if( fOTree ) { fOTree->Fill(); }  }
  TBranch *GetBranch(){return fBrJSF;}

  void SetReturnCode(Int_t ir){ fReturnCode=ir; }
  Int_t GetReturnCode(){ return fReturnCode; }

  void SetWriteMode(Int_t writemode){ fWriteMode=writemode; }
  inline Int_t GetWriteMode(){ return fWriteMode;}
  
  ClassDef(JSFSteer, 2)   // Steering class to control JLC study modules

};

R__EXTERN JSFSteer *gJSF;

typedef JSFSteer::EJSFReturnCode JSFReturnCode_t;

#endif
