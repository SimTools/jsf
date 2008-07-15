//*************************************************************************
//* ==================
//*  ANLTrack Classes
//* ==================
//*
//* (Description)
//*    Track Class for JLC analysis
//* (Requires)
//*     class TVector
//*     class TObjNum
//*     class JSFSIMDST, etc
//*     class JSFLTKCLTrack
//*     class ANL4DVector
//* (Provides)
//*     class ANLTrack
//* (Update Recored)
//*    1999/08/17  K.Ikematsu   Original version.
//*    1999/09/05  K.Ikematsu   Replaced LockableLVector with ANL4DVector.
//*    1999/09/13  K.Ikematsu   Added GetLTKCLTrack method.
//*    1999/09/13  K.Ikematsu   Added GetTrackPtr method.
//*    2001/07/09  K.Ikematsu   Changed IsElectron, IsMuon & GetCharge
//*                             method to virtual.
//*    2001/10/21  K.Ikematsu   Added SetColorSingletID method and
//*                             fColorSingletID member.
//*    2001/10/22  K.Ikematsu   Added TObjNum class from FlavourGetter class.
//*    2002/02/08  K.Fujii      fMSNPriHad is now a pointer.
//*                             Added operator=.
//*    2008/07/15  K.Ikematsu   Changed EFlavourGetterDetectorID to
//*                             EDetectorID.
//*                             Moved TObjNum class to $LEDAROOT/src/utils/.
//*
//* $Id$
//*************************************************************************
//
#include "ANLTrack.h"
//_____________________________________________________________________
//  --------------
//  ANLTrack Class
//  --------------
//
ClassImp(ANLTrack)

//*--
//*  Constructors
//*--
ANLTrack::ANLTrack() : ANL4DVector(0.), fTrackPtr(0), fGen(0),
                       fMSNPriHad(0), fColorSingletID(9999) {}
