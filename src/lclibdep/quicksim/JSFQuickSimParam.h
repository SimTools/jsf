#ifndef __JSFQuickSimParam__
#define __JSFQuickSimParam__

//////////////////////////////////////////////////////////////////////////
//
// JSFQuickSimParam
//
// A QuickSim Parameter class
//
//$Id$
//                                                                      //
// 30-Mar-1999 A.Miyamoto  Add fNUMVTX in JSFQuickSimParam
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
#ifndef __JSFBasicClasses__
#include "JSFBasicClasses.h"
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
  Float_t fZminus ; // Z location of -Zee endcap Calorimeter
  Float_t fZplus  ; // Z location of +Zee endcap Calorimeter

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

  virtual JSFRThPhi HitPosition(Int_t iCellID);
  void HitCellGeom(Int_t iCellID, Float_t geo[]);

  Float_t GetRmin(){ return fRmin; }
  Float_t GetRmax(){ return fRmax; } 
	       
  ClassDef(JSFCALGeoParam,1)  // Calorimeter Geometry parameter.
};

// ******* JSFQuickSimParam *************************************
class JSFQuickSimParam : public TNamed {
protected:
   Char_t  fParamFile[256] ; // Where to get simulator parameter.
   Int_t fSeed ;    // Seed of random variable;
   Int_t   fNSMPVX   ; // Number of VTX layer.
   Int_t   fNERRVX;          // VTX Error flag. 
   Int_t   fNUMVTX;          // Number of VTX layer
   Float_t fBfield; // Bfield (kgauss).
   Float_t fTrack[8]; // Parameter for tracking device.
   Float_t fEMCal[11]; // Parameter for tracking device.
   Float_t fHDCal[11]; // Parameter for tracking device.
   Float_t fDPHIVX   ; // VTX-phi pitch (cm)                        
   Float_t fDZEEVX   ; // VTX-Z   pitch (cm)                        
   Float_t fVTXLayer[22][4]; // VTX Layer
   Float_t fVTXError[5];     // VTX resolution parameter
   Float_t fCLSPAR[4][5]   ; // Clustering parameter.
   Float_t fCMBCUT[3] ; // ADXEVT, ENSGEM, ENSGHD for CMBCUT
   Float_t fANMPAR[2][2]; // Prameter for lateral spread of calorimeter shower
   Float_t fALMPAR[2][2]; // Prameter for lateral spread of calorimeter shower
   Float_t fXRADCDC ; // Radiation length of CDC gas.               

   JSFCALGeoParam *fEMCGeom; //! EMC Geometry ( set by SetSmearParam() )
   JSFCALGeoParam *fHDCGeom; //! HDC Geometry
public:
   JSFQuickSimParam();
   ~JSFQuickSimParam();

   virtual void SetSwimParam() ; // Set swimmer parameters
   virtual void SetSmearParam() ; // Set smear parameters
   virtual void ReadParamDetector(Char_t *file) ;// Read detector parameter
					      
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

   Float_t GetEMCNPhi(){ return fEMCal[0]; }
   Float_t GetEMCNTheta(){ return fEMCal[1]; }
   Float_t GetEMCNRadial(){ return fEMCal[2]; }
   Float_t GetEMCRMinimum(){ return fEMCal[3]; }
   Float_t GetEMCRMaximum(){ return fEMCal[4]; }
   Float_t GetEMCZMinus(){ return fEMCal[5]; }
   Float_t GetEMCZPlus(){ return fEMCal[6]; }

   Float_t GetHDCNPhi(){ return fHDCal[0]; }
   Float_t GetHDCNTheta(){ return fHDCal[1]; }
   Float_t GetHDCNRadial(){ return fHDCal[2]; }
   Float_t GetHDCRMinimum(){ return fHDCal[3]; }
   Float_t GetHDCRMaximum(){ return fHDCal[4]; }
   Float_t GetHDCZMinus(){ return fHDCal[5]; }
   Float_t GetHDCZPlus(){ return fHDCal[6]; }

 
   Int_t   GetVTXNInnerTracker(){ return fNSMPVX-1-fNUMVTX; }
   Int_t   GetVTXNVTX(){ return fNUMVTX; }
   Int_t   GetVTXNERRVX(){ return fNERRVX; }
   Int_t   GetVTXNLayer(){ return fNSMPVX; }
   Float_t GetVTXPhiPitch(){ return fDPHIVX; }
   Float_t GetVTXZPitch(){ return fDZEEVX; }
   Float_t GetVTXRadius(Int_t layer){ return fVTXLayer[layer][0]; }
   Float_t GetVTXZminus(Int_t layer){ return fVTXLayer[layer][1]; }
   Float_t GetVTXZplus(Int_t layer){ return fVTXLayer[layer][2]; }
   Float_t GetVTXThickness(Int_t layer){ return fVTXLayer[layer][3]; }
   Float_t GetVTXSigmaSP(){ return fVTXError[0]; }
   Int_t   GetVTXLayerNumber(Float_t radius);   

   inline Float_t GetANMPAR(Int_t i, Int_t j){ return fANMPAR[i][j]; };
   inline Float_t GetALMPAR(Int_t i, Int_t j){ return fALMPAR[i][j]; };
   inline Float_t GetXRADCDC(){ return fXRADCDC; };

   Float_t GetClusteringParameter(Int_t idev, Int_t ipar){ return fCLSPAR[idev][ipar];}
   Float_t GetCMBCUT(Int_t ipar){ return fCMBCUT[ipar]; }


   ClassDef(JSFQuickSimParam,5) // JSFQuickSim Parameters
};
 
#endif

