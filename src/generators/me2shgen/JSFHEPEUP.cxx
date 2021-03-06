//_____________________________________________________________________________
//
//  JSF's HEPEUP Class
//
//  Store information in common /HEPEUP/
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
#include "JSFHEPEUP.h"

#include "TString.h"
#ifdef __USEISOCXX__
#include <iostream>
#include <fstream>
#else
#include <fstream>
#include <iostream.h>
#endif

#include <sstream>
#include <string>

#if __GNUC__ >= 5 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3)
#include <cstdlib>
#endif

using namespace std;

#include "JSFReadGZippedFile.h"

// --- HEPEUP Common
static const Int_t kMAXNUP=500;
typedef struct {
  Int_t NUP;
  Int_t IDPRUP;
  Double_t XWGTUP;
  Double_t SCALUP;
  Double_t AQEDUP;
  Double_t AQCDUP;
  Int_t    IDUP[kMAXNUP];
  Int_t    ISTUP[kMAXNUP];
  Int_t    MOTHUP[2*kMAXNUP];
  Int_t    ICOLUP[2*kMAXNUP];
  Double_t PUP[5*kMAXNUP];
  Double_t VTIMUP[kMAXNUP];
  Double_t SPINUP[kMAXNUP];
} COMMON_HEPEUP_t ;


ClassImp(JSFHEPEUP)

// ---------------------------------------------------------------
JSFHEPEUP::JSFHEPEUP(Int_t nup)
{

  fNUP    = nup;
  fNUP2   = 2*fNUP;
  fNUP5   = 5*fNUP;
  fIDPRUP = 0;
  fXWGTUP = 0E0;
  fSCALUP = 0E0;
  fAQEDUP = 0E0;
  fAQCDUP = 0E0;

  fIDUP   = NULL;
  fISTUP  = NULL;
  fVTIMUP = NULL;
  fSPINUP = NULL;

  fMOTHUP = NULL;
  fICOLUP = NULL;
  fPUP    = NULL;
  
  if( fNUP > 0 ) {
    fIDUP   = new Int_t[fNUP];
    fISTUP  = new Int_t[fNUP];
    fMOTHUP = new Int_t[2*fNUP];
    fICOLUP = new Int_t[2*fNUP];
    fPUP    = new Double_t[5*fNUP];
    fVTIMUP = new Double_t[fNUP];
    fSPINUP = new Double_t[fNUP];
  }

  for(Int_t i=0;i<4;i++){
    for(Int_t j=0;j<5;j++){
      fIncomming[i][j]=0.0;
    }
  }

}

// ---------------------------------------------------------------
JSFHEPEUP::~JSFHEPEUP()
{
  if( fIDUP ) delete fIDUP;
  if( fISTUP ) delete fISTUP;
  if( fVTIMUP ) delete fVTIMUP;
  if( fSPINUP ) delete fSPINUP;
  if( fMOTHUP ) delete fMOTHUP;
  if( fICOLUP ) delete fICOLUP;
  if( fPUP ) delete fPUP;

}

// ---------------------------------------------------------------
JSFHEPEUP::JSFHEPEUP(const JSFHEPEUP &hepeup)
{
  *this = hepeup;
}

// ---------------------------------------------------------------
JSFHEPEUP &JSFHEPEUP::operator=(const JSFHEPEUP &hepeup)
{
  if( this != &hepeup ) {
    fNUP    = hepeup.fNUP;
    fNUP2   = hepeup.fNUP2;
    fNUP5   = hepeup.fNUP5;
    fIDPRUP = hepeup.fIDPRUP;
    fXWGTUP = hepeup.fXWGTUP;
    fSCALUP = hepeup.fSCALUP;
    fAQEDUP = hepeup.fAQEDUP;
    fAQCDUP = hepeup.fAQCDUP;

    fIDUP   = NULL;
    fISTUP  = NULL;
    fVTIMUP = NULL;
    fSPINUP = NULL;

    fMOTHUP = NULL;
    fICOLUP = NULL;
    fPUP    = NULL;
    if( fNUP > 0 ) {
      fIDUP   = new Int_t[fNUP];
      fISTUP  = new Int_t[fNUP];
      fMOTHUP = new Int_t[2*fNUP];
      fICOLUP = new Int_t[2*fNUP];
      fPUP    = new Double_t[5*fNUP];
      fVTIMUP = new Double_t[fNUP];
      fSPINUP = new Double_t[fNUP];

      for(Int_t i=0;i<fNUP;i++){
	fIDUP[i]=hepeup.fIDUP[i];
	fISTUP[i]=hepeup.fISTUP[i];
	fVTIMUP[i]=hepeup.fVTIMUP[i];
	fSPINUP[i]=hepeup.fSPINUP[i];
      }
      for(Int_t i=0;i<2*fNUP;i++){
	fMOTHUP[i]=hepeup.fMOTHUP[i];
	fICOLUP[i]=hepeup.fICOLUP[i];
      }
      for(Int_t i=0;i<5*fNUP;i++){
	fPUP[i]=hepeup.fPUP[i];
      }

    }

    for(Int_t i=0;i<4;i++){
      for(Int_t j=0;j<5;j++){
	fIncomming[i][j]=hepeup.fIncomming[i][j];
      }
    }

  } 
  return *this;
 
}

