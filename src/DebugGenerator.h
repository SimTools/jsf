#ifndef __DebugGenerator__
#define __DebugGenerator__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Debug Generator                                                      //
//                                                                      //
// Header file for debug generator
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TH1.h"
#include "TMath.h"

#ifndef __JSFGenerator__
#include "JSFGenerator.h"
#endif


class DebugGenerator : public JSFGenerator {
private:
  Int_t            fSpec;   //! Species counter

  Int_t     fRandomSeed;  // random seed.
  Int_t     fNgen;        // Number of particles to generate.
  Double_t  fP[2];        // momentum min. and max. (GeV)
  Double_t  fCosth[2];    // Costh min and max (-1 to 1 )
  Double_t  fAzimA[2];    // Azimuthal angle min and max. (degree)
  Double_t  fR[2];        // souce point rmin and rmax (cm)
  Double_t  fPhi[2];      // source point phi-min and phi max (degree)
  Double_t  fZ[2];        // source point zmin and z max. (cm);
  Int_t     fNspecies;    // Number of particle species ( <= 10 )
  Int_t     fID[10];      // particle ID
  Double_t  fMasses[10];  // masses.
  Double_t  fCharges[10]; // charges

public:
   DebugGenerator(const char *name="DebugGenerator", 
		  const char *title="Debug Generator");
   virtual      ~DebugGenerator();

   virtual void  Hcreate();
   virtual void  Hfill();

   virtual Bool_t Initialize();
   virtual Bool_t Process(Int_t event);
   virtual Bool_t BeginRun(Int_t nrun);
   
   void Print();
   void SetNgen(Int_t n){ fNgen=n;}
   void SetP(Double_t min, Double_t max) 
                            { fP[0]=min ; fP[1]=max; }
   void SetCosth(Double_t min, Double_t max ) 
                            { fCosth[0]=min ; fCosth[1]=max; }
   void SetAzimuthAngle(Double_t min, Double_t max ) 
                            { fAzimA[0]=min ; fAzimA[1]=max; }
   void SetVTXR(Double_t min, Double_t max ) 
                            { fR[0]=min ; fR[1]=max; }
   void SetVTXPhi(Double_t min, Double_t max ) 
                            { fPhi[0]=min ; fPhi[1]=max; }
   void SetVTXZ(Double_t min, Double_t max ) 
                            { fZ[0]=min ; fZ[1]=max; }
   void AddSpecies(Int_t ID, Double_t charge, Double_t mass);

   void SetSpecies(Int_t ID, Double_t charge, Double_t mass)
       { fNspecies=0 ; AddSpecies(ID, charge, mass) ; }

   void PrintParam();


   ClassDef(DebugGenerator,1)  // Debug structure
};


#endif
