///////////////////////////////////////////////////////////////////
//
//  CDC Hit class in ROOT
//
//$Id$
//
//////////////////////////////////////////////////////////////////

#include "G4Tubs.hh"
#include "J4CDCDriftRegionHit.hh"
#include "JSFJ4CDCHit.h"


ClassImp(JSFJ4CDCHit)

// ------------------------------------------------------------------
JSFJ4CDCHit::JSFJ4CDCHit():
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
JSFJ4CDCHit::JSFJ4CDCHit(J4CDCDriftRegionHit *ahit)
{

  J4VComponent *vc=ahit->GetComponent();

  fWireNo=vc->GetMyID();
  fLayerNo=vc->GetMother()->GetMother()->GetMyID();

  fPDGParticleID = ahit->GetParticle()->GetPDGEncoding();   
  

  fDeltaPhiAngle = ((G4Tubs*)vc->GetSolid())->GetDeltaPhiAngle();
  fStartPhiAngle = ((G4Tubs*)vc->GetSolid())->GetStartPhiAngle();
  
  Double_t ghphi    = ahit->GetHitPosition().phi();
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
  fHitPosition=TVector3( ahit->GetHitPosition().x()/cm, ahit->GetHitPosition().y()/cm,
			  ahit->GetHitPosition().z()/cm);
  fMomentum=TVector3( ahit->GetMomentum().x()/GeV, ahit->GetMomentum().y()/GeV,
		      ahit->GetMomentum().z()/GeV);
  fEnergyDeposit=ahit->GetEnergyDeposit();
  fTof=ahit->GetTof();	

}


// ------------------------------------------------------------------
JSFJ4CDCHit::~JSFJ4CDCHit()
{
}

// ------------------------------------------------------------------
void JSFJ4CDCHit::Print()
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
  cout << "HitPosition(" << GetHitPosition().x() ;
  cout << "," << GetHitPosition().y() << "," << GetHitPosition().z() << ")" ;
  cout << "Momentum(" << GetMomentum().x() ;
  cout << "," << GetMomentum().y() << "," << GetMomentum().z() << ")" ;
  cout << endl;
  

}