// ---------------------------------------------------------------
Bool_t JSFHEPEUP::SetParticle(Int_t i, Int_t idup, Int_t istup, Int_t mother[2],
		   Int_t colup[2], Double_t p[5], Double_t vtimup,
		   Double_t spinup)
{
  // Set Particle information.  0 <= i < fNUP

  if( i >= fNUP ) { 
    cout << "JSFHEPEUP::SetParticle.. index i (" << i ;
    cout << ") exceeded fNUP(" << fNUP << ") " << endl;
    return kFALSE;
  }
  fIDUP[i]  = idup;
  fISTUP[i] = istup;
  for(Int_t j=0;j<2;j++) {
    fMOTHUP[2*i+j] = mother[j];
    fICOLUP[2*i+j] = colup[j];
  }
  for(Int_t j=0;j<5;j++) {
    fPUP[5*i+j] = p[j];
  }
  
  fVTIMUP[i]=vtimup;
  fSPINUP[i]=spinup;

  return kTRUE;

}

// ---------------------------------------------------------------
Bool_t JSFHEPEUP::GetParticle(Int_t i, Int_t &idup, Int_t &istup, 
			      Int_t mother[2],
		   Int_t colup[2], Double_t p[5], Double_t &vtimup,
		   Double_t &spinup)
{
  if( i >= fNUP ) { 
    cout << "JSFHEPEUP::GetParticle.. index i (" << i ;
    cout << ") exceeded fNUP(" << fNUP << ") " << endl;
    return kFALSE;
  }
  idup  = fIDUP[i];
  istup = fISTUP[i];
  for(Int_t j=0;j<2;j++) {
    mother[j] = fMOTHUP[2*i+j];
    colup[j]  = fICOLUP[2*i+j];
  }
  for(Int_t j=0;j<5;j++) {
    p[j] = fPUP[5*i+j];
  }
  
  vtimup=fVTIMUP[i];
  spinup=fSPINUP[i];

  return kTRUE;

}
		   
// ---------------------------------------------------------------
Bool_t JSFHEPEUP::AppendParticles(Int_t n, Int_t idup[], Int_t istup[], Int_t mother[][2],
		   Int_t colup[][2], Double_t p[][5], Double_t vtimup[],
		   Double_t spinup[])
{
  JSFHEPEUP old(*this);

  if( fNUP != 0 ) {
    delete  fIDUP;
    delete  fISTUP;
    delete  fMOTHUP;
    delete  fICOLUP;
    delete  fPUP;
    delete  fVTIMUP;
    delete  fSPINUP;
    fNUP += n;
    fIDUP   = new Int_t[fNUP];
    fISTUP  = new Int_t[fNUP];
    fMOTHUP = new Int_t[2*fNUP];
    fICOLUP = new Int_t[2*fNUP];
    fPUP    = new Double_t[5*fNUP];
    fVTIMUP = new Double_t[fNUP];
    fSPINUP = new Double_t[fNUP];
  }

  fNUP2   = 2*fNUP;
  fNUP5   = 5*fNUP;
  fIDPRUP = old.GetIDPRUP();
  fXWGTUP = old.GetXWGTUP();
  fSCALUP = old.GetSCALUP();
  fAQEDUP = old.GetAQEDUP();
  fAQCDUP = old.GetAQCDUP();
  for(Int_t i=1;i<=old.GetNUP();i++){
    fIDUP[i-1]=old.GetIDUP(i);
    fISTUP[i-1]=old.GetISTUP(i);
    fVTIMUP[i-1]=old.GetVTIMUP(i);
    fSPINUP[i-1]=old.GetSPINUP(i);
  }
  for(Int_t i=0;i<2*fNUP;i++){
    fMOTHUP[i]=old.fMOTHUP[i];
    fICOLUP[i]=old.fICOLUP[i];
  }
  for(Int_t i=0;i<5*fNUP;i++){
    fPUP[i]=old.fPUP[i];
  }

  // Add 

  Int_t oldnup=old.GetNUP();
  for(Int_t i=0;i<n;i++){
    if( !SetParticle(oldnup+i, idup[i], istup[i], &mother[i][0],
		     colup[i], &p[i][0], vtimup[i], spinup[i]) ) {
      cout << "Fatal error in JSFHEPEUP." << endl;
      exit (0);
    }
  }

  return kTRUE;

}


