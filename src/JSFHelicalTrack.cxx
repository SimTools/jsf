//*LastUpdate : jsf-1-12  16-October-1999  A.Miyamoto
//*LastUpdate : jsf-1-11  26-July-1999  A.Miyamoto
//*LastUpdate : jsf-1-5  1-March-1999  A.Miyamoto
//*-- Author  : Akiya Miyamoto  1-March-1999  A.Miyamoto

////////////////////////////////////////////////////////////////////////
//
//  JSFHelicalTrack
//
//  Basic class to describe helical track
//
//(Update)
//  16-October-1999  In SetHelixByFitToCyl, trial helix is determined
//                   by using first, center, and last space points
//$Id$ 
//
////////////////////////////////////////////////////////////////////////

#include <TMinuit.h>
#include "JSFSteer.h"
#include "JSFHelicalTrack.h"
#include "JSFUtil.h"

ClassImp(JSFHelicalTrack)

extern void JSFHTFitFuncCylinderGeometry(
       Int_t &npar, Double_t *dChisqdX, Double_t &f, Double_t *par, Int_t flag);

JSFHelicalTrack *pFitHelix;
JSFHitCylinder  *pSP;
Int_t            nSP;
Double_t         pFitChisq;

//_____________________________________________________________________________
JSFHelicalTrack::JSFHelicalTrack()
{
  fWithError=kFALSE;
  fChisq=-1.0;
  fCL=0.0;
  fNDF=0;
  fBfield=0.0;
  fAlpha=0.0;

}

//_____________________________________________________________________________
JSFHelicalTrack::JSFHelicalTrack(Float_t helix[], Float_t pivot[],
		  Int_t ndf, Double_t chisq, Double_t cl,
		  Double_t *error )
{
  fHelix.dr    = helix[0];
  fHelix.phi0  = helix[1];
  fHelix.kappa = helix[2];
  fHelix.dz    = helix[3];
  fHelix.tanl  = helix[4];
  fHelix.pivot.x = pivot[0];
  fHelix.pivot.y = pivot[1];
  fHelix.pivot.z = pivot[2];
  fWithError     = kFALSE;

  Int_t i;
  if( ndf > -1 ) {
    fWithError = kTRUE;
    fNDF       = ndf;
    fChisq     = chisq;
    fCL        = cl;
    for(i=0;i<15;i++){ fError.data[i]=error[i]; }
  }
  fBfield=0;
  fAlpha=0;
}

//_____________________________________________________________________________
JSFHelicalTrack::JSFHelicalTrack(Float_t px, Float_t py, Float_t pz, 
				 Float_t bz, Float_t charge, 
				 Float_t x, Float_t y, Float_t z)
{
  // Construct the object from momentum (px,py,pz), charge, Bz, and 
  // starting point of the helix (x,y,z)

  fHelix.pivot.x=x;
  fHelix.pivot.y=y;
  fHelix.pivot.z=z;
  fHelix.dr=0;
  fHelix.dz=0;
  SetBfield(bz);
  Double_t pt=TMath::Sqrt(px*px+py*py);
  fHelix.kappa=charge/pt;
  fHelix.tanl=TMath::Abs(fHelix.kappa)*pz;
  fHelix.phi0=TMath::ATan2(-px,py);
  fWithError = kFALSE;
}


//_____________________________________________________________________________
JSFHelicalTrack::JSFHelicalTrack(JSFHelixParameter hlx)
{
  fHelix = hlx;
  fWithError = kFALSE;
  fBfield=0;
  fAlpha=0;
}

//_____________________________________________________________________________
JSFHelicalTrack::JSFHelicalTrack(JSF3DV p1, JSF3DV p2, JSF3DV p3, Double_t bfield)
{
  SetHelix(p1, p2, p3, bfield);
}

