///////////////////////////////////////////////////////////////////
//
//  Interface to JSFGenerator to Geant4/Jupiter generator
//
//
//$Id$
//
//////////////////////////////////////////////////////////////////

#include <stack>

#include "G4Event.hh"
#include "G4HEPEvtParticle.hh"
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"


#include "JSFJ4PrimaryGeneratorAction.h"  
#include "JSFSteer.h" 
#include "JSFJ4.h"
#include "JSFJ4PrimaryParticle.h"
#include "JSFJ4PrimaryVertex.h"
#include "JSFGenerator.h" 

//_____________________________________________________________________________
void JSFJ4PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) 
{

  // Jupiter event is generated
  if( fUseJupiterGenerator ) {
    J4PrimaryGeneratorAction::GeneratePrimaries(anEvent);
  }
  else {
    LoadJSFGenerator(anEvent);
  }

  return ;

}

// ______________________________________________________________________
void JSFJ4PrimaryGeneratorAction::SavePrimaries(G4Event *anEvent)
{

  GetEventBuf()->Print();

  TObjArray    *vertices=(TObjArray*)GetEventBuf()->GetComponent("PrimaryVertices");

  vertices->Clear();   vertices->Compress();
  Int_t iserv=0;
  for(Int_t iv=0;iv<anEvent->GetNumberOfPrimaryVertex();iv++){
    G4PrimaryVertex *v4=anEvent->GetPrimaryVertex(iv);
    JSFJ4PrimaryVertex *v=new JSFJ4PrimaryVertex(v4, iserv++);

    G4PrimaryParticle *p4=v4->GetPrimary();
    stack<G4PrimaryParticle*>  p4mother;
    stack<JSFJ4PrimaryParticle*> jsfj4mother;

    while( p4 != 0 ) {
      JSFJ4PrimaryParticle *p=new JSFJ4PrimaryParticle(p4);
      v->Add(p);
      p->SetPrimaryVertex(v);
      G4PrimaryParticle *p4dau=p4->GetDaughter();
      if( !p4mother.empty() ) { (jsfj4mother.top())->AddDaughter(p); }
      if( p4dau != 0 ) {
	p4mother.push(p4);
	jsfj4mother.push(p);
	p4=p4dau;
      }
      else {
	p4=p4->GetNext();
	if( p4 == 0 && !p4mother.empty()) {
	  p4=p4mother.top();
	  p4mother.pop();
	  jsfj4mother.pop();
	  p4=p4->GetNext();
	}
      }
    }
    vertices->Add(v);

  }
}

// -----------------------------------------------------------
void JSFJ4PrimaryGeneratorAction::LoadJSFGenerator(G4Event *anEvent)
{

  // JSF Generator event is generated.
  JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *gbuf=(JSFGeneratorBuf*)gen->EventBuf();
  TClonesArray    *gps=gbuf->GetParticles();

  vector<G4HEPEvtParticle*> hplist;
 
  Int_t nskip=0;
  for(Int_t ig=0;ig<gbuf->GetNparticles();ig++) {
    JSFGeneratorParticle *gp=(JSFGeneratorParticle*)gps->At(ig);
    G4int mother=gp->GetMother();

    if( mother <= 0 ) {
      nskip++;
      continue;
    }

    G4int ndau=gp->GetNDaughter();
    G4int status=1;
    G4int firstdaughter=0;
    G4int lastdaughter=0;
    if( ndau > 0 ) status=2 ;
    G4int pid=gp->GetID();
    if( ndau > 0 ) {
      firstdaughter=gp->GetFirstDaughter()-nskip;
      lastdaughter=firstdaughter + gp->GetNDaughter() - 1;
    }

    G4double px=gp->GetPx()*GeV;
    G4double py=gp->GetPy()*GeV;
    G4double pz=gp->GetPz()*GeV;
    G4double mass=gp->GetMass()*GeV;

    G4PrimaryParticle *particle=new G4PrimaryParticle(pid, px,py,pz);
    particle->SetMass(mass);
    G4HEPEvtParticle* hepParticle
      = new G4HEPEvtParticle(particle, status, firstdaughter, lastdaughter);
    
    hplist.push_back(hepParticle);
    
  }

  // 
  // make connection between daughter particles decayed from 
  // the same mother
  for( size_t i=0; i<hplist.size(); i++ )
  {

    if( hplist[i]->GetJDAHEP1() > 0 ) //  it has daughters
    {
      G4int jda1 = hplist[i]->GetJDAHEP1()-1; // FORTRAN index starts from 1
      G4int jda2 = hplist[i]->GetJDAHEP2()-1; // but C++ starts from 0.
      G4PrimaryParticle* mother = hplist[i]->GetTheParticle();
      for( G4int j=jda1; j<=jda2; j++ )
      {
        G4PrimaryParticle* daughter = hplist[j]->GetTheParticle();
        if(hplist[j]->GetISTHEP()>0)
        {
          mother->SetDaughter( daughter );
          hplist[j]->Done();
        }
      }
    }
  }

  // create G4PrimaryVertex object
  G4ThreeVector ip_position;
  G4double ip_time = 0.0;

  G4PrimaryVertex* vertex = new G4PrimaryVertex(ip_position, ip_time);

  // put initial particles to the vertex
  for( size_t ii=0; ii<hplist.size(); ii++ )
  {
    if( hplist[ii]->GetISTHEP() > 0 ) // ISTHEP of daughters had been 
                                       // set to negative
    {
      G4PrimaryParticle* initialParticle = hplist[ii]->GetTheParticle();
      vertex->SetPrimary( initialParticle );
    }
  }

  // clear G4HEPEvtParticles
  //HPlist.clearAndDestroy();
  for(size_t iii=0;iii<hplist.size();iii++)  { delete hplist[iii]; }
  hplist.clear();

  // Put the vertex to G4Event object
  anEvent->AddPrimaryVertex( vertex );

}
