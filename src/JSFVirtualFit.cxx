//*LastUpdate : jsf-1-14  21-Feburary-2000  A.Miyamoto
//*-- Author  : Akiya Miyamoto  21-Feburary-2000

////////////////////////////////////////////////////////////////////////
//
// JSFVirtualFit
//
// (Description)
//  Virtual Fit class for Neutonian minimization.
//
// (Author)
//   Akiya Miyamoto  21-Feburary-2000
//
//
//$Id$
//
////////////////////////////////////////////////////////////////////////

#include "JSFUtil.h"
#include "JSFVirtualFit.h"

ClassImp(JSFVirtualFit)

//____________________________________________________________
JSFVirtualFit::JSFVirtualFit()
{
  fMaxtry=50;
  fChitst=1.0E-6; 
  fNpar=0;
  fNDF=0;
  Initialize();
}

//____________________________________________________________
JSFVirtualFit::~JSFVirtualFit()
{
}

//____________________________________________________________
void JSFVirtualFit::Initialize()
{
  // Initialize Fitting.   User should set value to 
  //   fNpar
  //   fNDF
  // then set size of fA and fDD array by
  //   fA.ResizeTo(fNpar,1);
  //   fDD.ResizeTo(fNpar,fNpar);

  //  Warning("Initialize","Virtual Initialize function is called.\n");
}

//________________________________________________________________________
Bool_t JSFVirtualFit::Fit()
{
  // Fitting class.

  Int_t ntry=0;
  Double_t epsl  = 1.e-12;
  Double_t chi2  = 1.e30;
  Double_t chi2s = 1.e30;

  if( fNpar < 1 ) Fatal("Fit","Number of parameter of the fit is not set yet.");

#if __ROOT_FULLVERSION__ < 30000
  JSFDMatrix as(fNpar,1);
  JSFDMatrix dchi2(fNpar,1), dchi2s(fNpar,1);
  JSFDMatrix d2chi2(fNpar,fNpar), d2chi2s(fNpar,fNpar);
#else
  TMatrixD as(fNpar,1);
  TMatrixD dchi2(fNpar,1), dchi2s(fNpar,1);
  TMatrixD d2chi2(fNpar,fNpar), d2chi2s(fNpar,fNpar);
#endif

  // Start Initalization loop.

  fStatus = kTRUE;
  Int_t i;
  for(ntry=0;ntry<fMaxtry;ntry++) {

    Derivative(chi2, dchi2, d2chi2);

    if ( TMath::Abs(chi2s-chi2) < fChitst ) goto enditeration;

    // Chi-square increase.  Multiply EPSL by 1.D2 and try again

    if( chi2 > chi2s ) {
      epsl *= 100.0;
      fA = as;
      dchi2 = dchi2s;
      d2chi2 = d2chi2s;
      fDD    = d2chi2s;
      for(i=0;i<fNpar;i++) fDD(i,i)*= (1.0+epsl);
    }

    // Chi-square decreases.  Accept this step and modify EPSL by 1.D-2

    else {
      epsl  *= 0.01;
      chi2s  = chi2;
      as     = fA;
      dchi2s = dchi2;
      d2chi2s= d2chi2;
      fDD    = d2chi2;
      for(i=0;i<fNpar;i++) fDD(i,i)*= (1.0+epsl);
    }

    // Invert DD and get next step.

    fDD.Invert();
#if __ROOT_FULLVERSION__ < 30000
    JSFDMatrix da(fDD, dchi2.kMult, dchi2);
#else
    TMatrixD da(fDD, dchi2.kMult, dchi2);
#endif

    fA -= da;

    //  Continue iteration if NTRY < MAXTRY.

    if( epsl > 1.0e60 ) break;

  }

  //  Fit did not converge. Recall the best fit.

  //  Warning("Fit","did not converge.");
  chi2=chi2s;
  fA=as;
  d2chi2 = d2chi2s;
  fStatus = kFALSE;

  //  End iteration loop. Now calculate vertex error matrix.

enditeration:
  fDD = d2chi2;
  fDD.Invert();

  fChisq = chi2;
  fNtry  = ntry;
  JSFUtil util;
  fConfidenceLevel=util.ConfidenceLevel(fNDF, fChisq);

  return fStatus;

}


//________________________________________________________________________
#if __ROOT_FULLVERSION__ < 30000
void JSFVirtualFit::Derivative(Double_t &chis, JSFDMatrix &grad, JSFDMatrix &second)
#else
void JSFVirtualFit::Derivative(Double_t &chis, TMatrixD &grad, TMatrixD &second)
#endif
{
  // 

  Warning("Derivative","Virtual Derivative is called.  Derivative is not implemented.");

}
