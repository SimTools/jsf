//*************************************************************************
//* =====================
//*  FlavourGetter Class
//* =====================
//*
//* (Description)
//*    A flavour of jets getting class
//* (Requires)
//*     class TObjInt
//*     class ANLJetFinder
//*     class ANLTrack
//*     class JSFSIMDST, etc
//* (Provides)
//*     class FlavourGetter
//* (Update Recored)
//*    2000/02/13  K.Ikematsu   Original version
//*    2000/04/28  K.Ikemtasu   Modified GetPrimaryHadronPID method
//*    2001/07/20  K.Ikemtasu   Added GetPrimaryHadronSN method
//*    2001/07/20  K.Ikemtasu   Added GetPartonID method
//*    2001/07/26  K.Ikematsu   Added TTL4JFlavourGetter class
//*    2001/07/31  K.Ikematsu   Supported to search generator particles
//*                             contributing to the EM cluster (gamma)
//*    2001/08/02  K.Ikematsu   Added fPIDOffVT and fSNOffVT members
//*    2001/10/23  K.Ikematsu   Moved TObjNum class to ANLTrack class
//*    2001/12/18  K.Ikematsu   Added SetThetaCut method
//*    2008/07/15  K.Ikematsu   Changed EFlavourGetterDetectorID to
//*                             EDetectorID.
//*    2008/07/17  K.Ikematsu   Replaced TObjNum with TObjInt.
//*
//* $Id$
//*************************************************************************
//
#include "FlavourGetter.h"
//_____________________________________________________________________
//  -------------------
//  FlavourGetter Class
//  -------------------
//
ClassImp(FlavourGetter)

//_____________________________________________________________________
//*--
//*  Setters
//*--
void FlavourGetter::SetDataArray(const ANLJet &jet) {

  fPIDPriHad.Clear();
  fSNPriHad.Clear();
  fMSNPriHad.Clear();
  fPIDOffVT.Clear();
  fSNOffVT.Clear();

  TIter next(&jet.GetParticlesInJet());
  ANLTrack *tp;
  while ((tp = (ANLTrack *)next())) {
    SearchPrimaryHadron(*tp);
  }

  fPIDPriHad.SetOwner();  // SetOwner() method only enabled
  fSNPriHad.SetOwner();   // after adding contents
  fMSNPriHad.SetOwner();
  fPIDOffVT.SetOwner();
  fSNOffVT.SetOwner();
}

//_____________________________________________________________________
//*--
//*  Getters
//*--
Int_t FlavourGetter::operator()(const ANLJet &jet) {

  static const Double_t kOffVRatioCut = 0.1;
  static const Double_t kBHadRatioCut = 0.9;
  static const Double_t kCHadRatioCut = 0.85;

  Int_t ntrkinjet = 0;
  Int_t noffvtrkinjet = 0;
  Int_t noffvtrkfrombhad = 0;
  Int_t noffvtrkfromchad = 0;

  SetDataArray(jet);

#if 0
  TIter nextprihadpid(&fPIDPriHad);
  TObjInt *prihadpidp;
  while ((prihadpidp = (TObjInt *)nextprihadpid())) {
    // fPIDPriHad : Not Yet contained primary hadron's PIDs contributing to
    //              the HDC cluster from neutral combined tracks.
    Int_t prihadpid = TMath::Abs(prihadpidp->GetNum());
#ifdef __DEBUG__
    cerr << "FlavourGetter::operator() : PID(pri had) = " << prihadpid << endl;
#endif
    ntrkinjet++;
  }
#else
  ntrkinjet = fPIDPriHad.GetEntries();
#endif

  TIter nextoffvtrkpid(&fPIDOffVT);
  TObjInt *offvtrkpidp;
  while ((offvtrkpidp = (TObjInt *)nextoffvtrkpid())) {
    Int_t offvtrkpid = TMath::Abs(offvtrkpidp->GetNum());
#ifdef __DEBUG__
    cerr << "FlavourGetter::operator() : PID(offv trk) = " << offvtrkpid << endl;
#endif
    noffvtrkinjet++;

    Int_t flavour = 0;
    if ( offvtrkpid/1000 > 0 && offvtrkpid/10000 == 0 ) { // Meson-Baryon branch
      flavour = offvtrkpid/1000;
    } else {
      flavour = (offvtrkpid/100)%10;
    }
    if ( flavour == 5 ) noffvtrkfrombhad++;
    if ( flavour == 4 ) noffvtrkfromchad++;
  }

  Double_t offvratio = (Double_t)noffvtrkinjet/ntrkinjet;
  Double_t bhadratio = (Double_t)noffvtrkfrombhad/noffvtrkinjet;
  Double_t chadratio = (Double_t)noffvtrkfromchad/noffvtrkinjet;

#ifdef __DEBUG__
  cerr << "R(OffVTrk,FromB,FromC) = ("
       << offvratio << ","
       << bhadratio << ","
       << chadratio << ")" << endl;
#endif

  if ( offvratio >= kOffVRatioCut ) {
    if ( bhadratio >= kBHadRatioCut ) return 3;
    else if ( chadratio >= kCHadRatioCut ) return 2;
    else return 0;
  } else {
    if ( bhadratio >= kBHadRatioCut || chadratio >= kCHadRatioCut ) return 0;
    else return 1;
  }
}

