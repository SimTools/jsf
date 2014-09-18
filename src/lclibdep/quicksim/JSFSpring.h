#ifndef __JSFSpring__
#define __JSFSpring__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFSpring                                                         //
//                                                                      //
// A template for JSFSpring                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "JSFConfig.h"
#include "TObject.h"
#include "TClonesArray.h"
#include "TString.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __JSFBases__
#include "JSFBases.h"
#endif
#ifndef __JSFSpringParton__
#include "JSFSpringParton.h"
#endif

class JSFSpringParton;

class JSFSpringBuf : public JSFEventBuf {
protected:
   Int_t          fNparton; // Number of partons
   TClonesArray  *fPartons; //-> Pointer to partons
public:
   JSFSpringBuf();
   JSFSpringBuf(const char *name,
	        const char *title="JSF Spring event buffer",
		JSFSpring *spring=0);
   virtual ~JSFSpringBuf();

   virtual void Clear(Option_t *option="");
   virtual Bool_t SetPartons();
   virtual void   Spevnt(Int_t &iret) {}

   Int_t             GetNpartons() const { return fNparton; }
   TClonesArray     *GetPartons() { return fPartons; }

   ClassDef(JSFSpringBuf,1)  // Spring Event data
};

//*************************************************************
class JSFSpring : public JSFModule {
protected:
   JSFBases *fBases;  //! Pointer to bases class
   Bool_t    fDoBases; //! Do bases at beginrun, or read bases data

#if __ROOT_FULLVERSION__ >= 30000
   TString    fBasesClassName;//
   TString    fBasesObjectName;//
   TString    fBasesObjectTitle;//
#endif

   Int_t      fMXTRY; // Max number of try in the Spring step.

   Int_t      fioNTAB; //
   Long_t     fSeed;   //
   Long_t     fSeedIY; //
   Long_t     *fSeedIV; //[fioNTAB]

   Bool_t     fSetSeed; //! true to copy fSeedRdm and fSeedIa1 to bases common.
   Bool_t     fPrintHist; //! Print histogram
   Bool_t     fPrintInfo; //! Print information
   std::string  fPygiveCommand ; 

public:
   JSFSpring(const char *name="JSFSpring", 
	     const char *title="JSF Spring", 
	     JSFBases *bases=NULL);

   virtual ~JSFSpring();
   virtual void  PrintInfo(){}

   virtual Bool_t Process(Int_t event);
   virtual Bool_t BeginRun(Int_t nrun);
   virtual Bool_t EndRun();
   virtual Bool_t Terminate();
   virtual Bool_t GetLastRunInfo();

   virtual JSFBases *GetBases() { return fBases; }

   virtual void   GetPy6frmProb(Int_t nseq, Double_t prob[7]);
   virtual void   GetPy4frmArgs(Int_t nseq, Double_t &atotsq, 
                  Double_t &a1sq, Double_t &a2sq, Int_t &istart);
   virtual void   DoHadronize(int &iret);

   void        SetPygiveCommandForHadronizer(const Char_t* cmd){ fPygiveCommand=std::string(cmd); }
   const std::string GetPygiveCommandForHadronizer(){ return fPygiveCommand; }
 
   void SetBases(JSFBases *bases){ fBases=bases; 
   if( fBases->GetSpring() != this ) fBases->SetSpring(this) ; }
   void DoBases(){ fDoBases=kTRUE; }
   void ReadBases(const Char_t *name);
   void Spring();
   void Spring(Int_t maxtry);
   
   void SetPrintInfo(Bool_t flag){ fPrintInfo=flag; }
   void SetPrintHist(Bool_t flag){ fPrintHist=flag; }

   void WriteRandomSeed(const Char_t *fw="");
   void ReadRandomSeed(const Char_t *fr="");
   void PrintRandomSeed(Int_t num=8);  // First num's data are printed
   
    ClassDef(JSFSpring,4)  // JSFSpring module
};


#endif

