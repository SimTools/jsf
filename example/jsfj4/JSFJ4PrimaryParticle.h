#ifndef __JSFJ4PrimaryParticle__
#define __JSFJ4PrimaryParticle__

///////////////////////////////////////////////////////////////////////
//
//  JSFJ4 ParimaryParticle class
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TVector3.h>
#include "JSFGeneratorParticle.h"

class G4PrimaryParticle; 
class JSFJ4PrimaryVertex;

// *******************************************************
class JSFJ4PrimaryParticle : public TObject
{
 public:
 protected:
  Int_t      fTrackID;       // Track Id G4 particle
  Int_t      fParticleID;    // PDG Code of this particle

  Float_t    fCharge;        // Charge
  Float_t    fMass;          // Mass
  TVector3   fMomentum;      // Momentum of particle (GeV)
  TVector3   fPolarization;  // Polarization
  Double_t   fWeight;        // Weigtht of this track
  Double_t   fProperTime;    //

  Int_t      fIndex2GeneratorParticle;     // Index to generator particle
  
  TObjArray  *fDaughters;   // Pointer to daughters ( they are not registered in vertex )
  JSFGeneratorParticle *fGeneratorParticle; //!
  JSFJ4PrimaryVertex   *fVertex;            //! Primary Vertex

  void  SetParticle(G4PrimaryParticle *p);
  
 public:
  JSFJ4PrimaryParticle();
  JSFJ4PrimaryParticle(G4PrimaryParticle *p, 
		       Int_t index=0, JSFGeneratorParticle *gp=0);
  virtual ~JSFJ4PrimaryParticle();

  void SetPrimaryVertex(JSFJ4PrimaryVertex *v){ fVertex=v; }
  void Clear(){ fDaughters->Clear(); }

  inline void AddDaughter(JSFJ4PrimaryParticle *d){ fDaughters->Add(d); }
  inline TObjArray *GetDaughters(){ return fDaughters; }
  inline JSFJ4PrimaryParticle *GetDaughter(Int_t i=0){ 
    return (JSFJ4PrimaryParticle*)fDaughters->UncheckedAt(i); } 

  inline Int_t GetTrackID(){ return fTrackID; }
  inline Int_t GetParticleID(){ return fParticleID; }
  inline Double_t GetMass(){ return (Double_t)fMass; }
  inline TVector3 GetMomentum(){ return fMomentum; }
  inline TVector3 GetPolarization(){ return fPolarization; }
  inline Double_t GetWeight(){ return fWeight; }
  inline Int_t    GetIndex2GeneratorParticle(){ 
    return fIndex2GeneratorParticle; 
  }
  inline JSFGeneratorParticle *GetGeneratorParticle(){ return fGeneratorParticle; }
  inline JSFJ4PrimaryVertex   *GetPrimaryVertex(){ return fVertex; }

  inline Double_t GetCharge(){ return fCharge; }
  inline Double_t GetProperTime(){ return fProperTime; }

  void Print();
  
  ClassDef(JSFJ4PrimaryParticle, 1)  // JSFJ4PrimaryParticle class

};

#endif










