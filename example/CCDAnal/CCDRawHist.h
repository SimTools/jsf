#ifndef __CCDRawHist__
#define __CCDRawHist__

//////////////////////////////////////////////////////////////////////////
//
// CCDRawHist
//
// A class to make a histogram of CCD Raw data.
//
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TH1.h"
#include "TCanvas.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __CCDRawData__
#include "CCDRawData.h"
#endif

struct THdef1 {
  Float_t nbin;
  Float_t xmin;
  Float_t xmax;
} ;


//*** Basic Analysis module 

class CCDRawHist : public JSFModule {
 public:
  TH1F *hPh0 ; //! CCD0 pulse height.
  TH1F *hPh1 ; //! CCD1 pulse height.
  TH1F *hPh2 ; //! CCD2 pulse height.
  TH1F *hPh3 ; //! CCD3 pulse height.
  TCanvas *hc1; //! Canvas where to draw histogram.
  THdef1  hdef1[4]; //! parameters to define histogram
 public:
   CCDRawHist(const Char_t *name="CCDRawHist",
	      const Char_t *title="Make a CCD raw data histogram");
   virtual ~CCDRawHist(){}

   Bool_t Initialize();
   Bool_t Process(Int_t ev);
   
   void Draw();

   ClassDef(CCDRawHist, 1) // Make a histogram of CCD Raw data
};

#endif