//_____________________________________________________________________________
void JSFHelicalTrack::SetHelix(JSF3DV p1, JSF3DV p2, JSF3DV p3, Double_t bfield)
{
  // Set HelicalTrackParameter from 3 points, p1, p2, p3, and
  // Magnetic field, bz.  bz is in unit of kgauss.
  // Curvature is calculated from x and y corrdinates of p1, p2, p3 
  // and tan-lambda is defined by z coordinates of p1 and p2.
  // Pivot is set to p1.

  SetBfield(bfield);
  fWithError = kFALSE;

  fChisq=0.0; fCL=0.0 ; fNDF=0 ; 
  
  fHelix.pivot.x=p1.x;
  fHelix.pivot.y=p1.y;
  fHelix.pivot.z=p1.z;
  fHelix.dr=0;
  fHelix.dz=0;
  fHelix.kappa=0;
  fHelix.phi0=0;

  Double_t d = (p2.x-p1.x)*(p3.y-p2.y) - (p3.x-p2.x)*(p2.y-p1.y) ;
  Double_t xc=0.0; Double_t yc=0.0 ;
  if( d != 0.0 ) {
    Double_t c1=( (p2.x-p1.x)*(p2.x+p1.x) + (p2.y-p1.y)*(p2.y+p1.y))/2.0;
    Double_t c2=( (p3.x-p2.x)*(p3.x+p2.x) + (p3.y-p2.y)*(p3.y+p2.y))/2.0;
    xc=( c1*(p3.y-p2.y)-c2*(p2.y-p1.y))/d;
    yc=(-c1*(p3.x-p2.x)+c2*(p2.x-p1.x))/d;
    Double_t r = TMath::Sqrt( (xc-p1.x)*(xc-p1.x) + (yc-p1.y)*(yc-p1.y) );
    fHelix.kappa = fAlpha/r;
    fHelix.phi0  = TMath::ATan2( (p1.y-yc), (p1.x-xc) );
  }  
  Double_t phi=TMath::ATan2( p2.y-yc, p2.x-xc );
  Double_t phidef=phi-fHelix.phi0;
  if( phidef > TMath::Pi() )  phidef += 2*TMath::Pi();
  if( phidef < -TMath::Pi() ) phidef -= 2*TMath::Pi();
  Double_t charge=-TMath::Sign(1.,phidef);
  if( charge > 0.0 ) { fHelix.phi0 += TMath::Pi(); 
      if( fHelix.phi0 > 2*TMath::Pi() ) fHelix.phi0 -= 2*TMath::Pi();}
  fHelix.kappa = charge*fHelix.kappa;
  fHelix.tanl  = ( fHelix.pivot.z - p2.z ) / phidef /fAlpha*fHelix.kappa ;
}

//_____________________________________________________________________________
void JSFHelicalTrack::Print()
{
  printf(" fHelix=%g %g %g %g %g \n",
	 fHelix.dr,fHelix.phi0,fHelix.kappa,fHelix.dz,fHelix.tanl);
  printf(" pviot=(%g,%g,%g)\n",fHelix.pivot.x,fHelix.pivot.y,fHelix.pivot.z);
  if( fWithError ) {
    printf(" Chisq=%g ndf=%d ConfidenceLevel=%g\n",fChisq,fNDF,fCL);
    printf(" Diagonal part of the error matrix is %g %g %g %g %g\n",
	   fError.m.drdr, fError.m.p0p0, fError.m.kk,fError.m.dzdz,
	   fError.m.tltl);
  }
}


//_____________________________________________________________________________
Int_t JSFHelicalTrack::IntersectOf2Circle(JSF2DV x1, Double_t r1, JSF2DV x2, Double_t r2,
      JSF2DV &c1, JSF2DV &c2)
{
  //  Calculate Intersection of Two circles, whose radius and center coordinates are
  //  (r1,x1) and (r2,x2), and return coordinates of intersection.
  //  Return value is a number of intersection, 0 1 or 2.

  JSF2DV dif=x2-x1;
  Double_t l=dif.Abs();
  if( l > r1+r2 ) return 0;
  if( r2 > l+r1 ) return 0;
  if( r1 > l+r2 ) return 0;

  JSF2DV u = dif.UnitV();
  if( l ==  r1+r2 ) {
    c1=x1+u*r1;
    return 1;
  } // Only one intersection


  Double_t rate= (r1*r1 - r2*r2 + l*l)/(2.0*l*r1);
  Double_t th=TMath::ACos( rate );
  JSF2DV v=u.NormalV();

  Double_t f1=r1*TMath::Cos(th);
  Double_t f2=r1*TMath::Sin(th);

  c1 = x1 + u*f1 + v*f2;
  c2 = x1 + u*f1 - v*f2;

  return 2;
}



