//*LastUpdate :  jsf-1-8  18-April-1999  By Akiya Miyamoto
//*-- Author  : A.Miyamoto  18-April-1999


///////////////////////////////////////////////////////////////////
//
// JSFJIMParam
//
// A JIM detector parameter class
// 
//$Id:
//
//////////////////////////////////////////////////////////////////

#include "JSFJIMParam.h"
#include "JSFJLCSIM.h"

ClassImp(JSFJIMParam)

//_____________________________________________________________________________
JSFJIMParam::JSFJIMParam()
{
}

//_____________________________________________________________________________
JSFJIMParam::~JSFJIMParam()
{
}

//_____________________________________________________________________________
void JSFJIMParam::ReadParameter()
{
  // Read detector parameter for JIM from JIM data buffer.

  const Int_t maxbuf=1000;
  Int_t idat[300];
  Int_t len=0;

  JSFJLCSIM::KZGET("MCBCPAR",0, len, idat);
  for(Int_t i=0;i<15;i++){ fBCPAR0I[i]=idat[i]; }
  Float_t *rtmp=(Float_t*)(idat+15);
  for(Int_t i=0;i<16;i++){ fBCPAR0R[i]=rtmp[i]; }

  fEMCal[0]=idat[3];  // # of Phi
  fEMCal[1]=idat[2];  // # of theta
  fHDCal[0]=idat[5];  // # of phi in HD
  fHDCal[1]=idat[4];  // # of theta in HD

  if( idat[1] > maxbuf ) {
    printf(" Fatal error in JSFJIMParam::ReadParameter()\n");
    printf(" Number of phi segment in EM Cal is larger than byfer size.\n");
  }
  
  JSFJLCSIM::KZGET("MCBCPAR",1,len,(Int_t*)fREM1BC);
  fEMCal[4]=fREM1BC[0];

  JSFJLCSIM::KZGET("MCBCPAR",2,len,(Int_t*)fZEM1BC);
  fEMCal[5]=fZEM1BC[idat[2]-1];
  fEMCal[6]=fZEM1BC[0];

  JSFJLCSIM::KZGET("MCBCPAR",3,len,(Int_t*)fPEM1BC);
  JSFJLCSIM::KZGET("MCBCPAR",4,len,(Int_t*)fRHD1BC);
  fHDCal[4]=fRHD1BC[0];
  JSFJLCSIM::KZGET("MCBCPAR",5,len,(Int_t*)fZHD1BC);
  fHDCal[5]=fZHD1BC[idat[4]-1];
  fHDCal[6]=fZHD1BC[0];
  JSFJLCSIM::KZGET("MCBCPAR",6,len,(Int_t*)fPHD1BC);

  // Get End Cap cal information.
  
  JSFJLCSIM::KZGET("MCECPAR",0, len, idat);
  for(Int_t i=0;i<2;i++){ fECPAR0I[i]=idat[i]; }
  rtmp=(Float_t*)(idat+2);
  for(Int_t i=0;i<10;i++){ fECPAR0R[i]=rtmp[i]; }

  //  printf(" NDREC=%d\n",fECPAR0I[1]);
  //printf(" RI1EC, RO1EC=%g %g\n",fECPAR0R[0],fECPAR0R[1]);

  JSFJLCSIM::KZGET("MCECPAR",1,len,fNDPEC);
  //printf(" fNDPEC=%d\n",len);
  JSFJLCSIM::KZGET("MCECPAR",2,len,(Int_t*)fREM1EC);
  //printf(" fREM1EC=%d\n",len);
  JSFJLCSIM::KZGET("MCECPAR",3,len,(Int_t*)fZEM1EC);
  //printf(" fZEM1EC=%d\n",len);
  JSFJLCSIM::KZGET("MCECPAR",4,len,(Int_t*)fPEM1EC);
  //printf(" fPEM1EC=%d\n",len);
  JSFJLCSIM::KZGET("MCECPAR",5,len,(Int_t*)fRHD1EC);
  //printf(" fRHD1EC=%d\n",len);
  JSFJLCSIM::KZGET("MCECPAR",6,len,(Int_t*)fZHD1EC);
  //printf(" fZHD1EC=%d\n",len);
  JSFJLCSIM::KZGET("MCECPAR",7,len,(Int_t*)fPHD1EC);
  //printf(" fPHD1EC=%d\n",len);

  fTrack[0]=45.0;
  fTrack[1]=155.0;
  fTrack[2]=-155.0;
  fTrack[3]=155.0;
  fTrack[4]=50.0;

 
}


