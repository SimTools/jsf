#ifndef __JSFCDCTrack__
#define __JSFCDCTrack__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFCDCTrack                                                          //
//                                                                      //
// CDC Track parameter information.                                     //
//                                                                      //
//$Id$
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
#ifndef __JSFQuickSimParam__
#include "JSFQuickSimParam.h"
#endif
#ifndef __JSFHelicalTrack__
#include "JSFHelicalTrack.h"
#endif

class JSFQuickSimParam;

const Int_t kMaxCDCTracks=500;
const Int_t kMaxVTXAssoc=22;

class JSFCDCTrack : public TObject {
friend class JSFQuickSimBuf;
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
  JSFVTXHit *fVTXHits[kMaxVTXAssoc] ; //! Pointer to linked VTX hits. 

public:
  JSFCDCTrack(){}
  virtual ~JSFCDCTrack();
  JSFCDCTrack( Int_t trkp[] );
  JSFCDCTrack( Float_t trkf[], Double_t trkd[] );
  JSFCDCTrack(JSFCDCTrack& t );

  void GetErrorMatrix(Double_t trkd[]);
  void GetTrackParameter(Float_t trkf[]);

  Int_t GetGenID(){ return fGenID;}
  Int_t GetNVTX(){ return fNVTX;}
  JSFVTXHit *GetVTXHit(Int_t i){ return (JSFVTXHit*)fVTXHits[i];}
  void AddVTXHit(JSFVTXHit *v);

  void SetGenID(Int_t igen){ fGenID=igen; }

  virtual void Print();
  void SetPositionAtEMC(Float_t pos[]);
  void GetPositionAtEMC(Float_t pos[3], Float_t poserr[2]);
  void ExtrapolateErrorAtEMC(Float_t helix[], Float_t x[], Float_t dx[]);

  void MovePivot(Float_t pivot[], Float_t bfield);
  void AddMSError(Float_t xrad, Float_t deltakappa=0.0);
  Bool_t MovePivotToIP(JSFQuickSimParam *spar);

  Int_t GetCharge(){ return fCharge;}
  Int_t GetNDF(){ return fNDF;}
  Float_t GetPx(){ return fP[0];}
  Float_t GetPy(){ return fP[1];}
  Float_t GetPz(){ return fP[2];}
  Float_t GetP(){ return fE;}
  Float_t GetX(){ return fX[0];}
  Float_t GetY(){ return fX[1];}
  Float_t GetZ(){ return fX[2];}

  TVector GetPV(){ TVector p(4); p(0)=fE; p(1)=fP[0]; p(2)=fP[1];
                   p(3)=fP[2]; return p;}
  TVector GetXV(){ TVector x(4); x(0)=0.0 ; x(1)=fX[0]; x(2)=fX[1];
                   x(3)=fX[2]; return x;}

  void GetHelix(Float_t helix[]){ 
     Int_t i; for(i=0;i<5;i++){ helix[i]=fHelix[i];} }

  void GetPivot(Float_t pivot[]){
     Int_t i; for(i=0;i<3;i++){ pivot[i]=fPivot[i];} }

  void GetError(Double_t err[]){
     Int_t i; for(i=0;i<15;i++){ err[i]=fError[i];} }

  void GetPosAtEMC(Float_t pos[]){
     Int_t i; for(i=0;i<3;i++){ pos[i]=fPosAtEMC[i];} 
              for(i=0;i<2;i++){ pos[i+3]=fEPosAtEMC[i];} }

  JSFHelixParameter GetHelix(){
     JSFHelixParameter hlx;
     hlx.dr=fHelix[0]; hlx.phi0=fHelix[1] ; hlx.kappa = fHelix[2];
     hlx.dz=fHelix[3]; hlx.tanl=fHelix[4];
     hlx.pivot.x=fPivot[0]; hlx.pivot.y=fPivot[1]; hlx.pivot.z=fPivot[2];
     return hlx;
  }

  JSFHelicalTrack GetHelicalTrack(){
     return  JSFHelicalTrack(fHelix, fPivot, fNDF, 1.0, 0.0, fError); 
  }

  JSF3DV_f GetPivot(){ JSF3DV_f pivot; pivot.x=fPivot[0]; 
         pivot.y=fPivot[1]; pivot.z=fPivot[2];  return pivot; }

  ClassDef(JSFCDCTrack,1)  //A CDC Track class
};

#endif





