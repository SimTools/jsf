#ifndef __JSFJ4__
#define __JSFJ4__

///////////////////////////////////////////////////////////////////////
//
//  JSF-G4 interface
//
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TMap.h>
#include <TString.h>
#include <TTree.h>
#include "JSFModule.h"

class JSFJ4RunManager;
class G4VisManager;
class G4UImanager;

class J4DetectorConstruction;
class J4Output;
class JSFJ4;

// ******************************************************
class JSFJ4BufferMap : public TObject
{
 protected:
  TString    fName;     //
  TObjArray *fObjArray; //
 public:
  JSFJ4BufferMap(Char_t *name="");
  virtual ~JSFJ4BufferMap();

  inline const Char_t  *GetName(){ return fName.Data(); }
  inline TObjArray  *GetObjArray(){ return fObjArray; }

  ClassDef(JSFJ4BufferMap, 1) // Wrapper to save JSFJ4 event data

};  


// *******************************************************
class JSFJ4Buf : public JSFEventBuf
{
 friend class JSFJ4;
 protected:

  Int_t           fEventID;
  TObjArray       fEvent;     // Object array to store data of each detector component

 public:
  JSFJ4Buf();
  JSFJ4Buf(const Char_t *name, const Char_t *title, JSFModule *module);
  virtual ~JSFJ4Buf();

  TObjArray *AddComponent(Char_t *name);
  TObjArray *GetComponent(Char_t *name) const;

  void Print(Option_t *opt="");
  inline Int_t GetEventID(){ return fEventID; }

  ClassDef(JSFJ4Buf, 1)  // Event Data buf of JSFJ4

};

// *******************************************************
class JSFJ4 : public JSFModule
{
 protected:
  Int_t fNcall;
  Int_t fNgood;

  JSFJ4RunManager *fRunManager;  //!
  G4VisManager *fVisManager;  //!
  G4UImanager  *fUImanager;   //!

  Bool_t  fUseJupiterGenerator;  // JSFGenerator(false) or Jupiter generator(true)
  TString fInitMacro;

  // Pointers to the detector
  J4DetectorConstruction *fpDetector;  //!

 public:
  JSFJ4(const char *name="JSFJ4",
		  const char *title="JSFJ4");
  virtual ~JSFJ4();

  void BuildJupiter();

  inline JSFJ4RunManager *GetRunManager(){ return fRunManager; }

  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t irun=1);
  virtual Bool_t Process(Int_t ev=1);
  virtual Bool_t EndRun();
  virtual Bool_t Terminate();

  ClassDef(JSFJ4, 1)  // JSFJ4 class

};

#endif










