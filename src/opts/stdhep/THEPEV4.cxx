///////////////////////////////////////////////////////////////////
//
//  THEPEV4
//
//  Class to hold HEPEV4 information.  For ROOT persistency
//  
//
//$Id$ 
//  
//////////////////////////////////////////////////////////////////

#include <iostream>

#include "stdhep.h"
#include "hepev4.h"
#include "THEPEV4.h"

ClassImp(THEPEV4)

//_____________________________________________________________________________
THEPEV4::THEPEV4()
{
  fEventWeight=0.0; 
  fAlphaQED=0.0; 
  fAlphaQCD=0.0; 
  fNHEP=0;
  fSpin0=0; 
  fSpin1=0; 
  fSpin2=0; 
  fColorFlow0=0; 
  fColorFlow1=0;
  fEventID=0;
}

//_____________________________________________________________________________
THEPEV4::~THEPEV4()
{
    if( fSpin0 ) delete fSpin0;
    if( fSpin1 ) delete fSpin1;
    if( fSpin2 ) delete fSpin2;
    if( fColorFlow0 ) delete fColorFlow0;
    if( fColorFlow1 ) delete fColorFlow1;
}

//_____________________________________________________________________________
Double_t THEPEV4::GetSpin(Int_t i, Int_t elm)
{
  if( i < 0 || i > fNHEP ) {
    std::cerr << "Fatal error " 
	      << " THEPEV4::GetSpin( , ) : First argument(" << i 
	      << ") out of range (" << fNHEP << ")" << std::endl;
    return -100.0;
  }

  switch (elm) {
    case 0:  return fSpin0[i]; 
    case 1:  return fSpin1[i]; 
    case 2:  return fSpin2[i]; 
  }
  std::cerr << "Fatal error " 
	    << " THEPEV4::GetSpin( , ) : Second argument(" << elm << "  out of range " << std::endl;
  return 100.0;
}

//_____________________________________________________________________________
Int_t THEPEV4::GetColorFlow(Int_t i, Int_t elm)
{
  if( i < 0 || i > fNHEP ) {
    std::cerr << "Fatal error " 
	      << " THEPEV4::GetColorFlow( , ) : First argument(" << i 
	      << ") out of range (" << fNHEP << ")" << std::endl;
    return -100;
  }

  switch (elm) {
    case 0:  return fColorFlow0[i]; 
    case 1:  return fColorFlow1[i]; 
  }
  std::cerr << "Fatal error " 
	    << " THEPEV4::GetColorFlow( , ) : Second argument(" << elm << "  out of range " << std::endl;
  return 100;
}

//_____________________________________________________________________________
void THEPEV4::SetHEPEV4(Int_t nhep)
{
  fEventWeight=hepev4_.eventweightlh;
  fAlphaQCD=hepev4_.alphaqcdlh;
  fAlphaQED=hepev4_.alphaqedlh;
  for(Int_t i=0;i<10;i++){
    fScale[i]=hepev4_.scalelh[i];
  }
  fNHEP=nhep;

  if( !fSpin0 ) fSpin0=new Double_t[NMXHEP];
  if( !fSpin1 ) fSpin1=new Double_t[NMXHEP];
  if( !fSpin2 ) fSpin2=new Double_t[NMXHEP];
  if( !fColorFlow0 ) fColorFlow0=new Int_t[NMXHEP];
  if( !fColorFlow1 ) fColorFlow1=new Int_t[NMXHEP];

  for(Int_t i=0;i<nhep;i++){
    fSpin0[i]=hepev4_.spinlh[i][0];
    fSpin1[i]=hepev4_.spinlh[i][1];
    fSpin2[i]=hepev4_.spinlh[i][2];
    fColorFlow0[i]=hepev4_.icolorflowlh[i][0];
    fColorFlow1[i]=hepev4_.icolorflowlh[i][1];
  }
  fEventID=hepev4_.idruplh;
  
}


//________________________________________________________________
void THEPEV4::Print()
{
  std::cout << "Event ID=" << fEventID
	    << " Weight=" << fEventWeight
	    << " AlphaQED=" << fAlphaQED
	    << " AlphaQCD=" << fAlphaQCD
	    << std::endl;

}
