//*LastUpdate : jsf-1-14  3-Feburary-2000  A.Miyamoto
//*-- Author  : Akiya Miyamoto  2-Feburary-2000

////////////////////////////////////////////////////////////////////////
//
// JSFGeoCFit
//
// (Description)
/*
C**********************************************************************
C* 
C*  -----------------------------------------------
C*  Subroutine UFGEOC( NTRK, LNxTRK, TRKINI, VTXINI,
C*                     CHIS, FITANS, NRET)
C*  -----------------------------------------------
C* 
C*(Function)
C*   Do geometrical constraint fit to the Track parameter.
C* 
C*(Input)
C*   NTRK    ; # of track.
C*   LNxTRK  ; Length of track parameter in I*4 unit.
C*             LNxTRK must be even number.
C*   TRKINI  ; Initial track parameter with TPC;Track_Parameter format.
C*   VTXINI  ; Initial vertex position.
C*             VTXINI(3) = ( Vx, Vy, Vz )
C* 
C*(Output)
C*   CHIS    ; Chi-square of Geofit.
C*   FITANS  ; R*4, R*8 mixed array of fit results, Should be decalred
C*            as REAL*4  FITANS(16, 0:NTRK) in the calling routine.
C*            Error matrix are saved in FITANS(5:14,0:NTRK) by REAL*8
C*            format.  To use Error matrix, declare as follows.
C*               REAL*4  FITANS(16,0:NTRK)
C*               REAL*8  DITANS( 8,0:NTRK)
C*               EQUIVALENCE (FITANS(1,0), DITANS(1,0))
C* 
C*       FITANS(1,i) = Vx (i=0) or Phi0(i^=0)
C*             (2,i) = Vy (i=0) or K   (i^=0)
C*             (3,i) = Vz (i=0) or Tanl(i^=0)
C*             (4,0) = -1, indicatting fitted vertex parameter.
C*                i) = Chi-square contribution of i-th track.
C*       DITANS(3,i) = Error matrix (1,1)  (k,l)
C*             (4,i) =              (2,1)     k=l=1  for Vx or Phi0
C*             (5,i) =              (3,1)        =2  for Vy orK
C*             (6,i) =              (2,2)        =3  for Vz of Tanl
C*             (7,i) =              (3,2)
C*             (8,i) =              (3,3)
C* 
C*   NRET  > 0 ; # of try, when fit converges
C*         =-1 ; Fit does not converge.
C*         =-2 ; Error during fit.
C*         =-3 ; Invalid error matrix.
C*         =-4 ; NTRK .GT. MFxTRK (50)
C* 
C*(Author)
C*  A. Miyamoto  24-Jun-1987
C* 
C**********************************************************************
C* 
   Converted to C++ By A.Miyamoto  2-Feburary-2000
*/
//
// (Update)
//  12-Jul-2000 A.Miyamoto  Remove memory leak in Initialize()
//
//$Id$
////////////////////////////////////////////////////////////////////////


#include "JSFGeoCFit.h"

ClassImp(JSFGeoCFit)

//____________________________________________________________
JSFGeoCFit::JSFGeoCFit(Int_t ntrk, TClonesArray *trk, TVector3 vtxini)
{
  Initialize(ntrk, trk, JSF3DV(vtxini.X(),vtxini.Y(),vtxini.Z()));
}

//____________________________________________________________
JSFGeoCFit::JSFGeoCFit(Int_t ntrk, TClonesArray *trk, JSF3DV vtxini)
{
  Initialize(ntrk, trk, vtxini);
}

//____________________________________________________________
JSFGeoCFit::~JSFGeoCFit()
{

  if( fNtrk > 1 ) {
    for(Int_t i=0;i<fNtrk;i++){
      delete fTrkerr[i];
      delete fTrkpar[i];
    }
    delete [] fTrkerr;
    delete [] fTrkpar;
    delete fTrackChisq;
  }

}

