#ifndef __JSFPythia__
#define __JSFPythia__

#include <TPythia6.h>
#include <TMCParticle6.h>

class JSFPythia : public TPythia6 
{
  private:
    Int_t fMRPY[6];
    Double_t fRRPY[100];
  public:
    JSFPythia(){}
    virtual ~JSFPythia(){}

    inline Int_t const GetVersionNumber(){ return 6; }
 
    ClassDef(JSFPythia, 1) // JSF's Pythia class

};

#endif

