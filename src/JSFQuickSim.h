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
 
class JSFQuickSim;

// ******* JSFQuickSimBuf class ********************************
class JSFQuickSimBuf : public JSFEventBuf {
private:
   Int_t            fNTracks   ;  // Number of particles 
   TClonesArray    *fTracks    ;  // Pointers to Particles
   Int_t            fNCDCTracks;  // Number of CDC tracks.
   TClonesArray    *fCDCTracks ;  // Pointer to CDC Tracks
   Int_t            fNVTXHits  ;  // Number of VTXHits
   TClonesArray    *fVTXHits   ;  // Pointers to VTX Hits
   Int_t            fNEMCHits  ;  // Number of EMC hit cells
   TClonesArray    *fEMCHits   ;  // Pointers to EMC Hit cells 
   Int_t            fNHDCHits  ;  // Number of HDC hit cells
   TClonesArray    *fHDCHits   ;  // Pointers to EMC Hit cells 

   void SetPointers() ; // Set pointers among Hits/Track classes.

   Bool_t MakeJSFLTKCLTracks();
   Bool_t MakeCALHits();
   Bool_t MakeVTXHits();
   Bool_t MakeCDCTracks();

public:
   JSFQuickSimBuf(const char *name="JSFQuickSimBuf",
		   const char *title="JSF QuickSim event class",
		   JSFQuickSim *sim=NULL);
   virtual      ~JSFQuickSimBuf();

   Bool_t MakeEventBuf();

  void SetNTracks(Int_t nt){ fNTracks=nt;}
  Int_t GetNtracks(){ return fNTracks; } // kept for backward compatibility
  Int_t GetNTracks(){ return fNTracks; }
  TClonesArray *GetTracks(){ return fTracks; }

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


  ClassDef(JSFQuickSimBuf, 1) // QuickSim event buffer class.

};

// ******* JSFQuickSim class *********************************
class JSFQuickSim : public JSFModule {
protected:
   JSFQuickSimParam  *fParam ; //! Parameters for JSFQuickSim
public:
   JSFQuickSim(const char *name="JSFQuickSim", 
	       const char *title="JSF Quick Simulator");
   virtual      ~JSFQuickSim();

   virtual Bool_t  Initialize(); 
   virtual Bool_t  Process(Int_t event);
   virtual Bool_t  BeginRun(Int_t nrun);
   JSFQuickSimParam *Param(){ return fParam; }

   Bool_t TBPUTGeneratorParticles();
   Bool_t ReviseGeneratorInfo();

   virtual  void MakeBranch(TTree *tree); // Make branch for the module 
   virtual  void SetBranch(TTree *tree);  // Set Branch address for the module

   ClassDef(JSFQuickSim,1)  // Debug structure
};


#endif

