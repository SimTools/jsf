#ifndef __FLAVOURGETTER__
#define __FLAVOURGETTER__
//*************************************************************************
//* =====================
//*  FlavourGetter Class
//* =====================
//*
//* (Description)
//*    A flavour of jets getting class
//* (Requires)
//*     class ANLJetFinder
//*     class ANLTrack
//*     class JSFSIMDST, etc
//* (Provides)
//*     class FlavourGetter
//* (Update Recored)
//*    2000/02/13  K.Ikematsu   Original version
//*    2000/04/28  K.Ikemtasu   Modified GetPrimaryHadronPID method
//*    2001/07/20  K.Ikemtasu   Added GetPrimaryHadronSN method
//*    2001/07/20  K.Ikemtasu   Added GetPartonID method
//*    2001/07/26  K.Ikematsu   Added TTL4JFlavourGetter class
//*    2001/07/31  K.Ikematsu   Supported to search generator particles
//*                             contributing to the EM cluster (gamma)
//*    2001/08/02  K.Ikematsu   Added fPIDOffVT and fSNOffVT members
//*    2001/10/23  K.Ikematsu   Moved TObjNum class to ANLTrack class
//*    2001/12/18  K.Ikematsu   Added SetThetaCut method
//*
//* $Id$
//*************************************************************************
//
#include <stdlib.h>
#include <stdlib.h>
#include "JSFSteer.h"
#include "JSFModule.h"
#include "JSFSIMDST.h"
#include "JSFGeneratorParticle.h"
#include "JSFSpring.h"
#include "ANLTrack.h"
#include "ANLJetFinder.h"
////typedef enum FlavourGetter::EFlavourGetterDetectorID {kECDC,kEEMC};
//_____________________________________________________________________
//  -------------------
//  FlavourGetter Class
//  -------------------
//
class FlavourGetter {
public:
  FlavourGetter() {
    JSFSIMDST     *sds = (JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
    JSFSIMDSTBuf  *evt = (JSFSIMDSTBuf*)sds->EventBuf();
    fGen   = evt->GetGeneratorParticles();
    fPIDPriHad.Delete();
    fSNPriHad.Delete();
    fMSNPriHad.Delete();
    fPIDOffVT.Delete();
    fSNOffVT.Delete();
  }                                                // default constructor

  virtual  ~FlavourGetter() {}                     // default destructor

  virtual  Int_t operator()(const ANLJet &jet);

protected:
  void      SetDataArray(const ANLJet &jet);
  TObjArray &GetPrimaryHadronPID();
  TObjArray &GetPrimaryHadronSN();
  TObjArray &GetPartonID();
  TObjArray &GetOffVertexHadronPID();
  TObjArray &GetOffVertexHadronSN();
  void      DebugPrint(const Char_t *opt="Brief") const;

private:
  void      SearchPrimaryHadron(const ANLTrack &t);
  void      ScanThroughDecayChain(EFlavourGetterDetectorID id,
				  JSFLTKCLTrack *ctp, Int_t i);
private:
  TClonesArray     *fGen;         //! Pointer to GeneratorParticles Array
  TObjArray         fPIDPriHad;   //  Array of primary hadron's PID
  TObjArray         fSNPriHad;    //  Array of primary hadron's S.N
  TObjArray         fMSNPriHad;   //  Array of primary hadron's Mother S.N
                                  //  (corresponding to PartonID)
  TObjArray         fPIDOffVT;    //  Array of off-vertex hadron's PID
  TObjArray         fSNOffVT;     //  Array of off-vertex hadron's S.N

  ClassDef(FlavourGetter, 2)      //  FlavourGetter class
};

//_____________________________________________________________________
//  ------------------------
//  TTL4JFlavourGetter Class
//  ------------------------
//
class TTL4JFlavourGetter : public FlavourGetter {
public:
  TTL4JFlavourGetter(JSFSpring &sp, Double_t theta=10.)
                   : fThetaCut(theta) {
    JSFSpringBuf  *spptn   = (JSFSpringBuf *)sp.EventBuf();
    fSpgen = spptn->GetPartons();
  }                                                // default constructor

  Int_t    operator()(const ANLJet &jet);
  void     SetThetaCut(Double_t theta);

private:
  TClonesArray     *fSpgen;       //! Pointer to SpringParton
  Double_t          fThetaCut;    //  ThetaCut value

  ClassDef(TTL4JFlavourGetter, 2) //  TTL4JFlavourGetter class
};

#endif
