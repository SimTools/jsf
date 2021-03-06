#ifndef __JSFQuickSim__
#define __JSFQuickSim__

//////////////////////////////////////////////////////////////////////////
//
// JSFQuickSim
//
// A template for JSFQuickSim
//
//$Id$
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TDatime.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __JSFBasicClasses__
#include "JSFBasicClasses.h"
#endif
#ifndef __JSFLTKCLTrack__
#include "JSFLTKCLTrack.h"
#endif
#ifndef __JSFCALHit__
#include "JSFCALHit.h"
#endif
#ifndef __JSFVTXHit__
#include "JSFVTXHit.h"
#endif
#ifndef __JSFCDCTrack__
#include "JSFCDCTrack.h"
#endif
#ifndef __JSFQuickSimParam__
#include "JSFQuickSimParam.h"
#endif
#ifndef __JSFGnerator__
#include "JSFGenerator.h"
#endif
 
class JSFQuickSim;

// ******* JSFQuickSimBuf class ********************************
class JSFQuickSimBuf : public JSFEventBuf {
private:
   Int_t            fNTracks   ;  // Number of particles 
   Int_t            fNCDCTracks;  // Number of CDC tracks.
   Int_t            fNVTXHits  ;  // Number of VTXHits
   Int_t            fNEMCHits  ;  // Number of EMC hit cells
   Int_t            fNHDCHits  ;  // Number of HDC hit cells

   TObjArray       *fTracks    ;  //-> Pointers to Particles
   TClonesArray    *fCDCTracks ;  //-> Pointer to CDC Tracks
   TClonesArray    *fVTXHits   ;  //-> Pointers to VTX Hits
   TClonesArray    *fEMCHits   ;  //-> Pointers to EMC Hit cells 
   TClonesArray    *fHDCHits   ;  //-> Pointers to EMC Hit cells 

   Int_t      fCDCTrackIsCDCVTX; //! 0 when CDC track is not CDC+VTX combined track.
   
   void SetPointers() ; 

   Bool_t MakeJSFLTKCLTracks();
   Bool_t MakeJSFLTKCLTrackPointers();
   Bool_t MakeCALHits();
   Bool_t MakeVTXHits();
   Bool_t MakeCDCTracks();

   void AppendCALHits(JSFQuickSimBuf *src);
   void AppendLTKCLTracks(JSFQuickSimBuf *src, Int_t numgp, JSFGeneratorBuf *gbuf);
   void SetTrackNSig();

public:
   JSFQuickSimBuf();
   JSFQuickSimBuf(const char *name,
		   const char *title="JSF QuickSim event class",
		   JSFQuickSim *sim=NULL);
   virtual      ~JSFQuickSimBuf();

   Bool_t MakeEventBuf();
   void SetGeneratorPointers(JSFGeneratorBuf *ingen=NULL) ; // Set pointers among Hits/Track classes.

  void SetNTracks(Int_t nt){ fNTracks=nt;}
  Int_t GetNtracks(){ return fNTracks; } // kept for backward compatibility
  Int_t GetNTracks(){ return fNTracks; }
  //   TClonesArray *GetTracks(){ return fTracks; }
  TObjArray *GetTracks(){ return fTracks; }

  void SetNCDCTracks(Int_t nt){ fNCDCTracks=nt;}
  Int_t GetNCDCTracks(){ return fNCDCTracks; }
  TClonesArray *GetCDCTracks(){ return fCDCTracks; }

  void SetNVTXHits(Int_t nt){ fNVTXHits=nt;}
  Int_t GetNVTXHits(){ return fNVTXHits; }
  TClonesArray *GetVTXHits(){ return fVTXHits; }

  void SetNEMCHits(Int_t nt){ fNEMCHits=nt;}
  Int_t GetNEMCHits(){ return fNEMCHits; }
  TClonesArray *GetEMCHits(){ return fEMCHits; }

  void SetNHDCHits(Int_t nt){ fNHDCHits=nt;}
  Int_t GetNHDCHits(){ return fNHDCHits; }
  TClonesArray *GetHDCHits(){ return fHDCHits; }

  void Clear(const Option_t *opt="");
  void Delete(const Option_t *opt="");
  void Append(JSFQuickSimBuf *src, Int_t numgp, JSFGeneratorBuf *gsrc);


  ClassDef(JSFQuickSimBuf, 3) // QuickSim event buffer class.

};

// ******* JSFQuickSim class *********************************
class JSFQuickSim : public JSFModule {
protected:
   JSFQuickSimParam  *fParam ; //! Parameters for JSFQuickSim
   Int_t fSMRRND;   // Random seed for smearing
   Int_t fSWMRND;   // Random seed for swiming
   Double_t fXAngle; // XAngle
   Double_t fSigmaX; // Beam spread in X direction
   Double_t fSigmaZ; // Beam spread in Z direction
   Int_t    fBeamShape; // 0 ( No spread ), 1=Gausian, 2=flat
public:
   JSFQuickSim(const Char_t *name="JSFQuickSim", 
	       const Char_t *title="JSF Quick Simulator",
	       const Char_t *opt="");
   virtual      ~JSFQuickSim();

   virtual Bool_t  Initialize(); 
   virtual Bool_t  Process(Int_t event);
   virtual Bool_t  BeginRun(Int_t nrun);
   virtual Bool_t  EndRun();
   virtual Bool_t  GetLastRunInfo();
   JSFQuickSimParam *Param(){ return fParam; }

   inline Double_t GetXAngle(){ return fXAngle; }
   inline Double_t GetSigmaX(){ return fSigmaX; }
   inline Double_t GetSigmaZ(){ return fSigmaZ; }
   inline Int_t    GetBeamShape(){ return fBeamShape; }

   Bool_t TBPUTGeneratorParticles();
   Bool_t ReviseGeneratorInfo();
   Bool_t BoostInitial();
   
   inline Int_t GetSMRRND(){ return fSMRRND;}
   inline Int_t GetSWMRND(){ return fSWMRND;}
   inline void SetSMRRND(Int_t i){ fSMRRND=i; }
   inline void SetSWMRND(Int_t i){ fSWMRND=i;}	

   virtual  void MakeBranch(TTree *tree); // Make branch for the module 
   virtual  void SetBranch(TTree *tree);  // Set Branch address for the module

   ClassDef(JSFQuickSim,2)  // Quick Simulator
};


#endif

