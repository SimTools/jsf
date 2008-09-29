//*************************************************************************
//* ====================
//*  ANLVTXTagger Class
//* ====================
//*
//* (Description)
//*    A very primitive vertex tagging class.
//* (Requires)
//*     class ANLJetFinder
//*     class ANLTrack
//*     class JSFSIMDST, etc.
//* (Provides)
//*     class ANLVTXTagger
//* (Update Recored)
//*    1999/10/09  K.Fujii      Original version.
//*    1999/10/18  K.Ikematsu   Implement Ks, Lambda, Sigma removal.
//*    2000/03/18  K.Ikematsu   Added SetNsig and SetNoff method.
//*    2000/03/18  K.Ikematsu   Added GetNsig and GetNoff method.
//*    2001/07/13  K.Ikematsu   Added public Getb method.
//*    2001/07/31  K.Ikematsu   Removed CDCTrack pointer
//*                             from neutral track in mixed CAL cluster.
//*                             (This causes double count of helix tracks)
//*
//* $Id$
//*************************************************************************
//
#include "ANLVTXTagger.h"

//_____________________________________________________________________
//  ------------------
//  ANLVTXTagger Class
//  ------------------
//
ClassImp(ANLVTXTagger)

//_____________________________________________________________________
//*--
//*  Setters
//*--
void ANLVTXTagger::SetNsig(Double_t nsig) {
  if ( nsig != fNsigCut ) {
    fNsigCut = nsig;
  }
}

void ANLVTXTagger::SetNOffVtrk(Int_t noffvtrks) {
  if ( noffvtrks != fNOffVtrks ) {
    fNOffVtrks = noffvtrks;
  }
}

//_____________________________________________________________________
//*--
//*  Getters
//*--
Bool_t   ANLVTXTagger::operator()(const ANLJet &jet){

  Int_t noffv = 0;

  TIter next(&jet.GetParticlesInJet());
  ANLTrack *tp;
  while ((tp = (ANLTrack *)next())) {
    Double_t bnorm = Getbnorm(*tp);
    if ( bnorm > fNsigCut ) noffv++;
  }

  if ( noffv >= fNOffVtrks ) return kTRUE;
  else return kFALSE;
}

//_____________________________________________________________________
Double_t ANLVTXTagger::Getb(const ANLTrack &t){

  JSFLTKCLTrack *ct  = t.GetLTKCLTrack();
  JSFCDCTrack *cdctp = ct->GetCDC();

  if ( !cdctp ) {
    return -9999;
    // Protection for double count of CDCTrack pointer
    // from neutral track in mixed CAL cluster.
  } else if ( (ct->GetType()==2||ct->GetType()==4) && ct->GetCharge() == 0 ) {
    return -9999;
  } else {

    Int_t gsn  = t.GetLTKCLTrack()->GetCDC()->GetGenID();

    // Warnig!! TClonesArray *fGen starts from i=0.
    JSFGeneratorParticle *g = (JSFGeneratorParticle *)fGen->UncheckedAt(gsn-1);
    //                                                                  ^^^^^
    Int_t gmsn = g->GetMother();

    // If this generator particle comes from SpringParton directly,
    // we cannot get pointer to JSFGeneratorParticle.
    if (gmsn > 0) {
      JSFGeneratorParticle *gm = (JSFGeneratorParticle *)fGen->UncheckedAt(gmsn-1);
      Int_t gmpid = gm->GetID();
      if (TMath::Abs(gmpid) == 310  || TMath::Abs(gmpid) == 3122 ||
	  TMath::Abs(gmpid) == 3112 || TMath::Abs(gmpid) == 3222) {
	return -9999.;
      }
    }

    // Access to Helix parameters

    JSFCDCTrack cdct(*cdctp);
    if (!cdct.MovePivotToIP(fParam)) return 0.;
    Float_t helix[5];
    cdct.GetHelix(helix);
    Double_t dr   = helix[0];
    Double_t dz   = helix[3];

    Double_t b = TMath::Sqrt(dr*dr+dz*dz);
    //cerr << "Impact parameter (Hit3D) = " << b << "[cm]" << endl
    //     << "Impact parameter (Hit2D) = " << dr << endl;
    return b;
  }
}

//_____________________________________________________________________
Double_t ANLVTXTagger::Getbnorm(const ANLTrack &t){

  JSFLTKCLTrack *ct  = t.GetLTKCLTrack();
  JSFCDCTrack *cdctp = ct->GetCDC();

  if ( !cdctp ) {
    return -9999;
    // Protection for double count of CDCTrack pointer
    // from neutral track in mixed CAL cluster.
  } else if ( (ct->GetType()==2||ct->GetType()==4) && ct->GetCharge() == 0 ) {
    return -9999;
  } else {

    Int_t gsn  = t.GetLTKCLTrack()->GetCDC()->GetGenID();

    // Warnig!! TClonesArray *fGen starts from i=0.
    JSFGeneratorParticle *g = (JSFGeneratorParticle *)fGen->UncheckedAt(gsn-1);
    //                                                                  ^^^^^
    Int_t gmsn = g->GetMother();

    // If this generator particle comes from SpringParton directly,
    // we cannot get pointer to JSFGeneratorParticle.
    if (gmsn > 0) {
      JSFGeneratorParticle *gm = (JSFGeneratorParticle *)fGen->UncheckedAt(gmsn-1);
      Int_t gmpid = gm->GetID();
      if (TMath::Abs(gmpid) == 310  || TMath::Abs(gmpid) == 3122 ||
          TMath::Abs(gmpid) == 3112 || TMath::Abs(gmpid) == 3222) {
        return -9999.;
      }
    }

    // Access to Helix parameters

    JSFCDCTrack cdct(*cdctp);
    if (!cdct.MovePivotToIP(fParam)) return 0.;
    Float_t helix[5];
    cdct.GetHelix(helix);
    Double_t err[15];
    cdct.GetError(err);
    Double_t dr   = helix[0];
    Double_t dz   = helix[3];
    Double_t drdr = err[0];
    Double_t dzdz = err[9];

    //cerr << "Normalized b (Hit3D) = "
    //     << TMath::Sqrt(dr*dr/drdr + dz*dz/dzdz) << endl;

    return TMath::Sqrt(dr*dr/drdr + dz*dz/dzdz);
  }
}
