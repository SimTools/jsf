//*LastUpdate : jsf-1-14  31-January-2000  A.Miyamoto
//*-- Author  : Akiya Miyamoto  31-January-2000

////////////////////////////////////////////////////////////////////////
//
// JSFVertexing
//
// (Description)
// Perorms vertex finding and geometrycally constraining 
// vertex finding.
//  
// (Usage) 
//   See example in example/v0tag/
// (Update records)
//   2000/01/31 A.Miyamoto  Original version.
//
//$Id$
//
////////////////////////////////////////////////////////////////////////

#include <TClass.h>
#include "JSFConfig.h"
#include "JSFVertexing.h"

ClassImp(JSFVertexing)

//________________________________________________________________________
JSFVertexing::JSFVertexing()
{
  fQuality=1.E10;
  fT=0;
  fChisq=1.E10;
  fEntries=0;
  fPairEpsilon=1.0;
}

//________________________________________________________________________
JSFVertexing::JSFVertexing(Int_t ntracks)
{
  // Create a JSFVertexing object for finding and fitting of a vertex
  // consisting of ntracks tracks.

  fQuality=1.E10;
  fChisq=1.E10;
  fEntries=ntracks;
  fPairEpsilon=1.0;
  fT=new TClonesArray("JSFHelicalTrack",ntracks);
}

//_____________________________________________________________________________
JSFVertexing::~JSFVertexing()
{
  Clear();
  if( fT ) delete fT;
}

//_____________________________________________________________________________
void JSFVertexing::Clear()
{
  // Clear JSFHelicalTrack objects saved in fT.
  if( fT ) fT->Clear();
}

//_________________________________________________________________
void JSFVertexing::SetTrack(Int_t id, JSFHelicalTrack t)
{
  // Stores JSFHelicalTrack objects for vertex finding and fitting.
  // id is a location where JSFHelicalTrack object is stored.
  // It should be a number from 0 to fEntries-1 ( nTracks-1 ).

  new((*fT)[id]) JSFHelicalTrack(t);
}

//_________________________________________________________________
Double_t JSFVertexing::FindVertex()
{
  if( fEntries == 2 ) return FindV0();
  return 0.0;
}


//_________________________________________________________________
Double_t JSFVertexing::FitVertex()
{
  return 0.0;
}

