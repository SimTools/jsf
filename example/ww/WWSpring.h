#ifndef __WWSpring__
#define __WWSpring__

//////////////////////////////////////////////////////////////////////////
//
// WWSpring
//
// e+e- -> w+w-
//
//$Id$
//
//////////////////////////////////////////////////////////////////////////

#include "JSFConfig.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __JSFBases__
#include "JSFBases.h"
#endif
#ifndef __JSFSpring__
#include "JSFSpring.h"
#endif


class WWBases  : public JSFBases {
protected:
  Float_t fEbeam;  // Beam Energy(GeV)
  Int_t   fNBSON;  // Flag for Beamstruhlung(NBSON)
  Int_t   fNBTYPE; // Type of Beamstrahlung spectrum(NBTYPE)
public:
  WWBases(const char *name="WWBases", 
	     const char *title="LLbar  Bases");
  virtual ~WWBases(){} 

  void SetEbeam(Float_t ebeam){ fEbeam=ebeam; }

  Float_t GetEbeam(){ return fEbeam;}
  void Userin();   // Bases user initialization
  void Userout();  // Bases user output 
  Double_t Func(Double_t x[]); // Bases integration function.
  void PrintParameters(); // Print parameters

  ClassDef(WWBases,1)  // Bases for e+e- -> l+l- process

};

class WWSpring;

class WWSpringBuf : public JSFSpringBuf {
public:
//  <<+LLbar>>
   Double_t fX[2];  // Two bases parameters, costh and phi.
//  <<-LLbar>>
public:
  WWSpringBuf(const char *name="WWSpringBuf", 
	     const char *title="WWSpring test event buffer",
	     WWSpring *spring=NULL)
	     : JSFSpringBuf(name,title,(JSFSpring*)spring) {} 
  virtual ~WWSpringBuf(){}
  virtual void Spevnt(Int_t &iret);

  ClassDef(WWSpringBuf,1)  // WWSpring event buffer
};

class WWSpring : public JSFSpring {
public:
   WWSpring(const char *name="WWSpring", 
	      const char *title="WWSpring test",
             WWBases *bases=NULL);
   virtual ~WWSpring();

   virtual Bool_t Initialize(); // 

   ClassDef(WWSpring,1)  // WWSpring class
};


#endif


