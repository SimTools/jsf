//*LastUpdate : jsf-1-4  6-Feburary-1999  A.Miyamoto
//*-- Author  : Akiya Miyamoto  6-Feburary-1999  A.Miyamoto

////////////////////////////////////////////////////////////////////////
//
//  JSFCALHitCell
//
//  Class for Calorimeters hits.
//
////////////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFCALHit.h"

ClassImp(JSFCALHit)
ClassImp(JSFEMCHit)
ClassImp(JSFHDCHit)
ClassImp(JSFCALGeoParam)

//_____________________________________________________________________________
JSFCALGeoParam::JSFCALGeoParam(EJSFCALType type, 
                    Int_t nphi, Int_t ntheta, Int_t nrad,
		    Float_t rmin, Float_t rmax,  Float_t zminus, Float_t zplus )
{
   SetGeoParam(type, nphi, ntheta, nrad,
		    rmin,  rmax,  zminus, zplus );
}

//_____________________________________________________________________________
void JSFCALGeoParam::SetGeoParam(EJSFCALType type, 
		 Int_t nphi, Int_t ntheta, Int_t nrad,
	         Float_t rmin, Float_t rmax,  Float_t zminus, Float_t zplus )
{
  // Initialize geometry parameters of the calorimeter.  Inputs are,
  //     type   : kEMC or kHDC
  //     nphi   : Number of phi segmentation.
  //     ntheta : Number of theta segmentation of barrel calorimeter
  //     nrad   : Number of radial segmentation of endcap calorimeter
  //     rmin   : Inner radius of endcap calorimeter (cm)
  //     rmax   : maximum radius of endcap calorimeter (cm)
  //     zminus : Z coordinate of -Z endcap calorimeter (cm)
  //     zplus  : Z coordinate of +Z endcap calorimeter (cm)
  //              Barrel calorimeter is from zmin to zmax, 
  //              and its surface radius is rmax.
  //

  fType = type;
  fRmin = rmin;
  fRmax = rmax;
  
  fPhiStep = 2.0*TMath::Pi()/(Float_t)nphi;

  Float_t csmn = zplus/TMath::Sqrt(rmin*rmin+zplus*zplus);
  Float_t csmx = zminus/TMath::Sqrt(rmin*rmin+zminus*zminus);
  fBZetaMin    = TMath::Log( (1-csmn)/(1+csmn) )/2 ;
  Float_t thmx = TMath::Log( (1-csmx)/(1+csmx) )/2 ;
  fBZetaStep   = ( thmx - fBZetaMin )/(Float_t)nphi ;

  Float_t snmn = rmin/TMath::Sqrt(rmin*rmin+zminus*zminus);
  Float_t snmx = rmax/TMath::Sqrt(rmax*rmax+zminus*zminus);
     fMZetaMin = TMath::Log( (1+snmn)/(1-snmn) )/2 ;
  Float_t  rmx = TMath::Log( (1+snmx)/(1-snmx) )/2 ;
    fMZetaStep = ( rmx - fMZetaMin )/(Float_t)nrad ;

          snmn = rmin/TMath::Sqrt(rmin*rmin+zplus*zplus);
          snmx = rmax/TMath::Sqrt(rmax*rmax+zplus*zplus);
     fPZetaMin = TMath::Log( (1+snmn)/(1-snmn) )/2 ;
           rmx = TMath::Log( (1+snmx)/(1-snmx) )/2 ;
    fPZetaStep = ( rmx - fMZetaMin )/(Float_t)nrad ;
}

