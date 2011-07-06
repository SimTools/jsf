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

  typedef struct {
    bool done;
    float rbuf[20];
    int   ibuf[7];
  } GenP_t ;

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

   Int_t   fHadronizerType;
   Int_t   fTauola_Keypol;
   Int_t   fFSQEDRadiation;

   static JSFHadronizer *fInstance;
   Float_t *fHelicity;     //! Particle helicity, used in Pytaud
   Int_t   *fColor;        //! color flow info al a hepeup
   Int_t   *fAntiColor;    //! Anti-color flow al a hepeup

   void SetColorFlow(int ip, int npgen0, int ibuf[]);
   void SetColorFlowInfo(int *lastColorID);

public:
   JSFHadronizer(const char *name="JSFHadronizer", 
		 const char *title="JSFHadronizer");
   virtual ~JSFHadronizer();

   static JSFHadronizer *Instance();
   void   Pytaud(int *itau, int *iorig, int *kforig, int *ndecay);

   virtual Bool_t Initialize();
   virtual Bool_t Process(Int_t event);
   virtual Bool_t EndRun();
   virtual Bool_t GetLastRunInfo();

   void SetCopySpringClassDataToBank(Bool_t flag){
     fCopySpringClassDataToBank= flag ;}
   Bool_t GetCopySpringClassDataToBank(){return fCopySpringClassDataToBank ; }
   TPythia6 *GetTPythia(){ return fPythia; }



   void Fragmentation(const Int_t nin, Double_t inlist[][10],
		      const Int_t mxxout, const Int_t nspar,
		      const Int_t ispar1[], const Int_t ispar2[],
		      const Int_t kstat[],  const Int_t jstat[],
		      Int_t &nout, Double_t outlst[][20],
		      Int_t &nret);

   void Hadronize(JSFSpring *spring, Int_t &nret);
   void HadronizeNew(JSFSpring *spring, Int_t &nret);
   void Hepevt2GeneratorParticle(Int_t &npgen, JSFGeneratorBuf *gbuf);
   void SetPythiaDBDStandard(Double_t mh=120.0, Double_t mw=0.3605E-2);

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


