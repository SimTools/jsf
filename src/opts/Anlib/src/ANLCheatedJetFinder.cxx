//*************************************************************************
//* =============================
//*  ANLCheatedJetFinder Classes
//* =============================
//*
//* (Description)
//*    Jet finder using generator information classes for JLC analysis
//* (Requires)
//*     class ANLTrack
//*     class ANLJet
//*     class ANLJetFinder
//* (Provides)
//*     class ANLTaggedJet
//*     class ANLCheatedJetFinder
//*     class ANLCheatedJadeJetFinder
//*     class ANLCheatedJadeEJetFinder
//*     class ANLCheatedDurhamJetFinder
//* (Usage)
//*     // Example
//*     Double_t ycut = 0.01;
//*     ANLCheatedJadeEJetFinder jclust(ycut);
//*     TIter nexttrk(&tracks);
//*     while ((trkp = (ANLTrack *)nexttrk())) {
//*       trkp->SetColorSingletID();
//*     }
//*     jclust.Initialize(tracks); // tracks: TObjArray of LVector derivatives.
//*     jclust.FindJets();         // finds jets with ycut = 0.01.
//*     Int_t njets = 2;           // One can also force the event to be
//*     jclust.ForceNJets(njets);  // "njets" jets.
//* (Update Recored)
//*    2001/10/22  K.Ikematsu   Original version
//*
//* $Id$
//*************************************************************************
//
#include "ANLCheatedJetFinder.h"
//_____________________________________________________________________
//  ------------------
//  ANLTaggedJet Class
//  ------------------
//
ClassImp(ANLTaggedJet)

//*--
//*  Setters
//*--
void ANLTaggedJet::Merge(TObject *part) {
  Add(part);
  fTag = ((ANLTrack *)part)->GetColorSingletID();
}

void ANLTaggedJet::Merge(ANLJet *jet) {
#ifdef __DEBUG__
  cerr << "ANLTaggedJet::Merge ; ANLJet::Merge(jet); is called ..." << endl;
#endif
  ANLJet::Merge(jet);
  if ( fTag == 9999 ) fTag = ((ANLTaggedJet *)jet)->GetTag();
}

//_____________________________________________________________________
//  -------------------------
//  ANLCheatedJetFinder Class
//  -------------------------
//
ClassImp(ANLCheatedJetFinder)

Double_t ANLCheatedJetFinder::GetYmass(const ANL4DVector &p1,
                                       const ANL4DVector &p2) const { return 0; }

//_____________________________________________________________________
//  -----------------------------
//  ANLCheatedJadeJetFinder Class
//  -----------------------------
//
ClassImp(ANLCheatedJadeJetFinder)

Double_t ANLCheatedJadeJetFinder::GetYmass(const ANL4DVector &p1,
					   const ANL4DVector &p2) const {
  ANLTaggedJet *j1 = (ANLTaggedJet *)&p1;
  ANLTaggedJet *j2 = (ANLTaggedJet *)&p2;
  Int_t j1tag = j1->GetTag();
  Int_t j2tag = j2->GetTag();

  if ( j1tag != 9999 && j2tag != 9999 && j1tag != j2tag ) {
    return fEvis*fEvis;
  } else {
    return 2 * p1.E() * p2.E() * ( 1 - p1.CosTheta(p2) );
  }
}

//_____________________________________________________________________
//  ------------------------------
//  ANLCheatedJadeEJetFinder Class
//  ------------------------------
//
ClassImp(ANLCheatedJadeEJetFinder)

Double_t ANLCheatedJadeEJetFinder::GetYmass(const ANL4DVector &p1,
					    const ANL4DVector &p2) const {
  ANLTaggedJet *j1 = (ANLTaggedJet *)&p1;
  ANLTaggedJet *j2 = (ANLTaggedJet *)&p2;
  Int_t j1tag = j1->GetTag();
  Int_t j2tag = j2->GetTag();

  if ( j1tag != 9999 && j2tag != 9999 && j1tag != j2tag ) {
    return fEvis*fEvis;
  } else {
    return (p1+p2).GetMass2();
  }
}

//_____________________________________________________________________
//  -------------------------------
//  ANLCheatedDurhamJetFinder Class
//  -------------------------------
//
ClassImp(ANLCheatedDurhamJetFinder)

Double_t ANLCheatedDurhamJetFinder::GetYmass(const ANL4DVector &p1,
					     const ANL4DVector &p2) const {
  ANLTaggedJet *j1 = (ANLTaggedJet *)&p1;
  ANLTaggedJet *j2 = (ANLTaggedJet *)&p2;
  Int_t j1tag = j1->GetTag();
  Int_t j2tag = j2->GetTag();

  if ( j1tag != 9999 && j2tag != 9999 && j1tag != j2tag ) {
    return fEvis*fEvis;
  } else {
    Double_t minE = TMath::Min(p1.E(),p2.E());
    return 2 * minE * minE * ( 1 - p1.CosTheta(p2) );
  }
}
