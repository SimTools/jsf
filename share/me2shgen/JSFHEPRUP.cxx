//_____________________________________________________________________________
//
//  JSF's HEPRUP Class
//
//  Store information of common /HEPRUP/
//
//  HEPEUP and HEPRUP is a common structure to save particle information 
//  of Matric Element generator.  It is used by  Shower and Hadronizer
//  generator, such as Pythia or Herwig, to generate stable particle list
//  for detector simulators nad phyiscs studies.
// 
//  This implementation is based on the description written at section 9.9 
//  of Pythia 6.2 manual., hep-ph/0108264.  See also hep-ph/0109068.
//
//_____________________________________________________________________________


#include "JSFConfig.h"
#include "JSFHEPRUP.h"
#include "TString.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// --- HEPEUP Common
static const Int_t kMAXPUP=100;
typedef struct {
  Int_t IDBMUP[2];
  Double_t EBMUP[2];
  Int_t PDFGUP[2];
  Int_t PDFSUP[2];
  Int_t    IDWTUP;
  Int_t    NPRUP;
  Double_t XSECUP[kMAXPUP];
  Double_t XERRUP[kMAXPUP];
  Double_t XMAXUP[kMAXPUP];
  Int_t    LPRUP[kMAXPUP];
} COMMON_HEPRUP_t ;

ClassImp(JSFHEPRUP)

// ---------------------------------------------------------------
JSFHEPRUP::JSFHEPRUP()
{
  for(Int_t i=0;i<2;i++){
    fIDBMUP[i]=0;
    fEBMUP[i]=0e0;
    fPDFGUP[i]=0;
    fPDFSUP[i]=0;
  }
  fIDWTUP=0;
  fNPRUP=0;
  fXSECUP=NULL;
  fXERRUP=NULL;
  fXMAXUP=NULL;
  fLPRUP=NULL;

}

// ---------------------------------------------------------------
JSFHEPRUP::~JSFHEPRUP()
{
  if( fXSECUP ) delete fXSECUP;
  if( fXERRUP ) delete fXERRUP;
  if( fXMAXUP ) delete fXMAXUP;
  if( fLPRUP )  delete fLPRUP;

}

// ---------------------------------------------------------------
JSFHEPRUP::JSFHEPRUP(const JSFHEPRUP &heprup)
{
  *this=heprup;
}

// ---------------------------------------------------------------
JSFHEPRUP &JSFHEPRUP::operator=(const JSFHEPRUP &heprup)
{

  if( this != &heprup ) {
    for(Int_t i=0;i<2;i++){
      fIDBMUP[i]=heprup.fIDBMUP[i];
	fEBMUP[i]=heprup.fEBMUP[i];
      fPDFGUP[i]=heprup.fPDFGUP[i];
      fPDFSUP[i]=heprup.fPDFSUP[i];
    }
    fIDWTUP=heprup.fIDWTUP;
    fNPRUP=heprup.fNPRUP;
    
    fXSECUP=NULL;
    fXERRUP=NULL;
    fXMAXUP=NULL;
    fLPRUP=NULL;
    if( fNPRUP == 0 ) {
      fXSECUP = new Double_t[fNPRUP];
      fXERRUP = new Double_t[fNPRUP];
      fXMAXUP = new Double_t[fNPRUP];
      fLPRUP  = new Int_t[fNPRUP];
    }
    for(Int_t i=0;i<fNPRUP;i++){
      fXSECUP[i]=heprup.fXSECUP[i];
      fXERRUP[i]=heprup.fXERRUP[i];
      fXMAXUP[i]=heprup.fXMAXUP[i];
      fLPRUP[i] =heprup.fLPRUP[i];
    }

  }
  return *this;
}


// ---------------------------------------------------------------
void JSFHEPRUP::Load(void *address)
{
  // Read contents of common/HEPRUP/ and store it as a data member 
  // of JSFHEPRUP class.

  COMMON_HEPRUP_t *heprup=(COMMON_HEPRUP_t*)address;

  if( fNPRUP < heprup->NPRUP ) {
    if( fXSECUP ) delete fXSECUP;
    if( fXERRUP ) delete fXERRUP;
    if( fXMAXUP ) delete fXMAXUP;
    if( fLPRUP  ) delete fLPRUP;
    fNPRUP=0;
  }
  fNPRUP  = heprup->NPRUP;
  fIDWTUP = heprup->IDWTUP;

  for(Int_t i=0;i<2;i++){
    fIDBMUP[i] = heprup->IDBMUP[i];
    fEBMUP[i]  = heprup->EBMUP[i];
    fPDFGUP[i] = heprup->PDFGUP[i];
    fPDFSUP[i] = heprup->PDFSUP[i];
  }

  if( fNPRUP > 0 ) {
    fXSECUP = new Double_t[fNPRUP];
    fXERRUP = new Double_t[fNPRUP];
    fXMAXUP = new Double_t[fNPRUP];
    fLPRUP  = new Int_t[fNPRUP];
  }
  for(Int_t i=0;i<fNPRUP;i++){
    fXSECUP[i] = heprup->XSECUP[i];
    fXERRUP[i] = heprup->XERRUP[i];
    fXMAXUP[i] = heprup->XMAXUP[i];
    fLPRUP[i]  = heprup->LPRUP[i];
  }

}

