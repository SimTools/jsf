//*************************************************************************
//* ====-================
//*  ANLGVTXTagger Class
//* ====-================
//*
//* (Description)
//*    A vertex tagging class using generator information.
//* (Requires)
//*     class ANLJetFinder
//*     class ANLTrack
//*     class JSFSIMDST, etc.
//* (Provides)
//*     class ANLGVTXTagger
//* (Update Recored)
//*    2001/07/07  K.Ikematsu    Original version
//*    2001/07/13  K.Ikematsu    Added public Getb method
//*    2001/07/31  K.Ikematsu    Removed CDCTrack pointer
//*                              from neutral track in mixed CAL cluster
//*                              (This causes double count of helix tracks)
//*
//* $Id$
//*************************************************************************
//
#include "ANLGVTXTagger.h"

//_____________________________________________________________________
//  -------------------
//  ANLGVTXTagger Class
//  -------------------
//
ClassImp(ANLGVTXTagger)

//_____________________________________________________________________
//*--
//*  Setters
//*--
void ANLGVTXTagger::SetNsig(Double_t nsig) {
  if ( nsig != fNsigCut ) {
    fNsigCut = nsig;
  }
}

void ANLGVTXTagger::SetNOffVtrk(Int_t noffvtrks) {
  if ( noffvtrks != fNOffVtrks ) {
    fNOffVtrks = noffvtrks;
  }
}

void ANLGVTXTagger::SetBfield(Double_t bfield) {
  if ( bfield != fBfield ) {
    fBfield = bfield;
  }
}

void ANLGVTXTagger::SetDebug(Bool_t flag) {
  if ( flag != fDEBUG ) {
    fDEBUG = flag;
  }
}

//_____________________________________________________________________
//*--
//*  Getters
//*--
Bool_t   ANLGVTXTagger::operator()(const ANLJet &jet){

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
Double_t ANLGVTXTagger::Getb(const ANLTrack &t){

  JSFLTKCLTrack *ct  = t.GetLTKCLTrack();
  JSFCDCTrack *cdctp = ct->GetCDC();

  if (fDEBUG) {
    cerr << "----------" << endl;
    if ( ct->GetType() == 2 || ct->GetType() == 4 )
      cerr << "Combined track in mixed CAL cluster : Charge = "
	   << ct->GetCharge() << endl;
  }

  if ( !cdctp ) {
    return -9999;
#if 1 // Protection for double count of CDCTrack pointer
      // from neutral track in mixed CAL cluster.
  } else if ( (ct->GetType()==2||ct->GetType()==4) && ct->GetCharge() == 0 ) {
    return -9999;
#endif
  } else {

    Int_t gsn  = t.GetLTKCLTrack()->GetCDC()->GetGenID();

    // Warnig!! TClonesArray *fGen starts from i=0.
    JSFGeneratorParticle *g = (JSFGeneratorParticle *)fGen->UncheckedAt(gsn-1);
    //                                                                  ^^^^^
    Int_t gmsn = g->GetMother();

    if (fDEBUG) cerr << "S.N. = " << gsn << "  PID = " << g->GetID()
                     << " Mother S.N.  = " << gmsn << endl;

    // If this generator particle comes from SpringParton directly,
    // we cannot get pointer to JSFGeneratorParticle.
    if (gmsn > 0) {
      JSFGeneratorParticle *gm = (JSFGeneratorParticle *)fGen->UncheckedAt(gmsn-1);
      Int_t gmpid = gm->GetID();
      if (fDEBUG) cerr << "Mother PID = " << gm->GetID() << endl;
      if (TMath::Abs(gmpid) == 310  || TMath::Abs(gmpid) == 3122 ||
          TMath::Abs(gmpid) == 3112 || TMath::Abs(gmpid) == 3222) {
        return -9999.;
      }
    }

    // Get decay point of final state stable particle

    ANL3DVector XV3(g->GetXV3());
    if (fDEBUG) XV3.DebugPrint();
    ANL2DVector XV2(XV3.GetTrans());
    if (fDEBUG) XV2.DebugPrint();

    // Get 3-momentum of final state stable particle

    ANL4DVector PV(g->GetLorentz());
    if (fDEBUG) PV.DebugPrint();
    ANL3DVector PV3(PV.Get3D());
    if (fDEBUG) PV3.DebugPrint();

    ANL3DVector NZ(0., 0., 1.);
    if (fDEBUG) NZ.DebugPrint();

    // Calculate a Unit vector from vertex-point to center of rotation

    ANL3DVector PC3 = PV3^NZ;
    if (fDEBUG) PC3.DebugPrint();
    ANL3DVector NC3(PC3.GetNorm());
    if (fDEBUG) NC3.DebugPrint();

    ANL2DVector NC2(NC3.GetTrans());
    if (fDEBUG) NC2.DebugPrint("Detailed");

    // radius of curvature

    Double_t Bfield = 2.;  // [Tesla]
    Double_t rho = 10e+2 * ( PV3.GetMag()/(0.3*Bfield) );  // [cm]
    if (fDEBUG) cerr << "radius of curvature: rho = " << rho << "[cm]" << endl;

    Double_t ch = g->GetCharge();

    // Calculate center point(2D) of track trajectory

    ANL2DVector XC2 = XV2 + (rho*ch*XC2);
    if (fDEBUG) XC2.DebugPrint("Detailed");

    Double_t b = TMath::Abs(XC2.Mod() - rho);
    if (fDEBUG) cerr << "Impact parameter (Gen2D) = " << b << "[cm]" << endl;

    return b;
  }
}

//_____________________________________________________________________
Double_t ANLGVTXTagger::Getbnorm(const ANLTrack &t){

  JSFCDCTrack *cdctp = t.GetLTKCLTrack()->GetCDC();
  if (!cdctp) {
    return -9999.;
  } else {

    // Access to Helix parameters

    JSFCDCTrack cdct(*cdctp);
    cdct.MovePivotToIP(fParam);
    Float_t helix[5];
    cdct.GetHelix(helix);
    Double_t err[15];
    cdct.GetError(err);
    Double_t drdr = err[0];

    Double_t b = Getb(t);

    if (fDEBUG) cerr << "VTX Resolution (Hit2D) = " << TMath::Sqrt(drdr) << endl
                     << "Normalized b (Gen2D) = " << TMath::Sqrt(b*b/drdr) << endl;

    if ( b == -9999.) return -9999.;
    return TMath::Sqrt(b*b/drdr);
  }
}