//_____________________________________________________________________
TObjArray & FlavourGetter::GetPrimaryHadronPID() { return fPIDPriHad; }

TObjArray & FlavourGetter::GetPrimaryHadronSN() { return fSNPriHad; }

TObjArray & FlavourGetter::GetPartonID() { return fMSNPriHad; }

TObjArray & FlavourGetter::GetOffVertexHadronPID() { return fPIDOffVT; }

TObjArray & FlavourGetter::GetOffVertexHadronSN() { return fSNOffVT; }

//_____________________________________________________________________
void FlavourGetter::SearchPrimaryHadron(const ANLTrack &t) {

  JSFLTKCLTrack *ctp = t.GetLTKCLTrack();

#ifdef __DEBUG__
  cerr << "  Track type : " << ctp->GetType()
       << " , " << ctp->GetTypeName() << endl
       << "  CDCEntries   = " << ctp->GetCDCEntries() << endl
       << "  EMGenEntries = " << ctp->GetEMGenEntries() << endl;
#endif

  // Should be weighted in charged tracks in cheating !!!
  // because 4-momentum is decided by charged track if there is.

  Int_t ncdctrk = ctp->GetCDCEntries();
  // If track charge in mixed CAL cluster = 0,
  // Don't use CDCTrack pointer because of subtracted track-information !!
  if ( ncdctrk > 0 && ctp->GetCharge() != 0 ) {
#ifdef __DEBUG__
    cerr << "# of charged tracks = " << ncdctrk << endl;
#endif
    for (Int_t i = 0; i < ncdctrk; i++ ) {
      ScanThroughDecayChain(kECDC, ctp, i);
    }
    return; // If ncdctrk > 0 && nemgen > 0 such as electron candidate,
            // should not scan generator particles in EMC cluster.
  }

  Int_t nemgen = ctp->GetEMGenEntries();
  if ( nemgen > 0 ) {
#ifdef __DEBUG__
    cerr << "# of neutral tracks in EMC = " << nemgen << endl;
#endif
    for (Int_t i = 0; i < nemgen; i++ ) {
      ScanThroughDecayChain(kEEMC, ctp, i);
    }
    return;
  }
}

