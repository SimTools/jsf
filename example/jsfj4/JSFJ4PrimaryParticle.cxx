///////////////////////////////////////////////////////////////////
//
//  JSFJ4 Primary particle class
//
//$Id$
//
//////////////////////////////////////////////////////////////////


#include "JSFJ4PrimaryParticle.h"
#include "G4PrimaryParticle.hh"


ClassImp(JSFJ4PrimaryParticle)

// ------------------------------------------------------------------
JSFJ4PrimaryParticle::JSFJ4PrimaryParticle():
  fTrackID(0), fParticleID(0), fCharge(0.0), fMass(0.0),
  fMomentum(0.0, 0.0, 0.0), fPolarization(0.0, 0.0, 0.0),
  fWeight(0.0),  fProperTime(0.0),
  fIndex2GeneratorParticle(0), fDaughters(0), fGeneratorParticle(0)
{
}

// ------------------------------------------------------------------
JSFJ4PrimaryParticle::JSFJ4PrimaryParticle(G4PrimaryParticle *p,
		   Int_t index, JSFGeneratorParticle *gp):
  fTrackID(0), fParticleID(0), fCharge(0.0), fMass(0.0),
  fMomentum(0.0, 0.0, 0.0), fPolarization(0.0, 0.0, 0.0),
  fWeight(0.0),  fProperTime(0.0),
  fIndex2GeneratorParticle(index), fDaughters(0), fGeneratorParticle(gp)
{
  fDaughters=new TObjArray();
  SetParticle(p);
}

// ------------------------------------------------------------------
void JSFJ4PrimaryParticle::SetParticle(G4PrimaryParticle *p)
{  
  fTrackID=p->GetTrackID();
  fParticleID=p->GetPDGcode();
  fCharge=p->GetCharge();
  fMass=p->GetMass()/GeV;
  fMomentum=TVector3(p->GetPx()/GeV, p->GetPy()/GeV, p->GetPz()/GeV);
  fPolarization=TVector3(p->GetPolX(), p->GetPolY(), p->GetPolZ());
  fWeight=p->GetWeight();
  fProperTime=p->GetProperTime()/ns;
}

// ------------------------------------------------------------------
JSFJ4PrimaryParticle::~JSFJ4PrimaryParticle()
{
  if( fDaughters ) {
    fDaughters->Delete();
    delete fDaughters;
  }
}


// ------------------------------------------------------------------
void JSFJ4PrimaryParticle::Print()
{
  cout << " Track ID " << GetTrackID();
  cout << " PDG ID " << GetParticleID();
  cout << " Charge " << GetCharge();
  cout << " Mass " << GetMass();

  cout << " Weight=" << GetWeight();
  cout << " ProperTime=" << GetProperTime();
  cout << " Index2GenP=" << GetIndex2GeneratorParticle();
  if( fDaughters ) {
    cout << " #of daughters=" << GetDaughters()->GetEntries() ;
  }
  else {
    cout << "No daughter particles ";
  }
  cout << endl;

  cout << " Momentum(" << GetMomentum().X();
  cout << "," << GetMomentum().Y() << "," << GetMomentum().Z() << ")" ;
  cout << " Polarization(" << GetPolarization().X();
  cout << "," << GetPolarization().Y() << "," << GetPolarization().Z() << ")" ;
  cout << endl;


}