// ---------------------------------------------------------------
void JSFHEPEUP::Load(void *address)
{
  // Read contents of common/HEPEUP/ and store it as a data member 
  // of JSFHEPEUP class.

  COMMON_HEPEUP_t *hepeup=(COMMON_HEPEUP_t*)address;

  if( fNUP < hepeup->NUP ) {
    delete  fIDUP;
    delete  fISTUP;
    delete  fMOTHUP;
    delete  fICOLUP;
    delete  fPUP;
    delete  fVTIMUP;
    delete  fSPINUP;
    fNUP=0;
  }
  if( fNUP == 0 ) {
    fIDUP   = new Int_t[fNUP];
    fISTUP  = new Int_t[fNUP];
    fMOTHUP = new Int_t[2*fNUP];
    fICOLUP = new Int_t[2*fNUP];
    fPUP    = new Double_t[5*fNUP];
    fVTIMUP = new Double_t[fNUP];
    fSPINUP = new Double_t[fNUP];
  }

  fNUP    = hepeup->NUP;
  fNUP2   = 2*fNUP;
  fNUP5   = 5*fNUP;
  fIDPRUP = hepeup->IDPRUP;
  fXWGTUP = hepeup->XWGTUP;
  fSCALUP = hepeup->SCALUP;
  fAQEDUP = hepeup->AQEDUP;
  fAQCDUP = hepeup->AQCDUP;
  for(Int_t i=0;i<fNUP;i++){
    fIDUP[i]=hepeup->IDUP[i];
    fISTUP[i]=hepeup->ISTUP[i];
    fVTIMUP[i]=hepeup->VTIMUP[i];
    fSPINUP[i]=hepeup->SPINUP[i];
  }
  for(Int_t i=0;i<2*fNUP;i++){
    fMOTHUP[i]=hepeup->MOTHUP[i];
    fICOLUP[i]=hepeup->ICOLUP[i];
  }
  for(Int_t i=0;i<5*fNUP;i++){
    fPUP[i]=hepeup->PUP[i];
  }

}

// ---------------------------------------------------------------
void JSFHEPEUP::Save(void *address)
{
  // Save contents of JSFHEPEUP data mambers into common /HEPEUP/

  COMMON_HEPEUP_t *hepeup=(COMMON_HEPEUP_t*)address;

  hepeup->NUP = fNUP ;
  hepeup->IDPRUP = fIDPRUP;
  hepeup->XWGTUP = fXWGTUP;
  hepeup->SCALUP = fSCALUP;
  hepeup->AQEDUP = fAQEDUP;
  hepeup->AQCDUP = fAQCDUP;
  for(Int_t i=0;i<fNUP;i++){
    hepeup->IDUP[i]   = fIDUP[i];
    hepeup->ISTUP[i]  = fISTUP[i];
    hepeup->VTIMUP[i] = fVTIMUP[i];
    hepeup->SPINUP[i] = fSPINUP[i];
  }
  for(Int_t i=0;i<2*fNUP;i++){
    hepeup->MOTHUP[i] = fMOTHUP[i];
    hepeup->ICOLUP[i] = fICOLUP[i];
  }
  for(Int_t i=0;i<5*fNUP;i++){
    hepeup->PUP[i] = fPUP[i];
  }

}

// ---------------------------------------------------------------
void  JSFHEPEUP::ResetBeamParticles(Double_t nominale[2])
{
  //  Squeeze first four lines to two and replace in-comming particle
  //  with those with nominal energy.  
  
  for(Int_t i=0;i<4;i++){
    for(Int_t j=0;j<5;j++){
      Double_t *pup=GetPUP(i+1);
      fIncomming[i][j]=pup[j];
    }
  }

  Double_t pe[5]={0.0, 0.0, 0.0, 0.0, 0.0};
  pe[2]=nominale[0];
  pe[3]=nominale[0];
  SetParticle(0, GetIDUP(1), GetISTUP(1), &fMOTHUP[0], &fICOLUP[0],
	      pe, GetVTIMUP(1), GetSPINUP(1));
  pe[2]=-nominale[1];
  pe[3]=nominale[0];
  SetParticle(1, GetIDUP(2), GetISTUP(2), &fMOTHUP[2], &fICOLUP[2],
	      pe, GetVTIMUP(2), GetSPINUP(2));

  for(Int_t i=5; i<=fNUP ; i++ ){
    SetParticle(i-3, GetIDUP(i), GetISTUP(i), &fMOTHUP[2*(i-1)], &fICOLUP[2*(i-1)],
	      GetPUP(i), GetVTIMUP(i), GetSPINUP(i));
  }    
  fNUP-=2;

}




