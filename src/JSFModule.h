//*LastUpdate:  v0.3.03 09/22/1998  by A.Miyamoto
//*-- Author :  Akiya Miyamoto  09/22/1998

//////////////////////////////////////////////////////////////////
//
//  JSFModule
//
//  Basic unit of data analysis
//
//$Id$
//
//////////////////////////////////////////////////////////////////

#ifndef __JSFModule__
#define __JSFModule__

#include <TROOT.h>
#include <TNamed.h>
#include <TFile.h>
#include <TTree.h>

class JSFModule;

class JSFEventBuf : public TNamed {
private:
   Int_t   fRunNumber;   // Run number 
   Int_t   fEventNumber; // Event number
   Int_t   fDate;        // event date.
   Int_t   fTime;        // event time
protected:
   JSFModule *fModule;   //! Pointer which defines this Event Buffer
   TTree     *fTree;     //! Pointer to tree
public:
   JSFEventBuf(){}
   JSFEventBuf(const char *name, const char *title, JSFModule *module);
   JSFEventBuf(JSFModule *module, const char *name, const char *title);
   virtual ~JSFEventBuf(){}

   void SetHeader(); // Set header information.

   Int_t GetRunNumber(){ return fRunNumber; }
   Int_t GetEventNumber(){ return fEventNumber; }
   Int_t GetDate(){ return fDate; }
   Int_t GetTime(){ return fTime; }
   void SetRunNumber(Int_t nrun){ fRunNumber=nrun; return; }
   void SetEventNumber(Int_t nevent){ fEventNumber=nevent; return; }
   JSFModule *Module(){ return fModule; }
   TTree     *Tree(){ return fTree; }

   ClassDef(JSFEventBuf, 1) // Class to save event data for the module.

};

typedef enum {kConstruct, kInitialize, kBeginRun, kEndRun,
              kEventLoop, kTerminate, kDestruct } EJSFModuleStatus;

class JSFModule : public TNamed {
protected:
    Int_t    fVersion;       // Version number of the module
    TList    *fHistograms;    //! Pointer to the histograms ( =0 no histo)
    TFile    *fFile;         //! Where the data of this module is read/write
    TTree    *fTree;         //! pointer to event tree. 
    TBranch  *fBranch;      //! Branch for this module.
    Int_t    fRunNumber;     // run number
    Int_t    fSplit;         // Split value for the branch
    Int_t    fBufferSize;    // Buffer size of the branch
    Bool_t   fMakeBranch;    // kTRUE to create branch for this module.
    JSFEventBuf *fEventBuf;  //! Pointer to event data, saved in the Event tree
    EJSFModuleStatus fStat;  //! Module status
    void ConfDirectory();
public:
   JSFModule(const Char_t *name="JSFModule", 
             const Char_t *title="JSF Default Module",
	     const Option_t *opt="");
   virtual ~JSFModule();

   void SetModuleStatus(EJSFModuleStatus stat){ fStat=stat;}
   void SetRunNumber(Int_t nrun){ fRunNumber=nrun; }

   virtual  void PrintInfo();

   virtual Bool_t ReadCommandArgument(int* argc, char** argv){return kTRUE;}
   virtual Bool_t Process(Int_t event);
   virtual Bool_t Initialize();
   virtual Bool_t BeginRun(Int_t nrun);
   virtual Bool_t EndRun();
   virtual Bool_t Terminate();
   virtual Bool_t GetLastRunInfo();

   virtual  void MakeBranch(TTree *tree); // Make branch for the module 
   virtual  void SetBranch(TTree *tree);  // Set Branch address for the module

   void SetSplit(Int_t split){ fSplit=split; }
   void SetBufferSize(Int_t size){ fBufferSize=size;} 
   TBranch *GetBranch(){ return fBranch;}
   void SetMakeBranch(Bool_t flag){ fMakeBranch=flag;}
   Bool_t GetMakeBranch(){ return fMakeBranch; }
   TFile *GetFile(){return fFile;}
   TTree *GetTree(){ return fTree;}

   void     SetFile(TFile *file){ fFile=file; return; }
   virtual  JSFEventBuf *EventBuf(){ return fEventBuf; }
   Bool_t   IsWritable() { if( !fFile ) return kFALSE ; 
            return fFile->IsWritable(); }

   ClassDef(JSFModule,1)  //Event structure
};


#endif


