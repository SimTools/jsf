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
		  Int_t ndf=-1, Double_t chisq=0, Double_t cl=0,
		  Double_t *error=0 )
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
				 Float_t x=0, Float_t y=0, Float_t z=0)
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

  //  printf(" x1=%g %g   x2=%g %g\n",x1.x,x1.y,x2.x,x2.y);

  JSF2DV u = dif.UnitV();
  if( l ==  r1+r2 ) {
    c1=x1+u*r1;
    return 1;
  } // Only one intersection


  Double_t rate= (r1*r1 - r2*r2 + l*l)/(2.0*l*r1);
  //  printf(" r1=%g r2=%g l=%g rate=%g\n",r1,r2,l,rate);
  Double_t th=TMath::ACos( rate );
  JSF2DV v=u.NormalV();

  Double_t f1=r1*TMath::Cos(th);
  Double_t f2=r1*TMath::Sin(th);

  // printf(" th=%g\n",th);
  // printf(" u=%g %g  v=%g %g \n",u.x, u.y, v.x, v.y);

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

  // printf(" fa=%g phi0=%g \n",fa,fHelix.phi0);
  if( fHelix.kappa > 0 ) {  // Make sure deflection angle is decreasing
    while( fa < -0.0001 ) { fa +=twopi; }  
  }
  else {    // Make sure deflection angle is increasing from origin to pivot.
    while( fa > 0.0001 ) { fa -= twopi; }
  }
  Phi0=fa;
  // printf(" kappa=%g Phi0=%g\n",fHelix.kappa,Phi0);

  // Find intersection with Barrel.
  JSF2DV center=GetCenter();
  JSF2DV cros[2];
  Double_t rabs=TMath::Abs(GetRadius());
  Int_t ncros=IntersectOf2Circle(orig, Rcyl, center, rabs, cros[0], cros[1]);
  // printf(" ncros is %d\n",ncros);
  // printf(" cros[0]=%g %g\n",cros[0].x,cros[0].y);
  // printf(" cros[1]=%g %g\n",cros[1].x,cros[1].y);
  // printf(" center is %g %g\n",center.x,center.y);
  // printf(" Rcyl=%g rabs=%g\n",Rcyl, rabs);
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

  //  printf(" fang is %g %g \n",fang[0],fang[1]);
  Phi1=fang[0];
  // printf(" Phi1 is %g\n",Phi1);
  if( ncros == 1 ) return 0;  // Only one intersection with barrel.

  if( fHelix.kappa > 0 ) {  // Select deflection angle close to one at origin
    if( Phi0 - Phi1 > Phi0 - fang[1] ) Phi1 = fang[1] ;
  }
  else {
    if( Phi1 - Phi0 > fang[1] - Phi0 ) Phi1 = fang[1];
  }

  // printf(" Phi0, Phi1=%g %g \n",Phi0, Phi1);
  endp=GetCoordinate(Phi1);
  // printf(" endp is %g %g %g\n",endp.x, endp.y, endp.z);
  if( TMath::Abs(endp.z) > Zcyl ) { goto ENDCAP; }

  return 0;


ENDCAP:
  // When helix hits barrel.  
  Double_t fb=0;
  Int_t ir=1;
  // printf(" Maxloop is %d\n",Maxloop);
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
  // printf(" Phi0, Phi1=%g %g \n",Phi0, Phi1);

  return ir;

}


