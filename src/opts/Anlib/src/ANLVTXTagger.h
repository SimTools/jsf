#ifndef __ANLVTXTAGGER__
#define __ANLVTXTAGGER__
//*************************************************************************
//* ====================
//*  ANLVTXTagger Class
//* ====================
//*
//* (Description)
//*    A very primitive vertex tagging class.
//* (Requires)
//*     class ANLJetFinder
//*     class ANLTrack
//*     class JSFSIMDST, etc.
//* (Provides)
//*     class ANLVTXTagger
//* (Update Recored)
//*    1999/10/09  K.Fujii      Original version.
//*    1999/10/18  K.Ikematsu   Implement Ks, Lambda, Sigma removal.
//*    2000/03/18  K.Ikematsu   Added SetNsig and SetNoff method.
//*    2000/03/18  K.Ikematsu   Added GetNsig and GetNoff method.
//*    2001/07/13  K.Ikematsu   Added public Getb method.
//*    2001/07/31  K.Ikematsu   Removed CDCTrack pointer
//*                             from neutral track in mixed CAL cluster.
//*                             (This causes double count of helix tracks)
//*
//* $Id$
//*************************************************************************
//
#include "JSFSteer.h"
#include "JSFModule.h"
#include "JSFSIMDST.h"
#include "JSFGeneratorParticle.h"
#include "JSFCDCTrack.h"
#include "ANLJetFinder.h"
#include "ANLTrack.h"
//_____________________________________________________________________
//  ------------------
//  ANLVTXTagger Class
//  ------------------
//
class ANLVTXTagger {
public:
  ANLVTXTagger(Double_t nsig=0., Int_t noffvtrks=1)
             : fNsigCut(nsig), fNOffVtrks(noffvtrks) {
    JSFSIMDST     *sds     = (JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
    JSFSIMDSTBuf  *evt     = (JSFSIMDSTBuf*)sds->EventBuf();
    fParam = sds->Param();
    fGen   = evt->GetGeneratorParticles();
  }                                                // default constructor

  virtual ~ANLVTXTagger(){}                        // default destructor

  Bool_t   operator()(const ANLJet &jet);
  Double_t GetNsig() const { return fNsigCut; }
  Int_t    GetNOffVtrk() const { return fNOffVtrks; }
  void     SetNsig(Double_t nsig);                 // sets nsig
  void     SetNOffVtrk(Int_t noffvtrks);           // sets noffvtrks

  //private:
  Double_t Getb(const ANLTrack &t);
  Double_t Getbnorm(const ANLTrack &t);

private:
  JSFQuickSimParam *fParam;	  //! Pointer to QuickSimParam
  TClonesArray     *fGen;         //! Pointer to GeneratorParticles
  Double_t          fNsigCut;     //  Cut value of Nsig
  Int_t             fNOffVtrks;   //  No. of off-vertex tracks

  ClassDef(ANLVTXTagger, 1) 	  // ANLVTXTagger Example
};

#endif
