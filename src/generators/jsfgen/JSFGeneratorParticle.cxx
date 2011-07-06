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
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,15,0)
#include "TClass.h"
#endif

#include <iostream>
#include <iomanip>


ClassImp(JSFGeneratorParticle)


// ---------------------------------------------------------------
void JSFGeneratorParticle::ls(Option_t *opt)
{
#if 1
  using namespace std;
  static Bool_t tline = kFALSE;
  TString sopt(opt);
  sopt.ToLower();
  Int_t outform=1;
  if ( sopt.Contains("form2") ) { outform=2; } 
  if ( sopt.Contains("form3") ) { outform=3; } 
  if (!tline || sopt.Contains("title")) {
    cout << "------------------------------------------------------"
         << "------------------------------------------------------"
         << "---------------------------------"
         << endl;
    if ( outform == 2 ) {
      cout  << "  No     PID     Q    Mass St Mom     Ndau  1stDau     " ;
      cout  << "  ctau       DL        E       Px       Py       Pz   " 
            << "    Vt       Vx       Vy       Vz" 
            << endl;
    }
    else if ( outform == 3 ) {
      cout  << "  No     PID     Q    Mass St Mom ND  1D [IC1,IC2] " ;
      cout  << "  ctau       DL        E       Px       Py       Pz   " 
            << "  PolX   PolY   PolZ  Hel." 
            << endl;
    }
    else {
      cout  << "  No     PID     Q    Mass Mother   Ndau  1stDau     ";
      cout  << "  ctau       DL        E       Px       Py       Pz   " 
            << "    Vt       Vx       Vy       Vz" 
           << endl;
    }
    cout << "------------------------------------------------------"
         << "------------------------------------------------------"
         << "---------------------------------"
         << endl;
    tline = kTRUE;
  }
  if ( sopt.Contains("only") ) { return ; }
  cout << setw( 4) << fSer
       << setw( 8) << fID
       << setw( 6) << setprecision(2) << showpoint << fixed << fCharge
       << setw( 8) << setprecision(3) << showpoint << fixed << fMass ;
  if ( outform==2 ) {
    cout << setw( 3) << fStatus
         << setw( 4) << fMother
         << setw(10) << fNdaughter
         << setw(10) << fFirstDaughter;
  }
  else if ( outform == 3 ) {
    cout << setw( 3) << fStatus
         << setw( 4) << fMother
         << setw( 3) << fNdaughter
         << setw( 4) << fFirstDaughter;
  }
  else {
    cout << setw( 7) << fMother
         << setw( 7) << (TMath::Abs(fNdaughter) > 1000 ? -99999 : fNdaughter) 
         << setw( 8) << fFirstDaughter;
  }
  if( outform != 3 ) {
    cout << setw(11) << setprecision(3) << showpoint << fixed << fLifeTime
         << setw( 9) << setprecision(3) << showpoint << fixed << fDecayLength
         << setw( 9) << setprecision(2) << showpoint << fixed << fP[0]
         << setw( 9) << setprecision(2) << showpoint << fixed << fP[1]
         << setw( 9) << setprecision(2) << showpoint << fixed << fP[2]
         << setw( 9) << setprecision(2) << showpoint << fixed << fP[3]
         << setw( 9) << setprecision(3) << showpoint << fixed << fX[0]
         << setw( 9) << setprecision(3) << showpoint << fixed << fX[1]
         << setw( 9) << setprecision(3) << showpoint << fixed << fX[2]
         << setw( 9) << setprecision(3) << showpoint << fixed << fX[3]
         << endl;
  }
  else {
    cout << " [" << setw(3) << fColorFlow[0] 
	 << ","  << setw(3) << fColorFlow[1] << "] " ;
    float pa= sqrt( fP[1]*fP[1]+fP[2]*fP[2]+fP[3]*fP[3]);
    float aspin = sqrt( fSpin[0]*fSpin[0] + fSpin[1]*fSpin[1] + fSpin[2]*fSpin[2] );
    float prod=pa != 0.0 && aspin != 0.0 ? (fP[1]*fSpin[0]+fP[2]*fSpin[1]+fP[3]*fSpin[2])/pa/aspin : 0.0 ;
    cout << setw( 9) << setprecision(3) << showpoint << fixed << fLifeTime
         << setw( 9) << setprecision(3) << showpoint << fixed << fDecayLength
         << setw( 9) << setprecision(2) << showpoint << fixed << fP[0]
         << setw( 9) << setprecision(2) << showpoint << fixed << fP[1]
         << setw( 9) << setprecision(2) << showpoint << fixed << fP[2]
         << setw( 9) << setprecision(2) << showpoint << fixed << fP[3]
         << setw( 7) << setprecision(3) << showpoint << fixed << fSpin[0]
         << setw( 7) << setprecision(3) << showpoint << fixed << fSpin[1]
         << setw( 7) << setprecision(3) << showpoint << fixed << fSpin[2]
         << setw( 7) << setprecision(3) << showpoint << fixed << prod
         << endl;

  }
#else
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
#endif
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
  fSecondMother=-1;
  fStatus=-9999;
  fSpin[0]=0.0;   fSpin[1]=0.0;   fSpin[2]=0.0;
  fColorFlow[0]=0;   fColorFlow[1]=0;
}


