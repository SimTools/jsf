#ifndef __JSFJ4CALHit__
#define __JSFJ4CALHit__

///////////////////////////////////////////////////////////////////////
//
//  JSFJ4 CAL Hit data
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TVector3.h>

class J4CALHit;

// *******************************************************
class JSFJ4CALHit : public TObject
{
 protected:
  Int_t      fTrackID;       // Serial Number of G4 particle
  Int_t      fMotherTrackID; // Mother track ID.
  Int_t      fParticleID;    // Particle ID

  Short_t    fThetaNumber;   // Section no. in theta
  Short_t    fPhiNumber;     // Section Number in Phi
  Bool_t     fIsBarrel;      // (kFALSE, kTRUE) = (endcap, barrel)
  Bool_t     fIsEM;          // (kFALSE, kTRUE) = (Hadron, EM)

  Double_t   fEnergyDeposit;  // Energy Deposit to this cell in GeV
  Double_t   fTof;            // Time-of-flight in nsec
  
  //  TVector3   fMomentum;      // Momentum of particle (GeV)
  TVector3   fPosition;      // Input Position (cm)

 public:
  JSFJ4CALHit();
  JSFJ4CALHit(J4CALHit *ahit);
  virtual ~JSFJ4CALHit();

  inline Int_t GetTrackID(){ return fTrackID; }
  inline Int_t GetMotherTrackID(){ return fMotherTrackID; }
  inline Int_t GetParticleID(){ return fParticleID; }
  inline Int_t GetThetaNumber(){ return (Int_t)fThetaNumber; }
  inline Int_t GetPhiNumber(){ return (Int_t)fPhiNumber; }
  inline Bool_t IsBarrel(){ return fIsBarrel; }
  inline Bool_t IsEM(){ return fIsEM; }
  inline Double_t  GetEnergyDeposit(){ return fEnergyDeposit; }
  inline Double_t  GetTof(){ return fTof; }
  //  inline TVector3  GetMomentum(){ return fMomentum; }
  inline TVector3  GetPosition(){ return fPosition; }

  void Print();
  
  ClassDef(JSFJ4CALHit, 1)  // JSFJ4CALHit class

};

#endif










