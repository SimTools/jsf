#ifndef __JSFHEPRUP__
#define __JSFHEPRUP__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFHEPRUP                                                            //
//
// Run information for Generic Generator User Process Interface         //
//                                                                      //
//                                                                      //
//$Id$
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

// ------------------------------------------------------------
class JSFHEPRUP : public TObject
{
 protected:
  Int_t    fIDBMUP[2];  //
  Double_t fEBMUP[2];   //
  Int_t    fPDFGUP[2];  //
  Int_t    fPDFSUP[2];  //
  Int_t    fIDWTUP;  
  Int_t    fNPRUP;
  Double_t *fXSECUP;  //[fNPRUP]
  Double_t *fXERRUP;  //[fNPRUP]
  Double_t *fXMAXUP;  //[fNPRUP]
  Int_t    *fLPRUP;   //[fNPRUP]
 public:
  JSFHEPRUP();
  virtual ~JSFHEPRUP();
  JSFHEPRUP(const JSFHEPRUP &hepeup);
  JSFHEPRUP &operator=(const JSFHEPRUP &rhs);
  
  inline Int_t GetIDBMUP(Int_t i){ return fIDBMUP[i-1]; }
  inline Double_t GetEBMUP(Int_t i){ return fEBMUP[i-1]; }
  inline Int_t GetPDFGUP(Int_t i){ return fPDFGUP[i-1]; }
  inline Int_t GetPDFSUP(Int_t i){ return fPDFSUP[i-1]; }
  inline Int_t GetIDWTUP(){ return fIDWTUP; }
  inline Int_t GetNPRUP(){ return fNPRUP; }
  inline Double_t GetXSECUP(Int_t i){ return fXSECUP[i-1]; }
  inline Double_t GetXERRUP(Int_t i){ return fXERRUP[i-1]; }
  inline Double_t GetXMAXUP(Int_t i){ return fXMAXUP[i-1]; }
  inline Int_t GetLPRUP(Int_t i){ return fLPRUP[i-1]; }


  inline void SetIDBMUP(const Int_t i1, const Int_t i2){
    fIDBMUP[0]=i1 ; fIDBMUP[1]=i2; }
  inline void SetEBMUP(const Double_t bm1, const Double_t bm2){
    fEBMUP[0]=bm1 ; fEBMUP[1]=bm2; }
  inline void SetPDFGUP(const Int_t bm1, const Int_t bm2){
    fPDFGUP[0]=bm1 ; fPDFGUP[1]=bm2; }
  inline void SetPDFSUP(const Int_t bm1, const Int_t bm2){
    fPDFSUP[0]=bm1 ; fPDFSUP[1]=bm2; }
  inline void SetIDWTUP(const Int_t idwtup){ fIDWTUP=idwtup; }

  inline void SetNPRUP(const Int_t nprup){ fNPRUP=nprup; }

  void AddProcess(const Int_t iprup, const Double_t xsecup, const Double_t xerrup,
			 const Double_t xmaxup, const Int_t lprup);

  void Load(void *address);   // Read from Common/HEPRUP/
  void Save(void *address);   // Write to  Common/HEPRUP/
  void Print();  // Print contents 
  
  Bool_t ReadFile(ifstream &ios);

  ClassDef(JSFHEPRUP, 1) // JSF class to save HEPRUP common

};


#endif
