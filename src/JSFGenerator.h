#ifndef __JSFGenerator__
#define __JSFGenerator__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFGenerator                                                         //
//                                                                      //
// A template for JSFGenerator                                          //
//                                                                      //
//$Id$
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TDatime.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __JSFGeneratorParticle__
#include "JSFGeneratorParticle.h"
#endif

class JSFGeneratorParticle;
class JSFGenerator;

//*************************************************************
class JSFGeneratorBuf : public JSFEventBuf {
protected:
   Int_t            fStartSeed;  // Seed value at the begining of the event
   Double_t         fEcm;        // Center of Mass energy of the event.
   Int_t            fNparticles;  // Number of particles 
   TClonesArray    *fParticles;  // Pointers to Particles
public:
   JSFGeneratorBuf(const char *name="JSFGeneratorBuf",
		   const char *title="JSF Generator",
		   JSFGenerator *generator=0);
   virtual ~JSFGeneratorBuf();

   virtual void Clear(Option_t *option="");
   void SetStartSeed(Int_t seed){ fStartSeed=seed; }
   void SetEcm(Double_t ecm){ fEcm=ecm;}
   void SetNparticles(Int_t np){ fNparticles=np;}

   Int_t GetStartSeed(){ return fStartSeed; }
   Double_t GetEcm(){ return fEcm; }
   Int_t         GetNparticles() const { return fNparticles;}
   TClonesArray *GetParticles(){ return fParticles; }

   void Append(JSFGeneratorBuf *src);

   ClassDef(JSFGeneratorBuf, 2) // JSF Generator Buffer class.
};
   

//*************************************************************
class JSFGenerator : public JSFModule {
public:
   JSFGenerator(const Char_t *name="JSFGenerator", 
		const Char_t *title="JSF Generator",
		const Char_t *opt="");
   virtual      ~JSFGenerator();

   virtual Bool_t Process(Int_t event);
//   virtual void  BeginRun(Int_t nrun);

    ClassDef(JSFGenerator,1)  // JSF Generator class.
};


#endif
