#ifndef __JSFSIMDST__
#define __JSFSIMDST__

///////////////////////////////////////////////////////////////////////
//
//  To output SIMDST record by Fortran program
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include "JSFModule.h"

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
#ifndef __JSFQuickSim__
#include "JSFQuickSim.h"
#endif


const Int_t kGenSize=10;
const Int_t kIGenSize=4;
const Int_t kGenMax=500;
const Int_t kClsSize=2;
const Int_t kClsMax=1000;
const Int_t kTrkMax=500;
const Int_t kCmbtSize=8;
const Int_t kTrkfSize=23;
const Int_t kTrkdSize=15;

const Int_t kVTXHMax=100;  // Maximum number of vertex hit
const Int_t kVTXBufSize=kVTXHMax*kTrkMax;


// *************************************************************
class JSFSIMDSTBuf : public JSFEventBuf {
protected:
//  Data to be saved in SIMDST data.
  Int_t fEndian  ; // Endian data
  Int_t fVersion ; // version
  Char_t fProduc[5]; // Name of the program

  Float_t fHead[2] ; // 
  Int_t fNGeneratorParticles;       // Number of generator particle
  TClonesArray *fGeneratorParticles;  //! Pointer to the generator particle buffer.
  Int_t fNCombinedTracks;        // Number of combined tracks.
  TClonesArray *fCombinedTracks; //!  Pointer to the Combined track
  
  Int_t fNCDCTracks;  // Number of CDC Tracks.
  TClonesArray *fCDCTracks; //! Pointer to the CDC tracks.

  Int_t   fNvtx[kTrkMax];  // Number of VTX hits of each track
  Int_t   fNPvtx;  // Total number of VTX Hits.

  Int_t fNVTXHits; // Number of VTX hits
  TClonesArray *fVTXHits; // VTXHits Hits


  Int_t fNEMCHits;  // Number of EMC Hits.
  TClonesArray *fEMCHits; // EMC Hits
  Int_t fNHDCHits;  // Number of HDC Hits.
  TClonesArray *fHDCHits; // HDC Hits

public:
  JSFSIMDSTBuf(const char *name="JSFSIMDSTBuf", 
	     const char *title="JSF SimDST event buffer",
  	     JSFModule *module=NULL);
  virtual ~JSFSIMDSTBuf(){}

  virtual Bool_t PackDST(Int_t nev); // Pack SIMDST information.
  friend class JSFSIMDST;
  
  ClassDef(JSFSIMDSTBuf,1)  // JSSF SimDST event buffer
};

// *************************************************************
class JSFSIMDST : public JSFModule {
protected:
  Int_t  fUnit;       // Fortran logical unit number to output SIMDST data.
  Char_t fOutputFileName[256]; // File name of output file.
  Char_t fParamFileName[256];   // Simulator parameter file name.

public:
  JSFSIMDST(const char *name="JSFSIMDST", 
		  const char *title="Output SIMDST data");
  virtual ~JSFSIMDST(){}

  friend class JSFSIMDSTBuf;

  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t nrun=0);
  virtual Bool_t Process(Int_t ev=1);
  virtual Bool_t EndRun();

  Int_t  GetUnit(){ return fUnit;}
  Char_t *GetOutputFileName(){ return fOutputFileName; }
  void SetUnit(Int_t nunit){ fUnit=nunit; }
  void SetOutputFileName(Char_t *name){ strcpy(fOutputFileName, name);}
  void SetParamFileName(Char_t *name){ strcpy(fParamFileName, name);}

  ClassDef(JSFSIMDST, 1)  // Make SIMDST data.

};

#endif


