///////////////////////////////////////////////////////////////////
//
//  CT Hit class in ROOT
//
//$Id$
//
//////////////////////////////////////////////////////////////////

#include "G4Tubs.hh"
#include "J4CTLayerHit.hh"
#include "JSFJ4CTHit.h"
using namespace std;

ClassImp(JSFJ4CTHit)

// ------------------------------------------------------------------
JSFJ4CTHit::JSFJ4CTHit():
  fLayerNo(0), fWireNo(0), fCellID(0), fTrackID(0), fUniqueHitID(0),
  fMotherTrackID(0), fPDGParticleID(0), 
  fDeltaPhiAngle(0.0), fStartPhiAngle(0.0),
  fDriftLength(0.0),
  fPrePosition(0.0, 0.0, 0.0), fPostPosition(0.0, 0.0, 0.0),
  fHitPosition(0.0, 0.0, 0.0), fMomentum(0.0, 0.0, 0.0), 
  fEnergyDeposit(0.0), fTof(0.0)
{
}

// ------------------------------------------------------------------
JSFJ4CTHit::JSFJ4CTHit(J4CTLayerHit *ahit)
{

  J4VComponent *vc=ahit->GetComponent();

  fWireNo=vc->GetMyID();
  fLayerNo=vc->GetMother()->GetMother()->GetMyID();

  fPDGParticleID = ahit->GetParticle()->GetPDGEncoding();   
  

  fDeltaPhiAngle = ((G4Tubs*)vc->GetSolid())->GetDeltaPhiAngle();
  fStartPhiAngle = ((G4Tubs*)vc->GetSolid())->GetStartPhiAngle();

  G4ThreeVector pos  = ahit->GetPrePosition();

  Double_t ghphi    = pos.phi();
  while (ghphi < 0)      ghphi += 2*TMath::Pi();
  while (ghphi > 2*M_PI) ghphi -= 2*TMath::Pi();   
  Double_t cellphi = ghphi - fStartPhiAngle;
  while(cellphi < 0)      cellphi += 2*TMath::Pi();
  while(cellphi > 2*TMath::Pi()) cellphi -= 2*TMath::Pi();   
  fCellID  = (Int_t)(cellphi/fDeltaPhiAngle);
		
  fPrePosition=TVector3( ahit->GetPrePosition().x()/cm, ahit->GetPrePosition().y()/cm,
			 ahit->GetPrePosition().z()/cm);
  fPostPosition=TVector3( ahit->GetPostPosition().x()/cm, ahit->GetPostPosition().y()/cm,
			  ahit->GetPostPosition().z()/cm);
  fHitPosition=TVector3( pos.x()/cm, pos.y()/cm, 
			  pos.z()/cm);
  fMomentum=TVector3( ahit->GetMomentum().x()/GeV, ahit->GetMomentum().y()/GeV,
		      ahit->GetMomentum().z()/GeV);
  fEnergyDeposit=ahit->GetEnergyDeposit();
  fTof=ahit->GetTof();	


}


// ------------------------------------------------------------------
JSFJ4CTHit::~JSFJ4CTHit()
{
}

// ------------------------------------------------------------------
void JSFJ4CTHit::Print()
{
  cout << " Layer no=" << GetLayerNo();
  cout << " Wire no=" << GetWireNo();
  cout << " Cell ID=" << GetCellID();
  cout << " Track ID=" << GetTrackID();
  cout << " Unique Hit ID=" << GetUniqueHitID();
  cout << " Mother Track ID=" << GetMotherTrackID();
  cout << " PDG Track ID=" << GetPDGParticleID()  << endl;

  cout << " DeltaPhiAngle=" << GetDeltaPhiAngle() ;
  cout << " StartPhiAngle=" << GetStartPhiAngle() ;
  cout << " DriftLength=" << GetDriftLength() ;
  cout << " EnergyDeposit=" << GetEnergyDeposit() ;
  cout << " Tof=" << GetTof() ;
  cout << endl;

  cout << "PrePosition(" << GetPrePosition().x() ;
  cout << "," << GetPrePosition().y() << "," << GetPrePosition().z() << ")" ;
  cout << "PostPosition(" << GetPostPosition().x() ;
  cout << "," << GetPostPosition().y() << "," << GetPostPosition().z() << ")" ;
  cout << endl;
  //  cout << "HitPosition(" << GetHitPosition().x() ;
  //  cout << "," << GetHitPosition().y() << "," << GetHitPosition().z() << ")" ;
  cout << "Momentum(" << GetMomentum().x() ;
  cout << "," << GetMomentum().y() << "," << GetMomentum().z() << ")" ;
  cout << endl;
  

}
