//*LastUpdate : v.01.01 undefined by undefined
//*-- Author  : undefined undefined

///////////////////////////////////////////////////////////////////
//
//  EventSelection
//
//  Sample program to make an event selection
//  Analysis of rest of the moduels are skiped based on 
//  the selection result.
//
//$Id$
//  
//////////////////////////////////////////////////////////////////


#include "JSFSteer.h"
#include "JSFGenerator.h"
#include "EventSelection.h"


ClassImp(EventSelection)

//_____________________________________________________________________________
EventSelection::EventSelection(const char *name, const char *title)
       : JSFModule(name,title)
{

}

//_____________________________________________________________________________
EventSelection::~EventSelection()
{
}


//_____________________________________________________________________________
Bool_t EventSelection::Initialize()
{
  fNcall=0;
  fNgood=0;
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t EventSelection::Terminate()
{
  printf("End of EventSelection module\n");
  printf("  Number of analized events = %d\n",fNcall);
  printf("  Number of good events     = %d\n",fNgood);
  printf("  Fraction of good event is = %g\n",(Float_t)fNgood/(Float_t)fNcall);
  
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t EventSelection::Process(Int_t nev)
{
// 

  JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *gb=(JSFGeneratorBuf*)gen->EventBuf();
  TClonesArray *gps=gb->GetParticles();

  fNcall++;
  // Here we select WW->l nu + 2 jets event

  Int_t nq=0;
  Int_t nl=0;
  Int_t nn=0;
  Int_t ne=0;
  Int_t nmu=0;
  Int_t ntau=0;
  for(Int_t i=0;i<gb->GetNparticles();i++){
    JSFGeneratorParticle *gp=(JSFGeneratorParticle*)gps->UncheckedAt(i);
    if( gp->GetMother() < 0 ) {
      Int_t ipm=TMath::Abs(gp->GetMother())-1;
      JSFGeneratorParticle *gpm=(JSFGeneratorParticle*)gps->UncheckedAt(ipm);
      if( TMath::Abs(gpm->GetID()) == 24 ) {
	// This particles is a decay daughter from W
	Int_t idp=TMath::Abs(gp->GetID());
	if( idp < 10 ) { nq++; }
	else if ( idp == 11 ) { ne++; }
	else if ( idp == 13 ) { nmu++; }
	else if ( idp == 15 ) { ntau++; }
	else if ( idp ==12 || idp==14 || idp==16 ) { nn++; }
	else {
	  printf(" Invalid particle ID was found \n");
	}
      }
    }
  }

  nl=ne+nmu+ntau;
  printf(" Event type is %d %d %d %d %d %d\n",nq, ne, nmu, ntau, nl, nn);

  if( nq == 2 && nl == 1 && nn == 1 ) {
    fNgood++;
    return kTRUE;
  }
  
  gJSF->SetReturnCode(JSFSteer::kJSFSkipRestModules);
  
  return kTRUE;

}

