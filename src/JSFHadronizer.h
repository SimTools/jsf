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
   Bool_t      fCopySpringClassDataToBank; // 
public:
public:
   JSFHadronizer(const char *name="JSFHadronizer", 
		 const char *title="JSFHadronizer");

   virtual Bool_t Initialize();
   virtual Bool_t Process(Int_t event);

   void SetCopySpringClassDataToBank(Bool_t flag){
     fCopySpringClassDataToBank= flag ;}
   Bool_t GetCopySpringClassDataToBank(){return fCopySpringClassDataToBank ; }
//   virtual void  BeginRun(Int_t nrun);


   ClassDef(JSFHadronizer,1)  // Debug structure
};


#endif


