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

typedef enum { kEMC , kHDC } EJSFCALType ;

//************************************************************
// Calorimeter's geometry parameter file.
//************************************************************
class JSFCALGeoParam : public TObject {
protected:
  EJSFCALType  fType ; // Calorimeter type
  Float_t fRmin ; // Inner radius of end-cap calorimeter
  Float_t fRmax ; // radius of barrel and outer radius of endcap.

  Float_t fPhiStep ; // Phi step/bin

  Float_t fBZetaMin ;   // Barrel Zeta min.
  Float_t fBZetaStep ;  // Barrel Zeta step/bin

//                     costh=( 1-exp(2*zeta))/(1+exp(2*zeta))
  Float_t fMZetaMin ;  // Zeta min of -Zee endcap.
  Float_t fMZetaStep ; // Zeta step of -Zee endcap.
  Float_t fPZetaMin ;  // Zeta min of +Zee endcap.
  Float_t fPZetaStep ; // Zeta step of +Zee endcap.

public:
  JSFCALGeoParam(EJSFCALType type, Int_t nphi, Int_t ntheta, Int_t nrad,
 	       Float_t rmin, Float_t rmax,  Float_t zminus, Float_t zplus );
  virtual ~JSFCALGeoParam(){}

  void SetGeoParam(EJSFCALType type, Int_t nphi, Int_t ntheta, Int_t nrad,
 	       Float_t rmin, Float_t rmax,  Float_t zminus, Float_t zplus );

  ClassDef(JSFCALGeoParam,1)  // Calorimeter Geometry parameter.
};

// ******* JSFQuickSimParam *************************************
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
   Int_t            fNTracks   ;  // Number of particles 
   TClonesArray    *fTracks    ;  //! Pointers to Particles
   Int_t            fNCDCTracks;  // Number of CDC tracks.
   TClonesArray    *fCDCTracks ;  //! Pointer to CDC Tracks
   Int_t            fNVTXHits  ;  // Number of VTXHits
   TClonesArray    *fVTXHits   ;  //!  Pointers to VTX Hits
   Int_t            fNEMCHits  ;  // Number of EMC hit cells
   TClonesArray    *fEMCHits   ;  //! Pointers to EMC Hit cells 
   Int_t            fNHDCHits  ;  // Number of HDC hit cells
   TClonesArray    *fHDCHits   ;  //! Pointers to EMC Hit cells 

   TBranch         *fBTracks   ;  //! Branch for LTKCL tracks.
   TBranch         *fBCDCTracks;  //! Branch for CDCTracks.
   TBranch         *fBVTXHits  ;  //! Branch for VTX branch.
   TBranch         *fBEMCHits  ;  //! Branch for EMC branch.
   TBranch         *fBHDCHits  ;  //! Branch for EMC branch.

public:
   JSFQuickSimBuf(const char *name="JSFQuickSimBuf",
		   const char *title="JSF QuickSim event class",
		   JSFQuickSim *sim=NULL);
   virtual      ~JSFQuickSimBuf();

  void SetNTracks(Int_t nt){ fNTracks=nt;}
//   Int_t GetNtracks(){ return fNTracks; }
  Int_t GetNTracks(){ return fNTracks; }
  TClonesArray *GetTracks(){ return fTracks; }
  void GetEventTracks(Int_t nevt){ fBTracks->GetEvent(nevt); }

  void SetNCDCTracks(Int_t nt){ fNCDCTracks=nt;}
  Int_t GetNCDCTracks(){ return fNCDCTracks; }
  TClonesArray *GetCDCTracks(){ return fCDCTracks; }
  void GetEventCDC(Int_t nevt){ fBCDCTracks->GetEvent(nevt); }

  void SetNVTXHits(Int_t nt){ fNVTXHits=nt;}
  Int_t GetNVTXHits(){ return fNVTXHits; }
  TClonesArray *GetVTXHits(){ return fVTXHits; }
  void GetEventVTX(Int_t nevt){ fBVTXHits->GetEvent(nevt); }

  void SetNEMCHits(Int_t nt){ fNEMCHits=nt;}
  Int_t GetNEMCHits(){ return fNEMCHits; }
  TClonesArray *GetEMCHits(){ return fEMCHits; }
  void GetEventEMC(Int_t nevt){ fBEMCHits->GetEvent(nevt); }

  void SetNHDCHits(Int_t nt){ fNHDCHits=nt;}
  Int_t GetNHDCHits(){ return fNHDCHits; }
  TClonesArray *GetHDCHits(){ return fHDCHits; }
  void GetEventHDC(Int_t nevt){ fBHDCHits->GetEvent(nevt); }

  virtual  void MakeBranch(TTree *tree); // Make branch for the event data
  virtual  void SetBranch(TTree *tree); // Set branches for the event data

  ClassDef(JSFQuickSimBuf, 1) // QuickSim event buffer class.

};

// ******* JSFQuickSim class *********************************
class JSFQuickSim : public JSFModule {
protected:
   JSFQuickSimParam  *fParam ; //! Parameters for JSFQuickSim
//   TBranch           *fBranch; //! For Simulator data.
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

   void GetEvent(Int_t nevt){ fBranch->GetEvent(nevt); }
   virtual  void MakeBranch(TTree *tree); // Make branch for the module 
   virtual  void SetBranch(TTree *tree);  // Set Branch address for the module

   ClassDef(JSFQuickSim,1)  // Debug structure
};


#endif

