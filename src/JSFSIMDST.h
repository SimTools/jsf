#ifndef __JSFSIMDST__
#define __JSFSIMDST__

///////////////////////////////////////////////////////////////////////
//
//  To output SIMDST record by Fortran program
//
//$Id$
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
// const Int_t kClsSize=2;
const Int_t kClsSize=3;
const Int_t kClsMax=1000;
const Int_t kTrkMax=500;
const Int_t kCmbtSize=8;
const Int_t kTrkfSize=23;
const Int_t kTrkdSize=15;

const Int_t kVTXHMax=10;  // Maximum number of vertex hit
const Int_t kVTXBufSize=kVTXHMax*kTrkMax;
const Int_t kVTXHSize=5;
const Int_t kVTXIDSize=2;


// *************************************************************
class JSFSIMDSTBuf : public JSFEventBuf {
protected:
//  Data to be saved in SIMDST data.
  Int_t fEndian  ; //! Endian data
  Int_t fVersion ; //! version
  Char_t fProduc[5]; //! Name of the program

  Float_t fHead[2] ; //! 
  Int_t fNGeneratorParticles;       //! Number of generator particle
  TClonesArray *fGeneratorParticles; //! Pointer to the generator particle buffer.
  Int_t fNCombinedTracks;         //! Number of combined tracks.
  TClonesArray *fCombinedTracks; //!  Pointer to the Combined track
  
  Int_t fNCDCTracks;        //! Number of CDC Tracks.
  TClonesArray *fCDCTracks; //! Pointer to the CDC tracks.

  Int_t   fNvtx[kTrkMax]; //! Number of VTX hits of each track
  Int_t   fNPvtx;         //! Total number of VTX Hits.

  Int_t fNVTXHits;        //! Number of VTX hits
  TClonesArray *fVTXHits; //! VTXHits Hits

  Int_t fNEMCHits;        //! Number of EMC Hits.
  TClonesArray *fEMCHits; //! EMC Hits
  Int_t fNHDCHits;        //! Number of HDC Hits.
  TClonesArray *fHDCHits; //! HDC Hits

  void SetClonesArray(); // Set TClonesArray pointers.


public:
  JSFSIMDSTBuf(const char *name="JSFSIMDSTBuf", 
	     const char *title="JSF SimDST event buffer",
  	     JSFModule *module=NULL);
  virtual ~JSFSIMDSTBuf(){}

  virtual void SetClassData(Int_t nev);       // Set Class Data
  virtual Bool_t PackDST(Int_t nev); // Pack SIMDST information.
  virtual Bool_t UnpackDST(Int_t nev); // Unpack SIMDST information.
  friend class JSFSIMDST;

  Int_t GetEndian(){ return fEndian; }
  Int_t GetVersion(){ return fVersion; }
  Char_t *GetProducer(){ return fProduc; }
  Float_t GetEventNumber(){ return fHead[0];}
  Float_t GetEcm(){ return fHead[1];}

  Int_t GetNGeneratorParticles(){ return fNGeneratorParticles; }
  TClonesArray *GetGeneratorParticles(){ return fGeneratorParticles;}
  Int_t GetNLTKCLTracks(){ return fNCombinedTracks;}
  TClonesArray *GetLTKCLTracks(){ return fCombinedTracks;}
  Int_t GetNCDCTracks(){ return fNCDCTracks;}
  TClonesArray *GetCDCTracks(){ return fCDCTracks;}
  Int_t GetNVTXHits(){ return fNVTXHits;}
  TClonesArray *GetVTXHits(){ return fVTXHits;}
  Int_t GetNEMCHits(){ return fNEMCHits;}
  TClonesArray *GetEMCHits(){ return fEMCHits;}
  Int_t GetNHDCHits(){ return fNHDCHits;}
  TClonesArray *GetHDCHits(){ return fHDCHits;}

  void AddVTXHit(Double_t r, Double_t phi, Double_t z, Double_t dphi,
	    Double_t dz, Int_t layer, Int_t trackid=0, Int_t gentrack=0);

 
  ClassDef(JSFSIMDSTBuf,1)  // JSSF SimDST event buffer
};

// *************************************************************
class JSFSIMDST : public JSFModule {
protected:
  Int_t  fUnit;       // Fortran logical unit number to output SIMDST data.
  Char_t fDataFileName[256]; //! File name of output file.
  Char_t fParamFileName[256];   //! Simulator parameter file name.
  Int_t  fReadWrite;  // =0 no read/write =1 to write data, =2 to read 
  JSFQuickSimParam  *fParam ; //! Parameters for JSFQuickSim
public:
  JSFSIMDST(const char *name="JSFSIMDST", 
 	    const char *title="Read/Write SIMDST data",
	    Bool_t constbuf=kTRUE);
  virtual ~JSFSIMDST(){}

  friend class JSFSIMDSTBuf;

  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t nrun=0);
  virtual Bool_t Process(Int_t ev=1);
  virtual Bool_t EndRun();

  virtual JSFQuickSimParam *Param(){ return fParam; }
  virtual void SetQuickSimParam(JSFQuickSimParam *par){ fParam=par; }

  Int_t  GetUnit(){ return fUnit;}
  Char_t *GetDataFileName(){ return fDataFileName; }
  void SetUnit(Int_t nunit){ fUnit=nunit; }
  void SetDataFileName(Char_t *name){ strcpy(fDataFileName, name);}
  void SetParamFileName(Char_t *name){ strcpy(fParamFileName, name);}

  virtual Bool_t WriteParameters(Int_t nrun);

  void WriteData(){ fReadWrite=1;}
  void ReadData(){ fReadWrite=2;}
  void NoReadWrite(){ fReadWrite=0;}
  Int_t GetReadWriteFlag(){ return fReadWrite; }

  ClassDef(JSFSIMDST, 2)  // Make SIMDST data.

};

#endif