//____________________________________________________________
void JSFGeoCFit::Initialize(Int_t ntrk, TClonesArray *trk, JSF3DV vtxini)
{
/*
C**********************************************************************
C* 
C*  --------------------------------------------------------=========
C*  Subroutine UFGEOI( NTRK, LNxTRK, TRKINI, DRKINI, VTXINI, X, NRET)
C*  --------------------------------------------------------=========
C* 
C*(Function)
C*   Called from PGFTRK to store initial parameter for the geometrical
C*   constraint fit to the buffer.
C* 
C*(Input)
C*   NTRK    ; # of track.
C*   LNxTRK  ; Length of track parameter in I*4 unit.
C*   TRKINI  ; Initial track parameter with TPC;Track_Parameter format.
C*   DRKINI  ; Track parameter by Real*8 format.
C*   VTXINI  ; Initial position of the vertex.
C* 
C*(Output)
C*   X       ; Parameter array ( Inital values are set.)
C*   NRET    ; Return code.
C*          = 0 when normal return
C*          < 0 when failed to invert matrix.
C* 
C*(Author)
C*  A. Miyamoto  24-Jun-1987
C* 
C**********************************************************************
C* 
*/

//C  
//C =====< Entry Point >=================================================
//C  

  fNtrk=ntrk;
  fNpar=3*(ntrk+1);
  fNDF = 5*fNtrk -fNpar;
  fDD.ResizeTo(fNpar,fNpar);
  fA.ResizeTo(fNpar,1);
  fTrackChisq = new Double_t[ntrk];

//
// ---------------------------------------------------------------------
// Save input track parameter and it's error
// ---------------------------------------------------------------------
//  
  fTrkpar = new JSFHelixParameter*[ntrk];
  fTrkerr = new JSFDMatrix*[ntrk];

  Int_t i=0;
  for(i=0;i<ntrk;i++){
    JSFHelicalTrack *t=(JSFHelicalTrack*)trk->UncheckedAt(i);
    
    JSF3DV dif=t->GetHelixParameter().pivot-vtxini;
    if( dif.X()*dif.X()+dif.Y()*dif.Y()+dif.Z()*dif.Z() > 1.E-12 ) {
      t->MovePivot(vtxini);
    }
    if(i==0) fPTOR=t->GetAlpha();
    fTrkpar[i]=new JSFHelixParameter(t->GetHelixParameter());

    //    JSFDMatrix *em=new JSFDMatrix(5,5);
    JSFDMatrix em(5,5);
    Double_t *err=t->GetHelixErrorMatrix();
    Int_t j,k;
    Int_t ip=0;
    for(j=0;j<5;j++)  for(k=0;k<=j;k++) { 
      em(j,k)=err[ip];  em(k,j)=err[ip++]; 
    }	
    em.Invert();
    fTrkerr[i]=new JSFDMatrix(em);
    //    delete em;
  }

//C  
//C ---------------------------------------------------------------------
//C (3) Prepare Initial parameter array.
//C ---------------------------------------------------------------------
//C  

  fA(0,0)=vtxini.X();
  fA(1,0)=vtxini.Y();
  fA(2,0)=vtxini.Z();

  Int_t ip=3;
  JSFHelixParameter *h;
  for(i=0;i<fNtrk;i++){
    h=fTrkpar[i];
    Double_t drho=fPTOR/h->kappa;
    Double_t dxc=h->pivot.x + (drho+h->dr)*TMath::Cos(h->phi0);
    Double_t dyc=h->pivot.y + (drho+h->dr)*TMath::Sin(h->phi0);
    if( drho < 0.0 ) 
      fA(ip,0)=TMath::ATan2( -(dyc-fA(1,0)), -(dxc-fA(0,0)) );
    else 
      fA(ip,0)=TMath::ATan2(  (dyc-fA(1,0)),  (dxc-fA(0,0)) );
    fA(ip+1,0)=h->kappa;
    fA(ip+2,0)=h->tanl;
    ip+=3;
  }

//C  
//C ---------------------------------------------------------------------
//C (5) Return to Caller.
//C ---------------------------------------------------------------------
//C  


}


