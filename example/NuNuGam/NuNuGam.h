#ifndef __NuNuGam__
#define __NuNuGam__

//////////////////////////////////////////////////////////////////////////
//
// NuNuGam
//
// Sample program
//
//////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TH1.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __JSFBases__
#include "JSFBases.h"
#endif
#ifndef __JSFSpring__
#include "JSFSpring.h"
#endif

                                                                            /*
******************************************************************************
*                  ===================================                       */
                   class NuNuGamBases : public JSFBases                       /*
*                  ===================================                       *
******************************************************************************
* ( Function )                                                               *
*   User defined derived class where the integrand function is re-defined.   *
******************************************************************************/
{
 public:
    Double_t W, EM, ZM, ZGAM, CZ, CV, CA, FACTOR, XK, COSTH;
    Double_t PI, ALP, GEVNB, GENER;

    Double_t ans, ANS1, TAU;
    Double_t S, S1, E, PP, D1, D2;
    Double_t EM2, EM4, S2, S3, CA2, CV2, W2, XK2, DSDX, rez, z2, ZM2;

    Double_t ALPHA, RAD, TWOPI, SQ, COSMIN, COSMAX, CSMIN, CSMAX, XKMIN, XKMAX;
    Double_t PHI, PXY, WM;

 public:
    NuNuGamBases( );
    virtual ~NuNuGamBases();

    Double_t Func( );
    virtual void Fill_hist( Double_t dsdx );

    ClassDef(NuNuGamBases, 1) // nu nubar gamma generator

};


class NuNuGamSpring;

class NuNuGamSpringBuf : public JSFSpringBuf {
public:
  Double_t fCosth;
  Double_t fXk;
public:
  NuNuGamSpringBuf(const char *name="NuNuGamSpringBuf", 
	     const char *title="NuNuGam Spring test event buffer",
	     NuNuGamSpring *spring=NULL)
	     : JSFSpringBuf(name,title,(JSFSpring*)spring) {} 
  virtual ~NuNuGamSpringBuf(){}
  Bool_t SetPartons();

  ClassDef(NuNuGamSpringBuf,1)  // NuNuGamSpring event buffer
};

class NuNuGamSpring : public JSFSpring {
public:
   NuNuGamSpring(const char *name="NuNuGamSpring", 
	      const char *title="NuNuGam Spring",
             NuNuGamBases *bases=NULL);
   virtual ~NuNuGamSpring();
   ClassDef(NuNuGamSpring,1)  // NuNuGamSpring class
};


#endif
