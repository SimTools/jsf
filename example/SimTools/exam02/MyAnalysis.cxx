///////////////////////////////////////////////////////////////////
//
/*

  Simple Analysis Program.  Does same analysis as exam01, namely
  create a root file, containing the ntuple which contains
    (1) Number of generatorparticles(ngen)
    (2) Total energies of charged tracks(ech), gammas(egam), and neutral hadrons(enh)
        from generator particle information
    (3) Total observed energies(evis) and missing Pt(ptmis).

  To build this program and to run do,
   $ xmkmf -a
   $ make
   $ jsf -b -q --maxevt=100 gui.C

  where --maxevt option defines a number of events to simulate and analize.

*/
//$Id$
//  
//////////////////////////////////////////////////////////////////

#include <iostream>
using namespace std;

#include "JSFSteer.h"
#include "JSFSIMDST.h"
#include "MyAnalysis.h"
#include "ANL4DVector.h"

#include "TNtuple.h"

ClassImp(MyAnalysis)

//_____________________________________________________________________________
  MyAnalysis::MyAnalysis(const char *name, const char *title)
       : JSFModule(name,title)
{

// MyAnalysisBuf is to be created to save event data.

//  fEventBuf = new MyAnalysisBuf("MyAnalysisBuf", 
//	 "MyAnalysis event uffer",this);

}

//_____________________________________________________________________________
MyAnalysis::~MyAnalysis()
{
}

//_____________________________________________________________________________
Bool_t MyAnalysis::Initialize()
{
  // Initialize : define Ntuple

  fNtuple = new TNtuple("ntuple","Event Information",
        "ngen:ech:egam:enh:evis:ptmis");

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t MyAnalysis::Process(Int_t nev)
{
// 
 
  if ( gJSF->GetEventNumber()%100 == 0 ) {
    cerr << "Passed " << gJSF->GetEventNumber() << endl;
  }
 
  JSFSIMDST    *simdst=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();
 
  Double_t ech=0;
  Double_t egam=0;
  Double_t enh=0;
  Double_t ngen=sdb->GetNGeneratorParticles();
 
// Accumulate ech, egam, enh
  TClonesArray *gen=sdb->GetGeneratorParticles();
  TIter nextgp(gen);
  JSFGeneratorParticle *gp=0;
  while( (gp=(JSFGeneratorParticle*)nextgp() ) ) {
    if ( gp->GetNDaughter() > 0 ) continue;
    Int_t apid=TMath::Abs(gp->GetID());
    if ( apid == 12 || apid == 14 || apid == 16 ) continue;
    if ( gp->GetID() == 22 ) {
       egam += gp->GetE();
    }
    else if ( TMath::Abs(gp->GetCharge()) > 0.1 ) {
       ech += gp->GetE();
    }
    else {
       enh += gp->GetE();
    }
  }
                                                                                                                                                                                                   
// Use QuickSim results
  TObjArray *pcols=sdb->GetLTKCLTracks();
  TIter nextsp(pcols);
  JSFLTKCLTrack *sp=0;
  ANL4DVector   psum(0.0, 0.0, 0.0, 0.0);
  while ( (sp=(JSFLTKCLTrack*)nextsp() ) ) {
    ANL4DVector p(sp->GetE(), sp->GetPx(), sp->GetPy(), sp->GetPz());
    psum += p;
  }
  Double_t evis = psum.E();
  Double_t ptmis = psum.GetPt();
                                                                                                   
  fNtuple->Fill(ngen,ech,egam,enh,evis,ptmis);

  return kTRUE;
}



