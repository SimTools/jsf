#ifndef __ANLCHEATEDJETFINDER__
#define __ANLCHEATEDJETFINDER__
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
#include "ANLTrack.h"
#include "ANLJetFinder.h"
//_____________________________________________________________________
//  ------------------
//  ANLTaggedJet Class
//  ------------------
//
class ANLTaggedJet : public ANLJet {
friend class ANLCheatedJetFinder;
public:
   ANLTaggedJet() : fTag(9999) {}
   ANLTaggedJet(const TObjArray &parts) : ANLJet(parts), fTag(9999) {}
   virtual ~ANLTaggedJet() {}

   void Merge(TObject *part);
   void Merge(ANLJet *jet);
   void SetTag(Int_t tag) { fTag = tag; }
   inline Int_t GetTag() const { return fTag; }

private:
   Int_t fTag;  // Color singlet ID

   ClassDef(ANLTaggedJet,1)  // ANLTaggedJet class
};

//_____________________________________________________________________
//  -------------------------
//  ANLCheatedJetFinder Class
//  -------------------------
//
class ANLCheatedJetFinder : public ANLJetFinder {
public:
   ANLCheatedJetFinder(Double_t y = 0.) : ANLJetFinder(y) {}  // default constructor
   ANLCheatedJetFinder(const ANLCheatedJetFinder &jf) : ANLJetFinder(jf) {

     ANLTaggedJet *jetsrc;
     ANLTaggedJet *jet;
     TIter nextsrc( & (((ANLCheatedJetFinder &)jf).GetJets()));
     TIter next(&GetJets());
     while ((jet = (ANLTaggedJet *)next())) {
       jetsrc = (ANLTaggedJet *)nextsrc();
       jet->SetTag(jetsrc->GetTag());
     }
   }

   ANLJet *NewJet() {               // make new ANLTaggedJet object
     return new ANLTaggedJet();
   }
   ANLJetFinder *NewJetFinder(ANLJetFinder *jf) {
     return new ANLCheatedJetFinder(*(ANLCheatedJetFinder *)jf);
                                    // make new ANLCheatedJetFinder object
   }

   virtual Double_t GetYmass(const ANL4DVector &p1,
			     const ANL4DVector &p2) const;

   ClassDef(ANLCheatedJetFinder,1)  // Cheated Jet Finder class
};

//_____________________________________________________________________
//  -----------------------------
//  ANLCheatedJadeJetFinder Class
//  -----------------------------
//
class ANLCheatedJadeJetFinder : public ANLCheatedJetFinder {
public:
   ANLCheatedJadeJetFinder(Double_t y = 0.) : ANLCheatedJetFinder(y) {}  // default constructor

   Double_t GetYmass(const ANL4DVector &p1, const ANL4DVector &p2) const;

   ClassDef(ANLCheatedJadeJetFinder,1)  // Cheated Jade Jet Finder class
};

//_____________________________________________________________________
//  ------------------------------
//  ANLCheatedJadeEJetFinder Class
//  ------------------------------
//
class ANLCheatedJadeEJetFinder : public ANLCheatedJetFinder {
public:
   ANLCheatedJadeEJetFinder(Double_t y = 0.) : ANLCheatedJetFinder(y) {}  // default constructor

   Double_t GetYmass(const ANL4DVector &p1, const ANL4DVector &p2) const;

   ClassDef(ANLCheatedJadeEJetFinder,1)  // Cheated Jade EJet Finder class
};

//_____________________________________________________________________
//  -------------------------------
//  ANLCheatedDurhamJetFinder Class
//  -------------------------------
//
class ANLCheatedDurhamJetFinder : public ANLCheatedJetFinder {
public:
   ANLCheatedDurhamJetFinder(Double_t y = 0.) : ANLCheatedJetFinder(y) {}  // default constructor

   Double_t GetYmass(const ANL4DVector &p1, const ANL4DVector &p2) const;

   ClassDef(ANLCheatedDurhamJetFinder,1)  // Cheated Durham Jet Finder class
};

#endif
