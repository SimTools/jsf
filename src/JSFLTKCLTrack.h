#ifndef __JSFLTKCLTrack__
#define __JSFLTKCLTrack__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFLTKCLTrack                                                        //
//                                                                      //
// Track-Cluster mathced track                                          //
//
//$Id$
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

#ifndef __JSFGeneratorParticle__
#include "JSFGeneratorParticle.h"
#endif

#if 1
enum {kCombinedGammaTrack, kCombinedLeptonTrack, kCombinedHadronTrack };
typedef Int_t EJSFLTKCLTrackBank;
#else
typedef enum {kCombinedGammaTrack, kCombinedLeptonTrack,
              kCombinedHadronTrack } EJSFLTKCLTrackBank;
#endif


// *********************************************************
class JSFLTKCLTrack : public TObject {
friend class JSFQuickSimBuf;
friend class JSFSIMDSTBuf;
friend class JSFReadJIMBankBuf;
protected:
  EJSFLTKCLTrackBank  fBank; // The original bank name
  Double_t    fP[4];    // four momentum (E,Px,Py,Pz), GeV
  Double_t    fEcl;     // ECL if electrons, unused otherwise.
  Int_t     fNEMC;   // EMC Cluster # if electrons, unused othersize.
  Int_t     fCharge;  // Charge
  Int_t     fType;    // ITYP
  Int_t     fSource;  // ISRC 
  Int_t     fNCDC;    // # used CDC tracks
  Int_t     f1stCDC;  // Element number of corresponding CDC:Track_Parameter or Index of CDC tracks.
  JSFCDCTrack *fCDC;  //! Address of corresponding CDC track.

  Float_t   fVTXDR;   // Track distance to IP in XY plane.
  Float_t   fVTXDZ;   // Track distance to IP in Z direction
  Float_t   fVTXDDR;  // Error of fVTXDR 
  Float_t   fVTXDDZ;  // Error of fVTXDZ
  Float_t   fVTXNSig; // Sqrt( (VTXDR/VTXDDR)^2 + (VTXDZ/VTXDDZ)^2 ), Siginificance

  Int_t     *fIDCDC;   //[fNCDC]  saves CDC tracks contributing to this LTKCLTrack
  TObjArray  *fCDCs;   //! Associated CDC tracks
  TObjArray  *fEMGen;  //!  Generator particles contributing to the EM cluster.  
  
  Int_t     fNEMGen;   //  Number of matched EMC cluster
  Int_t    *fIDEMGen;  //[fNEMGen]  ID's of Matched EM cluster

  inline void SetCDC(Int_t ind, JSFCDCTrack *t){ f1stCDC=ind; fCDC=t; fCDCs->Add(t); }; 
  inline void SetCDCR(Int_t ind, JSFCDCTrack *t){ f1stCDC=ind; fCDC=t; }; 
  inline void SetEMGen(JSFGeneratorParticle *emg){ fEMGen->Add(emg); }
  inline void SetNEMGen(Int_t nemc){ fNEMGen=nemc; }


public:
  JSFLTKCLTrack();
  virtual ~JSFLTKCLTrack();

  //  JSFLTKCLTrack(EJSFLTKCLTrackBank bank, TVector& P, Real_t ecl,
  //         Int_t nemc, Int_t charge, Int_t type, Int_t source, 
  // 	 Int_t ncdc, Int_t first);
  JSFLTKCLTrack(EJSFLTKCLTrackBank bank, Float_t data[]);
  JSFLTKCLTrack(Float_t data[]);
  JSFLTKCLTrack(JSFLTKCLTrack& t);


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
  const Char_t  *GetTypeName();
  Int_t    GetSource(){return fSource; }
  Int_t    GetNCDC(){ return fNCDC;}
  Int_t    Get1stCDC(){ return f1stCDC;}
  JSFCDCTrack *GetCDC(){ return fCDC;}

  inline Float_t GetVTXDR(){ return fVTXDR; }
  inline Float_t GetVTXDZ(){ return fVTXDZ; }
  inline Float_t GetVTXDDR(){ return fVTXDDR; }
  inline Float_t GetVTXDDZ(){ return fVTXDDZ; }
  inline Float_t GetVTXNSig(){ return fVTXNSig; }

  inline Int_t GetEMGenEntries(){ return 
			    ( fEMGen == NULL ? 0 : fEMGen->GetEntries() ) ; }
  inline JSFGeneratorParticle *GetEMGenAt(Int_t i){ 
    return ( fEMGen == NULL ? NULL : ((JSFGeneratorParticle*)fEMGen->UncheckedAt(i)) ) ; 
  }

  inline TObjArray *GetEMGen(){ return fEMGen; }
  inline TObjArray *GetCDCs(){ return fCDCs; }

  inline Int_t GetCDCEntries(){ return 
			  ( fCDCs == NULL ? 0 : fCDCs->GetEntries()); }
  inline JSFCDCTrack *GetCDCTrackAt(Int_t i){ 
    return ( fCDCs == NULL ? NULL : ((JSFCDCTrack*)fCDCs->UncheckedAt(i)) ) ; 
  }
  inline Int_t GetIDCDC(Int_t i){ return ( fIDCDC == NULL ? -1 : fIDCDC[i] ) ; }


  TVector GetPV(){ TVector p(4) ; 
          p(0)=fP[0] ; p(1) =fP[1] ; p(2)=fP[2] ; p(3)=fP[3] ; return p ; }

  ClassDef(JSFLTKCLTrack,5)  //A JSFLTKCLTrack
};

#endif
