#ifndef __JSFReadParton__
#define __JSFReadParton__

//////////////////////////////////////////////////////////////////////////
//
// JSFReadParton
//
// Read parton data from a file fo subsequent hadronize of JSFHadronize
//
//////////////////////////////////////////////////////////////////////////

#include <TNamed.h>
#include <TMath.h>
#include <TDatime.h>

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __JSFSpring__
#include "JSFSpring.h"
#endif

class JSFReadParton;

class JSFReadPartonBuf : public JSFSpringBuf {
protected:
public:
  JSFReadPartonBuf(const char *name="JSFReadPartonBuf", 
	     const char *title="Read Parton data from a file",
	     JSFReadParton *spring=NULL)
	     : JSFSpringBuf(name,title,(JSFSpring*)spring) {} 
  virtual ~JSFReadPartonBuf(){}
  virtual Bool_t SetPartons();

  ClassDef(JSFReadPartonBuf,1)  // JSFReadParton event buffer
};

class JSFReadParton : public JSFSpring {
protected:
  Int_t  fUnit;       // Fortran logical unit number to output SIMDST data.
  Char_t fDataFileName[256]; //! File name of output file.
  Int_t  fType;  // Input data type (0=binary, 1=asc)
public:
   JSFReadParton(const char *name="JSFReadParton", 
	      const char *title="Read Parton data from a file");
   virtual ~JSFReadParton();

   virtual Bool_t BeginRun(Int_t run);
   virtual Bool_t EndRun();
   virtual Bool_t Process(Int_t event);

   Int_t  GetUnit(){ return fUnit;}
   Int_t  GetType(){ return fType;}
   Char_t *GetDataFileName(){ return fDataFileName; }
   void SetUnit(Int_t nunit){ fUnit=nunit; }
   void SetType(Int_t type){ fType=type; }
   void SetDataFileName(Char_t *name){ strcpy(fDataFileName, name);}

   ClassDef(JSFReadParton,1)  // JSFReadParton class
};


#endif