//________________________________________________________________
Bool_t JSFJIMParam::GetEMPosition(Int_t cell, Float_t pos[3], Float_t width[3])
{
  //(Function)
  //   Get EM cell position and width from cell number.
  //(Input)
  //   cell ; Cell number
  //(Output)
  //   pos[0] ; r (cm)
  //   pos[1] ; phi(radian)
  //   pos[2] ; z(cm)
  //   width[0]; width in r (cm) ( = 0 for Barrel Cap cell )
  //   width[1]; width in phi (radian)
  //   width[2]; width in z (cm) ( = 0 for End Cal cell )

  Int_t iz=cell/1000000 ;
  Int_t ith=(cell-iz*1000000)/1000;
  Int_t iph=cell%1000;

  // In the case of Barrel calroimeter
  if( iz == 0 ) {
    pos[0]=fREM1BC[ith-1];
    pos[2]=fZEM1BC[ith-1];
    pos[1]=fPEM1BC[iph-1];

    width[0]=0;
    if( iph > 2 ) { width[1]=fPEM1BC[iph]-fPEM1BC[iph-1]; }
    else { width[1]=fPEM1BC[iph+1]-fPEM1BC[iph]; }

    if( width[1] >  TMath::Pi() ) { width[1] -= 2*TMath::Pi(); }
    if( width[1] < -TMath::Pi() ) { width[1] += 2*TMath::Pi(); }
    
    width[1]=TMath::Abs(width[1]);

    ith--;
    if( ith > 2 ) { width[2]=fZEM1BC[ith]-fZEM1BC[ith-1]; }
    else { width[2]=fZEM1BC[ith+1]-fZEM1BC[ith]; }
    width[2]=TMath::Abs(width[2]);
  }
  // In the case of Endcap calorimeter
  else {
    ith*=iz;
    ith--;
    iph*=iz;
    pos[0]=fREM1EC[ith-1];
    pos[2]=iz*fZEM1EC[ith-1];
    pos[1]=fPEM1EC[iph-1];
    width[2]=0;
    if( iph > 2 ) { width[1]=fPEM1EC[iph]-fPEM1EC[iph-1]; }
    else { width[1]=fPEM1EC[iph+1]-fPEM1EC[iph]; }
    if( width[1] >  TMath::Pi() ) { width[1] -= 2*TMath::Pi(); }
    if( width[1] < -TMath::Pi() ) { width[1] += 2*TMath::Pi(); }
    
    width[1]=TMath::Abs(width[1]);

    if( ith > 2 ) { width[0]=fREM1EC[ith]-fREM1EC[ith-1]; }
    else { width[0]=fREM1EC[ith+1]-fREM1EC[ith]; }
    width[0]=TMath::Abs(width[0]);
  }  


  return kTRUE;

}

//________________________________________________________________
Bool_t JSFJIMParam::GetHDPosition(Int_t cell, Float_t pos[3], Float_t width[3])
{
  //(Function)
  //   Get HD cell position and width from cell number.
  //(Input)
  //   cell ; Cell number
  //(Output)
  //   pos[0] ; r (cm)
  //   pos[1] ; phi(radian)
  //   pos[2] ; z(cm)
  //   width[0]; width in r (cm) ( = 0 for End Cap cell )
  //   width[1]; width in phi (radian)
  //   width[2]; width in z (cm)

  Int_t iz=cell/1000000 ;
  Int_t ith=(cell-iz*1000000)/1000;
  Int_t iph=cell%1000;

  if( iz == 0 ) {
    pos[0]=fRHD1BC[ith-1];
    pos[2]=fZHD1BC[ith-1];
    pos[1]=fPHD1BC[iph-1];

    width[0]=0;
    if( iph > 2 ) { width[1]=fPHD1BC[iph]-fPHD1BC[iph-1]; }
    else { width[1]=fPHD1BC[iph+1]-fPHD1BC[iph]; }
    if( width[1] >  TMath::Pi() ) { width[1] -= 2*TMath::Pi(); }
    if( width[1] < -TMath::Pi() ) { width[1] += 2*TMath::Pi(); }

    ith--;
    if( ith > 2 ) { width[2]=fZHD1BC[ith]-fZHD1BC[ith-1]; }
    else { width[2]=fZHD1BC[ith+1]-fZHD1BC[ith]; }
  }

  // In the case of Endcap calorimeter
  else {
    ith*=iz;
    ith--;
    iph*=iz;
    pos[0]=fRHD1EC[ith-1];
    pos[2]=iz*fZHD1EC[ith-1];
    pos[1]=fPHD1EC[iph-1];
    width[2]=0;
    if( iph > 2 ) { width[1]=fPHD1EC[iph]-fPHD1EC[iph-1]; }
    else { width[1]=fPHD1EC[iph+1]-fPHD1EC[iph]; }
    
    if( width[1] >  TMath::Pi() ) { width[1] -= 2*TMath::Pi(); }
    if( width[1] < -TMath::Pi() ) { width[1] += 2*TMath::Pi(); }
    width[1]=TMath::Abs(width[1]);

    if( ith > 2 ) { width[0]=fRHD1EC[ith]-fRHD1EC[ith-1]; }
    else { width[0]=fRHD1EC[ith+1]-fRHD1EC[ith]; }
    width[0]=TMath::Abs(width[0]);
  }  


  return kTRUE;

}

