#ifndef __JSFJ4PrimaryVertex__
#define __JSFJ4PrimaryVertex__

///////////////////////////////////////////////////////////////////////
//
//  JSFJ4 Parimary Vertex class
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TVector3.h>
#include "JSFJ4PrimaryParticle.h"

class G4PrimaryVertex; 
class JSFJ4PrimaryVertex;

// *******************************************************
class JSFJ4PrimaryVertex : public TObject
{
 protected:
  Int_t               fSerialNo;    // Serial Number of vertex ( 0,1,2... )
  Double_t            fT0;          // Time
  Double_t            fWeight;      // Weight
  TVector3            fPosition;    // Vertex position
  TObjArray          *fParticles;   // Particles belong to this vertex
  JSFJ4PrimaryVertex *fNextVertex;  //! next vertex

 public:
  JSFJ4PrimaryVertex();
  JSFJ4PrimaryVertex(G4PrimaryVertex *p, Int_t ser);
  virtual ~JSFJ4PrimaryVertex();

  inline Int_t    GetSerialNo(){ return fSerialNo; }
  inline Double_t GetT0(){ return fT0; }
  inline Double_t GetWeight(){ return fWeight; }
  inline TVector3 GetPosition(){ return fPosition; }
  inline Int_t GetNumberOfParticles(){ return fParticles->GetEntries(); }
  inline TObjArray *GetParticles(){ return fParticles; }
  inline JSFJ4PrimaryParticle *GetParticleAt(Int_t i){ 
    return (JSFJ4PrimaryParticle*)fParticles->At(i); }
  inline JSFJ4PrimaryVertex *GetNextVertex(){ return fNextVertex; }

  inline void Add(JSFJ4PrimaryParticle *p){ fParticles->Add(p); }
  inline void Clear(){ fParticles->Clear(); }

  void Print(Option_t *opt="");
  
  ClassDef(JSFJ4PrimaryVertex, 1)  // JSFJ4PrimaryVertex class

};

#endif










