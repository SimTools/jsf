///////////////////////////////////////////////////////////////////
//
//  A sample program for Selection of StdHep event.
//  Only events which passed the criteria are passed to the 
//  subsequent module.
//  
//////////////////////////////////////////////////////////////////

#include <iostream>

#include "JSFSteer.h"
#include "MySelection.h"
#include "JSFGenerator.h"
#include "ANL4DVector.h"
#include "JSFWriteStdHep.h"
#include "JSFStdHepStdCM.h"

#include "TNtuple.h"
#include "TH1F.h"

ClassImp(MySelection)

//_____________________________________________________________________________
  MySelection::MySelection(const char *name, const char *title)
       : JSFModule(name,title)
{

}

//_____________________________________________________________________________
MySelection::~MySelection()
{
}

//_____________________________________________________________________________
Bool_t MySelection::Initialize()
{
  // Initialize : define Ntuple
  fNtuple=new TNtuple("fNtuple","Analysis","pt:pz:evis:nch");
  
  fWriteBeginRun=kTRUE;

  return kTRUE;
}

//____________________________________________________________________________
void MySelection::MakeBeginRunInfo()
{  
  JSFWriteStdHep *w=(JSFWriteStdHep*)gJSF->FindModule("JSFWriteStdHep");
  JSFStdHepStdCM *wstdcm=w->GetStdHepCM();
  wstdcm->LoadStdHepCM();
  wstdcm->SetGeneratorName("MyPreselection");  // Generator Name should be < 19
}


//_____________________________________________________________________________
Bool_t MySelection::Process(Int_t nev)
{
// 
//  stdhep=(JSFReadStdHep*)gJSF->FindModule("JSFReadStdHep");
//  JSFReadStdHepBuf *buf=(JSFReadStdHepBuf*)stdhep->EventBuf();
//  buf->GetHEPEV4().Print();

  if( fWriteBeginRun ) {
     MakeBeginRunInfo();
     fWriteBeginRun=kFALSE;
  }

  static TH1F* hcut=0;
  if( hcut == 0 ) {
    hcut=new TH1F("hcut","Cut statistics",10,0,5.0);
  }

  JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *genb=(JSFGeneratorBuf*)gen->EventBuf();
  
  TClonesArray *prts=genb->GetParticles();

// Calculate evis, Pz, ...
  ANL4DVector psum(0.0, 0.0, 0.0, 0.0);
  TIter nextgp(prts);
  Int_t nch=0;

  JSFGeneratorParticle *gp=0;
  while( (gp=(JSFGeneratorParticle*)nextgp() ) ) {
    if ( gp->GetNDaughter() > 0 ) continue;
    Int_t apid=TMath::Abs(gp->GetID());
    if ( apid == 12 || apid == 14 || apid == 16 ) continue;

    if ( TMath::Abs(gp->GetCharge()) > 0.1 ) { nch++; }

    ANL4DVector p(gp->GetE(), gp->GetPx(), gp->GetPy(), gp->GetPz());
    psum += p;

  }
  Double_t evis = psum.E();
  Double_t pt   = psum.Pt();
  Double_t pz   = psum.Pz();

  fNtuple->Fill(evis,pt,pz,(Float_t)nch);
  

  if( evis < 490.0 ) {
    gJSF->SetReturnCode(JSFSteer::kJSFSkipRestModules+JSFSteer::kJSFNoOutput);
    hcut->Fill(2.0);
  }
  else {
    hcut->Fill(1.0);
  }

  return kTRUE;
}



