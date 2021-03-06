#ifndef __JSFGeneratorParticle__
#define __JSFGeneratorParticle__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFGeneratorParticle                                                 //
//                                                                      //
// Description of JSFGeneratorParticle parameters                       //
//                                                                      //
//$Id$                                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TVector.h"
#include "TLorentzVector.h"

class JSFGeneratorParticle : public TObject {

public:
enum EGeneratorParticle_Idata 
     { kStatus=0, kSecondMother=1, kColorFlow0=2, kColorFlow1=3, 
       kIdataSize=4  } ;
enum EGeneratorParticle_Rdata
     { kSerial=0, kID=1, kMass=2, kCharge=3, 
       kPx=4, kPy=5, kPz=6, kE=7, kX=8, kY=9, kZ=10, kT=14,
       kNDaughter=11, kFirstDaughter=12, kMother=13,
       kLifeTime=15,  kDecayLength=16, 
       kSpinX=17,     kSpinY=18,  kSpinZ=19,
       kRdataSize=20 } ;

public:
  Int_t    fSer;        // particle serial number ( 1 to n )
  Int_t    fID;         // Particle ID a la PDG.
  Float_t  fMass;    // Particle mass (GeV);
  Float_t  fCharge;  // Charge
  Float_t  fP[4];    // four momentum (E,Px,Py,Pz), GeV
  Float_t  fX[4];    // (t,x,y,z) (cm and nsec)
  Int_t    fNdaughter;     // Number of daughter particles ( =0 for stable )
  Int_t    fFirstDaughter; // Serial number of 1st daughter
  Int_t    fMother;  // Serial number of mother particle ( =0 for initial)
  Float_t  fLifeTime; // Particle life time ( c x nsec, =0 for stable, cm)
  Float_t  fDecayLength;  // Decay length (cm, =0 for stable)
// If fDecayLength <= 0, vertex point is re-evaluated using fLifeTime.
  Int_t    fSecondMother;   // Serial Number of 2nd mother
  Int_t    fStatus;      // Particle status code.
// Followling variables are extended since version 5
  Float_t  fSpin[3]; 
  Int_t    fColorFlow[2];

public:
  JSFGeneratorParticle():fSer(0),fID(0),fMass(0.0),fCharge(0.0),
    fNdaughter(0),fFirstDaughter(0),fMother(0),fLifeTime(0.0),
    fDecayLength(0.0),fSecondMother(-1),fStatus(-9999) {}

  JSFGeneratorParticle(Int_t Ser, Int_t ID, Float_t Mass, Float_t Charge,
         TVector& P, TVector& X,
	 Int_t Ndaughter, Int_t FirstDaughter, Int_t Mother,
         Float_t LifeTime, Float_t DecayLength);

  JSFGeneratorParticle(Int_t Ser, Int_t ID, Float_t Mass, Float_t Charge,
		       TVector& P, TVector& X );

  JSFGeneratorParticle(Int_t Ser, Int_t ID, Float_t Mass, Float_t Charge,
		       TVector& P);

  JSFGeneratorParticle(Float_t data[]);
  JSFGeneratorParticle(Double_t data[]);
  JSFGeneratorParticle(Float_t data[], Int_t idata[]);
  JSFGeneratorParticle(Double_t data[],Int_t idata[]);
  JSFGeneratorParticle(JSFGeneratorParticle& g);
  void SetDataByFloat(Float_t data[]);
  void SetDataByDouble(Double_t data[]);

  inline Float_t GetPx(){ return fP[1] ;}
  inline Float_t GetPy(){ return fP[2] ;}
  inline Float_t GetPz(){ return fP[3] ;}
  inline Float_t GetE(){  return fP[0] ;}
  inline Float_t GetX(){ return fX[1] ;}
  inline Float_t GetY(){ return fX[2] ;}
  inline Float_t GetZ(){ return fX[3] ;}
  inline Float_t GetT(){  return fX[0] ;}

  inline Float_t GetPabs(){ 
    return TMath::Sqrt( fP[1]*fP[1]+fP[2]*fP[2]+fP[3]*fP[3]);}
  inline Float_t GetAzimthAngle(){ return TMath::ATan2( fP[2], fP[1]);}
  inline Float_t GetPt(){ return TMath::Sqrt( fP[1]*fP[1]+fP[2]*fP[2]);}
  inline Float_t GetCosth(){ return fP[3]/GetPabs() ;}

  Int_t   GetSerial(){ return fSer;}
  Int_t   GetID(){ return fID;}
  Float_t GetCharge(){ return fCharge;}
  Float_t GetMass(){ return fMass;}
  Int_t GetNDaughter(){ return (Int_t)fNdaughter;}
  Int_t GetFirstDaughter(){ return (Int_t)fFirstDaughter;}
  Int_t GetMother(){ return (Int_t)fMother; }
  Float_t GetLifeTime(){ return fLifeTime; }
  Float_t GetDecayLength(){ return fDecayLength;}
//
  const Float_t *GetSpin(){ return fSpin; }
  const Int_t   *GetColorFlow(){ return fColorFlow; }   

  TVector GetPV(){ TVector p(4) ; 
          p(0)=fP[0] ; p(1) =fP[1] ; p(2)=fP[2] ; p(3)=fP[3] ; return p ; }
  TVector GetXV(){ TVector x(4) ; 
          x(0)=fX[0] ; x(1) =fX[1] ; x(2)=fX[2] ; x(3)=fX[3] ; return x ; }

  inline TVector3 GetXV3() const;
  // return 3D vector of vertex

  inline TLorentzVector GetLorentz() const;
  // return LorentzVector

  virtual void ls(Option_t* option="");

  virtual ~JSFGeneratorParticle() {}


  inline void   SetSecondMother(Int_t mod){ fSecondMother=mod; }
  inline void   SetStatus(Int_t stat){ fStatus=stat; }
  inline Int_t  GetSecondMother(){ return fSecondMother; }
  inline Int_t  GetStatus(){ return fStatus; }
  inline void   SetMomentum(Double_t e, Double_t px, Double_t py, Double_t pz) {
        fP[0]=e ; fP[1]=px ; fP[2]=py ; fP[3]=pz; 
  }
  inline void   SetPosition(Double_t x, Double_t y, Double_t z) {
        fX[1]=x ; fX[2]=y ; fX[3]=z ; 
  }
  inline void   SetTime(Double_t t) {  fX[0]=t ;  }

  inline void   SetSpinAndColorflow(Float_t spin[3], Int_t colorflow[2]) {
    for(Int_t i=0;i<3;i++){ fSpin[i]=spin[i]; }
    for(Int_t i=0;i<2;i++){ fColorFlow[i]=colorflow[i]; }
  }

  inline Bool_t HasSecondMother(){ return ( fSecondMother < 0 ? kFALSE : kTRUE ) ; }

  ClassDef(JSFGeneratorParticle, 5)  //A JSFGeneratorParticle segment
};

//**********************

  inline TLorentzVector JSFGeneratorParticle::GetLorentz() const {
    return TLorentzVector(fP[1], fP[2], fP[3],fP[0]); 
  }

  inline TVector3 JSFGeneratorParticle::GetXV3() const { 
    return TVector3(fX[1], fX[2], fX[3]);
  }


#endif