//___________________________________________________________________________
void JSFVertexing::UCRCLX(Double_t r1, TVector2 xc1, Double_t r2, TVector2 xc2,
			     Double_t eps, 
			     TVector2 &x1, TVector2 &x2, Int_t &isect)
{
/*
CC********************************************************************CC
C*                                                                    *C
C*======================================----------===                 *C
C*  Subroutine UCRCLX(R1,XC1,R2,XC2,EPS,X1,X2,ISCT)                   *C
C*======================================----------===                 *C
C*                                                                    *C
C* (Purpose)                                                          *C
C*    Calculates intersections of 2 circles.                          *C
C* (Inputs)                                                           *C
C*      R1     : (R*8) ; radius of 1-st circle.                       *C
C*      XC1(*) : (R*8) ; center of 1-st circle.                       *C
C*      R2     : (R*8) ; radius of 2-nd circle.                       *C
C*      XC2(*) : (R*8) ; center of 2-nd circle.                       *C
C*      EPS    : (R*8) ; tolerance for non-intersection 2 circles     *C
C*                       in cm.                                       *C
C* (Outputs)                                                          *C
C*      X1 (*) : (R*8) ; 1-st intersection.                           *C
C*      X2 (*) : (R*8) ; 2-nd intersection.                           *C
C*      ISCT   : (I*4) ; # intersections.                             *C
C* (Relation)                                                         *C
C*    Requires no subroutines or functions.                           *C
C* (Update Record)                                                    *C
C*    5/22/87  K.Fujii        Original version.                       *C
C*    6/26/90  K.Fujii        Fixed a bug. This routine has been      *C
C*                            giving a fake intersection when         *C
C*                            one of the given two circle contains    *C
C*                            the other completely.                   *C
C*                                                                    *C
CC********************************************************************CC
      Converted to C++ by A.Miyamoto  1-Feb-2000 Akiya Miyamoto
*/

//C========< Entry Point >================================================
//C  
//C--
//C  Check if 2 circles intersect.
//C--

  TVector2 x12 = xc2 - xc1;
  Double_t a = x12*x12;
  Double_t sqa = TMath::Sqrt(a);
  Double_t radd = r1 + r2 ;
  Double_t rsub = TMath::Sqrt( r1-r2 );
  //  printf(" In UFpair.. sqa, radd, eps,dif=%g, %g, %g,%g\n",sqa,radd,eps,sqa-radd+eps);

  if( sqa > radd+eps || sqa <= 1.e-3 || sqa < rsub-eps ) {
// (1) No Intersection
    isect=0;
    return; 
  }
// (2) Single intersection
  else if ( sqa > radd-eps ) {
    Double_t d=r1/sqa;
    x1 = xc1 + d*x12;
    //    printf(" 1 intersection... dif=%g %x",sqa-radd+eps,(sqa > radd-eps ));
    //    printf(" x1=%g %g\n",x1.X(),x1.Y());
    isect = 1;
    return;
  }
// (3) 2 intersection
  Double_t d = (r1+r2)*(r1-r2) + a;
  a = 1.0/sqa;
  d = 0.5*d*a;
  Double_t dp = (r1+d)*(r1-d);
  if( dp <= 0.0 ) { dp = 0.0; }
  else { dp = a*TMath::Sqrt(dp); }
  d = d*a ;
  TVector2 d1 = xc1 + x12*d;
  TVector2 d2(dp*x12.Y(), -dp*x12.X());
  x1 = d1 + d2;
  x2 = d1 - d2;
  isect = 2;
  return;
} 


