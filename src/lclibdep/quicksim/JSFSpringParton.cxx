//*LastUpdate:  v.01.01 11-November-1998  Akiya Miyamoto
//*-- Author :    Akiya Miyamoto   1-August-1998
//_____________________________________________________________________________
//
//  SpringParton Class
//
//  To store particle information produced by event generators
// 
//  When JSFSpringParton is created, following parameters are specified.
//
//   Int_t  fSer;        // parton serial number ( 1 to n )
//   Int_t  fID;         // Particle ID a la PDG.
//   Double_t  fMass;    // Particle mass (GeV);
//   Double_t  fCharge;  // Charge
//   Real_t    fP[4];    // four momentum (E,Px,Py,Pz), GeV
//   Int_t     fNdaughter;     // Number of daughter particles
//   Int_t     fFirstDaughter; // Serial number of 1st daughter
//   Int_t     fMother;  // Serial number of mother particle ( =0 for initial)
//   Int_t     fHelicity;  // parton helicity
//   Int_t     fColorID;   // Color singlet group ID
//   Int_t     fShowerInfo; // ShowerLevel*100 + Shower-PairID
//
//  Where fNdaughter, fFirstDaughter, fMother are numbers ranging from 1 to N
//  and corresponds to the fSer value.    
//  
//  For fHelicity, fColorID and fShowerInfo, see comments below,
//  which is taken from FORTRAN subroutine, LUFRAG.
// 
//  The parton showering is handled level by level, since showers
//  from parent parton pairs might modify the momenta of their
//  daughter partons.
//  The level is defined by
//     RBUF(19,*) = ISLEV*100 + ISPAIR
//  where partons with the same ISLEV are passed to LUFRAG at one
//  time (that means they comprise a set of color singlet states and
//  that there is no parent-daughter relation among the set) and
//  partons with the same ISPAIR are paired in LUSHOW calls in
//  LUFRAG. 
//  Lepton pairs are required to have ISLEV = 0 and NDAU = 0 for
//  proper polarization handling when the pair contains a tau.
//  If you are satisfied with the default tau decay treatment, you
//  do not have to distinguish leptons from quarks in the decay
//  daughters from W or Z, etc.
//  Color singlet groups are distinguished by
//     RBUF(18,*) = ICF
//  where ICF must have different values for different color singlet
//  groups.
//  Helicities of the primary partons should be stored in
//     RBUF(17,*) = IHEL
//  when necessary as with tau leptons.
//
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#include "JSFConfig.h"
#include "JSFSpringParton.h"

#include <iostream>
#include <iomanip>

ClassImp(JSFSpringParton)


// ---------------------------------------------------------------
void JSFSpringParton::ls(Option_t *opt)
{
  using namespace std;
  TString sopt(opt);
  sopt.ToLower();
  Int_t outform=1;
  if ( sopt.Contains("form2")) { outform=2; }
  if ( sopt.Contains("title") && outform != 1 ) {
    cout << "------------------------------------------------------"
         << "------------------------------------------------------"
         << "-------------------------------"
         << endl 
         << "  No   PID     Q    Mass Mother   Ndau  1stDau     "
         << "Helicity ColorID ShowerInf       E       Px       Py       Pz   "
         << endl
         << "------------------------------------------------------"
         << "------------------------------------------------------"
         << "-------------------------------"
         << endl;
  }
  if ( sopt.Contains("only") ) { return ; }
  
  if ( outform==2 ) {
    cout << setw( 4) << fSer 
         << setw( 6) << fID 
         << setw( 6) << setprecision(2) << showpoint << fixed << fCharge
         << setw( 8) << setprecision(3) << showpoint << fixed << fMass 
         << setw( 7) << fMother
         << setw( 9) << fNdaughter
         << setw( 8) << fFirstDaughter
         << setw( 8) << fHelicity
         << setw( 8) << fColorID
         << setw( 8) << fShowerInfo
         << setw( 9) << setprecision(2) << showpoint << fixed << fP[0]
         << setw( 9) << setprecision(2) << showpoint << fixed << fP[1]
         << setw( 9) << setprecision(2) << showpoint << fixed << fP[2]
         << setw( 9) << setprecision(2) << showpoint << fixed << fP[3]
         << endl;
  } else {
    printf(" Particle ID=%d  Mass=%g GeV Charge=%g",fID, fMass, fCharge);
    printf(" (E,Px,Py,Pz)=(%g,%g,%g,%g) (GeV)",fP[0],fP[1],fP[2],fP[3]);
    printf("\n");
  }
}


// ---------------------------------------------------------------
JSFSpringParton::JSFSpringParton(Float_t data[])
{
   fSer=(Int_t)data[0];  
   fID=(Int_t)data[1] ; fMass=data[2] ; fCharge=data[3];
   fP[1]=data[4] ; fP[2]=data[5] ; fP[3]=data[6] ; fP[0]=data[7];
   fNdaughter=(Int_t)data[11] ; fFirstDaughter=(Int_t)data[12] ;
   fMother=(Int_t)data[13];
   fHelicity=(Int_t)data[16] ; fColorID=(Int_t)data[17]; 
   fShowerInfo=(Int_t)data[18];
}

#if __ROOT_FULLVERSION >= 30000
//_____________________________________________________________________________
void JSFSpringParton::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFSpringParton.

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v=R__b.ReadVersion(&R__s, &R__c);
     if( R__v > 1 ) {
       JSFSpringParton::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
       return;
     }
     TObject::Streamer(R__b);
     R__b >> fSer;
     R__b >> fID;
     R__b >> fMass;
     R__b >> fCharge;
     R__b.ReadStaticArray(fP);
     R__b >> fNdaughter;
     R__b >> fFirstDaughter;
     R__b >> fMother;
     R__b >> fHelicity;
     R__b >> fColorID;
     R__b >> fShowerInfo;
     R__b.CheckByteCount(R__s, R__c, JSFSpringParton::IsA());

   } else {
     JSFSpringParton::Class()->WriteBuffer(R__b, this);
   }
}
#else

//______________________________________________________________________________
void JSFSpringParton::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFSpringParton.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> fSer;
      R__b >> fID;
      R__b >> fMass;
      R__b >> fCharge;
      R__b.ReadStaticArray(fP);
      R__b >> fNdaughter;
      R__b >> fFirstDaughter;
      R__b >> fMother;
      R__b >> fHelicity;
      R__b >> fColorID;
      R__b >> fShowerInfo;
   } else {
      R__b.WriteVersion(JSFSpringParton::IsA());
      TObject::Streamer(R__b);
      R__b << fSer;
      R__b << fID;
      R__b << fMass;
      R__b << fCharge;
      R__b.WriteArray(fP, 4);
      R__b << fNdaughter;
      R__b << fFirstDaughter;
      R__b << fMother;
      R__b << fHelicity;
      R__b << fColorID;
      R__b << fShowerInfo;
   }
}

#endif
