#ifndef __CCDCorrectDark__
#define __CCDCorrectDark__

//////////////////////////////////////////////////////////////////////////
//
// CCDCorrectDark
//
// Subtrack dark data from CCD Rawdata
//
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TDatime.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __CCDRawData__
#include "CCDRawData.h"
#endif
#ifndef __CCDDBS__
#include "CCDDBS.h"
#endif


class CCDCorrectDark;

//*** Dark data.

class CCDCorrectedADCBuf : public JSFEventBuf {
  protected:
    Int_t   fNCCD;      //  Number of CCD.
    Int_t  *fOffset;    //! Offset of dark data  for i-th CCD
    Int_t  *fNx;        //! Number of X pixel  for i-th CCD
    Int_t  *fNy;        //! Number of Y pixel for i-th CCD
    Float_t *fADC;      //! Dark subtrcted ADC data.
  public:
    CCDCorrectedADCBuf(const char *name="CCDCorrectedADCBuf",
	const char *title="CCD dark corrected data", CCDCorrectDark *mod=NULL);
    CCDCorrectedADCBuf(CCDCorrectDark *mod, 
        const char *name="CCDCorrectedADCBuf", 
	const char *title="CCD dark corrected data");
  virtual ~CCDCorrectedADCBuf();

  void AllocateBuf(CCDEnvironmentBuf *env);

  Float_t *ADC(Int_t ccd){return &fADC[fOffset[ccd]] ; }
  Int_t GetNx(Int_t ccd){ return fNx[ccd];}
  Int_t GetNy(Int_t ccd){ return fNy[ccd];}

  ClassDef(CCDCorrectedADCBuf, 1) // Dark corrected ADC

};

//*** Basic Analysis module of CCDRawData

class CCDCorrectDark : public JSFModule {
 protected:
   Bool_t    fSubtract;      //! really subtracvt when kTRUE.
   CCDDarkData *fDark;       //! Dark data
 public:
   CCDCorrectDark(const Char_t *name="CCDCorrectDark",
	      const Char_t *title="Correct dark");
   virtual ~CCDCorrectDark();

   void CorrectDark(CCDRawDataBuf *buf, CCDEnvironmentBuf *env);

   Bool_t BeginRun(Int_t nrun);
   Bool_t Process(Int_t ev);

   ClassDef(CCDCorrectDark, 1) // Subtract dark signal from ADC value
};

#endif