// ---------------------------------------------------------------
JSFGeneratorParticle::JSFGeneratorParticle(Int_t Ser, 
         Int_t ID, Float_t Mass, Float_t Charge, TVector& P, TVector& X ) 
{
  fSer = Ser ; fID=ID; fMass=Mass; fCharge=Charge;
  for(Int_t i=0;i<4;i++){ fP[i]=P(i) ; fX[i]=X(i) ; }
  fNdaughter=0; fFirstDaughter=0;
  fMother=0; fLifeTime=0.0; fDecayLength=0.0; 
  fSecondMother=-1;
  fStatus=-9999;
  fSpin[0]=0.0;   fSpin[1]=0.0;   fSpin[2]=0.0;
  fColorFlow[0]=0;   fColorFlow[1]=0;
}

// ---------------------------------------------------------------
JSFGeneratorParticle::JSFGeneratorParticle(Int_t Ser, 
         Int_t ID, Float_t Mass, Float_t Charge, TVector& P ) 
{
  fSer = Ser ; fID=ID; fMass=Mass; fCharge=Charge;
  for(Int_t i=0;i<4;i++){ fP[i]=P(i) ; fX[i]=0.0 ; }
  fNdaughter=0; fFirstDaughter=0;
  fMother=0; fLifeTime=0.0; fDecayLength=0.0; 
  fSecondMother=-1;
  fStatus=-9999;
  fSpin[0]=0.0;   fSpin[1]=0.0;   fSpin[2]=0.0;
  fColorFlow[0]=0;   fColorFlow[1]=0;
}


// ---------------------------------------------------------------
JSFGeneratorParticle::JSFGeneratorParticle(Float_t data[])
{
   SetDataByFloat(data);
}

// ---------------------------------------------------------------
JSFGeneratorParticle::JSFGeneratorParticle(Float_t data[], Int_t idata[])
{
   SetDataByFloat(data);
   fStatus=idata[kStatus];
   fSecondMother=idata[kSecondMother];
   fColorFlow[0]=idata[kColorFlow0];
   fColorFlow[1]=idata[kColorFlow1];
}

// ---------------------------------------------------------------
void JSFGeneratorParticle::SetDataByFloat(Float_t data[])
{
// Format of Data 
   fSer=(Int_t)data[kSerial];
   fID=(Int_t)data[kID] ; fMass=data[kMass] ; fCharge=data[kCharge];
   fP[1]=data[kPx] ; fP[2]=data[kPy] ; fP[3]=data[kPz] ; fP[0]=data[kE];
   fX[1]=data[kX] ; fX[2]=data[kY] ; fX[3]=data[kZ]; fX[0]=data[kT];
   fNdaughter=(Int_t)data[kNDaughter] ; fFirstDaughter=(Int_t)data[kFirstDaughter] ;
   fMother=(Int_t)data[kMother]; fLifeTime=data[kLifeTime];
   fDecayLength=data[kDecayLength];
   fSpin[0]=data[kSpinX];
   fSpin[1]=data[kSpinY];
   fSpin[2]=data[kSpinZ];
   fColorFlow[0]=0;
   fColorFlow[1]=0;
   fSecondMother=-1;
   fStatus=-9999;
}

// ---------------------------------------------------------------
JSFGeneratorParticle::JSFGeneratorParticle(Double_t data[])
{
   SetDataByDouble(data);
}

// ---------------------------------------------------------------
JSFGeneratorParticle::JSFGeneratorParticle(Double_t data[], Int_t idata[])
{
   SetDataByDouble(data);
   fStatus=idata[kStatus];
//   fMother=idata[1];
   fSecondMother=idata[kSecondMother];
   fColorFlow[0]=idata[kColorFlow0];
   fColorFlow[1]=idata[kColorFlow1];
}

// ---------------------------------------------------------------
void JSFGeneratorParticle::SetDataByDouble(Double_t data[])
{
   fSer=(Int_t)data[kSerial];
   fID=(Int_t)data[kID] ; fMass=data[kMass] ; fCharge=data[kCharge];
   fP[1]=data[kPx] ; fP[2]=data[kPy] ; fP[3]=data[kPz] ; fP[0]=data[kE];
   fX[1]=data[kX] ; fX[2]=data[kY] ; fX[3]=data[kZ]; fX[0]=data[kT];
   fNdaughter=(Int_t)data[kNDaughter] ; fFirstDaughter=(Int_t)data[kFirstDaughter] ;
   fMother=(Int_t)data[kMother]; fLifeTime=data[kLifeTime];
   fDecayLength=data[kDecayLength];
   fSpin[0]=data[kSpinX];
   fSpin[1]=data[kSpinY];
   fSpin[2]=data[kSpinZ];
   fColorFlow[0]=0;
   fColorFlow[1]=0;
   fSecondMother=-1;
   fStatus=-9999;
}

// ---------------------------------------------------------------
JSFGeneratorParticle::JSFGeneratorParticle(JSFGeneratorParticle& g)
{
  fSer=g.fSer ; fID=g.fID ; fMass=g.fMass ; fCharge=g.fCharge;
  for(Int_t i=0;i<4;i++){ fP[i]=g.fP[i]; fX[i]=g.fX[i]; }
  fNdaughter=g.fNdaughter ; fFirstDaughter=g.fFirstDaughter;
  fMother=g.fMother ; fLifeTime=g.fLifeTime;
  fDecayLength=g.fDecayLength;
  fSecondMother=g.fSecondMother;
  fStatus=g.fStatus;
  fSpin[0]=g.fSpin[0];   fSpin[1]=g.fSpin[1];  fSpin[2]=g.fSpin[2];
  fColorFlow[0]=g.fColorFlow[0];   fColorFlow[1]=g.fColorFlow[1];
}

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

