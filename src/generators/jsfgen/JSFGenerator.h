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
#include "TParticle.h"

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
   TClonesArray    *fParticles;  //-> Pointers to Particles
public:
   JSFGeneratorBuf();
   JSFGeneratorBuf(const char *name,
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
   Int_t         GetNParticles() const { return fNparticles;}
   TClonesArray *GetParticles(){ return fParticles; }

   virtual void Print(const Option_t *opt="");

   void Append(JSFGeneratorBuf *src);

   ClassDef(JSFGeneratorBuf, 2) // JSF Generator Buffer class.
};
   

//*************************************************************
class JSFGenerator : public JSFModule {
protected:
   Double_t fXAngle; // Crossing angle
public:
   JSFGenerator(const Char_t *name="JSFGenerator", 
		const Char_t *title="JSF Generator",
		const Char_t *opt="",
		const Bool_t makebuf=kTRUE);
   virtual      ~JSFGenerator();

   virtual Bool_t Process(Int_t event);

//   virtual void  BeginRun(Int_t nrun);
    inline Double_t GetXAngle(){ return fXAngle; }
    virtual const Char_t *GetEventTypeString();

    ClassDef(JSFGenerator,1)  // JSF Generator class.
};


#endif
