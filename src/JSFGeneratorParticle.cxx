//*LastUpdate :  jsf-1-7  7-April-1999  By Akiya Miyamoto
//*-- Author  : Akiya Miyamoto  1-August-1999

///////////////////////////////////////////////////////////////////
//
// JSFGeneratorParticle
// 
//  To store particle information produced by event generators
//
//$Id:
///////////////////////////////////////////////////////////////////

#include "JSFGeneratorParticle.h"


ClassImp(JSFGeneratorParticle)


// ---------------------------------------------------------------
void JSFGeneratorParticle::ls(Option_t *)
{
  printf(" Particle ID=%d  Mass=%g GeV Charge=%g",fID, fMass, fCharge);
  printf(" (E,Px,Py,Pz)=(%g,%g,%g,%g) (GeV)",fP[0],fP[1],fP[2],fP[3]);
  printf(" (T,X,Y,Z)=(%g,%g,%g,%g) (cm)",fX[0],fX[1],fX[2],fX[3]);
  printf("\n");
}


// ---------------------------------------------------------------
JSFGeneratorParticle::JSFGeneratorParticle(Int_t Ser, 
         Int_t ID, Float_t Mass, Float_t Charge, TVector& P, TVector& X,
	 Int_t Ndaughter, Int_t FirstDaughter, Int_t Mother,
         Float_t LifeTime, Float_t DecayLength)
{
  fSer = Ser ; fID=ID; fMass=Mass; fCharge=Charge;
  for(Int_t i=0;i<4;i++){ fP[i]=P(i) ; fX[i]=X(i) ; }
  fNdaughter=Ndaughter ; fFirstDaughter=FirstDaughter ;
  fMother=Mother; fLifeTime=LifeTime ; fDecayLength=DecayLength; 
}


// ---------------------------------------------------------------
JSFGeneratorParticle::JSFGeneratorParticle(Int_t Ser, 
         Int_t ID, Float_t Mass, Float_t Charge, TVector& P, TVector& X ) 
{
  JSFGeneratorParticle(Ser, ID, Mass, Charge, P);
  for(Int_t i=0;i<4;i++){ fX[i]=X(i) ; }
}

// ---------------------------------------------------------------
JSFGeneratorParticle::JSFGeneratorParticle(Int_t Ser, 
         Int_t ID, Float_t Mass, Float_t Charge, TVector& P ) 
{
  fSer = Ser ; fID=ID; fMass=Mass; fCharge=Charge;
  for(Int_t i=0;i<4;i++){ fP[i]=P(i) ; fX[i]=0.0 ; }
  fNdaughter=0; fFirstDaughter=0;
  fMother=0; fLifeTime=0.0; fDecayLength=0.0; 
}


// ---------------------------------------------------------------
JSFGeneratorParticle::JSFGeneratorParticle(Float_t data[])
{
   fSer=(Int_t)data[0];  
   fID=(Int_t)data[1] ; fMass=data[2] ; fCharge=data[3];
   fP[1]=data[4] ; fP[2]=data[5] ; fP[3]=data[6] ; fP[0]=data[7];
   fX[1]=data[8] ; fX[2]=data[9] ; fX[3]=data[10]; fX[0]=data[14];
   fNdaughter=(Int_t)data[11] ; fFirstDaughter=(Int_t)data[12] ;
   fMother=(Int_t)data[13]; fLifeTime=data[15];
   fDecayLength=data[16];
}
