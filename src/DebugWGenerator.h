#ifndef __DebugWGenerator__
#define __DebugWGenerator__

//////////////////////////////////////////////////////////////////////////
//
// DebugWGenerator
//
// Generate W for debugging
//
//////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TMath.h"
#include "TDatime.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __JSFSpring__
#include "JSFSpring.h"
#endif

class DebugWGenerator;

class DebugWGeneratorBuf : public JSFSpringBuf {
public:
public:
  DebugWGeneratorBuf(const char *name="DebugWGeneratorBuf", 
	     const char *title="DebugWGenerator event buffer",
	     DebugWGenerator *spring=NULL)
	     : JSFSpringBuf(name,title,(JSFSpring*)spring) {} 
  virtual ~DebugWGeneratorBuf(){}
  virtual Bool_t SetPartons();

  ClassDef(DebugWGeneratorBuf,1)  // DebugWGenerator event buffer
};

class DebugWGenerator : public JSFSpring {
public: 
   Float_t fWmass ;  // W boson mass
   Float_t fWwidth;  // W boson width
   Float_t fRp[2];   // Boson momentum min. and max.
   Float_t fRcosth[2];  // Boson costh range min. and max.
   Float_t fRphi[2];    // Boson azimuthal angle min. and max. (radian)
public:
   DebugWGenerator(const char *name="DebugWGenerator", 
	      const char *title="DebugWGenerator",
             JSFBases *bases=NULL);
   virtual ~DebugWGenerator();

   virtual Bool_t BeginRun(Int_t nrun){ return kTRUE;}
   virtual Bool_t Process(Int_t ev);

   void SetWmass(Float_t mass){ fWmass=mass; }
   void SetWwidth(Float_t width){ fWwidth=width; }
   void SetRangeP(Float_t min, Float_t max){ fRp[0]=min ; fRp[1]=max; }
   void SetRangeCosth(Float_t min, Float_t max){fRcosth[0]=min;fRcosth[1]=max; }
   void SetRangePhi(Float_t min, Float_t max){ fRphi[0]=min ; fRphi[1]=max; }


   ClassDef(DebugWGenerator,1)  // DebugWGenerator class

};


#endif


