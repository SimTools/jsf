//*-- Author :    Akiya Miyamoto   01/08/98
//_____________________________________________________________________________
//
//  SpringParton Class
//
//  To store particle information produced by event generators
//
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

#include "JSFSpringParton.h"


ClassImp(JSFSpringParton)


// ---------------------------------------------------------------
void JSFSpringParton::ls(Option_t *)
{
  printf(" Particle ID=%d  Mass=%g GeV Charge=%g",fID, fMass, fCharge);
  printf(" (E,Px,Py,Pz)=(%g,%g,%g,%g) (GeV)",fP[0],fP[1],fP[2],fP[3]);
  printf("\n");
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



