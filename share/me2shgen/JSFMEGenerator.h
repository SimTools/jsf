#ifndef __JSFMEGenerator__
#define __JSFMEGenerator__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFMEGenerator                                                       //
//
// A basic class to save information of Matrix Element generator
// a la format used in Pythia6.2 
//                                                                      //
//                                                                      //
//$Id$
//////////////////////////////////////////////////////////////////////////

#ifndef __JSFSteer__
#include "JSFSteer.h"
#endif
#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __JSFHEPRUP__
#include "JSFHEPRUP.h"
#endif
#ifndef __JSFHEPEUP__
#include "JSFHEPEUP.h"
#endif

class JSFMEGenerator;

// ---------------------------------------------
class JSFMEGeneratorBuf : public JSFEventBuf {
protected:
  JSFHEPEUP  fHEPEUP;  //
public:
  JSFMEGeneratorBuf(){}
   JSFMEGeneratorBuf(const char *name,
        const char *title="JSF Matrix Element Generator Event data",
		JSFMEGenerator *module=0);
   virtual ~JSFMEGeneratorBuf();

   virtual void SetHEPEUP(JSFHEPEUP &hepeup);

   JSFHEPEUP *GetJSFHEPEUP(){ return &fHEPEUP; }

   ClassDef(JSFMEGeneratorBuf,1)  // Matrix Element generator event data
};

//*************************************************************
class JSFMEGenerator : public JSFModule {
protected:
   JSFHEPRUP  fHEPRUP;
public:
   JSFMEGenerator(const char *name="JSFMEGenerator", 
		  const char *title="JSF Matrix Event Generator");

   virtual ~JSFMEGenerator();
   virtual void  PrintInfo(){}

   JSFHEPRUP  *GetJSFHEPRUP(){ return &fHEPRUP; }
   
   virtual Bool_t SetHEPRUP()=0;
   virtual JSFReturnCode_t SetHEPEUP(JSFHEPEUP &hepeup)=0;

   virtual Bool_t Process(Int_t event);
   virtual Bool_t BeginRun(Int_t nrun);

   ClassDef(JSFMEGenerator,1) // JSF Matrix Element Generator

};

#endif
