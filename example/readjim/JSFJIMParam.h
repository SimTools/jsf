#ifndef __JSFJIMParam__
#define __JSFJIMParam__

//////////////////////////////////////////////////////////////////////////
//
// JSFJIMParam
//
// A JIM Parameter class
//
//$Id: 18-Oct-2000 Y.Ishizawa ; Add some functions.
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
  Float_t fBCPAR0R[15] ; // Float part of the contents of Master segment of MCBCPAR
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

   inline Int_t GetCommonBankVersion(){ return fBCPAR0I[0]; }
   inline Int_t GetCALTypeFlag(){ return fBCPAR0I[1]; }
   inline Int_t GetEMThetaMax(){ return fBCPAR0I[2]; }
   inline Int_t GetEMPhiMax(){ return fBCPAR0I[3]; }
   inline Int_t GetHDThetaMax(){ return fBCPAR0I[4]; }
   inline Int_t GetHDPhiMax(){ return fBCPAR0I[5]; }
   inline Int_t GetNShmaxStrip(){ return fBCPAR0I[6]; }
   inline Int_t GetNPreshLayer(){ return fBCPAR0I[7]; }
   inline Int_t GetNSHMaxLayer(){ return fBCPAR0I[8]; }
   inline Int_t GetNEM1Layer(){ return fBCPAR0I[9]; }
   inline Int_t GetNSHTailLayer(){ return fBCPAR0I[10]; }
   inline Int_t GetNEM2Layer(){ return fBCPAR0I[11]; }
   inline Int_t GetNHD1Layer(){ return fBCPAR0I[12]; }
   inline Int_t GetNHD2Layer(){ return fBCPAR0I[13]; }
   inline Int_t GetNHD3Layer(){ return fBCPAR0I[14]; }
   inline Int_t GetNHD4Layer(){ return fBCPAR0I[15]; }
   inline Float_t GetBCInnerR(){ return fBCPAR0R[0]; }
   inline Float_t GetBCZLength(){ return fBCPAR0R[1]; }
   inline Float_t GetBCTotalDepth(){ return fBCPAR0R[2]; }
   inline Float_t GetEMScitiThick(){ return fBCPAR0R[3]; }
   inline Float_t GetEMPlasticThick(){ return fBCPAR0R[4]; }
   inline Float_t GetEMPbThick(){ return fBCPAR0R[5]; }
   inline Float_t GetHDScitiThick(){ return fBCPAR0R[6]; }
   inline Float_t GetHDPlasticThcik(){ return fBCPAR0R[7]; }
   inline Float_t GetHDPbThick(){ return fBCPAR0R[8]; }
   inline Float_t GetSHMSciThick(){ return fBCPAR0R[9]; }
   inline Float_t GetSHMPlasticThick(){ return fBCPAR0R[10]; }
   inline Float_t GetPhiTiltAngle(){ return fBCPAR0R[11]; }
   inline Float_t GetPointingOffset(){ return fBCPAR0R[12]; }
   inline Float_t GetZTowerSize(){ return fBCPAR0R[13]; }
   inline Float_t GetPhiTowerSize(){ return fBCPAR0R[14]; }

   // Functions to get EndCap Calorimeter information
   inline Int_t GetECType(){ return fECPAR0I[0]; }
   inline Bool_t IsECConstantTheta(){ return (fECPAR0I[0]==1) ? kTRUE : kFALSE ; }
   inline Int_t  GetECEMNTheta(){ return 2*fECPAR0I[1]; }
   inline Int_t  GetECHDNTheta(){ return fECPAR0I[1]; }
   inline Float_t GetECFrontInnerR(){ return fECPAR0R[0]; }
   inline Float_t GetECFrontOuterR(){ return fECPAR0R[1]; }
   inline Float_t GetECRearInnerR(){ return fECPAR0R[2]; }
   inline Float_t GetECRearOuterR(){ return fECPAR0R[3]; }
   inline Float_t GetECFrontZ(){ return fECPAR0R[4]; }
   inline Float_t GetECDepth(){ return fECPAR0R[5]; }
   inline Float_t GetECEMDepth(){ return fECPAR0R[6]; }
   inline Float_t GetECEMSciThick(){ return fECPAR0R[7]; }
   inline Float_t GetECHDSciThick(){ return fECPAR0R[8]; }
   inline Float_t GetECHDLateralSize(){ return fECPAR0R[9]; }
   
   inline Int_t GetECEMNPhi(Int_t ith){ 
     if ( ith > 0 && ith <= GetECEMNTheta() ) { return 2*fNDPEC[ith-1]; }
     else { 
       printf("JSFJIMParam::GetECNPhi(ith) .. ith=%d is out of range\n",ith) ;
       return 0;
     }
   }

   inline Int_t GetECHDNPhi(Int_t ith){ 
     if ( ith > 0 && ith <= GetECHDNTheta() ) { return fNDPEC[ith-1]; }
     else { 
       printf("JSFJIMParam::GetECNPhi(ith) .. ith=%d is out of range\n",ith) ;
       return 0;
     }
   }


   ClassDef(JSFJIMParam,2) // JSFJIMParam
};
 
#endif
