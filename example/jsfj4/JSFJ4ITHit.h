#ifndef __JSFJ4ITHit__
#define __JSFJ4ITHit__

///////////////////////////////////////////////////////////////////////
//
//  JSFJ4 IT Hit data
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TVector3.h>

class J4ITLayerHit;

// *******************************************************
class JSFJ4ITHit : public TObject
{
 protected:
  Int_t      fTrackID;       // Serial Number of G4 particle
  Int_t      fMotherTrackID; // Mother track ID.
  Int_t      fParticleID;    // Particle ID
  Short_t    fLayerID;       // Layer ID of Hit
  Short_t    fLadderID;      // Lader ID
  Short_t    fSensorID;      // Sensor ID;
  Short_t    fPixelThetaID;  // Theta ID of pixel;
  Short_t    fPixelPhiID;    // PhiID of pixel;
  TVector3   fMomentum;      // Momentum of particle (GeV)
  TVector3   fPosition;      // Input Position (cm)
  Double_t   fEnergyDeposit; // in eV

 public:
  JSFJ4ITHit();
  JSFJ4ITHit(J4ITLayerHit *ahit);
  virtual ~JSFJ4ITHit();

  inline Int_t GetTrackID(){ return fTrackID; }
  inline Int_t GetMotherTrackID(){ return fMotherTrackID; }
  inline Int_t GetParticleID(){ return fParticleID; }
  inline Int_t GetLayerID(){ return (Int_t)fLayerID; }
  inline Int_t GetLadderID(){ return (Int_t)fLadderID; }
  inline Int_t GetSensorID(){ return (Int_t)fSensorID; }
  inline Int_t GetPixelThetaID(){ return (Int_t)fPixelThetaID; }
  inline Int_t GetPixelPhiID(){ return (Int_t)fPixelPhiID; }
  inline TVector3  GetMomentum(){ return fMomentum; }
  inline TVector3  GetPosition(){ return fPosition; }
  inline Double_t  GetEnergyDeposit(){ return fEnergyDeposit; }

  void Print();
  
  ClassDef(JSFJ4ITHit, 1)  // JSFJ4ITHit class

};

#endif










