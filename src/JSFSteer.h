//*LastUpdate:  v0.3.04 09/24/1998  by A.Miyamoto
//*-- Author :  Akiya Miyamoto  09/24/1998

//////////////////////////////////////////////////////////////////
//
//  JSFSteer
//
//  Controls execution of JSF Modules
//  
//////////////////////////////////////////////////////////////////

#ifndef __JSFSteer__
#define __JSFSteer__

#include <TNamed.h>
#include <TTree.h>
#include <TEnv.h>
#include <TChain.h>
#include <TDatime.h>

// #ifndef __JSFModule__     
// #include "JSFModule.h"
// #endif                    

class JSFModule;

class JSFSteerConf : public TNamed {
protected:
  Int_t     fNmodule;  // Number of modules;
  Char_t   **fClasses; // Names of module Clases 
  Char_t   **fNames;   // Module names.
  Bool_t    *IsWriting;// kTRUE, when corresponding module is writting 
  Char_t     *fOutfile; // Name of output file.
  Char_t     *fInfile;  // Name of input file.
public:
  JSFSteerConf(const char *name="JSFSteerConf", 
               const char *title="JSF Configuration");
  virtual   ~JSFSteerConf();

  void Initialize(TList *module); // Mkae fClasses, fNames from fModules

  friend class JSFSteer;

  ClassDef(JSFSteerConf,1) // Stores JSF Module info at Initialize()
};


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
  TEnv       *fEnv;         //! Pointer to the env data.
  TBranch    *fBrJSF;      //! Branch address of JSFSteer.

// Followings are event-by-event information
  Int_t      fVersion;      // JSFSteer version number
  Int_t      fVersionDate;  // JSFSteer version date
  Int_t      fRun   ;       // Run number
  Int_t      fEvent ;       // Event number (1 to n )
  Int_t      fDate  ;       // Event Date
  Int_t      fTime  ;       // Event time

private:
  virtual void MakeConfDir();
  void InitializeMessage();

public:
  JSFSteerConf *fConf;       //! Pointer to module information.

public:
  JSFSteer(const char *name="JSF", const char *title="The JLC Study Frame");
  virtual   ~JSFSteer();
  
  void SetInput(TFile& file){ fIFile = &file ; return ; }
  void SetOutput(TFile& file){ fOFile = &file ; return ; }
  TFile *GetOutput(){ return fOFile; }
  TFile *GetInput(){ return fIFile; }

  virtual void  SetRunNumber(Int_t run=1) {           fRun=run;  }
  virtual void  SetEventNumber(Int_t event=1) {fEvent=event;}
  
  TTree     *ITree() {return fITree;}
  TTree     *OTree() {return fOTree;}
  TList     *Modules()  {return fModules;}
  JSFModule *FindModule(Text_t *classname);
  TEnv      *Env(){ return fEnv;}

  Int_t     GetVersion(){ return fVersion;}
  Int_t     GetVersionDate() { return fVersionDate;}
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

  virtual void  PrintInfo();
  virtual void  FillTree(){ fOTree->Fill(); }
  TBranch *GetBranch(){return fBrJSF;}

  
  ClassDef(JSFSteer, 1)   // Steering class to control JLC study modules

};

EXTERN JSFSteer *gJSF;

#endif