//_____________________________________________________________________________
Int_t JSFHelicalTrack::OriginToCylinder(Double_t Rcyl, Double_t Zcyl, 
		Double_t &Phi0, Double_t &Phi1, Int_t Maxloop, 
		Double_t StartX, Double_t StartY)
{
  // Calculate deflection angle from origin to the cylinder:
  // (Input)
  //    Rcyl    :  radius of the cylinder.
  //    Zcyl    :  half length of the cylynder in z direction.
  //    Maxloop :  If helix hits cylynder's end cap, number of rotation is limitted to Maxloop
  //               instead of giving diflection angle to reach endcap.
  //    StartX   : X coordinate of start point of the helix.   
  //    StartY   : Y coordinate of start point of the helix.

  // (Output)
  //    Phi0    :  deflection angle at origin.
  //    Phi1    :  deflection angle at cylynder surface or after maxloop.
  //   [return code] = 0  when intersect with cylynder's barrel.
  //                 = 1  when intersect with cylynder's endcap.
  //                 = 2  when looped more than Maxloop
  //                 = -1 when error detected in the calculation.

  if( !TestBfield() ) return -1;

  // Get deflection angle near origin, assuming less than one rotation from
  // origin to the pivot.

  Double_t twopi=2*TMath::Pi();
  JSF2DV orig(StartX, StartY);
  JSF3DV endp;
  Double_t fa=GetDeflectionAngle2D(orig);

  if( fHelix.kappa > 0 ) {  // Make sure deflection angle is decreasing
    while( fa < -0.0001 ) { fa +=twopi; }  
  }
  else {    // Make sure deflection angle is increasing from origin to pivot.
    while( fa > 0.0001 ) { fa -= twopi; }
  }
  Phi0=fa;

  // Find intersection with Barrel.
  JSF2DV center=GetCenter();
  JSF2DV cros[2];
  Double_t rabs=TMath::Abs(GetRadius());
  Int_t ncros=IntersectOf2Circle(orig, Rcyl, center, rabs, cros[0], cros[1]);
  if( ncros == 0 ) goto ENDCAP;

  // When intersects with Barrel.
  Int_t i;
  Double_t fang[2];
  for(i=0;i<ncros;i++){
    fang[i]=GetDeflectionAngle2D(cros[i]);
    if( fHelix.kappa > 0 ) {  // make sure deflection angle is decreasing
      while( fang[i] < fa - twopi ) { fang[i] += twopi; }  
      while( fang[i] > fa ) { fang[i] -= twopi; }  
    }
    else {   // deflection angle is increasing
      while( fang[i] > fa + twopi) { fang[i] -= twopi; }
      while( fang[i] < fa ) { fang[i] += twopi; }
    }
  }

  Phi1=fang[0];
  if( ncros == 1 ) return 0;  // Only one intersection with barrel.

  if( fHelix.kappa > 0 ) {  // Select deflection angle close to one at origin
    if( Phi0 - Phi1 > Phi0 - fang[1] ) Phi1 = fang[1] ;
  }
  else {
    if( Phi1 - Phi0 > fang[1] - Phi0 ) Phi1 = fang[1];
  }

  endp=GetCoordinate(Phi1);
  if( TMath::Abs(endp.z) > Zcyl ) { goto ENDCAP; }

  return 0;


ENDCAP:
  // When helix hits barrel.  
  Double_t fb=0;
  Int_t ir=1;
  if( fHelix.tanl > 0 ) {
    fb = ( fHelix.pivot.z + fHelix.dz - Zcyl ) / ( GetRadius()*fHelix.tanl );
  }
  else {
    fb = ( fHelix.pivot.z + fHelix.dz + Zcyl ) / ( GetRadius()*fHelix.tanl );
  }
  if( Phi0 - fb > Maxloop*twopi ) {
    fb=Phi0 - Maxloop*twopi ;
    ir=2;
  }
  else if( fb - Phi0 > Maxloop*twopi ) {
      fb=Phi0 + Maxloop*twopi ;
      ir=2;
  }
  Phi1 = fb;

  return ir;

}