// ---------------------------------------------------------------
void JSFHEPEUP::Print()
{
  printf("JSFHEPEUP\n");
  printf(" NUP=%d  IDPRUP=%d",fNUP,fIDPRUP);
  printf(" XWGTUP=%g SCALUP=%g",fXWGTUP,fSCALUP);
  printf(" AQEDUP%g AQCDUP=%g",fAQEDUP, fAQCDUP);
  printf("\n");
  printf("  i ID    IST "); 
  printf("Moth1 Moth2 Col1 Col2");
  printf(" Px Py Pz E Mass");
  printf(" Vtim  Spin");
  printf("\n");

  for(Int_t i=0;i<fNUP;i++){
    printf("%d %d %d ",i,fIDUP[i], fISTUP[i]);
    printf("%d %d %d %d ",fMOTHUP[2*i],fMOTHUP[2*i+1],fICOLUP[2*i],fICOLUP[2*i+1]);
    printf("(%g,%g,%g,%g,%g)",fPUP[5*i],fPUP[5*i+1],fPUP[5*i+2],fPUP[5*i+3],fPUP[5*i+4]);
    printf("(%g,%g)",fVTIMUP[i],fSPINUP[i]);
    printf("\n");
  }


}

//----------------------------------------------------------------
Bool_t JSFHEPEUP::ReadFile(ifstream &in)
{

  Int_t noldup = sizeof(fIDUP)/4;

  in >> fNUP >> fIDPRUP >> fXWGTUP >> fSCALUP >> fAQEDUP >> fAQCDUP ;

  if( in.eof() ) {
    fNUP=0;
    return kFALSE;
  }

  if( fNUP > noldup ) {
    if( fIDUP ) {
      delete fIDUP;
      delete fISTUP;
      delete fMOTHUP;
      delete fICOLUP;
      delete fPUP;
      delete fVTIMUP;
      delete fSPINUP;
    }
    fIDUP   = new Int_t[fNUP];
    fISTUP  = new Int_t[fNUP];
    fMOTHUP = new Int_t[2*fNUP];
    fICOLUP = new Int_t[2*fNUP];
    fPUP    = new Double_t[5*fNUP];
    fVTIMUP = new Double_t[fNUP];
    fSPINUP = new Double_t[fNUP];
  }

  for(Int_t i=0;i<fNUP;i++) {
    in >> fIDUP[i] >> fISTUP[i] >> fMOTHUP[2*i] >> fMOTHUP[2*i+1] 
       >> fICOLUP[2*i] >> fICOLUP[2*i+1] 
       >> fPUP[5*i] >> fPUP[5*i+1] >> fPUP[5*i+2] >> fPUP[5*i+3] >> fPUP[5*i+4] 
       >> fVTIMUP[i] >> fSPINUP[i] ;

    if( in.eof() ) return kFALSE;
  }

  return kTRUE;

}


//----------------------------------------------------------------
Bool_t JSFHEPEUP::ReadFile(JSFReadGZippedFile &gzfile)
{

  Int_t noldup = sizeof(fIDUP)/4;

  char instr[4096];

  Int_t ir=gzfile.GetLine(instr, 1024);

  if( ir <= 0 || gzfile.IsEOF() ) {
    fNUP=0;
    return kFALSE;
  }

    
  string input(instr);
  istringstream istr(input);
  istr >> fNUP >> fIDPRUP >> fXWGTUP >> fSCALUP >> fAQEDUP >> fAQCDUP ;

  if( fNUP > noldup ) {
    if( fIDUP ) {
      delete fIDUP;
      delete fISTUP;
      delete fMOTHUP;
      delete fICOLUP;
      delete fPUP;
      delete fVTIMUP;
      delete fSPINUP;
    }
    fIDUP   = new Int_t[fNUP];
    fISTUP  = new Int_t[fNUP];
    fMOTHUP = new Int_t[2*fNUP];
    fICOLUP = new Int_t[2*fNUP];
    fPUP    = new Double_t[5*fNUP];
    fVTIMUP = new Double_t[fNUP];
    fSPINUP = new Double_t[fNUP];
  }

  for(Int_t i=0;i<fNUP;i++) {
    Int_t ir=gzfile.GetLine(instr, 1024);
     if( ir <= 0 || gzfile.IsEOF() ) {
      fNUP=0;
      return kFALSE;
    }
    string input2(instr);
    stringstream istr(input2);
    istr >>  fIDUP[i] >> fISTUP[i] >> fMOTHUP[2*i] >> fMOTHUP[2*i+1] 
       >> fICOLUP[2*i] >> fICOLUP[2*i+1] 
       >> fPUP[5*i] >> fPUP[5*i+1] >> fPUP[5*i+2] >> fPUP[5*i+3] >> fPUP[5*i+4] 
       >> fVTIMUP[i] >> fSPINUP[i] ;

  }

  return kTRUE;

}

