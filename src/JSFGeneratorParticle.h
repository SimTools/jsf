#ifndef __JSFGeneratorParticle__
#define __JSFGeneratorParticle__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFGeneratorParticle                                                 //
//                                                                      //
// Description of JSFGeneratorParticle parameters                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TVector.h"

class JSFGeneratorParticle : public TObject {

public:
  Short_t  fSer;        // particle serial number ( 1 to n )
  Short_t  fID;         // Particle ID a la PDG.
  Float_t  fMass;    // Particle mass (GeV);
  Float_t  fCharge;  // Charge
  Float_t    fP[4];    // four momentum (E,Px,Py,Pz), GeV
  Float_t    fX[4];    // (t,x,y,z) (cm and nsec)
  Short_t     fNdaughter;     // Number of daughter particles ( =0 for stable )
  Short_t     fFirstDaughter; // Serial number of 1st daughter
  Short_t     fMother;  // Serial number of mother particle ( =0 for initial)
  Float_t  fLifeTime; // Particle life time ( c x nsec, =0 for stable, cm)
  Float_t  fDecayLength;  // Decay length (cm, =0 for stable)

public:
  JSFGeneratorParticle() {}

  JSFGeneratorParticle(Int_t Ser, Int_t ID, Float_t Mass, Float_t Charge,
         TVector& P, TVector& X,
	 Int_t Ndaughter, Int_t FirstDaughter, Int_t Mother,
         Float_t LifeTime, Float_t DecayLength){
         fSer = Ser ; fID=ID; fMass=Mass; fCharge=Charge;
	 for(Int_t i=0;i<4;i++){ fP[i]=P(i) ; fX[i]=X(i) ; }
	 fNdaughter=Ndaughter ; fFirstDaughter=FirstDaughter ;
	 fMother=Mother; fLifeTime=LifeTime ; fDecayLength=DecayLength; }


  JSFGeneratorParticle(Int_t Ser, Int_t ID, Float_t Mass, Float_t Charge,
		       TVector& P, TVector& X ) {
         fSer = Ser ; fID=ID; fMass=Mass; fCharge=Charge;
	 for(Int_t i=0;i<4;i++){ fP[i]=P(i) ; fX[i]=X(i) ; }
	 fNdaughter=0; fFirstDaughter=0;
	 fMother=0; fLifeTime=0.0; fDecayLength=0.0; }

  JSFGeneratorParticle(Int_t Ser, Int_t ID, Float_t Mass, Float_t Charge,
		       TVector& P ) {
         fSer = Ser ; fID=ID; fMass=Mass; fCharge=Charge;
	 for(Int_t i=0;i<4;i++){ fP[i]=P(i) ; fX[i]=0.0 ; }
	 fNdaughter=0; fFirstDaughter=0;
	 fMother=0; fLifeTime=0.0; fDecayLength=0.0; }

  JSFGeneratorParticle(Float_t data[]);

  Float_t GetPx(){ return fP[1] ;}
  Float_t GetPy(){ return fP[2] ;}
  Float_t GetPz(){ return fP[3] ;}
  Float_t GetE(){  return fP[0] ;}
  Float_t GetPabs(){ return TMath::Sqrt( fP[1]*fP[1]+fP[2]*fP[2]+fP[3]*fP[3]);}
  Float_t GetAzimthAngle(){ return TMath::ATan2( fP[2], fP[1]);}
  Float_t GetPt(){ return TMath::Sqrt( fP[1]*fP[1]+fP[2]*fP[2]);}
  Float_t GetCosth(){ return fP[3]/GetPabs() ;}


  TVector GetPV(){ TVector p(4) ; 
          p(0)=fP[0] ; p(1) =fP[1] ; p(2)=fP[2] ; p(3)=fP[3] ; return p ; }
  TVector GetXV(){ TVector x(4) ; 
          x(0)=fX[0] ; x(1) =fX[1] ; x(2)=fX[2] ; x(3)=fX[3] ; return x ; }

  virtual void ls(Option_t* option="");

  virtual ~JSFGeneratorParticle() {}

  ClassDef(JSFGeneratorParticle,1)  //A JSFGeneratorParticle segment
};

#endif