ANLTrack::ANLTrack(const TObject *track) :
  ANL4DVector(((JSFLTKCLTrack *)track)->GetPV()),
  fTrackPtr(track), fMSNPriHad(0), fColorSingletID(9999) {
  JSFSIMDST     *sds = (JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
  JSFSIMDSTBuf  *evt = (JSFSIMDSTBuf*)sds->EventBuf();
  fGen   = evt->GetGeneratorParticles();
}
ANLTrack::ANLTrack(const TVector &pv, const TObject *ptr) :
  ANL4DVector(pv), fTrackPtr(ptr), fGen(0), fMSNPriHad(0),
  fColorSingletID(9999) {}

//*--
//*  Destructor
//*--
ANLTrack::~ANLTrack() {}

//*--
//*  Getters
//*--
Bool_t   ANLTrack::IsElectron() const {
  return ( ((JSFLTKCLTrack *)fTrackPtr)->GetType() == 11 );
}
Bool_t   ANLTrack::IsMuon() const {
  return ( ((JSFLTKCLTrack *)fTrackPtr)->GetType() == 13 );
}
Bool_t   ANLTrack::IsLepton() const {
  return ( IsElectron() || IsMuon() );
}
Bool_t   ANLTrack::IsGamma() const {
  return ( ((JSFLTKCLTrack *)fTrackPtr)->GetType() == 1 );
}
Double_t ANLTrack::GetCharge() const {
  return ((JSFLTKCLTrack *)fTrackPtr)->GetCharge();
}
Double_t ANLTrack::GetConeEnergy(const Double_t cth,
                                 const TObjArray *tracks) const {
  Double_t e = 0.;
  TIter next(tracks);
  ANLTrack *t;
  while ((t = (ANLTrack *)next())) {
    if ( t == this ) continue;
    if ( CosTheta(*t) > cth ) e += t->E();
  }
  return e;
}
TObject *ANLTrack::GetTrackPtr() const {
  return (TObject *)fTrackPtr;
}
JSFLTKCLTrack *ANLTrack::GetLTKCLTrack() const {
  return (JSFLTKCLTrack *)fTrackPtr;
}
Int_t ANLTrack::GetColorSingletID() const {
  return fColorSingletID;
}

//*--
//*  Setters
//*--
void ANLTrack::SetColorSingletID() {

  if (!fMSNPriHad) fMSNPriHad = new TObjArray();
  else		   fMSNPriHad->Clear();

  JSFLTKCLTrack *ctp = GetLTKCLTrack();

#ifdef __DEBUG__
  cerr << "  Track type : " << ctp->GetType()
       << " , " << ctp->GetTypeName() << endl
       << "  CDCEntries   = " << ctp->GetCDCEntries() << endl
       << "  EMGenEntries = " << ctp->GetEMGenEntries() << endl;
#endif

  // Should be weighted in charged tracks in cheating !!!
  // because 4-momentum is decided by charged track if there is.

  Int_t ncdctrk = ctp->GetCDCEntries();
  Int_t nemgen = ctp->GetEMGenEntries();

  // If track charge in mixed CAL cluster = 0,
  // Don't use CDCTrack pointer because of subtracted track-information !!
  if ( ncdctrk > 0 && ctp->GetCharge() != 0 ) {
    Double_t egen = 0.;
    for ( Int_t i = 0; i < ncdctrk; i++ ) {
      if ( GetEGeneratorParticle(kECDC, ctp, i) > egen ) {
        ScanThroughDecayChain(kECDC, ctp, i);
      }
      egen = GetEGeneratorParticle(kECDC, ctp, i);
    }

    TIter nextid(fMSNPriHad);
    TObjNum *idp;
    while ((idp = (TObjNum *)nextid())) {
      fColorSingletID = idp->GetNum();
    }
    // If ncdctrk > 0 && nemgen > 0 such as electron candidate,
    // should not scan generator particles in EMC cluster.
  } else if ( nemgen > 0 ) {
    Double_t egen = 0.;
    for ( Int_t i = 0; i < nemgen; i++ ) {
      if ( GetEGeneratorParticle(kEEMC, ctp, i) > egen ) {
        ScanThroughDecayChain(kEEMC, ctp, i);
      }
      egen = GetEGeneratorParticle(kEEMC, ctp, i);
    }

    TIter nextid(fMSNPriHad);
    TObjNum *idp;
    while ((idp = (TObjNum *)nextid())) {
      fColorSingletID = idp->GetNum();
    }
  }

  fMSNPriHad->SetOwner();  // SetOwner() method only enabled
                           // after adding contents
}

const ANLTrack & ANLTrack::operator=(const ANLTrack & track) {
  *(ANL4DVector *)this = (ANL4DVector)track;
  fTrackPtr 	= track.fTrackPtr;
  fGen	  	= track.fGen;
  if (fMSNPriHad) {
    delete fMSNPriHad;
    fMSNPriHad = 0;
  }
  if (track.fMSNPriHad) fMSNPriHad = new TObjArray(*track.fMSNPriHad);
  fColorSingletID = track.fColorSingletID;
  return *this;
}

//_____________________________________________________________________
void ANLTrack::ScanThroughDecayChain(EDetectorID id,
                                     JSFLTKCLTrack *ctp, Int_t i) {

#ifdef __DEBUG__
  cerr << "i = " << i << endl;
#endif
  ////Int_t gpid = 0;
  ////Int_t gsn  = 0;
  Int_t gmsn = 0;
  if (id == kEEMC) gmsn = ctp->GetEMGenAt(i)->GetSerial();
  else if (id == kECDC) gmsn = ctp->GetCDCTrackAt(i)->GetGenID();
  else {
    cerr << "Unsupported detector type !!" << endl;
    exit(1);
  }
  ////Double_t gdln = 0;
  ////Int_t gpidoffvt = 0;
  ////Int_t gsnoffvt  = 0;
  while ( gmsn >= 0 ) {
    JSFGeneratorParticle *g = (JSFGeneratorParticle *)fGen->UncheckedAt(gmsn-1);
    ////gpid = g->GetID();
    ////gsn  = g->GetSerial();
    gmsn = g->GetMother();
    ////gdln = g->GetDecayLength();

#ifdef __DEBUG__
    ////    cerr << "(PID, S.N, M.S.N, DLength) = ("
    ////	 << gpid << ","
    ////	 << gsn  << ","
    ////	 << gmsn << ","
    ////	 << gdln << ")" << endl;
#endif

    ////if ( gdln > 0 ) {
#ifdef __DEBUG__
    ////    cerr << "This generator particle has a finite decay length." << endl;
#endif
    ////  gpidoffvt = gpid;
    ////  gsnoffvt  = gsn;
    ////}
  }
#ifdef __DEBUG__
  /*
  cerr << "-- Search ended --" << endl;
  if (TMath::Abs(gpidoffvt) == 310  || TMath::Abs(gpidoffvt) == 3122 ||
      TMath::Abs(gpidoffvt) == 3112 || TMath::Abs(gpidoffvt) == 3222 ||
      TMath::Abs(gpidoffvt) == 3322 || TMath::Abs(gpidoffvt) == 3334)
    cerr << "This off-vertex generator particle is weak decaying hadron." << endl;
  */
#endif
  /*
  if ( TMath::Abs(gpidoffvt) > 0 &&
       TMath::Abs(gpidoffvt) != 310  && TMath::Abs(gpidoffvt) != 3122 &&
       TMath::Abs(gpidoffvt) != 3112 && TMath::Abs(gpidoffvt) != 3222 &&
       TMath::Abs(gpidoffvt) != 3322 && TMath::Abs(gpidoffvt) != 3334 ) {
    TObjNum *gpidoffvtp = new TObjNum(gpidoffvt);
    TObjNum *gsnoffvtp  = new TObjNum(gsnoffvt);
    fPIDOffVT.Add(gpidoffvtp);  // *gpidoffvtp, *gsnoffvtp and *gmsnoffvtp stays
    fSNOffVT.Add(gsnoffvtp);    // but (TObjArray *)obj->SetOwner() deletes its elements.
  }
  */
  ////TObjNum *gpidp = new TObjNum(gpid);
  ////TObjNum *gsnp  = new TObjNum(gsn);
#ifdef __DEBUG__
  cerr << "ANLTrack::ScanThroughDecayChain() : gmsn = " << gmsn << endl;
#endif
  TObjNum *gmsnp = new TObjNum(gmsn);
  ////fPIDPriHad.Add(gpidp);  // *gpidp, *gsnp and *gmsnp stays
  ////fSNPriHad.Add(gsnp);    // but (TObjArray *)obj->SetOwner() deletes
  fMSNPriHad->Add(gmsnp);  // its elements.
}

//_____________________________________________________________________
Double_t ANLTrack::GetEGeneratorParticle(EDetectorID id,
                                         JSFLTKCLTrack *ctp, Int_t i) {

  Double_t egen = 0.;
  Int_t gmsn = 0;
  if (id == kEEMC) gmsn = ctp->GetEMGenAt(i)->GetSerial();
  else if (id == kECDC) gmsn = ctp->GetCDCTrackAt(i)->GetGenID();
  else {
    cerr << "Unsupported detector type !!" << endl;
    exit(1);
  }
  JSFGeneratorParticle *g = (JSFGeneratorParticle *)fGen->UncheckedAt(gmsn-1);
  egen = g->GetE();

  /*
  cerr << "(i, PID, S.N, M.S.N, E) = ("
       << i << ","
       << g->GetID() << ","
       << g->GetSerial() << ","
       << g->GetMother() << ","
       << egen << endl;
  */

  return egen;
}