//_____________________________________________________________________________
Bool_t JSFHelicalTrack::IntersectWithCylinder(JSFRPhiZ ref, Double_t &fang)
{
  // Find track position at cylinder with radius refr
  // (Input)
  //    ref     : Track position near ref is calculated.
  // (Output)
  //    deflection angle at intersection

  // Find intersection with Barrel.
  JSF2DV center=GetCenter();
  JSF2DV orig(0.0, 0.0);
  JSF2DV cros[2];
  Double_t rabs=TMath::Abs(GetRadius());
  Int_t ncros=IntersectOf2Circle(orig, ref.r, center, rabs, cros[0], cros[1]);
  if( ncros == 0 ) return kFALSE;

  // Select point close to ref.
  //  p.r=ref.r;
  Double_t refx=ref.r*TMath::Cos(ref.phi);
  Double_t refy=ref.r*TMath::Sin(ref.phi);
  Double_t dist0=TMath::Sqrt((refx-cros[0].x)*(refx-cros[0].x) +
			     (refy-cros[0].y)*(refy-cros[0].y));
  Double_t dist1=TMath::Sqrt((refx-cros[1].x)*(refx-cros[1].x) +
			     (refy-cros[1].y)*(refy-cros[1].y));
  if( dist1 < dist0 ) cros[0]=cros[1];

  // When intersects with Barrel.
  fang=GetDeflectionAngle2D(cros[0]);

  return kTRUE;

}


//_____________________________________________________________________________
Bool_t JSFHelicalTrack::IntersectWithCylinder(JSFRPhiZ ref, JSFRPhiZ &p)
{
  // Find track position at cylinder with radius ref.r
  // (Input)
  //    ref     : Track position near ref is calculated.
  // (Output)
  //    p       : Track position at cylinder 
  //              p.r = ref.r and p.phi and p.z are calculated.
  //    retun code is false, when not intersect with cylinder.

  // When intersects with Barrel.
  Double_t fang;

  if( !IntersectWithCylinder(ref, fang) ) return kFALSE;

  JSF3DV xp=GetCoordinate(fang);
  p.r=ref.r;
  p.phi=TMath::ATan2(xp.y, xp.x);
  p.z  = xp.z;

  return kTRUE;

}


//_________________________________________________________________
THelix *JSFHelicalTrack::GetTHelix(Double_t rcyl, Double_t zcyl)
{
  // returns THelix object
  // rcyl and zcyl is a radius and a half Z length where Helix is drown.

     Double_t hp[3], hx[3];
     hx[0]=fHelix.pivot.x ; hx[1]=fHelix.pivot.y; hx[2]=fHelix.pivot.z;
     Double_t pt  = 1.0/TMath::Abs(fHelix.kappa);
     Double_t r   = fAlpha*pt;
     Double_t w   = pt/r;
     Double_t vt  = r*w;
     JSF3DV pmom = GetMomentum(0.0);
     hp[0]=vt*pmom.x/pt;  
     hp[1]=vt*pmom.y/pt;  
     hp[2]=pmom.z;
     w *= GetCharge();

     Double_t range[2]; 
     Double_t zlast;

     Double_t phi0, phi1;
     OriginToCylinder(rcyl, zcyl, phi0, phi1);
     JSF3DV end=GetCoordinate(phi1);
     zlast = end.z;     if( zlast > 0.0 ) { range[0]=hx[2] ; range[1]=zlast ;}
     else {  range[1]=hx[2] ; range[0]=zlast; }

     THelix *thelix=new THelix();
     thelix->SetHelix(hx, hp, w, range, kHelixZ);

     return thelix;
}


//__________________________________________________________________________
// void JSFHelicalTrack::FirstDerivative(Double_t ang, Double_t dXdHelix[][5])
void JSFHelicalTrack::FirstDerivative(Double_t ang, Double_t *dXdHelix)
{
  // Calculate 1st derivative at deflection angle, ang.
  // Result are returned in array ans.
  // ans[i][j] = d(x,y,z)/d(dr, phi0, kappa, dz, tanl)



  Double_t cs =TMath::Cos(fHelix.phi0);
  Double_t csa=TMath::Cos(fHelix.phi0+ang);
  Double_t sn =TMath::Sin(fHelix.phi0);
  Double_t sna=TMath::Sin(fHelix.phi0+ang);
  Double_t aok=fAlpha/fHelix.kappa;
  Double_t aokk=aok/fHelix.kappa;

  dXdHelix[0]=cs;

  dXdHelix[1]= -(fHelix.dr+aok)*sn + aok*sna;
  dXdHelix[2]= -aokk*(cs-csa);
  dXdHelix[3]=0;
  dXdHelix[4]=0;

  dXdHelix[5]= sn;
  dXdHelix[6]=  (fHelix.dr+aok)*cs -aok*csa;
  dXdHelix[7]= -aokk*(sn-sna);
  dXdHelix[8]=0;
  dXdHelix[9]=0;

  dXdHelix[10]=0;
  dXdHelix[11]=0;
  dXdHelix[12]=aokk*fHelix.tanl*ang;
  dXdHelix[13]=1;
  dXdHelix[14]=-aok*ang;

  return ;
}



