///////////////////////////////////////////////////////////////////
//
//  CAL Hit class in ROOT
//
//$Id$
//
//////////////////////////////////////////////////////////////////


#include "J4CALHit.hh"
#include "JSFJ4CALHit.h"
using namespace std;

ClassImp(JSFJ4CALHit)

// ------------------------------------------------------------------
JSFJ4CALHit::JSFJ4CALHit():
  fTrackID(0), fMotherTrackID(0), fParticleID(0), 
  fThetaNumber(0), fPhiNumber(0), fIsBarrel(kTRUE), fIsEM(kTRUE),
  fEnergyDeposit(0.0), fTof(0), fPosition(0.0, 0.0, 0.0)
{
}

// ------------------------------------------------------------------
JSFJ4CALHit::JSFJ4CALHit(J4CALHit *ahit)
{
  fTrackID=ahit->GetTrackID();
  fMotherTrackID=ahit->GetMotherTrackID();
  fParticleID=ahit->GetParticle()->GetPDGEncoding();

  fThetaNumber=ahit->GetThetaNumber();
  fPhiNumber=ahit->GetPhiNumber();
  fIsBarrel=ahit->IsBarrel();
  fIsEM=ahit->IsEM();
  fTof=ahit->GetTof()/nanosecond;

  //  fMomentum=TVector3(ahit->GetMomentum().x()/GeV, ahit->GetMomentum().y()/GeV, ahit->GetMomentum().z()/GeV);
  fPosition=TVector3(ahit->GetInjectionPoint().x()/cm, ahit->GetInjectionPoint().y()/cm, 
		     ahit->GetInjectionPoint().z()/cm);
 
  fEnergyDeposit=ahit->GetEnergyDeposit()/GeV;

}

// ------------------------------------------------------------------
JSFJ4CALHit::~JSFJ4CALHit()
{
}

// ------------------------------------------------------------------
void JSFJ4CALHit::Print()
{

  cout << " Track id is " << GetTrackID() ;
  cout << " MotherTrack ID is " << GetMotherTrackID() ;
  cout << " PDG ID is " << GetParticleID() ;

  cout << " (Theta#,Phi#)=(" << GetThetaNumber() << "," << GetPhiNumber() << ")" ;
  cout << " IsBarrel=" << IsBarrel();
  cout << " IsEM=" << IsEM();
  cout << " Tof=" << GetTof() << "(nsec) " ;

  cout << " EnergySum is " << GetEnergyDeposit() << " GeV" << endl ;

  /*
  cout << " PMomentum(GeV) " << GetMomentum().X();
  cout << "," << GetMomentum().Y();
  cout << "," << GetMomentum().Z();
  */

  cout << " InPostion(cm) is (" << GetPosition().X();
  cout << "," << GetPosition().Y();
  cout << "," << GetPosition().Z();
  cout << ")" ;
  //  cout << " OutPostion(cm) is " <<GetOutPosition()/cm;
  cout << endl;

}
