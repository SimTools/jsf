///////////////////////////////////////////////////////////////////
//
//  VTX Hit class in ROOT
//
//$Id$
//
//////////////////////////////////////////////////////////////////


#include "J4VTXPixelHit.hh"
#include "JSFJ4VTXHit.h"
using namespace std;

ClassImp(JSFJ4VTXHit)

// ------------------------------------------------------------------
JSFJ4VTXHit::JSFJ4VTXHit():
  fTrackID(0), fParticleID(0), fLayerID(0), fLadderID(0), fSensorID(0),
  fPixelThetaID(0), fPixelPhiID(0), fMomentum(0.0, 0.0, 0.0), 
  fInPosition(0.0, 0.0, 0.0), fEnergyDeposit(0.0)
{
}

// ------------------------------------------------------------------
JSFJ4VTXHit::JSFJ4VTXHit(J4VTXPixelHit *ahit)
{
  fTrackID=ahit->GetTrackID();
  fParticleID=ahit->GetPID()->GetPDGEncoding();
  fLayerID=ahit->GetLayerID();
  fLadderID=ahit->GetLadderID();
  fSensorID=ahit->GetSensorID();
  fPixelThetaID=ahit->GetPixThetaID();
  fPixelPhiID=ahit->GetPixPhiID();
 
  fMomentum=TVector3(ahit->GetPMomentum().x()/GeV, ahit->GetPMomentum().y()/GeV, ahit->GetPMomentum().z()/GeV);
  fInPosition=TVector3(ahit->GetInPosition().x()/cm, ahit->GetInPosition().y()/cm, ahit->GetInPosition().z()/cm);
  fEnergyDeposit=ahit->GetEDeposit()/eV;

}

// ------------------------------------------------------------------
JSFJ4VTXHit::~JSFJ4VTXHit()
{
}

// ------------------------------------------------------------------
void JSFJ4VTXHit::Print()
{

  cout << "Track id is " << GetTrackID() ;
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

}
