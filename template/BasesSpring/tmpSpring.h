#ifndef __<<Spring>>__
#define __<<Spring>>__

//////////////////////////////////////////////////////////////////////////
//
// <<Spring>>
//
// <<Comment>>
//
//////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TMath.h"
#include "TDatime.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __JSFBases__
#include "JSFBases.h"
#endif
#ifndef __JSFSpring__
#include "JSFSpring.h"
#endif

<<+LLbar>>
const Double_t   kPi=3.141592654;
const Double_t   kAlpha=0.0078125;   // = 1/128
const Double_t   kGev2fb=0.38927e12;
<<-LLbar>>

class <<Bases>>  : public JSFBases {
protected:
<<+LLbar>>
  Int_t    fID   ; // Parton ID.
  Float_t  fEcm  ; // Center of mass energy
  Float_t  fCharge ; // Parton Charge.		 
<<-LLbar>>
public:
  <<Bases>>(const char *name="<<Bases>>", 
	     const char *title="LLbar  Bases");
  virtual ~<<Bases>>(){} 

<<+LLbar>>
  void SetID(Int_t id){ fID=id; }
  void SetEcm(Float_t ecm){ fEcm=ecm; }
  void SetCharge( Float_t c){ fCharge=c;}
  void SetParton(Int_t id, Float_t c){ fID=id; fCharge=c;}

  Int_t GetID(){return fID;}
  Float_t GetEcm(){ return fEcm;}
  Float_t GetCharge(){ return fCharge;}
<<-LLbar>>
  void Userin();   // Bases user initialization
  void Userout();  // Bases user output 
  Double_t Func(Double_t x[]); // Bases integration function.

  ClassDef(<<Bases>>,1)  // Bases for e+e- -> l+l- process

};

class <<Spring>>;

class <<Spring>>Buf : public JSFSpringBuf {
public:
<<+LLbar>>
   Double_t fX[2];  // Two bases parameters, costh and phi.
<<-LLbar>>
public:
  <<Spring>>Buf(const char *name="<<Spring>>Buf", 
	     const char *title="<<SpringTitle>> event buffer",
	     <<Spring>> *spring=NULL)
	     : JSFSpringBuf(name,title,(JSFSpring*)spring) {} 
  virtual ~<<Spring>>Buf(){}
  virtual Bool_t SetPartons();

  ClassDef(<<Spring>>Buf,1)  // <<Spring>> event buffer
};

class <<Spring>> : public JSFSpring {
public:
   <<Spring>>(const char *name="<<Spring>>", 
	      const char *title="<<SpringTitle>>",
             <<Bases>> *bases=NULL);
   virtual ~<<Spring>>();
   ClassDef(<<Spring>>,1)  // <<Spring>> class
};


#endif