//_____________________________________________________________________________
Double_t JSFVertexing::FindV0()
{
/*
CC********************************************************************CC
C*                                                                    *C
C*================================------------====                    *C
C*  Subroutine UFPAIR(HELX1,HELX2,XV,QUAL,IRET)                       *C
C*================================------------====                    *C
C*                                                                    *C
C* (Purpose)                                                          *C
C*    Get approximate intersection of 2 helices by 1-st calculating   *C
C*    intersections of 2 circles and then checking their Z coords.    *C
C* (Inputs)                                                           *C
C*      HELX1(): (R*4) ; helix parameters for 1-st track.             *C
C*      HELX2(): (R*4) ; helix parameters for 2-nd track.             *C
C*                       1-st 8 words in                              *C
C*                       Production:TPC;Track_Parameter.              *C
C* (Outputs)                                                          *C
C*      XV()   : (R*4) ; vertex vector.                               *C
C*      QUAL   : (R*4) ; vertex quality. ( Z difference in cm )       *C
C*      IRET   : (I*4) ; return code. (0,<0)=(OK,Error).              *C
C* (Relation)                                                         *C
C*    Calls    :  UCRCLX.                                             *C
C* (Update Record)                                                    *C
C*    5/22/87  K.Fujii        Original version.                       *C
C*                                                                    *C
CC********************************************************************CC
     Converted to C++ by A.Miyamoto  1-Feb-2000
*/
//C  
//C--
//C  Reset return code and quality.
//C--
  Int_t iret=-1;
  fQuality=1.E10;
//C--
//C  Change helix parametrization.
//C     X = XC(1) - R*cos(FI+FI0)
//C     Y = XC(2) - R*sin(FI+FI0)
//C     Z = XC(3) - R*TNL*FI
//C--
  Int_t itk;
  Double_t fi0[2], tnl[2], r[2], xcz[2];
  TVector2   xc[2];
  Int_t ichg[2];
  for(itk=0;itk<2;itk++){
    JSFHelicalTrack *t=(JSFHelicalTrack*)fT->UncheckedAt(itk);
    JSFHelixParameter hlx=t->GetHelixParameter();     
    fi0[itk]   = hlx.phi0;
    tnl[itk]   = hlx.tanl;
    ichg[itk]  = (Int_t)TMath::Sign(1.1, hlx.kappa);
    r[itk]     = t->GetRadius();
    Double_t rdr = r[itk]+hlx.dr;
    xc[itk].Set( hlx.pivot.x + rdr*TMath::Cos(fi0[itk]) ,
                 hlx.pivot.y + rdr*TMath::Sin(fi0[itk])); 
    xcz[itk]   = hlx.pivot.z + hlx.dz;
  }
//C--
//C  Galculate intersection of 2 circles.
//C--
  TVector2 xx[2];
  Int_t nx;
  UCRCLX(TMath::Abs(r[0]), xc[0], TMath::Abs(r[1]), xc[1], fPairEpsilon,
	      xx[0], xx[1], nx);

//C--
//C  If no intersection found, return with IRET = -1.
//C--
  if( nx <= 0 ) return fQuality;
//C--
//C  Loop over intersections.
//C--
  Int_t ix;
  Double_t twopi=2.0*TMath::Pi();
  Double_t z[2], qual;
  for(ix=0;ix<nx;ix++){
    for(itk=0;itk<2; itk++){

       Double_t x = xx[ix].X() - xc[itk].X();
       Double_t y = xx[ix].Y() - xc[itk].Y();

       Double_t dfi = TMath::ATan2(y,x) - fi0[itk] - TMath::Pi()*0.5*(1.0+ichg[itk]);
       Int_t ifi=(Int_t)(dfi/twopi);
       dfi = dfi - ifi*twopi;
       if( dfi > TMath::Pi() ) dfi -= twopi;
       if( dfi < -TMath::Pi() ) dfi += twopi; 
       if( ichg[itk]*dfi < -1.e-3 ) goto next300;
       z[itk]=xcz[itk] - r[itk]*tnl[itk]*dfi;     
    }
    iret=0;
    qual = TMath::Abs(z[0] - z[1]);
    if( qual < fQuality ) {
      fQuality = qual;
      fV[0] = xx[ix].X();
      fV[1] = xx[ix].Y();
      fV[2] = 0.5*( z[0] + z[1] );
    }
  next300: continue;
  }
  return fQuality; 
}

#if __ROOT_FULLVERSION__ >= 30000
//______________________________________________________________________________
void JSFVertexing::Streamer(TBuffer &R__b)
{
  // Stream an object of class JSFVertexing.

  if (R__b.IsReading()) {
    UInt_t R__s, R__c;
    Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
    if( R__v > 1 ) {
      JSFVertexing::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
      return;
    }

    TObject::Streamer(R__b);
    R__b.ReadStaticArray(fV);
    R__b >> fQuality;
    fT->Streamer(R__b);
    R__b >> fChisq;
    R__b >> fEntries;
    R__b >> fPairEpsilon;
    R__b.CheckByteCount(R__s, R__c, JSFVertexing::IsA());
    
  } 
  else {
    JSFVertexing::Class()->WriteBuffer(R__b, this);
  }

}

#else


//______________________________________________________________________________
void JSFVertexing::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFVertexing.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TObject::Streamer(R__b);
      R__b.ReadStaticArray(fV);
      R__b >> fQuality;
      fT->Streamer(R__b);
      R__b >> fChisq;
      R__b >> fEntries;
      R__b >> fPairEpsilon;
   } else {
      R__b.WriteVersion(JSFVertexing::IsA());
      TObject::Streamer(R__b);
      R__b.WriteArray(fV, 3);
      R__b << fQuality;
      fT->Streamer(R__b);
      R__b << fChisq;
      R__b << fEntries;
      R__b << fPairEpsilon;
   }
}

#endif

