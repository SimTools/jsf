#ifndef __JSFGeoCFit__
#define __JSFGeoCFit__

//********************************
//*
//*  JSFGeoCFit
//*
//*  Perform geometrical constraint fit.
//*
//*$ID$
//*
//********************************

#include "TVector3.h"

#ifndef __JSFHelicalTrack__
#include "JSFHelicalTrack.h"
#endif

#ifndef __JSFVirtualFit__
#include "JSFVirtualFit.h"
#endif

class JSFGeoCFit :  public JSFVirtualFit
{
 protected:
  Int_t fNtrk;                   // Number of tracks
  JSFHelixParameter   **fTrkpar; //! Track parameter
  TMatrixD            **fTrkerr; //! Error matrix
  Double_t            *fTrackChisq ; //! Chisq of each track.
  Double_t            fPTOR   ; // Factor to conver Pt to R  (R = PTOR*Pt)

 public:
  JSFGeoCFit(){}
  JSFGeoCFit(Int_t ntrk, TClonesArray *trk, TVector3 vtxini);
  JSFGeoCFit(Int_t ntrk, TClonesArray *trk, JSF3DV vtxini);
  virtual ~JSFGeoCFit();

  void Initialize(Int_t ntrk, TClonesArray *trk, JSF3DV vtxini);

  void Derivative(Double_t &chisq, TMatrixD &dchi2, TMatrixD &d2chi2);

  inline Int_t GetNtrk(){ return fNtrk;}
  inline void GetTrackChisq(Double_t trackchisq[]){ 
    for(Int_t i=0;i<fNtrk;i++){ trackchisq[i]=fTrackChisq[i]; }
  }
  inline void GetTrackChisq(Float_t trackchisq[]){ 
    for(Int_t i=0;i<fNtrk;i++){ trackchisq[i]=fTrackChisq[i]; }
  }

  ClassDef(JSFGeoCFit,1) // Geometrical Constraint Fit of tracks

};

#endif
