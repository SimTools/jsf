//*LastUpdate : jsf-1-8  2-May-1999  A.Miyamoto
//*LastUpdate : jsf-1-4  7-Feburary-1999  A.Miyamoto
//*-- Author  : Akiya Miyamoto  7-Feburary-1999  A.Miyamoto

/*
2-May-1999 A.Miyamoto  Bug in MovePivot is fixed.
5-May-1999 A.Miyamoto  Add MovePivotToIP(...) and AddMSError(...)
14-May-1999 A.Miyamoto Avoid negative costh in MovePivotToIP.
*/

////////////////////////////////////////////////////////////////////////
//
//  JSFCDCTracks
//
//  Utilities for CDC Tracks 
//
// 
//$Id$
//
////////////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFQuickSimParam.h"
#include "JSFCDCTrack.h"
#include "JSFHelicalTrack.h"

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

#ifdef R__ACC
  memcpy(&fError[0],&itrkp[18],120);
#else
  Double_t *err=(Double_t*)&itrkp[18];
  for(i=0;i<15;i++){ fError[i]=err[i]; }
#endif

  fNVTX = 0;
  for(i=0;i<10;i++){ fVTXHits[i]=NULL; }

}


//_____________________________________________________________________________
JSFCDCTrack::JSFCDCTrack(Float_t trkf[], Double_t trkd[])
{
  // Make a JSFCDCTrack class from a data from Production:CDC;Track_Parameter
  fP[0]=trkf[0]; fP[1]=trkf[1]; fP[2]=trkf[2];
  fE=trkf[3];
  fX[0]=trkf[4]; fX[1]=trkf[5] ; fX[2]=trkf[6];
  fCharge=(Int_t)trkf[7];  fGenID=(Int_t)trkf[8];
  Int_t i;
  for(i=0;i<5;i++){ fHelix[i]=trkf[9+i] ;}
  fPivot[0]=trkf[14] ; fPivot[1]=trkf[15]; fPivot[2]=trkf[16] ;
  fNDF=(Int_t)trkf[17];
  fPosAtEMC[0]=trkf[18]; fPosAtEMC[1]=trkf[19] ; fPosAtEMC[2]=trkf[20] ;
  fEPosAtEMC[0]=trkf[21] ;    fEPosAtEMC[1]=trkf[22];  
  for(i=0;i<15;i++){ fError[i]=trkd[i]; }

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
//       Float_t x[0] ; radius of the pivot ( distance from the beam line)
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
  


#ifdef R__ACC
   Double_t erm[15];
   memcpy(erm,&helix[8],120);
#else
   Double_t *erm = (Double_t*)&helix[8];
#endif

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


//_____________________________________________________________________________
void JSFCDCTrack::MovePivot(Float_t pivot[], Float_t bfield)
{
  // Current helix parameter is expressed in terms of new pivot.

  Double_t ptor=333.5640952*10.0/bfield;
  Double_t x2pid = 2.0*TMath::Pi();
  Double_t xpid  = TMath::Pi();
  Double_t dr  = fHelix[0];
  Double_t fi0 = fHelix[1];
  Double_t cpa = fHelix[2];
  Double_t dz  = fHelix[3];
  Double_t tnl = fHelix[4];
  Double_t x0  = fPivot[0];
  Double_t y0  = fPivot[1];
  Double_t z0  = fPivot[2];
  Double_t xv  = pivot[0];
  Double_t yv  = pivot[1];
  Double_t zv  = pivot[2];
  //
  // Transform helix parameters
  //
  Double_t r   = ptor/cpa;
  Double_t rdr = r + dr;
  Double_t fip = (fi0+2*x2pid)-x2pid*((Double_t)((Int_t)((fi0+2*x2pid)/x2pid)));
  Double_t csf0 = TMath::Cos(fip);
  Double_t snf0 = TMath::Sqrt( TMath::Max(0.0, (1.0-csf0)*(1.0+csf0)) );
  if( fip > xpid ) snf0 = -snf0 ;

  Double_t xc  = x0 + rdr*csf0 ;
  Double_t yc  = y0 + rdr*snf0 ;
  Double_t csf = ( xc-xv)/r;
  Double_t snf = (yc-yv)/r;
  Double_t anrm = 1.0/TMath::Sqrt(csf*csf+snf*snf);
           csf  *= anrm;
	   snf  *= anrm;
  Double_t csfd  = csf*csf0 + snf*snf0;
  Double_t snfd  = snf*csf0 - csf*snf0;
           fip   = TMath::ATan2(snf, csf);
  Double_t fid   = (fip-fi0 + 4*x2pid)
                 - x2pid*((Double_t)(Int_t)((fip-fi0 + 4*x2pid)/x2pid));
  if( fid > xpid ) fid -= x2pid;
  Double_t drp   = (x0+dr*csf0+r*(csf0-csf)-xv)*csf
                 + (y0+dr*snf0+r*(snf0-snf)-yv)*snf;
  Double_t dzp   = z0 + dz - r*tnl*fid - zv;
  //C--
  //C  Calculate @AP/@A.
  //C     AP = ( DRP, FIP, CPA, DZP, TNL )
  //C     A  = ( DR , FI0, CPA, DZ , TNL )
  //C--
  Double_t rdrpr = 1.0/(r+drp);
  Double_t rcpar = r/cpa;
  Double_t dapda[5][5];
  memset(dapda,0,200);
  
  dapda[0][0] = csfd ; 
  dapda[1][0] = rdr*snfd;  
  dapda[2][0] = rcpar*(1.0-csfd);   
   
  dapda[0][1] = - rdrpr*snfd;
  dapda[1][1] =   rdr*rdrpr*csfd;
  dapda[2][1] =   rcpar*rdrpr*snfd;
   
  dapda[2][2] =   1.0;
   
  dapda[0][3] =   r*rdrpr*tnl*snfd;
  dapda[1][3] =   r*tnl*(1.0-rdr*rdrpr*csfd);
  dapda[2][3] =   rcpar*tnl*(fid-r*rdrpr*snfd);
  dapda[3][3] =   1.0;
  dapda[4][3] = - r*fid;
   
  dapda[4][4] =   1.0;

  //C--
  //C  Copy error matrix to EEP and symmetrize it into EE.
  //C--
  Double_t ee[5][5];
  Int_t i,j,n;
  n=0;
  for(i=0;i<5;i++){ for(j=0;j<=i;j++) {
    ee[i][j]=fError[n];
    ee[j][i]=ee[i][j];
    n++;
  }}
  //C--
  //C  Transform error matrix EEP to that of XP.
  //C--
  n=0;
  for(i=0;i<5;i++){ for(j=0;j<=i;j++) {
    fError[n]=0.0;
    Int_t k,l;
    for(k=0;k<5;k++){ for(l=0;l<5;l++){
      fError[n]+= dapda[k][i]*ee[l][k]*dapda[l][j];
    }}
    n++;
  }}
  //C--
  //C  Fill HELXOT array.
  //C--
  fHelix[0]=drp;
  fHelix[1]=fip;
  fHelix[2]=cpa;
  fHelix[3]=dzp;
  fHelix[4]=tnl;
  fPivot[0]=xv;
  fPivot[1]=yv;
  fPivot[2]=zv;

  return;

}

//______________________________________________________________________________
void JSFCDCTrack::AddMSError(Float_t xrad)
{
  // Increase the error matrix to include the effect of 
  // the multiple scattering in the matterinal of radiation length xrad.

  Double_t tnlsq=fHelix[4]*fHelix[4];
  Double_t tnlsqone=1.0+tnlsq;
  Double_t pt=1.0/TMath::Abs(fHelix[2]);
  Double_t p =pt*TMath::Sqrt(tnlsqone);
  Double_t radx=xrad;
  Double_t sigms=0.0141*(1.0+TMath::Log10(radx)/9.0)*TMath::Sqrt(radx)/p;
  Double_t sigmsq=sigms*sigms;

  fError[2] = fError[2] +sigmsq*tnlsqone;
  fError[5] = fError[5] +sigmsq*(fHelix[2]*fHelix[2]*tnlsq);
  fError[12]= fError[12]+sigmsq*fHelix[2]*fHelix[4]*tnlsqone;
  fError[14]= fError[14]+sigmsq*tnlsqone*tnlsqone;

  //C .. E(2,2)=EDAT(3), E(3,3)=Edat(6), E(3,5)=Edat(13), E(5,5)=Edat(15)

}

//____________________________________________________________________________
Bool_t JSFCDCTrack::MovePivotToIP(JSFQuickSimParam *spar)
{
  // Move Pivot of Track parameter to IP
  // This program assumes that pivot of the track parameter is at the
  // first layer of VTX.  Procedure is 
  //   (1) Include multiple-scattering effect in first layer of VTX, 
  //   (2) Move pivot to beam pipe,
  //   (3) Include multiple-scatteing efect in the beam pipe.
  //   (4) Move pivot to IP.
  // Geometry information is obtained from JSFQuickSimParam
  
  JSFHelicalTrack *helix=new JSFHelicalTrack(GetHelix());
  JSFHelixParameter hp=helix->GetHelixParameter();
  Double_t pnorm=TMath::Sqrt(1.0+hp.tanl*hp.tanl);
  Double_t rnow=TMath::Sqrt(hp.pivot.x*hp.pivot.x+hp.pivot.y*hp.pivot.y);
  Double_t costh=(-hp.pivot.x*TMath::Sin(hp.phi0) + 
		  hp.pivot.y*TMath::Cos(hp.phi0)) / (rnow*pnorm) ;
  if( costh < 0.0 ) {
	printf("Warning in JSFCDCTrack::MovePivotToIP()...vtx1");
	printf(" costh=%g <=0, though it should be positive\n",costh);
	printf("Abs(costh) is used instead..\n");	
	costh= -costh;
  }
  Float_t rad1=spar->GetVTXThickness(1)/costh;
  AddMSError(rad1);  // Include MS at the first layer of VXT

  Float_t field=spar->GetBField();
  helix->SetBfield(field);
  Double_t rcyl=spar->GetVTXRadius(0);
  Double_t zcyl=spar->GetVTXZplus(0);
  Double_t phi0, phi1;
  Int_t maxloop=5;
  Int_t ncros=helix->OriginToCylinder(rcyl, zcyl, phi0, phi1, maxloop);
  if( ncros != 0 ) {
    printf("Eror in JSFCDCTrack::MovePivotToIP(...)  .. Track does not intersect");
    printf(" with beam pipe.\n");
    printf("Track parameter is not changed.\n");
    return kFALSE;
  }
  //
  JSF3DV piv=helix->GetCoordinate(phi1);
  Float_t pivot[3]={piv.x, piv.y, piv.z};
  // Move to Beam pipe.
  MovePivot(pivot, field);

  hp=helix->GetHelixParameter();
  pnorm=TMath::Sqrt(1.0+hp.tanl*hp.tanl);
  costh=(-pivot[0]*TMath::Sin(hp.phi0)+pivot[1]*TMath::Cos(hp.phi0))
                / (rcyl*pnorm) ;
  if( costh < 0.0 ) {
	printf("Warning in JSFCDCTrack::MovePivotToIP()...ip");
	printf(" costh=%g <=0, though it should be positive\n",costh);
	printf("Abs(costh) is used instead..\n");	
	costh= -costh;
  }
  Float_t rad0=spar->GetVTXThickness(0)/costh;
  AddMSError(rad0);
  Float_t ip[3]={0.0, 0.0, 0.0};
  MovePivot(ip, field);

  return kTRUE;

}
//____________________________________________________________________________
void JSFCDCTrack::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFCDCTrack.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TObject::Streamer(R__b);
      R__b.ReadStaticArray(fP);
      R__b >> fE;
      R__b.ReadStaticArray(fX);
      R__b >> fCharge;
      R__b >> fGenID;
      R__b.ReadStaticArray(fHelix);
      R__b.ReadStaticArray(fPivot);
      R__b.ReadStaticArray(fError);
      R__b >> fNDF;
      R__b.ReadStaticArray(fPosAtEMC);
      R__b.ReadStaticArray(fEPosAtEMC);
      //      R__b >> fNVTX;
      fNVTX=0; // fNVTX is set to zero untill VTX address is set in SetPointers()
   } else {
      R__b.WriteVersion(JSFCDCTrack::IsA());
      TObject::Streamer(R__b);
      R__b.WriteArray(fP, 3);
      R__b << fE;
      R__b.WriteArray(fX, 3);
      R__b << fCharge;
      R__b << fGenID;
      R__b.WriteArray(fHelix, 5);
      R__b.WriteArray(fPivot, 3);
      R__b.WriteArray(fError, 15);
      R__b << fNDF;
      R__b.WriteArray(fPosAtEMC, 3);
      R__b.WriteArray(fEPosAtEMC, 2);
      //  R__b << fNVTX;
   }
}





