#ifndef __JSFSpring__
#define __JSFSpring__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFSpring                                                         //
//                                                                      //
// A template for JSFSpring                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TDatime.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __JSFBases__
#include "JSFBases.h"
#endif
#ifndef __JSFSpringParton__
#include "JSFSpringParton.h"
#endif
// Define class for parameters.
// #ifndef __JSFSpringParam__
// #include "JSFSpringParam.h"
// #endif

class JSFSpringParton;

class JSFSpringBuf : public JSFEventBuf {
protected:
   Int_t          fNparton; // Number of partons
   TClonesArray  *fPartons; // Pointer to partons
public:
   JSFSpringBuf(const char *name="JSFSpringBuf", 
	     const char *title="JSF Spring event buffer",
		JSFSpring *spring=0);
   virtual ~JSFSpringBuf();

   virtual void Clear(Option_t *option="");
   virtual Bool_t SetPartons(){ fNparton=0; return kTRUE; }

   Int_t             GetNpartons() const { return fNparton; }
   TClonesArray     *GetPartons() { return fPartons; }

   ClassDef(JSFSpringBuf,1)  // Spring Event data
};

//*************************************************************
class JSFSpring : public JSFModule {
protected:
   JSFBases        *fBases;  //! Pointer to bases class
   Bool_t          fDoBases; //! Do bases at beginrun, or read bases data
   TFile           *fBasesFile; //! Pointer to a bases file
   Int_t            fMXTRY; // Max number of try in the Spring step.

   Float_t    fSeedRdm[33]; //! Random seed array at the begining of current event
   Int_t    fSeedIa1[12]; //! I*4 Random seed array at the begining of current event
   Bool_t     fSetSeed; //! true to copy fSeedRdm and fSeedIa1 to bases common.
public:
   JSFSpring(const char *name="JSFSpring", 
	     const char *title="JSF Spring", 
	     JSFBases *bases=NULL);

   virtual ~JSFSpring();
   virtual void  PrintInfo(){}

   virtual Bool_t Process(Int_t event);
   virtual Bool_t BeginRun(Int_t nrun);
   virtual Bool_t EndRun();
   virtual Bool_t GetLastRunInfo();

   virtual JSFBases *Bases() { return fBases; }
   
   void SetBases(JSFBases *bases){ fBases=bases; 
   if( fBases->Spring() != this ) fBases->SetSpring(this) ; }
   void DoBases(){ fDoBases=kTRUE; }
   void ReadBases(const char *name);
   
//   JSFSpringParam    *GetParam(){ return &gJSFSpringParam; }

    ClassDef(JSFSpring,1)  // JSFSpring module
};


#endif

