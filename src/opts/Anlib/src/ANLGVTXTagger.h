#ifndef __ANLGVTXTAGGER__
#define __ANLGVTXTAGGER__
//*************************************************************************
//* ====-================
//*  ANLGVTXTagger Class
//* ====-================
//*
//* (Description)
//*    A vertex tagging class using generator information.
//* (Requires)
//*     class ANLJetFinder
//*     class ANLTrack
//*     class JSFSIMDST, etc.
//* (Provides)
//*     class ANLGVTXTagger
//* (Update Recored)
//*    2001/07/07  K.Ikematsu    Original version
//*    2001/07/13  K.Ikematsu    Added public Getb method
//*    2001/07/31  K.Ikematsu    Removed CDCTrack pointer
//*                              from neutral track in mixed CAL cluster
//*                              (This causes double count of helix tracks)
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
//  -------------------
//  ANLGVTXTagger Class
//  -------------------
//
class ANLGVTXTagger {
public:
  ANLGVTXTagger(Double_t nsig=0., Int_t noffvtrks=1,
                Double_t bfield=2., Bool_t flag=kFALSE)
             : fNsigCut(nsig), fNOffVtrks(noffvtrks),
               fBfield(bfield), fDEBUG(flag) {
    JSFSIMDST     *sds     = (JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
    JSFSIMDSTBuf  *evt     = (JSFSIMDSTBuf*)sds->EventBuf();
    fParam = sds->Param();
    fGen   = evt->GetGeneratorParticles();
  }                                                // default constructor

  virtual ~ANLGVTXTagger(){}                       // default destructor

  Bool_t   operator()(const ANLJet &jet);
  Double_t GetNsig() const { return fNsigCut; }
  Int_t    GetNOffVtrk() const { return fNOffVtrks; }
  void     SetNsig(Double_t nsig);                 // sets nsig
  void     SetNOffVtrk(Int_t noffvtrks);           // sets noffvtrks
  void     SetBfield(Double_t bfield);             // sets bfield
  void     SetDebug(Bool_t flag);                  // sets debug flag

  //private:
  Double_t Getb(const ANLTrack &t);
  Double_t Getbnorm(const ANLTrack &t);

private:
  JSFQuickSimParam *fParam;	  //! Pointer to QuickSimParam
  TClonesArray     *fGen;         //! Pointer to GeneratorParticles
  Double_t          fNsigCut;     //  Cut value of Nsig
  Int_t             fNOffVtrks;   //  No. of off-vertex tracks
  Double_t          fBfield;      //  B field [Tesla]
  Bool_t            fDEBUG;       //  Debug flag

  ClassDef(ANLGVTXTagger, 1) 	  // ANLGVTXTagger Example
};

#endif
