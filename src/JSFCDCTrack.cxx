//*LastUpdate : jsf-1-4  7-Feburary-1999  A.Miyamoto
//*-- Author  : Akiya Miyamoto  7-Feburary-1999  A.Miyamoto

////////////////////////////////////////////////////////////////////////
//
//  JSFCDCTracks
//
//  Utilities for CDC Tracks 
//
////////////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFCDCTrack.h"

ClassImp(JSFCDCTrack)

extern "C" {
extern void utrkmv_(Int_t *lnxhlx, Float_t helxin[], 
		    Float_t xp[], Float_t helxot[]);
};


//_____________________________________________________________________________
JSFCDCTrack::JSFCDCTrack(Int_t itrkp[])
{
  // Make a JSFCDCTrack class from a data from Production:CDC;Track_Parameter
  fGenID=itrkp[56];
  Float_t *trk=(Float_t*)&itrkp[0];
  fP[0]=trk[0];  fP[1]=trk[1];  fP[2]=trk[2];
  fE=trk[3];
  fX[0]=trk[4]; fX[1]=trk[5]; fX[2]=trk[6];
  fCharge = itrkp[8];
  Int_t i;
  for(i=0;i<5;i++){ fHelix[i]=trk[10+i] ;}
  fPivot[0]=trk[15] ; fPivot[1]=trk[16]; fPivot[2]=trk[17] ;
  Short_t *ndf=(Short_t*)&itrkp[50];
  fNDF=*ndf;

  fPosAtEMC[0]=-1.0; fPosAtEMC[1]=0.0 ; fPosAtEMC[2]=0.0 ;
  fEPosAtEMC[0]=0.0 ;    fEPosAtEMC[1]=0.0 ;  
  Double_t *err=(Double_t*)&itrkp[18];
  for(i=0;i<15;i++){ fError[i]=err[i]; }

  fNVTX = 0;
  for(i=0;i<10;i++){ fVTXHits[i]=NULL; }

}


//_____________________________________________________________________________
JSFCDCTrack::~JSFCDCTrack()
{
}

//_____________________________________________________________________________
void JSFCDCTrack::AddVTXHit(JSFVTXHit *v)
{ 
  if( fNVTX >= 10 ) { 
    printf("Too many VTX hits linked to CDC tracks in JSFCDCTrack::AddVTXHit.\n");
  }
  fVTXHits[fNVTX]=v; 
  fNVTX++ ; 
}


//_____________________________________________________________________________
void JSFCDCTrack::Print()
{
  printf(" CDCTrack parameter \n");
  printf(" fHelix=%g %g %g %g %g \n",
	 fHelix[0],fHelix[1],fHelix[2],fHelix[3],fHelix[4]);
  printf(" fPivot[0:2]=%g %g %g\n",fPivot[0],fPivot[1],fPivot[2]);
  printf(" fError[0,2,5,9,14]=%g %g %g %g %g\n",
	 fError[0],fError[2],fError[5],fError[9],fError[14]);

}

//_____________________________________________________________________________
void JSFCDCTrack::GetErrorMatrix(Double_t trkd[])
{
  Int_t i;
  for(i=0;i<15;i++){ trkd[i]=fError[i]; }
}

//_____________________________________________________________________________
void JSFCDCTrack::GetTrackParameter(Float_t trkf[])
{
  // Get the Track parameter with the SIMDST format.
  
  trkf[0]=fP[0]; trkf[1]=fP[1];  trkf[2]=fP[2];
  trkf[3]=fE;
  trkf[4]=fX[0]; trkf[5]=fX[1]; trkf[6]=fX[2];
  trkf[7]=(Float_t)fCharge;   trkf[8]=(Float_t)fGenID;
  Int_t i;
  for(i=0;i<5;i++){ trkf[9+i]=fHelix[i] ;}
  trkf[14]=fPivot[0] ; trkf[15]=fPivot[1]; trkf[16]=fPivot[2] ;
  trkf[17]=fNDF;
  trkf[18]=fPosAtEMC[0];    trkf[19]=fPosAtEMC[1];  trkf[20]=fPosAtEMC[2];
  trkf[21]=fEPosAtEMC[0];   trkf[22]=fEPosAtEMC[1];
}

//_____________________________________________________________________________
void JSFCDCTrack::SetPositionAtEMC(Float_t xp[])
{
  // Calculate position and its error of CDC Track at EMC entrance.
  // and set it to the data member of the class.

  Int_t j;
  const Int_t lenhelix=40;
  Float_t helixini[lenhelix];
  for(j=0;j<5;j++){ helixini[j]=fHelix[j]; }  
  helixini[5]=fPivot[0];  helixini[6]=fPivot[1];  helixini[7]=fPivot[2]; 
  Double_t *err=(Double_t*)&helixini[8];
  Int_t map[15]={0, 1,5,  2,6,9,  3,7,10,12,  4,8,11,13,14 };
   //  LC-packing   ------> Packing used by utrkmv.
   //  0                       0
   //  1   2                   1  5
   //  3   4   5               2  6   9
   //  6   7   8   9           3  7  10  12
   // 10  11  12  13  14       4  8  11  13  14
  for(j=0;j<15;j++){err[map[j]]=fError[j];}

  Float_t helixnew[lenhelix];
  Int_t il=lenhelix;
  utrkmv_(&il, helixini, xp, helixnew);
  ExtrapolateErrorAtEMC(helixnew, fPosAtEMC, fEPosAtEMC);
 
}


