//*LastUpdate : jsf-1-5  1-March-1999  A.Miyamoto
//*-- Author  : Akiya Miyamoto  1-March-1999  A.Miyamoto

////////////////////////////////////////////////////////////////////////
//
//  JSFHelicalTrack
//
//  Basic class to describe helical track
//
//$Id: 
//
////////////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFHelicalTrack.h"

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
Bool_t JSFHelicalTrack::IntersectWithCylinder(JSFRPhiZ ref, JSFRPhiZ &p)
{
  // Find track position at cylinder with radius ref.r
  // (Input)
  //    ref     : Track position near ref is calculated.
  // (Output)
  //    p       : Track position at cylinder 
  //              p.r = ref.r and p.phi and p.z are calculated.
  //    retun code is false, when not intersect with cylinder.

  //  Double_t twopi=2*TMath::Pi();

  // Find intersection with Barrel.
  JSF2DV center=GetCenter();
  JSF2DV orig(0.0, 0.0);
  JSF2DV cros[2];
  Double_t rabs=TMath::Abs(GetRadius());
  Int_t ncros=IntersectOf2Circle(orig, ref.r, center, rabs, cros[0], cros[1]);
  if( ncros == 0 ) return kFALSE;

  // Select point close to ref.
  p.r=ref.r;
  Double_t refx=ref.r*TMath::Cos(ref.phi);
  Double_t refy=ref.r*TMath::Sin(ref.phi);
  Double_t dist0=TMath::Sqrt((refx-cros[0].x)*(refx-cros[0].x) +
			     (refy-cros[0].y)*(refy-cros[0].y));
  Double_t dist1=TMath::Sqrt((refx-cros[1].x)*(refx-cros[1].x) +
			     (refy-cros[1].y)*(refy-cros[1].y));
  if( dist1 < dist0 ) cros[0]=cros[1];

  // When intersects with Barrel.
  Double_t fang=GetDeflectionAngle2D(cros[0]);

  JSF3DV xp=GetCoordinate(fang);
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
