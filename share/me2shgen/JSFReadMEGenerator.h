#ifndef __JSFReadMEGenerator__
#define __JSFReadMEGenerator__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFReadMEGenerator
//
// Read ME Generator event data from a file
// for sub-sequent hadronization by JSFSHGenerator
//
//
//$Id$
//////////////////////////////////////////////////////////////////////////

#ifndef __JSFMEGenerator__
#include "JSFMEGenerator.h"
#endif

class JSFReadMEGenerator;
class JSFReadGZippedFile;

// ---------------------------------------------
class JSFReadMEGeneratorBuf : public JSFMEGeneratorBuf {
public:
  JSFReadMEGeneratorBuf(){}
   JSFReadMEGeneratorBuf(const char *name,
        const char *title="Read Matrix Element Generator Event data",
			   JSFReadMEGenerator *module=0){}
   virtual ~JSFReadMEGeneratorBuf(){}


   ClassDef(JSFReadMEGeneratorBuf,1)  // Matrix Element generator event data
};

//*************************************************************
class JSFReadMEGenerator : public JSFMEGenerator {
protected:
  
  TString   fEventFileName;   
  TString   fRunFileName;   
  ifstream *fEventInput;  //!
  ifstream *fRunInput;    //!

  JSFReadGZippedFile *fgzfile; //!

  Bool_t    fIsGZipped;   //!
  Int_t     fReadEvents;  //
  Int_t     fFirstEvent; //

public:
   JSFReadMEGenerator(const char *name="JSFReadMEGenerator", 
		      const char *title="JSF Read Matrix Event Generator");

   virtual ~JSFReadMEGenerator();

   Bool_t IsGZipped(){ return fIsGZipped; }

   virtual Bool_t Initialize();

   virtual Bool_t SetHEPRUP();
   virtual JSFReturnCode_t SetHEPEUP(JSFHEPEUP &hepeup);

   ClassDef(JSFReadMEGenerator,1) // JSF Read Matrix Element Generator

};

#endif
