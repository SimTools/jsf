#ifndef __PythiaGenerator__
#define __PythiaGenerator__

///////////////////////////////////////////////////////////////////////
//
//  JSF Pythia generator
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include <TTree.h>
#include <TClonesArray.h>

#ifndef __JSFFULLGenerator__
#include "JSFFULLGenerator.h"
#endif

class COMMON_PYSUBS {
 public:
     Int_t msel;              //! msel
     Int_t msub[200];         //! msub[200]
     Int_t kfin[81][2];       //! kfin[81][2]
     Float_t ckin[200];       //! ckin[200]
//   ClassDef(COMMON_PYSUBS, 1)  // common /pysubs/
};

class COMMON_PYPARS {   
 public:
     Int_t mstp[200];         //! mstp[200]
     Float_t parp[200];       //! parp[200]
     Int_t msti[200];         //! msti[200]
     Float_t pari[200];       //! pari[200]
//  ClassDef(COMMON_PYPARS, 1)  // common /pypars/
};

class COMMON_LUDAT1  {
public:
     Int_t mstu[200];         //! mstu[200]
     Int_t paru[200];         //! paru[200]
     Int_t mstj[200];         //! mstj[200]
     Float_t parj[200];       //! parj[200]
//  ClassDef(COMMON_LUDAT1, 1)  // common /ludat1/
};


extern "C" {
extern COMMON_PYSUBS pysubs_;
extern COMMON_PYPARS pypars_;
extern COMMON_LUDAT1 ludat1_;

};


class PythiaGenerator : public JSFFULLGenerator  {

private:
  Double_t fEcm;     ; // Center of mass energy (GeV)

public:
  PythiaGenerator(const char *name="PythiaGenerator", 
		  const char *title="Pythia Event generator");
  virtual ~PythiaGenerator();

  virtual Bool_t Initialize();
  virtual Bool_t BeginRun(Int_t nrun);
  virtual Bool_t Process(Int_t ev=1);
  virtual Bool_t EndRun();
  virtual Bool_t Terminate();

  void SetEcm(Double_t ecm){fEcm=ecm;}
  void SetParam(const Char_t *common, const Char_t *vname, Int_t value);
  void SetParam(const Char_t *common, const Char_t *vname, Int_t index, Int_t value);
  void SetParam(const Char_t *common, const Char_t *vname, Int_t index, Double_t value);
  void SetParam(const Char_t *common, const Char_t *vname, Int_t ind1, Int_t ind2, Int_t value);


  ClassDef(PythiaGenerator, 1)  // Pythia Generator


};


#endif


