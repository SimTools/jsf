#ifndef __JSFHelicalTrack__
#define __JSFHelicalTrack__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFHelicalTrack                                                      //
//                                                                      //
// Helical Track parameter class
//                                                                      //
//$Id:
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#include "TObject.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TVector.h"
#include "THelix.h"

#ifndef __JSFBasicClasses__
#include "JSFBasicClasses.h"
#endif

static const Double_t kLightVelocity=2.99792458E8; // 
static const Double_t kGiga=1.0E9 ; //  

class JSFCDCTrack;

class JSFHelixParameter_f { 
public:
  Float_t dr ; 
  Float_t phi0; 
  Float_t kappa;
  Float_t dz;
  Float_t tanl;
  JSF3DV_f pivot;
} ;

class JSFHelixParameter { 
public:
  Double_t dr ; 
  Double_t phi0; 
  Double_t kappa;
  Double_t dz;
  Double_t tanl;
  JSF3DV   pivot;
} ;

class JSFHelixErrorMatrix {
public:
  Double_t drdr;  
  Double_t drp0;  Double_t p0p0;
  Double_t drk;   Double_t p0k;   Double_t kk;
  Double_t drdz;  Double_t p0dz;  Double_t kdz; Double_t dzdz;
  Double_t drtl;  Double_t p0tl;  Double_t ktl; Double_t dztl; Double_t tltl;
} ;

union JSFHelixErrorMatrix_u {
    Double_t data[15];
    JSFHelixErrorMatrix m;
} ;


class JSFHelicalTrack {
protected:
  JSFHelixParameter     fHelix; //!
  JSFHelixErrorMatrix_u fError; //!
  Bool_t fWithError;  //! True when error matrix is meaningfull
  Double_t fChisq  ;  //! Chisq of the fit.
  Double_t fCL  ;     //! Fit confidence level
  Int_t    fNDF ;     //! Number of degree of freedom.

  Double_t fBfield;   //! Solenoidal Magnetic field ( in unit of kGauss )
  Double_t fAlpha ;   //! = ( 1/c Bfield);

  inline Bool_t TestBfield(){ 
    if( fAlpha > 0.0 ) return kTRUE;
    printf("Do JSFHelicalTrack::SetBfield(...) prior to helix manupilation.\n");
    return kFALSE;
  }

public:
  JSFHelicalTrack(Float_t helix[], Float_t pivot[],
		  Int_t ndf=-1, Double_t chisq=0, Double_t cl=0,
		  Double_t *error=0 );
  JSFHelicalTrack(Float_t px, Float_t py, Float_t pz, Float_t bz, 
		  Float_t charge, 
		  Float_t x=0, Float_t y=0, Float_t z=0); 
  JSFHelicalTrack(JSFHelixParameter hlx);
  JSFHelicalTrack(JSF3DV p1, JSF3DV p2, JSF3DV p3, Double_t bfield);

  virtual ~JSFHelicalTrack(){}

  Double_t GetBField(){ return fBfield; }
  void Print();
  Int_t IntersectOf2Circle(JSF2DV x1, Double_t r1, JSF2DV x2, Double_t r2,
      JSF2DV &c1, JSF2DV &c2);
  Bool_t IntersectWithCylinder(JSFRPhiZ ref, JSFRPhiZ &p);

  Int_t OriginToCylinder(Double_t Rcyl, Double_t Zcyl, 
			 Double_t &phi0, Double_t &phi1, Int_t Maxloop=3,
			 Double_t StartX=0, Double_t StartY=0);

  inline JSFHelixParameter GetHelixParameter(){return fHelix; }
  inline void SetHelix(Double_t *par){
    fHelix.dr=par[0];fHelix.phi0=par[1];fHelix.kappa=par[2];
    fHelix.dz=par[3];fHelix.tanl=par[4];}
  inline void SetErrorMatrix(Int_t ndf, 
			     Double_t chisq, Double_t cl, Double_t *emat){
    fNDF=ndf; fChisq=chisq; fCL=cl;
    Int_t i;  for(i=0;i<15;i++){ fError.data[i]=emat[i]; } }

  inline void SetBfield(Double_t field) {
    fBfield=field; fAlpha=kGiga/kLightVelocity*100.0/(fBfield/10.0); }

  inline JSF3DV GetCoordinate(Double_t phi){
     Double_t r=fAlpha/fHelix.kappa;
     Double_t x=fHelix.pivot.x + fHelix.dr*TMath::Cos(fHelix.phi0)
               +r*(TMath::Cos(fHelix.phi0)- TMath::Cos(fHelix.phi0+phi));
     Double_t y=fHelix.pivot.y + fHelix.dr*TMath::Sin(fHelix.phi0)
               +r*(TMath::Sin(fHelix.phi0)- TMath::Sin(fHelix.phi0+phi));
     Double_t z=fHelix.pivot.z + fHelix.dz - r*fHelix.tanl*phi;
     return JSF3DV(x,y,z);
  }

  inline JSF2DV GetCenter(){
     Double_t r=fAlpha/fHelix.kappa;
     Double_t x=fHelix.pivot.x + (fHelix.dr+r)*TMath::Cos(fHelix.phi0);
     Double_t y=fHelix.pivot.y + (fHelix.dr+r)*TMath::Sin(fHelix.phi0);
     return JSF2DV(x,y);
  }

  inline Double_t GetRadius(){ return fAlpha/fHelix.kappa; }
  inline Double_t GetCharge(){ return TMath::Sign(1.0,fHelix.kappa); }

  inline JSF3DV GetMomentum(Double_t phi){
    Double_t kinv=1.0/TMath::Abs(fHelix.kappa);
    return JSF3DV( -kinv*TMath::Sin(fHelix.phi0+phi),
		    kinv*TMath::Cos(fHelix.phi0+phi),
		    kinv*fHelix.tanl);
  }

  inline Double_t GetDeflectionAngle2D(JSF2DV p){
    Double_t csphi0=TMath::Cos(fHelix.phi0);
    Double_t snphi0=TMath::Sin(fHelix.phi0);
    Double_t r=fAlpha/fHelix.kappa;
    Double_t dy= ( fHelix.pivot.y + (fHelix.dr+r)*TMath::Sin(fHelix.phi0) - p.y )/r;
    Double_t dx= ( fHelix.pivot.x + (fHelix.dr+r)*TMath::Cos(fHelix.phi0) - p.x )/r;
    Double_t ang=TMath::ATan2( dy*csphi0 - dx*snphi0, dx*csphi0 + dy*snphi0 );
    return ang;
  }

  THelix *GetTHelix(Double_t rcyl, Double_t zcyl);
    
			  

};

#endif





