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

// ******* Commons for Smearing ****************
typedef struct {
  Float_t bfield, ptor, ropt;
} COMMON_PRFELD;

typedef struct { 
  Int_t iseed;
} COMMON_SWMRND;

typedef struct {
  Int_t imodbf[2];
  Float_t bfdata[2];
} COMMON_SMRFLD;

extern COMMON_SWMRND swmrnd_;
extern COMMON_PRFELD prfeld_;
extern COMMON_SMRFLD smrfld_;


// ******* Commons for smearng *****************
static const Int_t kMXxVXL=11;
typedef struct {
  Int_t   nsmpvx;
  Float_t dphivx, dzeevx;
  Float_t rcyvx[kMXxVXL], zcyvx[kMXxVXL][2], rdlvx[kMXxVXL];
  Int_t   nerrvx;
  Float_t errvx[5];
} COMMON_SMRVGO ;
extern COMMON_SMRVGO smrvgo_;

typedef struct {
  Int_t nsmptk, nsmpct;
  Float_t dsmptk, rcytk[2], zcytk[2];
} COMMON_SMRTGO;
extern COMMON_SMRTGO smrtgo_;

typedef struct {
  Int_t nphiem, nthtem, nradem;
  Float_t  dphiem, dthtem, drdmem;
  Float_t  drdpem, thmnem, thmxem, rmmnem, rmmxem;
  Float_t  rpmnem, rpmxem, rcyem[2], zcyem[2];
  Int_t    npadem;
} COMMON_SMREGO;
extern COMMON_SMREGO smrego_;


typedef struct {
  Int_t   nphihd, nththd, nradhd;
  Float_t dphihd, dththd, drdmhd;
  Float_t drdphd, thmnhd, thmxhd, rmmnhd, rmmxhd;
  Float_t rpmnhd, rpmxhd, rcyhd[2], zcyhd[2];
} COMMON_SMRHGO;
extern COMMON_SMRHGO smrhgo_;

typedef struct {
  Float_t sigrf , sigze;
  Float_t sgemb0, sgemb, sgeme0, sgeme, sghdb0, sghdb;
  Float_t sghde0, sghde;
} COMMON_SMRRES;
extern COMMON_SMRRES smrres_;

typedef struct {
  Float_t clspar[4][5];
} COMMON_SMRCLP;
extern COMMON_SMRCLP smrclp_;

typedef struct {
  Float_t adxect, ensgem, ensghd;
} COMMON_CMBCUT;
extern COMMON_CMBCUT cmbcut_;

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
    
public:
   JSFQuickSimParam();
   ~JSFQuickSimParam();

   void SetSwimParam() ; // Set swimmer parameters
   void SetSmearParam() ; // Set smear parameters
   void ReadParamDetector(Char_t *file) ;// Read detector parameter

   ClassDef(JSFQuickSimParam,1) // JSFQuickSim Parameters
};
 
// ******* JSFQuickSimData class 
// This class is special purpose to save TBS format event data.
class JSFQuickSimData : public TNamed {
public:
  Int_t   fNVTXSP;         // number of VTX Space point
  Int_t   *fSimdata;       // simulator data.
public:
  JSFQuickSimData(){}
  JSFQuickSimData(const char *name, const char *title);
  ~JSFQuickSimData(){}

  void SetVTXSP(){}

  ClassDef(JSFQuickSimData, 1) // TBS Data buffer

};
 
class JSFQuickSim;

// ******* JSFQuickSimBuf class 
class JSFQuickSimBuf : public JSFEventBuf {
public:
   Int_t            fNtracks;  // Number of particles 
   TClonesArray    *fTracks;  // Pointers to Particles
public:
   JSFQuickSimBuf(const char *name="JSFQuickSimBuf",
		   const char *title="JSF QuickSim event class",
		   JSFQuickSim *sim=NULL);

  void SetNtracks(Int_t nt){ fNtracks=nt;}
  Int_t GetNtracks(){ return fNtracks; }
  TClonesArray *GetTracks(){ return fTracks; }

  ClassDef(JSFQuickSimBuf, 1) // QuickSim event buffer class.

};

// ******* JSFQuickSim class 
class JSFQuickSim : public JSFModule {
public:
   JSFQuickSimParam  *fParam ; //! Parameters for JSFQuickSim
   JSFQuickSimData   *fData  ; //! Pointer to TBS data
public:
   JSFQuickSim(const char *name="JSFQuickSim", 
	       const char *title="JSF Quick Simulator");
   virtual      ~JSFQuickSim();

   virtual Bool_t  Initialize(); 
   virtual Bool_t  Process(Int_t event);
   virtual Bool_t  BeginRun(Int_t nrun);

   Bool_t TBPUTGeneratorParticles();
   Bool_t MakeJSFLTKCLTracks();
   
   virtual  void MakeBranch(TTree *tree); // Make branch for the module 
   virtual  void SetBranch(TTree *tree);  // Set Branch address for the module

   ClassDef(JSFQuickSim,1)  // Debug structure
};


#endif
