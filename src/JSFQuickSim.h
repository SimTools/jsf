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
protected:
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

   Float_t GetBField(){ return fBfield;}
   Float_t GetCDCInnerRadius(){ return fTrack[0];}
   Float_t GetCDCOuterRadius(){ return fTrack[1];}
   Float_t GetCDCZMinus(){ return fTrack[2];}
   Float_t GetCDCZPlus(){ return fTrack[3];}
   Float_t GetCDCNSample(){ return fTrack[4];}
   Float_t GetCDCMinimumSample(){ return fTrack[5];}
   Float_t GetCDCSigmaRPhi(){ return fTrack[6];}
   Float_t GetCDCSigmaZ(){ return fTrack[7];}



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

