#ifndef __JSFVirtualFit__
#define __JSFVirtualFit__

//***************************************************************
//*
//*  JSFVirtualFit
//*
//*  Virtual Class for minimization
//*
//*$Id$
//*
//***************************************************************

#include "TObject.h"
#include "JSFConfig.h"

#if __ROOT_FULLVERSION__ < 30000
#ifndef __JSFDMatrix__
#include "JSFDMatrix.h"
#endif
#else
#ifndef __TMatrixD__
#include "TMatrixD.h"
#endif
#endif

class JSFVirtualFit : public TObject
{
 protected:
  Double_t   fChitst ; // Maximum chi-square change to terminate fit.
  Int_t      fMaxtry ; // Maximum # of fit loop.
  Int_t      fNpar   ; // Number of fitted parameter 
  Int_t      fNDF    ; // Number of freedom of the fit

#if __ROOT_FULLVERSION__ < 30000
  JSFDMatrix fDD     ; // Error Matrix of fit
  JSFDMatrix fA      ; // Fitted parameter array.
#else
  TMatrixD fDD     ; // Error Matrix of fit
  TMatrixD fA      ; // Fitted parameter array.
#endif
  Double_t   fChisq  ; // Results of the fit.
  Bool_t     fStatus ; // kTRUE, when fit is converged.
  Double_t   fConfidenceLevel ;// Confidence level of the fit.
  Int_t      fNtry   ;  // Number of try before converge.

 public:
  JSFVirtualFit();
  virtual ~JSFVirtualFit();

  virtual Bool_t Fit();  

  virtual void Initialize();
#if __ROOT_FULLVERSION__ < 30000
  virtual void Derivative(Double_t &chisq, JSFDMatrix &grad, JSFDMatrix &second);
#else
  virtual void Derivative(Double_t &chisq, TMatrixD &grad, TMatrixD &second);
#endif

  Bool_t    GetStatus(){ return fStatus; }
  Int_t     GetNtry(){ return fNtry; }
  Double_t  GetConfidenceLevel(){ return fConfidenceLevel;}
  Double_t  GetChisq(){ return fChisq; }
  Double_t  GetChiTest(){ return fChitst; }
  Int_t     GetMaxTry(){ return fMaxtry; }
  Int_t     GetNpar(){ return fNpar;}
  Int_t     GetNDF(){ return fNDF;}
  
  void SetChiTest(Double_t tst){ fChitst=tst; }
  void SetMaxTry(Int_t ntry){ fMaxtry=ntry; }
  void SetNpar(Int_t np){ fNpar=np; }
  void SetNDF(Int_t ndf){ fNDF=ndf; }


#if __ROOT_FULLVERSION__ < 30000
  JSFDMatrix &GetParameter(){ return fA; }
  JSFDMatrix &GetErrorMatrix(){ return fDD;}
#else
  TMatrixD &GetParameter(){ return fA; }
  TMatrixD &GetErrorMatrix(){ return fDD;}
#endif

  inline Double_t GetError(Int_t i, Int_t j){ return fDD(i,j); }
  inline Double_t GetParameter(Int_t i){ return fA(i,0); }

  ClassDef(JSFVirtualFit, 1) // Virtual fit class

};

#endif
