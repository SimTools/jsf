#ifndef __JSFWriteStdHep__
#define __JSFWriteStdHep__

///////////////////////////////////////////////////////////////////////
//
//  JSFWriteStdHep
//
//  Interface to HepStd Program
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>

#include <TObject.h>
#include "JSFModule.h"
#include "JSFStdHepStdCM.h"
#include "THEPEV4.h"
#include "JSFSpring.h"
#include <TString.h>

// *******************************************************
class JSFWriteStdHep : public JSFModule
{
 protected:
  //  std::ifstream fInStream;   //! Input File stream
  std::string   fOutFileName; // Output filename
  std::string   fOutTitle;    // Title of Output File  

  Int_t         fNTries; // Expected number of events in the output file;
  Int_t         fOutStream; // Index to array ixdrstd holfing stream pointer
  Int_t         fDebugLevel;  //
  //  Bool_t        fDoXWInit; // Does StdXWInit if true
 //  Bool_t        fConvertJetset;  // Does conversion of JETSET Common
  Int_t	        fEventSource;   // Event source
	// =0, /hepevt/, =1 /jetset/, 2=JSFGenerator, 3=JSFGenerator+SpringParton
  Bool_t        fWriteBeginRun; //! True if beginRun is not written yet
  Bool_t        fWriteEndRun;   //! True if endRun is not written yet
  Int_t         fProcessID;     // If fHEPEV4 is not set, fProcessID is write as HEPEV4 data
  Int_t         fComputeTime;   // Compute the production time, useful for long-lived particles

  Int_t         fNumberOfWriteEvents;   // Number of events written in file.
  Int_t         fCurrentNumberOfFiles;  // Number of files writeen so far.
  Int_t         fCurrentOutputSizeInkB; // Output data size size to the current file (in kB)
  Int_t         fMaxOutputSizeInkB;  // Output data size max.(kB unit)
  Int_t         fMaxNumberOfFiles;  // Maximum number of files.
  Int_t         fStdxwrtMode;       // 1 to write only HEPEVT, 4=1+hepev4
  Float_t       fCrossingAngle;     // Crossingle angle in unit of rad. no boost if > pi )

	JSFSpring *fSpring;

  JSFStdHepStdCM   fStdHepCM; 
  static THEPEV4 *fHEPEV4; // HEPEV4 event associated to this event
    // User should create and delete THEPEV4 object, in order to store his own HEPEV4 info.
  void HepEvtCheckAndMod();
 public:
  JSFWriteStdHep(const char *name="JSFWriteStdHep", 
		 const char *title="JSFWriteStdHep");
  virtual ~JSFWriteStdHep();

  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t runno=1);
  virtual Bool_t EndRun();
  virtual Bool_t Terminate();
  virtual Bool_t Process(Int_t ev=1);

  //  void DoXWInit(Bool_t flag=kTRUE){ fDoXWInit=flag; }
  //  void DoConvertJetset(Bool_t flag=kTRUE){ fConvertJetset=flag; }
  JSFStdHepStdCM  *GetStdHepCM(){ return &fStdHepCM;} 
  
  // Static function
  static void StdXWINIT(std::string ifilename, std::string ititle,
		 Int_t ntry, Int_t &iostream, Int_t &iok);
  static void StdXWOPEN(std::string ifilename, std::string ititle,
		 Int_t ntry, Int_t &iostream, Int_t &iok);
  static void StdFLPYXSEC(Int_t ntry);
  static void StdXWRT(Int_t ldbl, Int_t iostream, Int_t &lok);
  static void StdXEND(Int_t iostream);
  static void SetHEPEV4(THEPEV4 *hepev4){ fHEPEV4=hepev4; }
  Int_t  GetProcessID(){ return fProcessID; }
  void   SetProcessID(Int_t pid){ fProcessID=pid; } 

  void   ForEventSource4(int nev);
  void   ForEventSource5(int nev);

  Int_t  GetCurrentNumberOfFiles(){ return fCurrentNumberOfFiles; }
  Int_t  GetCurrentOutputSizeInkB(){ return fCurrentOutputSizeInkB; }
  TString GetStdHepFileName(Int_t iseq);
  
  ClassDef(JSFWriteStdHep, 2)  // JSFWriteStdHep class

};

#endif