//_____________________________________________________________________
void JSFHelicalTrack::SetTrackByFitToCyl(Int_t npnt, JSFHitCylinder *hits, Double_t bf)
{
  //  Make a helix fit to the hit points. Hit points are measurements of
  // cylindrical geometry.  They are given by a cylindrical coordinate of (phi, z)
  // and its error, (dphi, dz), at radius r.  Those data are given by a class,
  // JSFHitCylinder.
  //(Input)
  //  npnt : number of hit points. 
  //  hits : space points.
  //  bf   : solenoid magnetic field (kgauss)
  //(Output)
  //  Result is given in a class data member.
  //(Notes) 
  //  npnt should be greater than 2.
  //  Usage of this function would be,
  //    JSFHelicalTrack hlx;
  //    hlx.FitCylinderGeometry(npnt, hits, bf);
  //  Then fitted result are saved in the object, hlx.


  // 
  nSP = npnt ;
  pSP = hits ;
  pFitHelix=this;
  if( npnt < 3 ) {
    printf("Error in JSFHelicalTrack::FitCylinderGeometry()  ");
    printf("Number of space points are less than 3.\n");
    return ;
  }

  // Set trial helix.
  JSF3DV p1=pSP[nSP].sp.XYZ();
  JSF3DV p2=pSP[nSP/2].sp.XYZ();
  JSF3DV p3=pSP[0].sp.XYZ();

  SetHelix(p1, p2, p3, bf);
  //  p1.Print(); p2.Print(); p3.Print();
  //Print();

  // Setup Minuit

  TMinuit *pMinuit = new TMinuit(5);  //initialize TMinuit with a maximum of 5 params
  pMinuit->SetFCN(JSFHTFitFuncCylinderGeometry);

  Double_t arglist[10];
  Int_t ierflg = 0;

  arglist[0] = -1;
  pMinuit->mnexcm("SET PRINTOUT", arglist ,1,ierflg);

  arglist[0] = 1;
  pMinuit->mnexcm("SET ERR", arglist ,1,ierflg);

// Set starting values and step sizes for parameters
  static Double_t vstart[5];
  vstart[0]=fHelix.dr;
  vstart[1]=fHelix.phi0;
  vstart[2]=fHelix.kappa;
  vstart[3]=fHelix.dz;
  vstart[4]=fHelix.tanl;
  static Double_t step[5] = {1.0E-3 , 1.0E-3 , 1.E-3 , 1.E-3, 1.E-3};
  pMinuit->mnparm(0, "dr", vstart[0], step[0], 0,0,ierflg);
  pMinuit->mnparm(1, "phi0", vstart[1], step[1], 0,0,ierflg);
  pMinuit->mnparm(2, "kappa", vstart[2], step[2], 0,0,ierflg);
  pMinuit->mnparm(3, "dz", vstart[3], step[3], 0,0,ierflg);
  pMinuit->mnparm(4, "tanl", vstart[4], step[4], 0,0,ierflg);

  //   "set gradient 1" Use first derivative of fcn.
  //   "set gradient "  Use first derivative, confirmming values.
  // pMinuit->mncomd("SET GRAD 1", ierflg);
  // pMinuit->mncomd("SET GRAD", ierflg);

  // Call FCN with flag=1 to initialize parameter
   arglist[0] = 1 ;  // iflag
   pMinuit->mnexcm("CALL", arglist ,1,ierflg);

// Now ready for minimization step
  arglist[0] = 500;  // Max. call
  arglist[1] = 1.;   // tolerance
  pMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);

  // Call  FCN with flag=3 at termination
   arglist[0] = 3 ;  // iflag
   pMinuit->mnexcm("CALL", arglist ,1,ierflg);

   Double_t emat[5][5];
   pMinuit->mnemat(&emat[0][0], 5);
   Int_t i=0; Int_t j=0; Int_t ip=0;
   for(i=0;i<5;i++){ for(j=0;j<=i;j++){ fError.data[ip]=emat[i][j]; ip++; } }

   fChisq=pFitChisq;
   fNDF=2*nSP - 5;
   fCL=JSFUtil::ConfidenceLevel(fNDF, fChisq);
   //printf(" ndf=%d chisq=%g cl=%g\n",fNDF, fChisq, fCL);

   fWithError=kTRUE;
   delete pMinuit;
}


