#ifndef __MakeJIMInput__
#define __MakeJIMInput___

///////////////////////////////////////////////////////////////////////
//
//  MakeJIMInput
//
//  Create ZBS format data for JIMsimulator
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include "JSFModule.h"
#include "TClonesArray.h"
#include "TString.h"

// *******************************************************
class MakeJIMInput : public JSFModule 
{
 protected:
  TString fOutputFile;     //!
  Int_t  fLunit;           //!
  Int_t  fFirstEvent;      //!

 public:
  MakeJIMInput(const char *name="MakeJIMInput", 
		  const char *title="MakeJIMInput");
  virtual ~MakeJIMInput();

  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t runno=1);
  virtual Bool_t EndRun();
  virtual Bool_t Terminate();
  virtual Bool_t Process(Int_t ev=1);

  virtual void   MakeTrack(Int_t jseg, Int_t idata[]);
  virtual Int_t  GetNtracks();
  
  ClassDef(MakeJIMInput, 1)  // MakeJIMInput class

};

// *******************************************************
class MakeJIMInputForHadronizer : public MakeJIMInput
{
 protected:
  Int_t            fNparticles; //!
  TClonesArray    *fParticles; //!
  
 public:
  MakeJIMInputForHadronizer(const char *name="MakeJIMInputForHadronizer", 
			    const char *title="MakeJIMInputForHadronizer");
  virtual ~MakeJIMInputForHadronizer(){}

  virtual void   MakeTrack(Int_t jseg, Int_t idata[]);
  virtual Int_t  GetNtracks();
  
  ClassDef(MakeJIMInputForHadronizer, 1)  // MakeJIMInputForHadronizer class

};

#endif










