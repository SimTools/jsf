#ifndef __JSFSpringParton__
#define __JSFSpringParton__

//////////////////////////////////////////////////////////////////////////
//
// JSFSpringParton 
//
// Description of JSFSpringParton parameters 
//
//$Id$
//
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TVector.h"

class JSFSpringParton : public TObject {

public:
  Int_t  fSer;        // parton serial number ( 1 to n )
  Int_t  fID;         // Particle ID a la PDG.
  Double_t  fMass;    // Particle mass (GeV);
  Double_t  fCharge;  // Charge
  Real_t    fP[4];    // four momentum (E,Px,Py,Pz), GeV
  Int_t     fNdaughter;     // Number of daughter particles
  Int_t     fFirstDaughter; // Serial number of 1st daughter
  Int_t     fMother;  // Serial number of mother particle ( =0 for initial)
  Int_t     fHelicity;  // parton helicity
  Int_t     fColorID;   // Color singlet group ID
  Int_t     fShowerInfo; // ShowerLevel*100 + Shower-PairID

public:
  JSFSpringParton() {}

  JSFSpringParton(Int_t Ser, Int_t ID, Double_t Mass, Double_t Charge,
         TVector& P, 
	 Int_t Ndaughter, Int_t FirstDaughter, Int_t Mother,
		  Int_t Helicity, Int_t ColorID, Int_t ShowerInfo){
         fSer = Ser ; fID=ID; fMass=Mass; fCharge=Charge;
	 for(Int_t i=0;i<4;i++){ fP[i]=P(i) ; }
	 fNdaughter=Ndaughter ; fFirstDaughter=FirstDaughter ;
	 fMother=Mother; fHelicity=Helicity, fColorID=ColorID; 
         fShowerInfo=ShowerInfo ; }


  JSFSpringParton(Int_t Ser, Int_t ID, Double_t Mass, Double_t Charge,
		       TVector& P ) {
         fSer = Ser ; fID=ID; fMass=Mass; fCharge=Charge;
	 for(Int_t i=0;i<4;i++){ fP[i]=P(i) ; }
	 fNdaughter=0; fFirstDaughter=0;
	 fMother=0; fHelicity=0 ; fColorID=0; fShowerInfo=0;}

  JSFSpringParton(Float_t data[]);

  Double_t GetPx(){ return fP[1] ;}
  Double_t GetPy(){ return fP[2] ;}
  Double_t GetPz(){ return fP[3] ;}
  Double_t GetE(){  return fP[0] ;}
  Double_t GetPabs(){ return TMath::Sqrt( fP[1]*fP[1]+fP[2]*fP[2]+fP[3]*fP[3]);}
  Double_t GetAzimthAngle(){ return TMath::ATan2( fP[2], fP[1]);}
  Double_t GetPt(){ return TMath::Sqrt( fP[1]*fP[1]+fP[2]*fP[2]);}
  Double_t GetCosth(){ return fP[3]/GetPabs() ;}


  Int_t   GetSerial(){ return fSer;}
  Int_t   GetID(){ return fID;}
  Double_t GetCharge(){ return fCharge;}
  Double_t GetMass(){ return fMass;}

  Int_t GetNDaughter(){ return fNdaughter;}
  Int_t GetFirstDaughter(){ return fFirstDaughter;}
  Int_t GetMother(){ return fMother; }
  Int_t GetHelicity(){ return fHelicity;}
  Int_t GetColorID(){ return fColorID;}
  Int_t GetShowerInfo(){ return fShowerInfo;}

  TVector GetPV(){ TVector p(4) ; 
          p(0)=fP[0] ; p(1) =fP[1] ; p(2)=fP[2] ; p(3)=fP[3] ; return p ; }

  virtual void ls(Option_t* option="");

  virtual ~JSFSpringParton() {}

  ClassDef(JSFSpringParton,1)  //A JSFSpringParton segment
};

#endif