//__________________________________________________________________________
void JSFHTFitFuncCylinderGeometry(
          Int_t &npar, Double_t *dChisqdX, Double_t &f, Double_t *par, Int_t flag)
{
  // Function for helix fitting by Minuit.  This function is for cylindrical
  // geometry, whose hit points are given by JSFHitCylinder class.
  //(Input)
  //  npar ; number of fit parameter = 5.
  //  flag ; type of calculation ( see minuit manual.)
  //      flag = 2 ; calculate gradient vector and chisq
  //      other    ; calculate chisq.
  //  par          : input helix parameter.
  //(Output)
  //  dChisqdX : First derivative of Chisq by helix parameter, valid only when flag=2.
  //  f        : Chisq.

  static Double_t *sxi2i;
  static Double_t *sz2i;
  if( flag == 1 ) {
    sxi2i=new Double_t[nSP];
    sz2i=new Double_t[nSP];
    for(Int_t l=0;l<nSP;l++){
      sxi2i[l]=1./(pSP[l].dphi*pSP[l].dphi);
      sz2i[l]=1./(pSP[l].dz*pSP[l].dz);
    }
    return;
  }
  
   Double_t chisq = 0.0;
   if( flag == 2 ) { for(Int_t i=0;i<5;i++){ dChisqdX[i]=0.0; } }

   Int_t l;
   Double_t delta;
   pFitHelix->SetHelix(par);
   JSFRPhiZ p;
   Double_t dxdhlx[3][5]; 
   Double_t dXidHlx[5];

   //  Loop over hit points to calculate chisq and firs derivative
   for (l=0;l<nSP;l++) {
     Double_t fang=0;
     if( !pFitHelix->IntersectWithCylinder(pSP[l].sp, fang) ) {
       chisq +=1.e10;
       //       printf(" No intersects with cylinder.\n");
       continue;
     }

     JSF3DV xyz=pFitHelix->GetCoordinate(fang);
     p=JSFRPhiZ(xyz);
     p.r=pSP[l].sp.r;
     Double_t dxi=pSP[l].sp.r*(p.phi-pSP[l].sp.phi);
     Double_t dz=p.z-pSP[l].sp.z;
     delta=dxi*dxi*sxi2i[l] + dz*dz*sz2i[l] ;
     chisq += delta*delta;
     
     if( flag == 2 ) {
	pFitHelix->FirstDerivative(fang, &dxdhlx[0][0]);
	Double_t cs=xyz.x/p.r;
	Double_t sn=xyz.y/p.r;
	for(Int_t i=0;i<3;i++){
	   dXidHlx[i]=cs*dxdhlx[1][i] - sn*dxdhlx[0][i];
	} 
        dChisqdX[0]+= 2*dxi*sxi2i[l]*dXidHlx[0];
	dChisqdX[1]+= 2*dxi*sxi2i[l]*dXidHlx[1];
	dChisqdX[2]+= 2*(dxi*sxi2i[l]*dXidHlx[2] + dz*sz2i[l]*dxdhlx[2][2]);
	dChisqdX[3]+= 2*dz*sz2i[l]*dxdhlx[2][3];
	dChisqdX[4]+= 2*dz*sz2i[l]*dxdhlx[2][4];
     }
   }
   f = chisq;
   pFitChisq=chisq;

   if( flag == 3 ) {
     delete sxi2i;
     delete sz2i;
   }

   //   printf(" chisq=%g\n",chisq);

}