//_____________________________________________________________________
void FlavourGetter::ScanThroughDecayChain(EDetectorID id,
                                          JSFLTKCLTrack *ctp, Int_t i) {

#ifdef __DEBUG__
  cerr << "i = " << i << endl;
#endif
  Int_t gpid = 0;
  Int_t gsn  = 0;
  Int_t gmsn = 0;
  if (id == kEEMC) gmsn = ctp->GetEMGenAt(i)->GetSerial();
  else if (id == kECDC) gmsn = ctp->GetCDCTrackAt(i)->GetGenID();
  else {
    cerr << "Unsupported detector type !!" << endl;
    exit(1);
  }
  Double_t gdln = 0;
  Int_t gpidoffvt = 0;
  Int_t gsnoffvt  = 0;
  while ( gmsn >= 0 ) {
    JSFGeneratorParticle *g = (JSFGeneratorParticle *)fGen->UncheckedAt(gmsn-1);
    gpid = g->GetID();
    gsn  = g->GetSerial();
    gmsn = g->GetMother();
    gdln = g->GetDecayLength();

#ifdef __DEBUG__
    cerr << "(PID, S.N, M.S.N, DLength) = ("
         << gpid << ","
         << gsn  << ","
         << gmsn << ","
         << gdln << ")" << endl;
#endif

    if ( gdln > 0 ) {
#ifdef __DEBUG__
      cerr << "This generator particle has a finite decay length." << endl;
#endif
      gpidoffvt = gpid;
      gsnoffvt  = gsn;
    }
  }
#ifdef __DEBUG__
  cerr << "-- Search ended --" << endl;
  if (TMath::Abs(gpidoffvt) == 310  || TMath::Abs(gpidoffvt) == 3122 ||
      TMath::Abs(gpidoffvt) == 3112 || TMath::Abs(gpidoffvt) == 3222 ||
      TMath::Abs(gpidoffvt) == 3322 || TMath::Abs(gpidoffvt) == 3334)
    cerr << "This off-vertex generator particle is weak decaying hadron." << endl;
#endif

  if ( TMath::Abs(gpidoffvt) > 0 &&
       TMath::Abs(gpidoffvt) != 310  && TMath::Abs(gpidoffvt) != 3122 &&
       TMath::Abs(gpidoffvt) != 3112 && TMath::Abs(gpidoffvt) != 3222 &&
       TMath::Abs(gpidoffvt) != 3322 && TMath::Abs(gpidoffvt) != 3334 ) {
    TObjInt *gpidoffvtp = new TObjInt(gpidoffvt);
    TObjInt *gsnoffvtp  = new TObjInt(gsnoffvt);
    fPIDOffVT.Add(gpidoffvtp);  // *gpidoffvtp, *gsnoffvtp and *gmsnoffvtp stays
    fSNOffVT.Add(gsnoffvtp);    // but (TObjArray *)obj->SetOwner() deletes its elements.
  }
  TObjInt *gpidp = new TObjInt(gpid);
  TObjInt *gsnp  = new TObjInt(gsn);
  TObjInt *gmsnp = new TObjInt(gmsn);
  fPIDPriHad.Add(gpidp);  // *gpidp, *gsnp and *gmsnp stays
  fSNPriHad.Add(gsnp);    // but (TObjArray *)obj->SetOwner() deletes
  fMSNPriHad.Add(gmsnp);  // its elements.
}


//_____________________________________________________________________
//  ------------------------
//  TTL4JFlavourGetter Class
//  ------------------------
//
ClassImp(TTL4JFlavourGetter)

//_____________________________________________________________________
//*--
//*  Setters
//*--
void TTL4JFlavourGetter::SetThetaCut(Double_t theta) {
  if ( theta != fThetaCut ) {
    fThetaCut = theta;
  }
}

