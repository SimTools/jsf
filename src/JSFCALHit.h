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

const Int_t kMaxCalHits=1000;


//************************************************************
class JSFCALHit : public TObject {
protected:
  Float_t fEMEnergy; // Energy deposited by HD paarticle (GeV)
  Float_t fHDEnergy; // Energy deposited by EM paarticle (GeV)
  Int_t   fCellID;   // Packed Cal hit info.

public:
  JSFCALHit() {}
  virtual ~JSFCALHit() {}

  JSFCALHit(Int_t cellid, Int_t iemenergy, Int_t ihdenergy)
    { fCellID = cellid ; fEMEnergy = (Float_t)iemenergy/1000.0 ; 
                         fHDEnergy = (Float_t)ihdenergy/1000.0 ; }
  JSFCALHit(Int_t cellid, Float_t emenergy, Float_t hdenergy)
    { fCellID = cellid ; fEMEnergy=emenergy ; fHDEnergy=hdenergy ; }

  virtual Int_t GetCellID(){ return fCellID;}
  virtual Float_t GetEMEnergy(){ return fEMEnergy; }
  virtual Float_t GetHDEnergy(){ return fHDEnergy; }
  virtual Float_t GetEnergy(){ return fEMEnergy+fHDEnergy; }
  void AddEnergy(Float_t em, Float_t hd){
     fEMEnergy=em;  fHDEnergy=hd; 
  }

  ClassDef(JSFCALHit,2)  // Calorimeter Hit Cell.
};

//************************************************************
class JSFEMCHit : public JSFCALHit {
public:
  JSFEMCHit() {}
  JSFEMCHit(Int_t cellid, Int_t iemenergy, Int_t ihdenergy)
   : JSFCALHit(cellid, iemenergy, ihdenergy){}
  virtual ~JSFEMCHit() {}

  ClassDef(JSFEMCHit,2)  // EMC Calorimeter HitCell.
};

//************************************************************
class JSFHDCHit : public JSFCALHit {
public:
  JSFHDCHit() {}
  JSFHDCHit(Int_t cellid, Int_t iemenergy, Int_t ihdenergy)
   : JSFCALHit(cellid, iemenergy, ihdenergy){}
  virtual ~JSFHDCHit() {}

  ClassDef(JSFHDCHit,2)  // HDC Calorimeter HitCell.
};

#endif

