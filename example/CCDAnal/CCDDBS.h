#ifndef __CCDDBS__
#define __CCDDBS__

//////////////////////////////////////////////////////////////////
//
//  CCDDBS
//
//  Data base class for CCD Beam data analysis
//  
//////////////////////////////////////////////////////////////////


#include <TNamed.h>

#ifndef __JSFDBS__
#include "JSFDBS.h"
#endif

// ************************************************************
//   RunIndex class
// ************************************************************
class CCDRunIndex : public JSFDBS {
 protected:
  Int_t   fRun;    // Run Number
  Int_t   fRefRun; // Reference run ( those with same configuration)
  Int_t   fNCCD;    // Number of CCD for this run.
  Char_t  f3rdCCD[8]; // Name of 3rd CCD
  Float_t fTemp;    // temperature.
  Float_t fAngle;   // Beam angle.
  Float_t fEbeam;   // Beam energ (GeV)
  Int_t   fNdark;   // Number of dark run.
  Int_t   fDarkRun[5]; // Run number of dark run.
 public:
   CCDRunIndex(Int_t runno=0);
   CCDRunIndex(Int_t run, Int_t refrun, Int_t nccd, Char_t *name,
	       Float_t temp, Float_t angle, Float_t ebeam,
	       Int_t ndark, Int_t darkrun[]);
   virtual ~CCDRunIndex(){}

   Int_t   GetRunNumber(){ return fRun;};
   Int_t   GetReferenceRunNumber(){ return fRefRun;};
   Int_t   GetNCCD(){ return fNCCD;};
   Char_t *GetNameOf3rdCCD(){return f3rdCCD;}
   Float_t GetTemperature(){ return fTemp; }
   Float_t GetAngle(){ return fAngle; }
   Float_t GetEbeam(){ return fEbeam; }
   Int_t   GetNdarkrun(){ return fNdark; }
   Int_t*  GetDarkRunNumber(){ return fDarkRun; }

   ClassDef(CCDRunIndex, 1) // CCD Beam test run index
};


// ************************************************************
//   Pedestal class for each CCD 
// ************************************************************
class CCDPedestal : public TObject {
 public:
  Int_t   fCCDID;  // CCD Id ( 0 to n-1 )
  Float_t  fMean;  // Mean pedestal value.
  Float_t  fEMean; // Error of mean
  Float_t  fRms;   // RMS
  Float_t  fERms;  // Error of RMS
  Float_t  fChisq; // Chisq of the fit.
  Int_t    fNover5s; // Number of entry above 5 sigma
  Int_t    fNunder5s; // Number of entry under 5sigma
  public:
    CCDPedestal(Int_t ccdid=0, Float_t mean=0.0 , Float_t emean=0.0,
		Float_t rms=0.0, Float_t erms=0.0, Float_t chisq=0.0,
		Int_t nover5s=0, Int_t nunder5s=0);
  ClassDef(CCDPedestal, 1) // CCD pedestal info for single CCD
};

// ************************************************************
//   Pedestal class 
// ************************************************************
class CCDPedestals : public JSFDBS {
 protected:
   Int_t   fRun;        // Run Number
   Int_t   fNCCD;       // Number of CCD of this run.
   Int_t   fNcall;      // Number of frame used to create average data
   CCDPedestal *fPed  ; //! CCDPedestal data for each CCD
 public:
   CCDPedestals(Int_t nrun=0);
   CCDPedestals(Int_t run, Int_t nccd, Int_t nall, CCDPedestal ped[]);
   virtual ~CCDPedestals();

   CCDPedestal *Get(Int_t ccdid){ return &fPed[ccdid]; }

   Int_t GetRunNumber(){ return fRun; }
   Int_t GetNCCD(){ return fNCCD;}
   Int_t GetNcall(){ return fNcall; }

   ClassDef(CCDPedestals, 1) // CCD pedestals of each run
};

// ************************************************************
//   DarkData
// ************************************************************
class CCDDarkData : public JSFDBS {
 protected:
  Int_t fNCCD;      // Number of CCD for this dark
  Int_t *fOffset;   //! Offset of dark data for each CCD.
  Int_t *fNx;       //! Number of x pixel.
  Int_t *fNy;       //! Number of y pixel.
  Float_t *fDark;   //! Dark data.
 public:
   CCDDarkData(Int_t run=0);
   virtual ~CCDDarkData();

   void LoadFile(const char *name);
   friend class CCDCorrectDark;

   ClassDef(CCDDarkData,1) // Dark data class
};

#endif
