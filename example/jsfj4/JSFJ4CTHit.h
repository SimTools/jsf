#ifndef __JSFJ4CTHit__
#define __JSFJ4CTHit__

///////////////////////////////////////////////////////////////////////
//
//  JSFJ4 CT Hit data
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TVector3.h>

class J4CTLayerHit; 

// *******************************************************
class JSFJ4CTHit : public TObject
{
 protected:
  Short_t fLayerNo;       // Layer number
  Short_t fWireNo;        // Wire number
  Short_t fCellID;        // CellId
  Int_t   fTrackID;       // TrackId
  Int_t   fUniqueHitID;   // UniqueHitId
  Int_t   fMotherTrackID; // Mother Track Id
  Int_t   fPDGParticleID; // PDGParticleID
  
  Double_t fDeltaPhiAngle; // DeletaPhiAngle
  Double_t fStartPhiAngle; //

  Double_t fDriftLength;
  TVector3 fPrePosition;
  TVector3 fPostPosition;
  TVector3 fHitPosition;
  TVector3 fMomentum;
  Double_t fEnergyDeposit;
  Double_t fTof;

 public:
  JSFJ4CTHit();
  JSFJ4CTHit(J4CTLayerHit *ahit);
  virtual ~JSFJ4CTHit();

  void Print();

  inline Int_t GetLayerNo(){ return (Int_t)fLayerNo; }
  inline Int_t GetWireNo(){ return (Int_t)fWireNo; }
  inline Int_t GetCellID(){ return (Int_t)fCellID; }
  inline Int_t GetTrackID(){ return fTrackID; }
  inline Int_t GetUniqueHitID(){ return fUniqueHitID; }
  inline Int_t GetMotherTrackID(){ return fMotherTrackID; }
  inline Int_t GetPDGParticleID(){ return fPDGParticleID; }

  inline Double_t GetDeltaPhiAngle(){ return fDeltaPhiAngle; }
  inline Double_t GetStartPhiAngle(){ return fStartPhiAngle; }
  inline Double_t GetDriftLength(){ return fDriftLength; }

  inline TVector3 GetPrePosition(){ return fPrePosition; }
  inline TVector3 GetPostPosition(){ return fPostPosition; }
  inline TVector3 GetHitPosition(){ return fHitPosition; }
  inline TVector3 GetMomentum(){ return fMomentum; }

  inline Double_t GetEnergyDeposit(){ return fEnergyDeposit; }
  inline Double_t GetTof(){ return fTof; }
  
  ClassDef(JSFJ4CTHit, 1)  // JSFJ4CTHit class

};

#endif