//________________________________________________________________________
void JSFGeoCFit::Derivative(Double_t &chis, JSFDMatrix &grad, JSFDMatrix &second)
{
// Calculate chi-square, and derivatives.
/*
C**********************************************************************
C* 
C*  ------------------------------------==================
C*  Subroutine UFGEOK( NPAR, MXxPAR, X, CHIS, GRAD, SECOND)
C*  ------------------------------------==================
C* 
C*(Function)
C*   Calculate chi-squre, first derivative of Chi-square and
C*   2nd-derivative of the matrix.
C* 
C*(Input)
C*   NPAR   ; # of parameter
C*   MXxPAR ; Size of matrix second.
C*   X  ; Fitted parameter array.
C*      X(1) = Xv
C*       (2) = Yv
C*       (3) = Zv
C*       (4) = Phi0 of 1st track
C*       (5) = K    of 1st track
C*       (6) = Tanl of 1st track
C*       (4) = Phi0 of 2nd track
C* 
C*      Dimension of X is 3 + 3*NUMTRK
C* 
C*(Output)
C*   CHIS   ; Chi-square
C*   GRAD   ; First derivative of the chi-square.
C*   SECOND ; Second derivative of the chi-square.
C* 
C*(Author)
C*  A. Miyamoto  27-Jun-1987
C* 
C**********************************************************************
C* 
*/

//C*    DADP(i,1:6) : Derivative of i-th measured track parameter with
//C*                  respect to the Xv, Yv, Zv, Phi0, K, Tanl
//C*    DAE(i)      : Product of dA vector and error matrix.
//C*    DXDA(i,j)   : Derivative of the calculated position with respect
//C*                  to the track parameter, Xv, Yv, Zv, Phi0, K, Tanl

//C  
//C =====< Entry Point >=================================================
//C  

  chis = 0.0;
  Double_t xv   = fA(0,0);
  Double_t yv   = fA(1,0);
  Double_t zv   = fA(2,0);
  
  grad.Zero();
  second.Zero();
  JSFDMatrix da(5,1), dadp(6,5);
  dadp.Zero();
  dadp(4,2)=1.0;
  dadp(2,3)=1.0;
  dadp(5,4)=1.0;

  //fA.Print();
//C  
//C ---------------------------------------------------------------------
//C (2) Track Loop
//C ---------------------------------------------------------------------
//C  

  Int_t itrk;
  for(itrk=0;itrk<fNtrk;itrk++){
    Int_t ip   = 3*itrk+3;
    Double_t a = fA(ip,0);
    Double_t b = fA(ip+1,0);
    Double_t c = fA(ip+2,0);

    JSFHelixParameter *h=fTrkpar[itrk];
    Double_t x0=h->pivot.x;
    Double_t y0=h->pivot.y;
    Double_t z0=h->pivot.z;
    Double_t rho=fPTOR/b;

    Double_t cosa=TMath::Cos(a);
    Double_t sina=TMath::Sin(a);
    Double_t xc  = xv + rho*cosa;
    Double_t yc  = yv + rho*sina;

    Double_t phi0=0;
    if( h->kappa > 0.0 ) phi0=TMath::ATan2( yc-y0, xc-x0);
    else phi0 = TMath::ATan2(y0-yc, x0-xc);
    
    Double_t cosp0 = TMath::Cos(phi0);
    Double_t sinp0 = TMath::Sin(phi0);
    Double_t xp0   = xv + rho*(cosa-cosp0);
    Double_t yp0   = yv + rho*(sina-sinp0);
    Double_t zp0   = zv - rho*c*(phi0-a);

    Double_t dr = (xp0-x0)*cosp0 + (yp0-y0)*sinp0;
    da(0,0) = h->dr - dr;
    da(1,0) = h->phi0 - phi0;
    da(2,0) = h->kappa - b;
    da(3,0) = h->dz - (zp0-z0);
    da(4,0) = h->tanl - c;

//C  
//C ... Calculate Chi-Square contribution of this track.
//C  
    JSFDMatrix *em = fTrkerr[itrk];
    JSFDMatrix prod(da,da.kTransposeMult,JSFDMatrix((*em),da.kMult,da));

    Double_t chi=prod(0,0);
    
    chis += chi;
    fTrackChisq[itrk]=chi;

//C  
//C (DADP(i,j)
//C       i = dr,Phi0,K,dZ,Tanl
//C       j = Xv,Yv,Zv,A,B,C
//C

    Double_t rhodri = 1/(rho+dr);
    Double_t rhob   = rho/b;

    dadp(0,0) =  cosp0 ;
    dadp(1,0) =  sinp0 ;
    dadp(3,0) =  rho* (       sinp0*cosa - cosp0*sina );
    dadp(4,0) =  rhob*( 1 - sinp0*sina - cosp0*cosa );
// 
    dadp(0,1) = -rhodri*sinp0;
    dadp(1,1) =  rhodri*cosp0;
    dadp(3,1) =  rho *rhodri*( sinp0*sina + cosp0*cosa);
    dadp(4,1) =  rhob*rhodri*( sinp0*cosa - cosp0*sina);
//c  
    dadp(0,3) =  rho*rhodri*c*sinp0;
    dadp(1,3) = -rho*rhodri*c*cosp0;
    dadp(3,3) =  rho*c*(  1.0 - dadp(3,1));
    dadp(4,3) =  rho*c*( (phi0-a)/b - dadp(4,1) );
    dadp(5,3) = -rho*(phi0-a);
//C  
//C (Derivative of chi-square.
//C  
    JSFDMatrix dae((*em),da.kMult,da);

//C  
//C Derivatives with respect to A, B, C
//C  

    JSFDMatrix daedadp(dadp,dae.kMult,dae);
    Int_t i;
    for(i=0;i<3;i++){
      grad(i,0) -= daedadp(i,0);
      grad(ip+i,0) -= daedadp(i+3,0);
    }

    JSFDMatrix sm1(dadp,dadp.kMult,(*em));
    JSFDMatrix sm(dadp,dadp.kMult,JSFDMatrix(sm1.kTransposed,sm1));
    Int_t j;
    for(i=0;i<3;i++) for(j=0;j<3;j++) {
      second(i,j)+=sm(i,j);
      second(i+ip,j)+=sm(i+3,j);
      second(i,j+ip)+=sm(i,j+3);
      second(i+ip,j+ip)+=sm(i+3,j+3);
    }

  }

}