//_____________________________________________________________________
//*--
//*  Getters
//*--
Int_t TTL4JFlavourGetter::operator()(const ANLJet &jet) {

  static const Double_t kCHadRatioCut = 0.8;

  Int_t ntrkinjet = 0;
  Int_t ntrkfromb = 0;
  Int_t ntrkfromw = 0;
  Int_t noffvtrkinjet = 0;
  Int_t noffvtrkfromchad = 0;

  SetDataArray(jet);

  TIter nextptnid(&GetPartonID());
  TObjInt *ptnidp;
  while ((ptnidp = (TObjInt *)nextptnid())) {
    // fMSNPriHad : Not Yet contained primary hadron's Mother S.N contributing to
    //              the HDC cluster from neutral combined tracks.
    Int_t partonid = TMath::Abs(ptnidp->GetNum());
#ifdef __DEBUG__
    cerr << "TTL4JFlavourGetter::operator() : MSN = " << partonid << endl;
#endif
    ntrkinjet++;

    if ( partonid == 3 ) ntrkfromb++;
    else if ( partonid == 7 || partonid == 9 ) ntrkfromw++;
  }

  TIter nextoffvtrkpid(&GetOffVertexHadronPID());
  TObjInt *offvtrkpidp;
  while ((offvtrkpidp = (TObjInt *)nextoffvtrkpid())) {
    Int_t offvtrkpid = TMath::Abs(offvtrkpidp->GetNum());
#ifdef __DEBUG__
    cerr << "FlavourGetter::operator() : PID(offv trk) = " << offvtrkpid << endl;
#endif
    noffvtrkinjet++;

    Int_t flavour = 0;
    if ( offvtrkpid/1000 > 0 && offvtrkpid/10000 == 0 ) { // Meson-Baryon branch
      flavour = offvtrkpid/1000;
    } else {
      flavour = (offvtrkpid/100)%10;
    }
    if ( flavour == 4 ) noffvtrkfromchad++;
  }

#ifdef __DEBUG__
  cerr << "R(Fromb,FromW,FromD) = (" << (Double_t)ntrkfromb/ntrkinjet << ","
       << (Double_t)ntrkfromw/ntrkinjet << ","
       << (Double_t)noffvtrkfromchad/noffvtrkinjet << ")" << endl;
#endif

  JSFSpringParton *spbbar  = (JSFSpringParton *)fSpgen->UncheckedAt(2);
  JSFSpringParton *spb     = (JSFSpringParton *)fSpgen->UncheckedAt(4);
  JSFSpringParton *spupbar = (JSFSpringParton *)fSpgen->UncheckedAt(6);
  JSFSpringParton *spdn    = (JSFSpringParton *)fSpgen->UncheckedAt(7);
  JSFSpringParton *spup    = (JSFSpringParton *)fSpgen->UncheckedAt(8);
  JSFSpringParton *spdnbar = (JSFSpringParton *)fSpgen->UncheckedAt(9);

  ANL4DVector qspbbar(spbbar->GetPV());
  ANL4DVector qspb(spb->GetPV());
  ANL4DVector qspupbar(spupbar->GetPV());
  ANL4DVector qspdn(spdn->GetPV());
  ANL4DVector qspup(spup->GetPV());
  ANL4DVector qspdnbar(spdnbar->GetPV());

  Double_t thetaspbbarj  = qspbbar.GetTheta(jet);
  Double_t thetaspbj     = qspb.GetTheta(jet);
  Double_t thetaspupbarj = qspupbar.GetTheta(jet);
  Double_t thetaspdnj    = qspdn.GetTheta(jet);
  Double_t thetaspupj    = qspup.GetTheta(jet);
  Double_t thetaspdnbarj = qspdnbar.GetTheta(jet);

#ifdef __DEBUG__
  if ( spdn->GetID() == 11 || spdn->GetID() == 13 ||  spdn->GetID() == 15 ) {
    cerr << "W- decayed leptonically" << endl
         << "Th(bbar_j,b_j,up_j,dnbar_j) = ("
         << thetaspbbarj  << ","
         << thetaspbj     << ","
         << thetaspupj    << ","
         << thetaspdnbarj << ")" << endl;
  } else {
    cerr << "W+ decayed leptonically" << endl
         << "Th(bbar_j,b_j,upbar_j,dn_j) = ("
         << thetaspbbarj  << ","
         << thetaspbj     << ","
         << thetaspupbarj << ","
         << thetaspdnj    << ")" << endl;
  }
#endif

  if ( (Double_t)ntrkfromb/ntrkinjet >= (Double_t)ntrkfromw/ntrkinjet ) {
    if ( TMath::Min(thetaspbbarj,thetaspbj) < fThetaCut ) {
      if ( thetaspbbarj < thetaspbj ) return -3;
      else return 3;
    } else {
#ifdef __DEBUG__
      cerr << "Th(bottom_j) = "
           << TMath::Min(thetaspbbarj,thetaspbj) << endl;
#endif
      return 0;
    }
  } else {
    if ( spdn->GetID() == 11 || spdn->GetID() == 13 ||  spdn->GetID() == 15 ) {
      if ( (Double_t)noffvtrkfromchad/noffvtrkinjet >= kCHadRatioCut ) {
        if ( thetaspupj < fThetaCut ) return 2;
        else if ( thetaspdnbarj < fThetaCut ) return -1;
        else {
#ifdef __DEBUG__
          cerr << "Th(charm_j) = " << thetaspupj << endl;
#endif
          return 0;
        }
      } else {
        if ( TMath::Min(thetaspupj,thetaspdnbarj) < fThetaCut ) {
          if ( thetaspupj < thetaspdnbarj ) return 1;
          else return -1;
        } else {
#ifdef __DEBUG__
          cerr << "Th(light_j) = "
               << TMath::Min(thetaspupj,thetaspdnbarj) << endl;
#endif
          return 0;
        }
      }
    } else {
      if ( (Double_t)noffvtrkfromchad/noffvtrkinjet >= kCHadRatioCut ) {
        if ( thetaspupbarj < fThetaCut ) return -2;
        else if ( thetaspdnj < fThetaCut ) return 1;
        else {
#ifdef __DEBUG__
          cerr << "Th(charm_j) = " << thetaspupbarj << endl;
#endif
          return 0;
        }
      } else {
        if ( TMath::Min(thetaspupbarj,thetaspdnj) < fThetaCut ) {
          if ( thetaspupbarj < thetaspdnj ) return -1;
          else return 1;
        } else {
#ifdef __DEBUG__
          cerr << "Th(light_j) = "
               << TMath::Min(thetaspupbarj,thetaspdnj) << endl;
#endif
          return 0;
        }
      }
    }
  }
}
