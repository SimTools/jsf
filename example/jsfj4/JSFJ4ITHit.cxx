///////////////////////////////////////////////////////////////////
//
//  IT Hit class in ROOT
//
//$Id$
//
//////////////////////////////////////////////////////////////////


#include "J4ITLayerHit.hh"
#include "JSFJ4ITHit.h"
using namespace std;

ClassImp(JSFJ4ITHit)

// ------------------------------------------------------------------
JSFJ4ITHit::JSFJ4ITHit():
  fTrackID(0), fMotherTrackID(0), fParticleID(0), 
  fLayerID(0), fLadderID(0), fSensorID(0),
  fPixelThetaID(0), fPixelPhiID(0), fMomentum(0.0, 0.0, 0.0), 
  fPosition(0.0, 0.0, 0.0), fEnergyDeposit(0.0)
{
}

// ------------------------------------------------------------------
JSFJ4ITHit::JSFJ4ITHit(J4ITLayerHit *ahit)
{
  fTrackID=ahit->GetTrackID();
  fMotherTrackID=ahit->GetMotherTrackID();
  fParticleID=ahit->GetPDGEncoding();
  fLayerID=ahit->GetComponent()->GetMyID();
  fLadderID=0;
  fSensorID=0;
  fPixelThetaID=0;
  fPixelPhiID=0;
 
  fMomentum=TVector3(ahit->GetMomentum().x()/GeV, ahit->GetMomentum().y()/GeV, ahit->GetMomentum().z()/GeV);
  fPosition=TVector3(ahit->GetPrePosition().x()/cm, ahit->GetPrePosition().y()/cm, ahit->GetPrePosition().z()/cm);
  fEnergyDeposit=ahit->GetEnergyDeposit()/eV;

}

// ------------------------------------------------------------------
JSFJ4ITHit::~JSFJ4ITHit()
{
}

// ------------------------------------------------------------------
void JSFJ4ITHit::Print()
{

  cout << "Track id is " << GetTrackID() ;
  cout << "MotherTrack ID is " << GetMotherTrackID() ;
  cout << "PDG ID is " << GetParticleID() ;
  cout << " Layer id is " << GetLayerID();
  cout << " Ladder id is " << GetLadderID();
  cout << " Sensor id is " << GetSensorID();
  cout << " PixelTheta  id is " << GetPixelThetaID();
  cout << " PixelPhi id is " << GetPixelPhiID();
  cout << " EDeposit is " << GetEnergyDeposit() << " eV" << endl ;
  cout << " PMomentum(GeV) " << GetMomentum().X();
  cout << "," << GetMomentum().Y();
  cout << "," << GetMomentum().Z();

  cout << " InPostion(cm) is " << GetPosition().X();
  cout << "," << GetPosition().Y();
  cout << "," << GetPosition().Z();
  //  cout << " OutPostion(cm) is " <<GetOutPosition()/cm;
  cout << endl;

}
