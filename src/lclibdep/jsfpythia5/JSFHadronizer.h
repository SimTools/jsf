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

#if __PYTHIA_VERSION__ >= 6 
#include "TPythia6.h"
#include "TMCParticle6.h"
#endif

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
   Int_t    fIHLON;     // Tau polarization switch
   Int_t    fDebug;     //! Debug flag
#if __PYTHIA_VERSION__ >= 6 
   Int_t    fMRPY[6];   // random seed for Jetset
   Float_t  fRRPY[100];  //
   TPythia6   *fPythia; //
   Bool_t   fDoesShower;  // Does shower, if TRUE.
#else
   Int_t    fMRLU[6];   // random seed for Jetset
   Float_t  fRRLU[100];  //
#endif
#ifndef __LCLIBRAN_USE_RANMAR__
   Float_t  fRASET1U[98];  // random seed for tauola
   Int_t    fRASET1IJ97[2]; // 
#endif

public:
   JSFHadronizer(const char *name="JSFHadronizer", 
		 const char *title="JSFHadronizer");
   virtual ~JSFHadronizer();

   virtual Bool_t Initialize();
   virtual Bool_t Process(Int_t event);
   virtual Bool_t EndRun();
   virtual Bool_t GetLastRunInfo();

   void SetCopySpringClassDataToBank(Bool_t flag){
     fCopySpringClassDataToBank= flag ;}
   Bool_t GetCopySpringClassDataToBank(){return fCopySpringClassDataToBank ; }


#if __PYTHIA_VERSION__ >= 6 
   void Fragmentation(const Int_t nin, Double_t inlist[][10],
		      const Int_t mxxout, const Int_t nspar,
		      const Int_t ispar1[], const Int_t ispar2[],
		      const Int_t kstat[],  const Int_t jstat[],
		      Int_t &nout, Double_t outlst[][20],
		      Int_t &nret);
   void Hadronize(JSFSpring *spring, Int_t &nret);
#endif


#ifndef __LCLIBRAN_USE_RANMAR__
   ClassDef(JSFHadronizer,2)  // Debug structure
#else
#if __PYTHIA_VERSION__ <= 5 
   ClassDef(JSFHadronizer,3)  // Debug structure
#else
   ClassDef(JSFHadronizer,4)  // Debug structure
#endif
#endif
};


#endif


