#ifndef __JSFJ4CDCHit__
#define __JSFJ4CDCHit__

///////////////////////////////////////////////////////////////////////
//
//  JSFJ4 CDC Hit data
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TVector3.h>

class J4CDCDriftRegionHit; 

// *******************************************************
class JSFJ4CDCHit : public TObject
{
 protected:
  Short_t fLayerNo;
  Short_t fWireNo;
  Short_t fCellId;
  Int_t   fTrackId;
  Int_t   fUniqueHitId;
  Int_t   fMotherTrackId;
  Int_t   fParticlePDGId;
  




     ofs << setw(1) << layerNo << " " << wireNo << " " << setw(3) << cellID << " "
         << setw(5) << GetTrackID() << " " << GetMotherTrackID() << " " << GetHitNumber()
         << setw(6) << pdid << " " << setw(2) << GetCharge()
         << " " << dphi << " " << setw(10) << sphi << " "
         << setw(10) << gwphi << " " << setw(10) << rw << " " << setw(10) << driftlen << " "
         << pre.x() << " " << pre.y() << " " << pre.z() << " "
         << post.x() << " " << post.y() << " " << post.z() << " "
         << fHitPosition.x() << " " << fHitPosition.y() << " " << fHitPosition.z() << " "
         << GetMomentum().x() << " " << GetMomentum().y() << " "
         << GetMomentum().z() << " " << GetTotalEnergy() << " "
         << GetEnergyDeposit() << " " << GetTof() << G4endl;


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
  JSFJ4VTXPixelHit();
  JSFJ4VTXPixelHit(J4VTXPixelHit *ahit);
  virtual ~JSFJ4VTXPixelHit();

  inline Int_t GetTrackId(){ return fTrackID; }
  inline Int_t GetParticleId(){ return fParticleID; }
  inline Int_t GetLayerId(){ return (Int_t)fLayerID; }
  inline Int_t GetLadderId(){ return (Int_t)fLadderID; }
  inline Int_t GetSensorId(){ return (Int_t)fSensorID; }
  inline Int_t GetPixelThetaId(){ return (Int_t)fPixelThetaID; }
  inline Int_t GetPixelPhiId(){ return (Int_t)fPixelPhiID; }
  inline TVector3  GetMomentum(){ return fMomentum; }
  inline TVector3  GetInPosition(){ return fInPosition; }
  inline Double_t  GetEnergyDeposit(){ return fEnergyDeposit; }

  void Print();
  
  ClassDef(JSFJ4VTXPixelHit, 1)  // JSFJ4VTXPixelHit class

};

#endif










