//*-- Author :    Akiya Miyamoto   01/08/98
//_____________________________________________________________________________
//
//  GeneratorParticle Class
//
//  To store particle information produced by event generators
//
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

// #include "JSFGenerator.h"
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
