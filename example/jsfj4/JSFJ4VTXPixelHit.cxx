///////////////////////////////////////////////////////////////////
//
//  VTX Hit class in ROOT
//
//$Id$
//
//////////////////////////////////////////////////////////////////


#include "J4VTXPixelHit.hh"
#include "JSFJ4VTXPixelHit.h"


ClassImp(JSFJ4VTXPixelHit)

// ------------------------------------------------------------------
JSFJ4VTXPixelHit::JSFJ4VTXPixelHit():
  fTrackID(0), fParticleID(0), fLayerID(0), fLadderID(0), fSensorID(0),
  fPixelThetaID(0), fPixelPhiID(0), fMomentum(0.0, 0.0, 0.0), 
  fInPosition(0.0, 0.0, 0.0), fEnergyDeposit(0.0)
{
}

// ------------------------------------------------------------------
JSFJ4VTXPixelHit::JSFJ4VTXPixelHit(J4VTXPixelHit *ahit)
{
  fTrackID=ahit->GetTrackID();
  fParticleID=ahit->GetPID()->GetPDGEncoding();
  fLayerID=ahit->GetLayerID();
  fLadderID=ahit->GetLadderID();
  fSensorID=ahit->GetSensorID();
  fPixelThetaID=ahit->GetPixThetaID();
  fPixelPhiID=ahit->GetPixPhiID();
 
  fMomentum=TVector3(ahit->GetMomentum().x()/GeV, ahit->GetMomentum().y()/GeV, ahit->GetMomentum().z()/GeV);
  fInPosition=TVector3(ahit->GetInPosition().x()/cm, ahit->GetInPosition().y()/cm, ahit->GetInPosition().z()/cm);
  fEnergyDeposit=ahit->GetEnergyDeposit()/eV;

}

// ------------------------------------------------------------------
JSFJ4VTXPixelHit::~JSFJ4VTXPixelHit()
{
}

// ------------------------------------------------------------------
void JSFJ4VTXPixelHit::Print()
{

  cout << "Track id is " << fTrackID ;

  /*
  cout << " Layer id is " << GetLayerID();
  cout << " Ladder id is " << GetLadderID();
  cout << " Sensor id is " << GetSensorID();
  cout << " PixelTheta  id is " << GetPixelThetaID();
  cout << " PixelPhi id is " << GetPixelPhiID();
  cout << " EDeposit is " << GetEnergyDeposit() << " eV" << endl ;
  cout << " PMomentum(GeV) " << GetMomentum().X();
  cout << "," << GetMomentum().Y();
  cout << "," << GetMomentum().Z();

  cout << " InPostion(cm) is " << GetInPosition().X();
  cout << "," << GetInPosition().Y();
  cout << "," << GetInPosition().Z();
  //  cout << " OutPostion(cm) is " <<GetOutPosition()/cm;
  cout << endl;
  */

}
