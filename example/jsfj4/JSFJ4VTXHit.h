#ifndef __JSFJ4VTXHit__
#define __JSFJ4VTXHit__

///////////////////////////////////////////////////////////////////////
//
//  JSFJ4 VTX Hit data
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TVector3.h>

class J4VTXPixelHit; 

// *******************************************************
class JSFJ4VTXHit : public TObject
{
 protected:
  Int_t      fTrackID;       // Serial Number of G4 particle
  Int_t      fParticleID;    // Particle ID
  Short_t    fLayerID;       // Layer ID of Hit
  Short_t    fLadderID;      // Lader ID
  Short_t    fSensorID;      // Sensor ID;
  Short_t    fPixelThetaID;  // Theta ID of pixel;
  Short_t    fPixelPhiID;    // PhiID of pixel;
  TVector3   fMomentum;      // Momentum of particle (GeV)
  TVector3   fInPosition;    // Input Position (cm)
  Double_t   fEnergyDeposit; // in eV

 public:
  JSFJ4VTXHit();
  JSFJ4VTXHit(J4VTXPixelHit *ahit);
  virtual ~JSFJ4VTXHit();

  inline Int_t GetTrackID(){ return fTrackID; }
  inline Int_t GetParticleID(){ return fParticleID; }
  inline Int_t GetLayerID(){ return (Int_t)fLayerID; }
  inline Int_t GetLadderID(){ return (Int_t)fLadderID; }
  inline Int_t GetSensorID(){ return (Int_t)fSensorID; }
  inline Int_t GetPixelThetaID(){ return (Int_t)fPixelThetaID; }
  inline Int_t GetPixelPhiID(){ return (Int_t)fPixelPhiID; }
  inline TVector3  GetMomentum(){ return fMomentum; }
  inline TVector3  GetInPosition(){ return fInPosition; }
  inline Double_t  GetEnergyDeposit(){ return fEnergyDeposit; }

  void Print();
  
  ClassDef(JSFJ4VTXHit, 1)  // JSFJ4VTXHit class

};

#endif










