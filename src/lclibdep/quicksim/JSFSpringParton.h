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
#include "TLorentzVector.h"

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
  Double_t  fLifeTime;   // ctau [cm]
  Int_t     fPyjetsPosition; // Position where this fermion is placed in pyjets
                          //  in hadronizer. ( 0 to N ), -1 when not valid
public:
  JSFSpringParton() {}

  JSFSpringParton(Int_t Ser, Int_t ID, Double_t Mass, Double_t Charge,
         TVector& P, 
	 Int_t Ndaughter, Int_t FirstDaughter, Int_t Mother,
		  Int_t Helicity, Int_t ColorID, Int_t ShowerInfo, 
		  Double_t ctau = 0., Int_t pyjetspos=-1){
         fSer = Ser ; fID=ID; fMass=Mass; fCharge=Charge;
	 for(Int_t i=0;i<4;i++){ fP[i]=P(i) ; }
	 fNdaughter=Ndaughter ; fFirstDaughter=FirstDaughter ;
	 fMother=Mother; fHelicity=Helicity, fColorID=ColorID; 
         fShowerInfo=ShowerInfo ; fLifeTime=ctau; fPyjetsPosition=pyjetspos;}


  JSFSpringParton(Int_t Ser, Int_t ID, Double_t Mass, Double_t Charge,
		       TVector& P ) {
         fSer = Ser ; fID=ID; fMass=Mass; fCharge=Charge;
	 for(Int_t i=0;i<4;i++){ fP[i]=P(i) ; }
	 fNdaughter=0; fFirstDaughter=0;
	 fMother=0; fHelicity=0 ; fColorID=0; fShowerInfo=0; fLifeTime=0.;
         fPyjetsPosition=-1; }

  JSFSpringParton(Float_t data[]);
  void SetPyjetsPosition(Int_t i){ fPyjetsPosition=i; }

  inline Double_t GetPx(){ return fP[1] ;}
  inline Double_t GetPy(){ return fP[2] ;}
  inline Double_t GetPz(){ return fP[3] ;}
  inline Double_t GetE(){  return fP[0] ;}
  inline Double_t GetPabs(){ return TMath::Sqrt( fP[1]*fP[1]+fP[2]*fP[2]+fP[3]*fP[3]);}
  inline Double_t GetAzimthAngle(){ return TMath::ATan2( fP[2], fP[1]);}
  inline Double_t GetPt(){ return TMath::Sqrt( fP[1]*fP[1]+fP[2]*fP[2]);}
  inline Double_t GetCosth(){ return fP[3]/GetPabs() ;}


  inline Int_t   GetSerial(){ return fSer;}
  inline Int_t   GetID(){ return fID;}
  inline Double_t GetCharge(){ return fCharge;}
  inline Double_t GetMass(){ return fMass;}

  inline Int_t GetNDaughter(){ return fNdaughter;}
  inline Int_t GetFirstDaughter(){ return fFirstDaughter;}
  inline Int_t GetMother(){ return fMother; }
  inline Int_t GetHelicity(){ return fHelicity;}
  inline Int_t GetColorID(){ return fColorID;}
  inline Int_t GetShowerInfo(){ return fShowerInfo;}
  inline Double_t GetLifeTime() { return fLifeTime; }
  inline Int_t GetPyjetsPosition(){ return fPyjetsPosition; }

  inline TVector GetPV(){ TVector p(4) ; 
          p(0)=fP[0] ; p(1) =fP[1] ; p(2)=fP[2] ; p(3)=fP[3] ; return p ; }

  inline TLorentzVector GetLorentzVector(){ return TLorentzVector(fP[1],fP[2],fP[3],fP[0]);}

  virtual void ls(Option_t* option="");

  virtual ~JSFSpringParton() {}

  ClassDef(JSFSpringParton,4)  //A JSFSpringParton segment
};

#endif
