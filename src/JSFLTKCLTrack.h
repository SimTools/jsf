#ifndef __JSFLTKCLTrack__
#define __JSFLTKCLTrack__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFLTKCLTrack                                                        //
//                                                                      //
// Track-Cluster mathced track                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TVector.h"

#ifndef __JSFCDCTrack__
#include "JSFCDCTrack.h"
#endif

typedef enum {kCombinedGammaTrack, kCombinedLeptonTrack,
              kCombinedHadronTrack } EJSFLTKCLTrackBank;


// *********************************************************
class JSFLTKCLTrack : public TObject {
friend class JSFQuickSim;
protected:
  EJSFLTKCLTrackBank  fBank; // The original bank name
  Double_t    fP[4];    // four momentum (E,Px,Py,Pz), GeV
  Double_t    fEcl;     // ECL if electrons, unused otherwise.
  Int_t     fNEMC;   // EMC Cluster # if electrons, unused othersize.
  Int_t     fCharge;  // Charge
  Int_t     fType;    // ITYP
  Int_t     fSource;  // ISRC 
  Int_t     fNCDC;    // # used CDC tracks
  Int_t     f1stCDC;  // Element number of corresponding CDC:Track_Parameter
  JSFCDCTrack *fCDC;  //! Address of corresponding CDC track.
  
  void SetCDC(JSFCDCTrack *t){ fCDC=t; }; 

public:
  JSFLTKCLTrack() {}
  virtual ~JSFLTKCLTrack() {}

  JSFLTKCLTrack(EJSFLTKCLTrackBank bank, TVector& P, Real_t ecl,
         Int_t nemc, Int_t charge, Int_t type, Int_t source, 
 	 Int_t ncdc, Int_t first);
  JSFLTKCLTrack(EJSFLTKCLTrackBank bank, Float_t data[]);

  Double_t GetPx(){ return fP[1] ;}
  Double_t GetPy(){ return fP[2] ;}
  Double_t GetPz(){ return fP[3] ;}
  Double_t GetE(){  return fP[0] ;}
  Double_t GetPabs(){ return TMath::Sqrt( fP[1]*fP[1]+fP[2]*fP[2]+fP[3]*fP[3]);}
  Double_t GetAzimthAngle(){ return TMath::ATan2( fP[2], fP[1]);}
  Double_t GetPt(){ return TMath::Sqrt( fP[1]*fP[1]+fP[2]*fP[2]);}
  Double_t GetCosth(){ return fP[3]/GetPabs();}

  Double_t GetECL(){ return fEcl; }
  Int_t    GetNEMC(){ return fNEMC; }
  Int_t    GetCharge(){ return fCharge; }
  Int_t    GetType(){ return fType;}
  Int_t    GetSource(){return fSource; }
  Int_t    GetNCDC(){ return fNCDC;}
  Int_t    Get1stCDC(){ return f1stCDC;}
  JSFCDCTrack *GetCDC(){ return fCDC;}

  TVector GetPV(){ TVector p(4) ; 
          p(0)=fP[0] ; p(1) =fP[1] ; p(2)=fP[2] ; p(3)=fP[3] ; return p ; }

  ClassDef(JSFLTKCLTrack,1)  //A JSFLTKCLTrack
};

#endif
