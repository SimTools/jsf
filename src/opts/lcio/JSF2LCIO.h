#ifndef __JSF2LCIO__
#define __JSF2LCIO__

///////////////////////////////////////////////////////////////////////
//
//  JSF2LCIO
//
//  JSF to LCIO Converer
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

/*
#include "lcio.h"

#include "IO/LCWriter.h"
#include "EVENT/LCIO.h"
#include "DATA/LCFloatVec.h"
#include "DATA/LCIntVec.h"
 
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"
*/

#include <TString.h>
#include <TObject.h>
#include "JSFModule.h"

//class IO::LCWriter;
//class IMPL::LCRunHeaderImpl;

// *******************************************************
class JSF2LCIO : public JSFModule 
{
 protected:
  TString  fOutputFileName; // Output File name
  TString  fDetectorName;   // Detector name
//  IO::LCWriter *flcWrt;        //! 
//  IMPL::LCRunHeaderImpl* frunHdr; //! 

 public:
  JSF2LCIO(const char *name="JSF2LCIO", 
		  const char *title="JSF2LCIO");
  virtual ~JSF2LCIO();

  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t runno=1);
  virtual Bool_t EndRun();
  virtual Bool_t Terminate();
  virtual Bool_t Process(Int_t ev=1);
  
  ClassDef(JSF2LCIO, 1)  // JSF2LCIO class

};

#endif










