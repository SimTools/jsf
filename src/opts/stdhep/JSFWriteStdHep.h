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
// #include "THEPEV4.h"

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
  Bool_t        fConvertJetset;  // Does conversion of JETSET Common
  Bool_t        fWriteBeginRun; //! True if beginRun is not written yet
  Bool_t        fWriteEndRun;   //! True if endRun is not written yet

  JSFStdHepStdCM   fStdHepCM; 

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
  void DoConvertJetset(Bool_t flag=kTRUE){ fConvertJetset=flag; }
  JSFStdHepStdCM  *GetStdHepCM(){ return &fStdHepCM;} 
  
  // Static function
  static void StdXWINIT(std::string ifilename, std::string ititle,
		 Int_t ntry, Int_t &iostream, Int_t &iok);
  static void StdXWOPEN(std::string ifilename, std::string ititle,
		 Int_t ntry, Int_t &iostream, Int_t &iok);
  static void StdFLPYXSEC(Int_t ntry);
  static void StdXWRT(Int_t ldbl, Int_t iostream, Int_t &lok);
  static void StdXEND(Int_t iostream);
  
  ClassDef(JSFWriteStdHep, 1)  // JSFWriteStdHep class

};

#endif










