//*LastUpdate :  jsf-1-7  7-April-1999  By Akiya Miyamoto
//*-- Author  : Akiya Miyamoto  1-August-1999

/*
  20-Apri-1999 Akiya Miyamoto
          One of the constructor of JSFGeneratorParticle calls constructor 
          twice, that causes the error.
*/

///////////////////////////////////////////////////////////////////
//
// JSFGeneratorParticle
// 
//  To store particle information produced by event generators
//
//$Id:
///////////////////////////////////////////////////////////////////

#include "JSFConfig.h"
#include "JSFGeneratorParticle.h"


ClassImp(JSFGeneratorParticle)


// ---------------------------------------------------------------
void JSFGeneratorParticle::ls(Option_t *opt)
{
  printf(" Particle ID=%d",fID);
  printf(" Mass=%g GeV Charge=%g",fMass, fCharge);
  if( strcmp(opt,"all") ==0 ) {
    printf(" Ser=%d Ndau=%d  1stDau=%d",fSer,fNdaughter,fFirstDaughter);
    printf(" Mother=%d",fMother);
    printf(" Life=%g DL=%g\n       ",fLifeTime, fDecayLength);
  }
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
  fSer = Ser ; fID=ID; fMass=Mass; fCharge=Charge;
  for(Int_t i=0;i<4;i++){ fP[i]=P(i) ; fX[i]=X(i) ; }
  fNdaughter=0; fFirstDaughter=0;
  fMother=0; fLifeTime=0.0; fDecayLength=0.0; 
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

// ---------------------------------------------------------------
JSFGeneratorParticle::JSFGeneratorParticle(JSFGeneratorParticle& g)
{
  fSer=g.fSer ; fID=g.fID ; fMass=g.fMass ; fCharge=g.fCharge;
  for(Int_t i=0;i<4;i++){ fP[i]=g.fP[i]; fX[i]=g.fX[i]; }
  fNdaughter=g.fNdaughter ; fFirstDaughter=g.fFirstDaughter;
  fMother=g.fMother ; fLifeTime=g.fLifeTime;
  fDecayLength=g.fDecayLength;
}


#if __ROOT_FULLVERSION__ >= 30000
//______________________________________________________________________________
void JSFGeneratorParticle::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFGeneratorParticle.

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v=R__b.ReadVersion(&R__s, &R__c);
     if( R__v > 2 ) {
       JSFGeneratorParticle::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
       return;
     }
     TObject::Streamer(R__b);
     R__b >> fSer;
     R__b >> fID;
     R__b >> fMass;
     R__b >> fCharge;
     R__b.ReadStaticArray(fP);
     R__b.ReadStaticArray(fX);
     R__b >> fNdaughter;
     R__b >> fFirstDaughter;
     R__b >> fMother;
     R__b >> fLifeTime;
     R__b >> fDecayLength;
     R__b.CheckByteCount(R__s, R__c, JSFGeneratorParticle::IsA());

   } else {
     JSFGeneratorParticle::Class()->WriteBuffer(R__b, this);
   }
}

#else


//______________________________________________________________________________
void JSFGeneratorParticle::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFGeneratorParticle.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> fSer;
      R__b >> fID;
      R__b >> fMass;
      R__b >> fCharge;
      R__b.ReadStaticArray(fP);
      R__b.ReadStaticArray(fX);
      R__b >> fNdaughter;
      R__b >> fFirstDaughter;
      R__b >> fMother;
      R__b >> fLifeTime;
      R__b >> fDecayLength;
   } else {
      R__b.WriteVersion(JSFGeneratorParticle::IsA());
      TObject::Streamer(R__b);
      R__b << fSer;
      R__b << fID;
      R__b << fMass;
      R__b << fCharge;
      R__b.WriteArray(fP, 4);
      R__b.WriteArray(fX, 4);
      R__b << fNdaughter;
      R__b << fFirstDaughter;
      R__b << fMother;
      R__b << fLifeTime;
      R__b << fDecayLength;
   }
}

#endif
