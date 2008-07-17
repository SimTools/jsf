#ifndef ANLTRACK_H
#define ANLTRACK_H
//*************************************************************************
//* ==================
//*  ANLTrack Classes
//* ==================
//*
//* (Description)
//*    Track Class for JLC analysis
//* (Requires)
//*     class TVector
//*     class TObjInt
//*     class JSFSIMDST, etc
//*     class JSFLTKCLTrack
//*     class ANL4DVector
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
//*                             Added operator=.
//*    2008/07/15  K.Ikematsu   Changed EFlavourGetterDetectorID to
//*                             EDetectorID.
//*                             Moved TObjNum class to $LEDAROOT/src/utils/.
//*    2008/07/17  K.Ikematsu   Replaced TObjNum with TObjInt.
//*
//* $Id$
//*************************************************************************
//
#include <iostream>
#include <stdlib.h>
#include "TObjInt.h"
#include "JSFSteer.h"
#include "JSFModule.h"
#include "JSFSIMDST.h"
#include "JSFGeneratorParticle.h"
#include "JSFLTKCLTrack.h"
#include "ANL4DVector.h"

enum EDetectorID {kECDC, kEEMC};

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
  void      ScanThroughDecayChain(EDetectorID id,
                                  JSFLTKCLTrack *ctp, Int_t i);
  Double_t  GetEGeneratorParticle(EDetectorID id,
                                  JSFLTKCLTrack *ctp, Int_t i);

private:
  const TObject *fTrackPtr;       //  Pointer to JSFLTKCLTrack
  TClonesArray  *fGen;            //! Pointer to GeneratorParticles Array
  TObjArray     *fMSNPriHad;      //  Pointer to Array of primary hadron's Mother S.N
                                  //  (corresponding to PartonID)
  Int_t         fColorSingletID;  // ColorSinglet ID

  ClassDef(ANLTrack, 3)  // ANLTrack class
};

#endif
