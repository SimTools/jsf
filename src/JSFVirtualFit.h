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

#ifndef __JSFDMatrix__
#include "JSFDMatrix.h"
#endif

class JSFVirtualFit : public TObject
{
 protected:
  Double_t   fChitst ; // Maximum chi-square change to terminate fit.
  Int_t      fMaxtry ; // Maximum # of fit loop.
  Int_t      fNpar   ; // Number of fitted parameter 
  Int_t      fNDF    ; // Number of freedom of the fit

  JSFDMatrix fDD     ; // Error Matrix of fit
  JSFDMatrix fA      ; // Fitted parameter array.
  Double_t   fChisq  ; // Results of the fit.
  Bool_t     fStatus ; // kTRUE, when fit is converged.
  Double_t   fConfidenceLevel ;// Confidence level of the fit.
  Int_t      fNtry   ;  // Number of try before converge.

 public:
  JSFVirtualFit();
  virtual ~JSFVirtualFit();

  virtual Bool_t Fit();  

  virtual void Initialize();
  virtual void Derivative(Double_t &chisq, JSFDMatrix &grad, JSFDMatrix &second);

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

  JSFDMatrix &GetParameter(){ return fA; }
  JSFDMatrix &GetErrorMatrix(){ return fDD;}

  ClassDef(JSFVirtualFit, 1) // Virtual fit class

};

#endif
