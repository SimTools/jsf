#ifndef __JSFCALHit__
#define __JSFCALHit__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFCALHit                                                            //
//                                                                      //
// Calorimeter's Hit cell information                                   //
//
//$Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TVector.h"

const Int_t kMaxCalHits=500;


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

