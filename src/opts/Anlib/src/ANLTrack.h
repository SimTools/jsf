#ifndef __ANLTRACK__
#define __ANLTRACK__
//*************************************************************************
//* ==================
//*  ANLTrack Classes
//* ==================
//*
//* (Description)
//*    Track Class for JLC analysis
//* (Requires)
//*     class TVector
//*     class JSFLTKCLTrack
//*     class ANL4DVector
//*     class JSFSIMDST, etc
//* (Provides)
//*     class ANLTrack
//* (Update Recored)
//*    1999/08/17  K.Ikematsu   Original version.
//*    1999/09/05  K.Ikematsu   Replaced LockableLVector with ANL4DVector.
//*    1999/09/13  K.Ikematsu   Added GetLTKCLTrack method.
//*    1999/09/13  K.Ikematsu   Added GetTrackPtr method.
//*    2001/07/09  K.Ikematsu   Changed IsElectron, IsMuon & GetCharge
//*                             method to virtual.
//*    2001/10/21  K.Ikematsu   Added SetColorSingletID method and
//*                             fColorSingletID member.
//*    2001/10/22  K.Ikematsu   Added TObjNum class from FlavourGetter class.
//*    2002/02/08  K.Fujii      fMSNPriHad is now a pointer.
//*				Added operator=.
//*
//* $Id$
//*************************************************************************
//
#include <iostream>
#include <stdlib.h>
#include "JSFLTKCLTrack.h"
#include "JSFSteer.h"
#include "JSFModule.h"
#include "JSFSIMDST.h"
#include "JSFGeneratorParticle.h"
#include "ANL4DVector.h"
typedef enum EFlavourGetterDetectorID {kECDC,kEEMC};

using namespace std;
//_____________________________________________________________________
//  --------------
//  ANLTrack Class
//  --------------
//
class ANLTrack : public ANL4DVector {
public:
  ANLTrack();
  ANLTrack(const TObject *track);
  ANLTrack(const TVector &pv, const TObject *ptr);
  virtual ~ANLTrack();

  virtual   Bool_t IsElectron() const;
  virtual   Bool_t IsMuon() const;
  Bool_t    IsLepton() const;
  Bool_t    IsGamma () const;
  virtual   Double_t GetCharge() const;
  Double_t  GetConeEnergy(const Double_t cth, const TObjArray *tracks) const;
  TObject  *GetTrackPtr() const;
  JSFLTKCLTrack *GetLTKCLTrack() const;
  Int_t     GetColorSingletID() const;
  void      SetColorSingletID();

  virtual   const ANLTrack & operator=(const ANLTrack & track);

private:
  void      ScanThroughDecayChain(EFlavourGetterDetectorID id,
				  JSFLTKCLTrack *ctp, Int_t i);
  Double_t  GetEGeneratorParticle(EFlavourGetterDetectorID id,
                                  JSFLTKCLTrack *ctp, Int_t i);

private:
  const TObject *fTrackPtr;       //  Pointer to JSFLTKCLTrack
  TClonesArray  *fGen;            //! Pointer to GeneratorParticles Array
  TObjArray     *fMSNPriHad;      //  Pointer to Array of primary hadron's Mother S.N
                                  //  (corresponding to PartonID)
  Int_t         fColorSingletID;  // ColorSinglet ID

  ClassDef(ANLTrack,2)  // ANLTrack class
};

//_____________________________________________________________________
//  -------------
//  TObjNum Class
//  -------------
//
// TObjNum is a simple container for an integer.
//
class TObjNum : public TObject {
private:
  Int_t  num;

public:
  TObjNum(Int_t i = 0) : num(i) {}

  ~TObjNum() {
#ifdef __DEBUG__
    cerr << "~TObjNum = " << num << endl;
#endif
  }

  void    SetNum(Int_t i) { num = i; }
  Int_t   GetNum() { return num; }
  void    Print(Option_t *) const { Printf("TObjNum = %d", num); }
  ULong_t Hash() const { return num; }
  Bool_t  IsEqual(const TObject *obj) const { return num == ((TObjNum*)obj)->num; }
  Bool_t  IsSortable() const { return kTRUE; }
  Int_t   Compare(const TObject *obj) const {
    if (num > ((TObjNum*)obj)->num)
      return 1;
    else if (num < ((TObjNum*)obj)->num)
      return -1;
    else
      return 0;
  }
};

#endif
