#ifndef __JSFUtil__
#define __JSFUtil__

//////////////////////////////////////////////////////////////////////////
//
// JSFUtil
// 
// Collection of utility routines
//
//////////////////////////////////////////////////////////////////////////

#include <TNamed.h>
#include <TMath.h>

//*************************************************************
class JSFUtil : public TNamed {
public:
   JSFUtil(){};
   virtual ~JSFUtil(){};

   static Double_t ConfidenceLevel(Int_t ndf, Double_t chisq);

   ClassDef(JSFUtil, 1) // A collection of utility routines
};
  
#endif
