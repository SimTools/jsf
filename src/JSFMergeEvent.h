#ifndef __JSFMergeEvent__
#define __JSFMergeEvent__

///////////////////////////////////////////////////////////////////////
//
//  Used to merge background data into signal event.
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include "JSFModule.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __JSFQuickSim__
#include "JSFQuickSim.h"
#endif
#ifndef __PythiaGenerator__
#include "PythiaGenerator.h"
#endif

// *************************************************************
class JSFMergeEvent : public JSFModule {
protected:
  PythiaGenerator *fPythiaGenerator; //! Pointer to input data
  JSFQuickSim   *fJSFQuickSim; //! Pointer to input data.
  Char_t fDataFileName[256]; //! File name of inpufile;
  Float_t      fLumPerTrain; // Luminosity per train ( 1/nb )
  Float_t       fNumAverage; //  Averaged number of background event.
  Int_t         fNumEvent; // Total number of event in the file.
  Int_t         fRandomSeed; // Seed of random number.
  
   				
public:
  JSFMergeEvent(const Char_t *name="JSFMergeEvent", 
 	    const Char_t *title="Read/Write Background data",
	    const Char_t *opt="");

  virtual ~JSFMergeEvent();

  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t nrun=0);
  virtual Bool_t Process(Int_t ev=1);
  virtual Bool_t EndRun();

  Char_t *GetDataFileName(){ return fDataFileName; }
  void SetDataFileName(Char_t *name){ strcpy(fDataFileName, name);}

  ClassDef(JSFMergeEvent, 1)  // Merge event data.

};

#endif


