//*LastUpdate :  jsf-1-11 24-July-1999  Akiya Miyamoto
//*-- Author  : A.Miyamoto  24-July-1999

////////////////////////////////////////////////////////////////////////
//
// JSFUtil
//
// A collection of utility functions
//
//$Id$
////////////////////////////////////////////////////////////////////////

#include "JSFUtil.h"

ClassImp(JSFUtil)

//_____________________________________________________________________________
Double_t JSFUtil::ConfidenceLevel(Int_t ndf, Double_t chisq)
{   
/*
CC********************************************************************CC
C*                                                                    *C
C*====================----=========                                   *C
C*  Subroutine UCONF8(PROB,N,CHI2)                                    *C
C*====================----=========                                   *C
C*                                                                    *C
C* 1) Purpose                                                         *C
C*     Give confidence level.                                         *C
C*     < Caution >                                                    *C
C* 2) Inputs                                                          *C
C*     N        ; degrees of freedom.                                 *C
C*     CHI2     ; chi-squared.                                        *C
C* 3) Outputs                                                         *C
C*     PROB     ; confidence level.                                   *C
C* 4) Relation                                                        *C
C*     Invoked by                                                     *C
C*     Calls                                                          *C
C* 5) Update record                                                   *C
C*      01/01/64  C.Letertre  Origival version.                       *C
C*      10/23/81  B.Schorr    Revised.                                *C
C*      02/04/82              CERN PROGLIB# G100    PROB              *C
C*                            Version KERNFOR                         *C
C*      04/16/86  K.Fujii     Copied from YLM.NAW.FORT01.             *C
C*      08/29/86  K.Fujii     Double precision version.               *C
C*      02/14/99  A.Miyamoto  Converted to C++                        *C
C*                                                                    *C
CC********************************************************************CC
*/

  Double_t srtopi=TMath::Sqrt( 2.0/TMath::Pi() );
  Double_t root2i=TMath::Sqrt( 0.5 );

  Double_t upl=3400.0;

  //C--
  //C  Reset cofidence level.
  //C--
  Double_t prob=0.0;

  //C--
  //C  Check if N and CHI2 physical.
  //C--
  if( ndf <= 0 || chisq < 0.0 ) return prob;

  //C--
  //C  Check if Gaussian approximation valid.
  //C--

  Double_t term, sum, srty;
  if( ndf <= 60 ) {
    if( chisq > upl ) return prob;
    //C--
    //C  Should use exact formula.
    //C--
    sum=TMath::Exp(-0.5*chisq);
    term=sum;
    Int_t    m=ndf/2;
    Int_t    i;
    if( 2*m == ndf ) {
      //C <Even>
      if( m != 1 ){
        for(i=2;i<=m;i++){
	  term *= 0.5*chisq/((Double_t)(i-1));
	  sum += term;
	}
      }
      prob=sum;
    }
    else {
      //C <Odd>
      srty=TMath::Sqrt(chisq);
      prob=TMath::Erfc(root2i*srty);
      if( ndf != 1 ) {
	if( ndf != 3 ) {
 	  for(i=1;i<m;i++){ term *=chisq/((Double_t)(2*i+1)); sum +=term; }
	}
	prob += srtopi*srty*sum;
      }
    }
  }
  else {
  //C--
  //C  Gaussian asymptotic formula.
  //C--
     srty=TMath::Sqrt(chisq) - TMath::Sqrt(ndf-0.5);
     if( srty < 12.0 ) prob = 0.5*TMath::Erfc(srty);
  }
  return prob;
}
