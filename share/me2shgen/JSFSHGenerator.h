#ifndef __JSFSHGenerator__
#define __JSFSHGenerator__

//////////////////////////////////////////////////////////////////////////
//
// JSFSHGenerator
//
// A basic class for interface from ME Generator to SH Generator
// Pythia hadronizer and Herwig Hadronizer should inherited from 
// this class.
//
//
//$Id$
//////////////////////////////////////////////////////////////////////////

#ifndef __JSFGenerator__
#include "JSFGenerator.h"
#endif
#ifndef __JSFMEGenerator__
#include "JSFMEGenerator.h"
#endif

class JSFSHGenerator;

// ---------------------------------------------
class JSFSHGeneratorBuf : public JSFGeneratorBuf {
public:
  JSFSHGeneratorBuf(){}
  JSFSHGeneratorBuf(const char *name,
    const char *title="JSF Shower/Hadronizer Generator Event data",
		    JSFSHGenerator *module=0);
   virtual ~JSFSHGeneratorBuf(){}

   ClassDef(JSFSHGeneratorBuf,1)  // Shower/Hadronizer Generator event data
};

//*************************************************************
class JSFSHGenerator : public JSFGenerator {
protected:

public:
   JSFSHGenerator(const char *name="JSFSHGenerator", 
		  const char *title="JSF Shower/Hadronizer Generator");

   virtual ~JSFSHGenerator();

   virtual Bool_t Process(Int_t event);
   virtual Bool_t BeginRun(Int_t nrun);

   ClassDef(JSFSHGenerator,1) // JSF Shower/Hadronizer Generator

};

#endif

