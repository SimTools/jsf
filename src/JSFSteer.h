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
    //  Version 1: up to JSF 1.16
    //  Version 2: up to JFF 1.17
};

//********************************************************************
class JSFSteer  : public TNamed {
protected:

  JSFSteer  *fReadin ; //! Pointer to readin JSF class

  Bool_t     fIsInitialized; //! kTRUE when JSF is initialized.
  Int_t      fLastRun;       //! Last Run number, or =0 when no run is not processed yet.
  Bool_t     fIsTerminated;  //! kTRUE when JSF is terminated.
  Bool_t     fRunEnded;      //! kTRUE after EndRun was called.
  Bool_t     fIsGetEvent;    //! kTRUE when GetEvent is called.

  TList      *fModules;     //! List of modules

  TFile      *fIFile;       //! Pointer to input file
  TFile      *fOFile;       //! Pointer to output file
  TTree      *fITree;        //! Pointer to input event tree
  TTree      *fOTree;        //! Pointer to output event tree
  JSFEnv       *fEnv;         //! Pointer to the env data.
  TBranch    *fBrJSF;      //! Branch address of JSFSteer.
  TChain     *fChain;     //! A tree of chained file.
  TObjArray  *fEventTrees;  //! Collection of event trees.

  Double_t    fCPUTime[30];   //!
  Double_t    fRealTime[30];   //!

// Followings are event-by-event information
  Int_t      fVersion;      // JSFSteer version number
  Int_t      fVersionDate;  // JSFSteer version date
  Int_t      fRun   ;       // Run number
  Int_t      fEvent ;       // Event number (1 to n )
  Int_t      fDate  ;       // Event Date
  Int_t      fTime  ;       // Event time

  Int_t fReturnCode ; //! Return code of JSF Module

private:
  virtual void MakeConfDir();
  void InitializeMessage();
  void LoadSharedLibraries();

public:
  JSFSteerConf *fConf;       //! Pointer to module information.

  enum  EJSFReturnCode {kJSFOK=0x00, kJSFSkipRestModules=0x01, kJSFNoOutput=0x02,
        kJSFEOF=0x04, kJSFDoEndRun=0x08, kJSFTerminate=0x10,
        kJSFQuit=0x20, kJSFNotShowDisplay=0x40, 
		kJSFNotShowHistogram=0x80, kJSFFALSE=0x8000} ;


public:
  JSFSteer(const char *name="JSF", const char *title="The JLC Study Frame");
  virtual   ~JSFSteer();
  
  void SetInput(TFile& file);
  void SetOutput(TFile& file){ fOFile = &file ; return ; }
  TFile *GetOutput(){ return fOFile; }
  TFile *GetInput(){ return fIFile; }
  virtual void SetIOFiles();

  virtual void  SetRunNumber(Int_t run=1) {           fRun=run;  }
  virtual void  SetEventNumber(Int_t event=1) {fEvent=event;}
  
  inline TTree     *ITree() {return fITree;}
  inline TTree     *OTree() {return fOTree;}
  inline void AddTree(TTree *t){ fEventTrees->Add(t); }
  inline TObjArray *GetListOfTrees(){ return fEventTrees; }

  inline void SetChain(TChain *chain){ fChain=chain; }
  inline TChain *GetChain(){ return fChain; }

  TList     *Modules()  {return fModules;}
  JSFModule *FindModule(const Text_t *classname,const Option_t *opt="");
  JSFEventBuf *FindEventBuf(TBranch *branch, const Option_t *opt="");
  JSFEnv    *Env(){ return fEnv;}

  Int_t     GetVersion(){ return fVersion;}
  Int_t     GetVersionDate() { return fVersionDate;}
  Int_t     GetMinorVersion();
  Int_t     GetPatchLevel();

  Int_t     GetRunNumber() { return fRun;}
  Int_t     GetEventNumber() { return fEvent;}
  Int_t     GetDate() { return fDate;}
  Int_t     GetTime() { return fTime;}
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
  
  ClassDef(JSFSteer, 1)   // Steering class to control JLC study modules

};

R__EXTERN JSFSteer *gJSF;

typedef JSFSteer::EJSFReturnCode JSFReturnCode_t;

#endif
