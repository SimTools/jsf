#ifndef __JSFCALHit__
#define __JSFCALHit__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFCALHit                                                            //
//                                                                      //
// Calorimeter's Hit cell information                                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TVector.h"

typedef enum { kEMC , kHDC } EJSFCALType ;
const Int_t kMaxCalHits=500;

//************************************************************
class JSFCALGeoParam : public TObject {
protected:
  EJSFCALType  fType ; // Calorimeter type
  Float_t fRmin ; // Inner radius of end-cap calorimeter
  Float_t fRmax ; // radius of barrel and outer radius of endcap.

  Float_t fPhiStep ; // Phi step/bin

  Float_t fBZetaMin ;   // Barrel Zeta min.
  Float_t fBZetaStep ;  // Barrel Zeta step/bin

//                     costh=( 1-exp(2*zeta))/(1+exp(2*zeta))
  Float_t fMZetaMin ;  // Zeta min of -Zee endcap.
  Float_t fMZetaStep ; // Zeta step of -Zee endcap.
  Float_t fPZetaMin ;  // Zeta min of +Zee endcap.
  Float_t fPZetaStep ; // Zeta step of +Zee endcap.

public:
  JSFCALGeoParam(EJSFCALType type, Int_t nphi, Int_t ntheta, Int_t nrad,
 	       Float_t rmin, Float_t rmax,  Float_t zminus, Float_t zplus );
  virtual ~JSFCALGeoParam(){}

  void SetGeoParam(EJSFCALType type, Int_t nphi, Int_t ntheta, Int_t nrad,
 	       Float_t rmin, Float_t rmax,  Float_t zminus, Float_t zplus );

  ClassDef(JSFCALGeoParam,1)  // Calorimeter Geometry parameter.
};


//************************************************************
class JSFCALHit : public TObject {
protected:
  Float_t fEnergy;  // Energy depositted in this cell (GeV)
  Int_t   fCellID;  // Packed Cal hit info.

public:
  JSFCALHit() {}
  virtual ~JSFCALHit() {}

  JSFCALHit(Int_t ienergy, Int_t cellid)
    { fCellID = cellid ; fEnergy = (Float_t)ienergy/1000.0 ; }

  virtual Int_t GetCellID(){ return fCellID;}
  virtual Int_t GetIEnergy(){ return (Int_t)(fEnergy*1000.0);}

  ClassDef(JSFCALHit,1)  // Calorimeter Hit Cell.
};

//************************************************************
class JSFEMCHit : public JSFCALHit {
public:
  JSFEMCHit() {}
  JSFEMCHit(Int_t cellid, Int_t ienergy)
   : JSFCALHit(cellid, ienergy){}
  virtual ~JSFEMCHit() {}

  ClassDef(JSFEMCHit,1)  // EMC Calorimeter HitCell.
};

//************************************************************
class JSFHDCHit : public JSFCALHit {
public:
  JSFHDCHit() {}
  JSFHDCHit(Int_t cellid, Int_t ienergy)
   : JSFCALHit(cellid, ienergy){}
  virtual ~JSFHDCHit() {}

  ClassDef(JSFHDCHit,1)  // HDC Calorimeter HitCell.
};

#endif

