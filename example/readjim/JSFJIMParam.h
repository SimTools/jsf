#ifndef __JSFJIMParam__
#define __JSFJIMParam__

//////////////////////////////////////////////////////////////////////////
//
// JSFJIMParam
//
// A JIM Parameter class
//
//$Id:
//
//////////////////////////////////////////////////////////////////////////

#ifndef __JSFQuickSimParam__
#include "JSFQuickSimParam.h"
#endif

static const Int_t kCALPMAX=200 ;

// ******* JSFQuickSimParam *************************************
class JSFJIMParam : public JSFQuickSimParam {
protected:
  Int_t   fBCPAR0I[15] ; // Integer part of the contents of Master segment of MCBCPAR
  Float_t fBCPAR0R[16] ; // Float part of the contents of Master segment of MCBCPAR
  Float_t fREM1BC[kCALPMAX] ; // R position of each EM tower (cm)
  Float_t fZEM1BC[kCALPMAX] ; // Z position of each EM tower (cm);
  Float_t fPEM1BC[kCALPMAX] ; // phi position of each EM tower (radian)
  Float_t fRHD1BC[kCALPMAX] ; // R position of each HD tower (cm)
  Float_t fZHD1BC[kCALPMAX] ; // Z position of each HD tower (cm)
  Float_t fPHD1BC[kCALPMAX] ; // phi position of each HD tower (cm) 

  Int_t   fECPAR0I[4] ;// Integer part of the content of master segment of MCECPAR
  Float_t fECPAR0R[14];// Real part of the content of master segment of MCECPAR

  Int_t   fNDPEC[kCALPMAX] ;// number of phi-division of each ring.
  Float_t fREM1EC[kCALPMAX] ; // r position of each EM cell (cm)
  Float_t fZEM1EC[kCALPMAX] ; // z position of each EM cell (cm)
  Float_t fPEM1EC[4000] ; // phi position of each EM cell (rad)
  Float_t fRHD1EC[kCALPMAX] ; // r position of each HD cell (cm)
  Float_t fZHD1EC[kCALPMAX] ; // z position of each HD cell (cm)
  Float_t fPHD1EC[1000] ; // phi position of each HD cell (rad)


public:
   JSFJIMParam();
   virtual ~JSFJIMParam();

   void ReadParameter() ;// Read detector parameter

   Bool_t GetEMPosition(Int_t cell, Float_t pos[3], Float_t width[3]); 
   Bool_t GetHDPosition(Int_t cell, Float_t pos[3], Float_t width[3]);
   
   inline Float_t GetPointingOffset(){ return fBCPAR0R[12]; }

				      
   ClassDef(JSFJIMParam,1) // JSFJIMParam
};
 
#endif

