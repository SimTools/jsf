#ifndef __JSFHadronizer__
#define __JSFHadronizer__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFHadronizer                                                        //
//                                                                      //
// A template for JSFHadronizer                                         //
//                                                                      //
//$Id$
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TDatime.h"

#include "JSFConfig.h"

#ifndef __JSFFULLGenerator__
#include "JSFFULLGenerator.h"
#endif
#ifndef __JSFSpring__
#include "JSFSpring.h"
#endif

class JSFHadronizer : public JSFFULLGenerator {
protected:
   JSFSpring  *fSpring;    //! Pointer to spring module
   Bool_t      fCopySpringClassDataToBank; //! 
   Int_t    fMRLU[6];   // random seed for Jetset
   Float_t  fRRLU[100];  //
#ifndef __LCLIBRAN_USE_RANMAR__
   Float_t  fRASET1U[98];  // random seed for tauola
   Int_t    fRASET1IJ97[2]; // 
#endif
public:
   JSFHadronizer(const char *name="JSFHadronizer", 
		 const char *title="JSFHadronizer");

   virtual Bool_t Initialize();
   virtual Bool_t Process(Int_t event);
   virtual Bool_t EndRun();
   virtual Bool_t GetLastRunInfo();

   void SetCopySpringClassDataToBank(Bool_t flag){
     fCopySpringClassDataToBank= flag ;}
   Bool_t GetCopySpringClassDataToBank(){return fCopySpringClassDataToBank ; }

#ifndef __LCLIBRAN_USE_RANMAR__
   ClassDef(JSFHadronizer,2)  // Debug structure
#else
   ClassDef(JSFHadronizer,3)  // Debug structure
#endif
};


#endif