//_____________________________________________________________________________
void JSFCDCTrack::ExtrapolateErrorAtEMC(Float_t helix[], Float_t x[], Float_t dx[])
{
// 
//   Calculate Pivot's polar coordinate (theta, phi) and its error.
//   (Input)
//       Float_t helix[] ; helix parameter and error matrix
//   (Output)
//       Float_t x[0] ; radius of the pivot
//               x[1] ; theta of pivot
//               x[2] ; phi 
//       Float_t dx[0] ; delta-costh
//               dx[1] ; delta-phi

   Double_t dr=helix[0];  Double_t f0=helix[1];  Double_t dz=helix[3];
   struct XYZ { Double_t x; Double_t y; Double_t z; } xyz;

   Double_t csf0 = TMath::Cos(f0);
   Double_t snf0 = TMath::Sin(f0);
   xyz.x=helix[5] + dr*csf0;  
   xyz.y=helix[6] + dr*snf0;  
   xyz.z=helix[7] + dz ;

   Double_t r2   = xyz.x*xyz.x + xyz.y*xyz.y;
   Double_t r    = TMath::Sqrt(r2);
   Double_t th   = TMath::ATan2(r,xyz.z);
   Double_t phi  = TMath::ATan2(xyz.y,xyz.x);
   
   Double_t csphi = xyz.x/r;
   Double_t snphi = xyz.y/r;
   
   Double_t dphidr = (-xyz.y*csf0 + xyz.x*snf0)/r2;
   Double_t dphif0 = ( xyz.y*snf0 + xyz.x*csf0)/r2*dr;

   Double_t R2 = xyz.x*xyz.x + xyz.y*xyz.y + xyz.z*xyz.z; 
   Double_t R  = TMath::Sqrt(R2);
   Double_t dthdr = xyz.z/R2*(csphi*csf0+snphi*snf0);
   Double_t dthf0 = xyz.z/R2*(-csphi*snf0 + snphi*csf0 )*dr;
   Double_t dthdz = -TMath::Sin(th)/R;

   const Int_t iPdrdr=0;  
   const Int_t iPdrf0=1;  const Int_t iPf0f0=5;  
   const Int_t iPdrdz=3;  const Int_t iPf0dz=7;  const Int_t iPdzdz=12;
   //  LC-packing   ------> Packing used by utrkmv.
   //  0                       0
   //  1   2                   1  5
   //  3   4   5               2  6   9
   //  6   7   8   9           3  7  10  12
   // 10  11  12  13  14       4  8  11  13  14

   Double_t *erm = (Double_t*)&helix[8];
   Double_t dphi2 = dphidr*dphidr/2.*erm[iPdrdr] 
                  + dphidr*dphif0*erm[iPdrf0] + dphif0*dphif0/2.0*erm[iPf0f0];
   Double_t dth2  = dthdr*dthdr/2.*erm[iPdrdr] 
                  + dthdr*dthf0*erm[iPdrf0] + dthdr*dthdz*erm[iPdrdz]
                  + dthf0*dthf0/2.0*erm[iPf0f0] +dthf0*dthdz*erm[iPf0dz]
                  + dthdz*dthdz/2.0*erm[iPdzdz];

   if( dth2 < 0.0 ) {
     printf(" Invalid dth2 .. \n");
     
     printf(" dth2=%g",dth2);
     printf(" dphi2 =%g dr,f0,dz=%g %g %g\n",dphi2, dr,f0,dz);
     printf(" helix[0:4]=%g %g %g %g %g\n",helix[0],helix[1],helix[2],
	    helix[3],helix[4]);
     printf(" helix[5:7]=%g %g %g\n",helix[5],helix[6],helix[7]);
     printf(" erm=%g %g %g %g %g %g\n",erm[iPdrdr],erm[iPdrf0],
	    erm[iPdrdz],erm[iPf0f0],erm[iPf0dz],erm[iPdzdz]);
     printf(" dthdr=%g",dthdr);
     printf(" dthf0=%g",dthf0);
     printf(" dthdz=%g",dthdz);
     printf("\n");

    Double_t t1  = dthdr*dthdr/2.*erm[iPdrdr] ;
    Double_t t2  = dthdr*dthf0*erm[iPdrf0] ;
    Double_t t3  = dthdr*dthdz*erm[iPdrdz] ;
    Double_t t4  = dthf0*dthf0/2.0*erm[iPf0f0] ;
    Double_t t5  = dthf0*dthdz*erm[iPf0dz];
    Double_t t6  = dthdz*dthdz/2.0*erm[iPdzdz];

    printf(" t1:t6=%g %g %g %g %g %g \n",t1,t2,t3,t4,t5,t6);

     printf("\n");
   }

   x[0] = r;
   x[1] = th;
   x[2] = phi;
   dx[0] = TMath::Sqrt(dth2);
   dx[1] = TMath::Sqrt(dphi2);

}





