#ifndef __JSFQuickSim__
#define __JSFQuickSim__

//////////////////////////////////////////////////////////////////////////
//
// JSFQuickSim
//
// A template for JSFQuickSim
//
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TDatime.h"

#ifndef __JSFModule__
#include "JSFModule.h"
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

// ******* JSFQuickSimParam *********
class JSFQuickSimParam : public TNamed {
public:
   Char_t  fParamFile[256] ; // Where to get simulator parameter.
   Int_t fSeed ;    // Seed of random variable;
   Float_t fBfield; // Bfield (kgauss).
   Float_t fTrack[8]; // Parameter for tracking device.
   Float_t fEMCal[11]; // Parameter for tracking device.
   Float_t fHDCal[11]; // Parameter for tracking device.
   Int_t   fNSMPVX   ; // Number of VTX layer.
   Float_t fDPHIVX   ; // VTX-phi pitch (cm)                        
   Float_t fDZEEVX   ; // VTX-Z   pitch (cm)                        
   Float_t fVTXLayer[10][4]; // VTX Layer
   Float_t fVTXError[5];     // VTX resolution parameter
   Int_t   fNERRVX;          // VTX Error flag. 
   Float_t fCLSPAR[4][5]   ; // Clustering parameter.
   Float_t fCMBCUT[3] ; // ADXEVT, ENSGEM, ENSGHD for CMBCUT

   JSFCALGeoParam *fEMCGeom; //! EMC Geometry ( set by SetSmearParam() )
   JSFCALGeoParam *fHDCGeom; //! HDC Geometry
public:
   JSFQuickSimParam();
   ~JSFQuickSimParam();

   void SetSwimParam() ; // Set swimmer parameters
   void SetSmearParam() ; // Set smear parameters
   void ReadParamDetector(Char_t *file) ;// Read detector parameter
					      
   virtual JSFCALGeoParam *GetEMCGeom(){ return fEMCGeom; }  
   virtual JSFCALGeoParam *GetHDCGeom(){ return fHDCGeom; }  

   Int_t   GetVTXNLayer(){ return fNSMPVX; }
   Float_t GetVTXPhiPitch(){ return fDPHIVX; }
   Float_t GetVTXZPitch(){ return fDZEEVX; }
   Float_t GetVTXRadius(Int_t layer){ return fVTXLayer[layer][0]; }
   Float_t GetVTXZminus(Int_t layer){ return fVTXLayer[layer][1]; }
   Float_t GetVTXZplus(Int_t layer){ return fVTXLayer[layer][2]; }
   Float_t GetVTXThickness(Int_t layer){ return fVTXLayer[layer][3]; }
   Float_t GetVTXSigmaSP(){ return fVTXError[0]; }
   Int_t   GetVTXLayerNumber(Float_t radius);   

   ClassDef(JSFQuickSimParam,1) // JSFQuickSim Parameters
};
 
class JSFQuickSim;

// ******* JSFQuickSimBuf class ********************************
class JSFQuickSimBuf : public JSFEventBuf {
private:
   Int_t            fNtracks   ;  // Number of particles 
   TClonesArray    *fTracks    ;  //! Pointers to Particles
   Int_t            fNCDCTracks;  // Number of CDC tracks.
   TClonesArray    *fCDCTracks ;  //! Pointer to CDC Tracks
   Int_t            fNVTXHits  ;  // Number of VTXHits
   TClonesArray    *fVTXHits   ;  //! Pointers to VTX Hits
   Int_t            fNEMCHits  ;  // Number of EMC hit cells
   TClonesArray    *fEMCHits   ;  //! Pointers to EMC Hit cells 
   Int_t            fNHDCHits  ;  // Number of HDC hit cells
   TClonesArray    *fHDCHits   ;  //! Pointers to EMC Hit cells 
public:
   JSFQuickSimBuf(const char *name="JSFQuickSimBuf",
		   const char *title="JSF QuickSim event class",
		   JSFQuickSim *sim=NULL);

  void SetNtracks(Int_t nt){ fNtracks=nt;}
  Int_t GetNtracks(){ return fNtracks; }
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

   Bool_t TBPUTGeneratorParticles();
   Bool_t ReviseGeneratorInfo();
   Bool_t MakeJSFLTKCLTracks();
   Bool_t MakeCALHits();
   Bool_t MakeVTXHits();
   Bool_t MakeCDCTracks();
   Bool_t LinkCDCandVTX();
   
   virtual  void MakeBranch(TTree *tree); // Make branch for the module 
   virtual  void SetBranch(TTree *tree);  // Set Branch address for the module

   ClassDef(JSFQuickSim,1)  // Debug structure
};


#endif
