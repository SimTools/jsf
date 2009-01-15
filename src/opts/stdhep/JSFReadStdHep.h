#ifndef __JSFReadStdHep__
#define __JSFReadStdHep__

///////////////////////////////////////////////////////////////////////
//
//  JSFReadStdHep
//
//  Interface to HepStd Program
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <TObject.h>
#include "JSFReadGenerator.h"
#include "JSFStdHepStdCM.h"
#include "THEPEV4.h"
// #include "TString.h"
// #include "TObjString.h"
// #include "TObjArray.h"

/*
namespace StdHep {
class StdRunInfo;
class StdEvent;
};
*/

class JSFReadStdHepBuf : public JSFReadGeneratorBuf
{
protected:
  Int_t fEventNumber;      //!
  THEPEV4 fHEPEV4;         // HEPEV4 data
public:
  JSFReadStdHepBuf(const char *name="JSFReadStdHepBuf", 
	     const char *title="JSFReadStdHep buffer",
		  JSFReadGenerator *module=NULL);
  virtual ~JSFReadStdHepBuf();

  inline Int_t GetEventNumber(){ return fEventNumber; }
  inline Int_t GetNumberOfParticles(){ return fHEPEV4.GetNHEP(); }
  inline THEPEV4  &GetHEPEV4(){ return fHEPEV4; }

  // add your own function here
  virtual Bool_t ReadHepEvent(const Int_t maxhep, Int_t &nevhep,
	 Int_t &nhep, Int_t isthep[], Int_t idhep[], Int_t jmohep[][2],
         Int_t jdahep[][2], Double_t phep[][5], Double_t vhep[][4]);

  ClassDef(JSFReadStdHepBuf,2)  // JSFReadStdHep event data buffer
};


// *******************************************************
class JSFReadStdHep : public JSFReadGenerator 
{
 protected:
  std::ifstream fInStream;   //! Input File stream
  std::string   fInFileName; // Input filename

  std::vector<std::string> fFNStack; // Used to read multiple files.
  std::vector<std::string>::iterator  fFNIter; // 

//  TObjArray     *fFNArray;  // Used to read multiple files.
//  TIter         *fFNIter;  

  Int_t         fNReadBlock;
  Int_t         fMCFIOStream; // Input file stream for MCFIO
  Int_t         fDebugLevel;  //
  Int_t         fPrintSkipMessage; //
  JSFStdHepStdCM  fStdHepCM; // StdHep BeginRun/EndRUn information

  static Bool_t   fIsInitialized;  //!  

 public:
  JSFReadStdHep(const char *name="JSFReadStdHep", 
		  const char *title="JSFReadStdHep",
		const Bool_t makebuf=kTRUE);
  virtual ~JSFReadStdHep();

  void Print();
  
  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t runno=1);
  virtual Bool_t EndRun();
  virtual Bool_t Terminate();
  virtual Bool_t Process(Int_t ev=1);
  
  inline Int_t NReadBlock(){ return fNReadBlock; }
  inline void   SetStdHepCM(){ fStdHepCM.LoadStdHepCM(); }
  inline JSFStdHepStdCM   GetStdHepCM(){ return fStdHepCM; }

  static Bool_t IsInitialized(){ return fIsInitialized; }

  ClassDef(JSFReadStdHep, 4)  // JSFReadStdHep class

};

#endif










