#ifndef __JSFHadronizer__
#define __JSFHadronizer__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFHadronizer                                                         //
//                                                                      //
// A template for JSFHadronizer                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TDatime.h"

#ifndef __JSFFULLGenerator__
#include "JSFFULLGenerator.h"
#endif
#ifndef __JSFSpring__
#include "JSFSpring.h"
#endif

class JSFHadronizer : public JSFFULLGenerator {
protected:
   JSFSpring  *fSpring;
public:
public:
   JSFHadronizer(const char *name="JSFHadronizer", 
		 const char *title="JSFHadronizer");
//   JSFHadronizer(const char *name, const char *title, JSFSpring *spring);
//   virtual      ~JSFGenerator();
//   virtual void  Clear(Option_t *option ="");
//   virtual void  PrintInfo(){}

   virtual Bool_t Initialize();
   virtual Bool_t Process(Int_t event);
//   virtual void  BeginRun(Int_t nrun);


   ClassDef(JSFHadronizer,1)  // Debug structure
};


#endif
