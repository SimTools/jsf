#ifndef __JSFCDCTrack__
#define __JSFCDCTrack__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFCDCTrack                                                          //
//                                                                      //
// CDC Track parameter information.                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TVector.h"

#ifndef __JSFVTXHit__
#include "JSFVTXHit.h"
#endif

const Int_t kMaxCDCTracks=500;

class JSFCDCTrack : public TObject {

protected:
  Float_t  fP[3] ; // Px, Py, Pz at cloest approach
  Float_t  fE    ; // Particle energy assuming mass less particle.
  Float_t  fX[3] ; // x,y,z of cloest approach
  Int_t    fCharge ; // Apparent charge
  Int_t    fGenID  ; // Corresponding generator track ID. ( 1 to n )
  Float_t  fHelix[5]; // Track Parameter, delta-r, phi, kappa, delta-z, tan-lambda
  Float_t  fPivot[3]; // (x,y,z) of Pivot
  Double_t fError[15]; // Helix's error matrix
  Int_t    fNDF     ; // NDF ( 2* Number of measured CDC layer - 5 )
  Float_t  fPosAtEMC[3]  ; // ( r, th, phi) at EMC entrance.
  Float_t  fEPosAtEMC[2] ; // (dth,dphi) of EMC entrance position.
  Int_t    fNVTX    ; // Number of matched VTX hits.
  JSFVTXHit *fVTXHits[10] ; //! Pointer to linked VTX hits. 

public:
  JSFCDCTrack(){}
  virtual ~JSFCDCTrack();
  JSFCDCTrack( Int_t trkp[] );

  void GetErrorMatrix(Double_t trkd[]);
  void GetTrackParameter(Float_t trkf[]);

  Int_t GetGenID(){ return fGenID;}
  Int_t GetNVTX(){ return fNVTX;}
  JSFVTXHit *GetVTXHit(Int_t i){ return (JSFVTXHit*)fVTXHits[i];}
  void AddVTXHit(JSFVTXHit *v);

  void SetGenID(Int_t igen){ fGenID=igen; }

  void Print();
  void SetPositionAtEMC(Float_t pos[]);
  void ExtrapolateErrorAtEMC(Float_t helix[], Float_t x[], Float_t dx[]);

  ClassDef(JSFCDCTrack,1)  //A CDC Track class
};

#endif





