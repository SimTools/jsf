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
#if __ROOT_VERSION__ >= 5 && __ROOT_MINORVERSION__ >= 16 
#include "TMCParticle.h"
#else
#include "TMCParticle6.h"
#endif
#endif

#ifndef __JSFFULLGenerator__
#include "JSFFULLGenerator.h"
#endif
#ifndef __JSFSpring__
#include "JSFSpring.h"
#endif

//#define __SAVE_TPYTHIA6__

class JSFHadronizer : public JSFFULLGenerator {
protected:
   JSFSpring  *fSpring;    //! Pointer to spring module
   Bool_t      fCopySpringClassDataToBank; //! 
   Int_t    fIHLON;     // Tau polarization switch
   Int_t    fDebug;     //! Debug flag
   Int_t    fMRPY[6];   // random seed for Jetset
   Float_t  fRRPY[100]; //
#if __PYTHIA_VERSION__ >= 6
#ifdef __SAVE_TPYTHIA6__
   TPythia6   *fPythia; //
#else
   TPythia6   *fPythia; //!
#endif
#endif
   Bool_t   fDoesShower;  // Does shower, if TRUE.
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


   void Fragmentation(const Int_t nin, Double_t inlist[][10],
		      const Int_t mxxout, const Int_t nspar,
		      const Int_t ispar1[], const Int_t ispar2[],
		      const Int_t kstat[],  const Int_t jstat[],
		      Int_t &nout, Double_t outlst[][20],
		      Int_t &nret);
   void Hadronize(JSFSpring *spring, Int_t &nret);


#ifndef __LCLIBRAN_USE_RANMAR__
   ClassDef(JSFHadronizer,2)  // Debug structure
#else
#if __PYTHIA_VERSION__ <= 5 
   ClassDef(JSFHadronizer,3)  // Debug structure
#else
#ifndef __SAVE_TPYTHIA6__
   ClassDef(JSFHadronizer,4)  // Debug structure
#else
   ClassDef(JSFHadronizer,5)  // Debug structure
#endif
#endif
#endif
};


#endif


