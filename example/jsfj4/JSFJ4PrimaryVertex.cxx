///////////////////////////////////////////////////////////////////
//
//  JSFJ4 Primary vertex class
//
//$Id$
//
//////////////////////////////////////////////////////////////////


#include "JSFJ4PrimaryParticle.h"
#include "JSFJ4PrimaryVertex.h"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
using namespace std;


ClassImp(JSFJ4PrimaryVertex)

// ------------------------------------------------------------------
JSFJ4PrimaryVertex::JSFJ4PrimaryVertex():
  fSerialNo(0), fT0(0), fWeight(0), fPosition(0.0, 0.0, 0.0),
  fParticles(0), fNextVertex(0)
{
}

// ------------------------------------------------------------------
JSFJ4PrimaryVertex::JSFJ4PrimaryVertex(G4PrimaryVertex *v, Int_t ser):
  fSerialNo(ser), fT0(0), fWeight(0), fPosition(0.0, 0.0, 0.0),
  fParticles(0), fNextVertex(0)
{
  fT0=v->GetT0();
  fWeight=v->GetWeight();
  fPosition=TVector3(v->GetX0(), v->GetY0(), v->GetZ0());

  fParticles = new TObjArray() ;

}

// ------------------------------------------------------------------
JSFJ4PrimaryVertex::~JSFJ4PrimaryVertex()
{
  if( fParticles ) {
     delete fParticles;
  }
}

// ------------------------------------------------------------------
void JSFJ4PrimaryVertex::Print(Option_t *opt)
{
  cout << " Position(" << GetPosition().X();
  cout << "," << GetPosition().Y() << "," << GetPosition().Z() << ")" ;
  
  cout << " SerialNo=" << GetSerialNo();
  cout << " T0=" << GetT0() ;
  cout << " Weight=" << GetWeight();
  cout << " Number of particles =" << GetNumberOfParticles();
  cout << endl;

  if( strcmp(opt,"") != 0 ) {
    for(Int_t ip=0;ip<fParticles->GetEntries();ip++){
      JSFJ4PrimaryParticle *p=(JSFJ4PrimaryParticle*)fParticles->UncheckedAt(ip);
      p->Print();
    }
  }

}