// ---------------------------------------------------------------
void JSFHEPRUP::Save(void *address)
{
  // Save contents of JSFHEPRUP data mambers into common /HEPRUP/

  COMMON_HEPRUP_t *heprup=(COMMON_HEPRUP_t*)address;

  for(Int_t i=0;i<2;i++){
    heprup->IDBMUP[i] = fIDBMUP[i] ;
    heprup->EBMUP[i]  = fEBMUP[i]  ;
    heprup->PDFGUP[i] = fPDFGUP[i] ;
    heprup->PDFSUP[i] = fPDFSUP[i] ;
  }

  heprup->NPRUP  = fNPRUP ;
  heprup->IDWTUP = fIDWTUP ;
  for(Int_t i=0;i<fNPRUP;i++){
    heprup->XSECUP[i] = fXSECUP[i] ;
    heprup->XERRUP[i] = fXERRUP[i] ;
    heprup->XMAXUP[i] = fXMAXUP[i] ;
    heprup->LPRUP[i]  = fLPRUP[i]  ;
  }

}

// ---------------------------------------------------------------
void JSFHEPRUP::AddProcess(const Int_t iprup, const Double_t xsecup, const Double_t xerrup,
			 const Double_t xmaxup, const Int_t lprup)
{
  // Set Cross section data

  if( iprup > fNPRUP ) {
    cout << "Fatal termination at JSFHEPRUP::AddProcess ";
    cout << "Specified UP number (" << iprup 
	 << ") is larger than pre-set value(" << fNPRUP << endl;
    exit(0);
  }

  fXSECUP[iprup-1] = xsecup;
  fXERRUP[iprup-1] = xerrup;
  fXMAXUP[iprup-1] = xmaxup;
  fLPRUP[iprup-1]  = lprup;

  return;

}

// ---------------------------------------------------------------
void JSFHEPRUP::Print()
{
  printf("JSFHEPRUP ...\n");
  printf(" IDBMUP=(%d,%d) ",fIDBMUP[0], fIDBMUP[1]);
  printf(" EBMUP=(%g,%g) ",fEBMUP[0], fEBMUP[1]);
  printf("\n");
  printf(" PDFGUP=(%d,%d) ",fPDFGUP[0], fPDFGUP[1]);
  printf(" PDFSUP=(%d,%d) ",fPDFSUP[0], fPDFSUP[1]);
  printf("\n");
  printf(" IDWTUP=%d  NPRUP=%d\n",fIDWTUP, fNPRUP);
  for(Int_t i=0;i<fNPRUP;i++){
    printf(" i=%d XSECUP=%g XERRUP=%g XMAXUP=%g LPRUP=%d\n",
	   i, fXSECUP[i], fXERRUP[i], fXMAXUP[i], fLPRUP[i]);
  }

}


//---------------------------------------------------
Bool_t JSFHEPRUP::ReadFile(ifstream &ios)
{

  TString inputs;

  inputs.ReadLine(ios);
  if( ios.eof() ) return kFALSE;

  Int_t noprup=fNPRUP;

  istringstream sin(inputs.Data());
  
  sin >> fIDBMUP[0] >> fIDBMUP[1] >> fEBMUP[0] >> fEBMUP[1] 
      >> fPDFGUP[0] >> fPDFGUP[1] >> fPDFSUP[0] >> fPDFSUP[1] 
      >> fIDWTUP >> fNPRUP ;

  if( noprup != 0 && fNPRUP > noprup ) {
    delete fXSECUP;
    delete fXERRUP;
    delete fXMAXUP;
    delete fLPRUP;
    fNPRUP=0;
  }
  if( fNPRUP != 0 ) {
    fXSECUP = new Double_t[fNPRUP];
    fXERRUP = new Double_t[fNPRUP];
    fXMAXUP = new Double_t[fNPRUP];
    fLPRUP  = new Int_t[fNPRUP];
  }

  for(Int_t i=0;i<fNPRUP;i++){
    ios >> fXSECUP[i] >> fXERRUP[i] >> fXMAXUP[i] >> fLPRUP[i] ;
    if( ios.eof() ) return kFALSE;
  }

  return kTRUE;

}
