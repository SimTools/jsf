//*LastUpdate :  jsf-1-4  13-Feburary-1999  By Akiya Miyamoto
//*LastUpdate :  0.04.01  5-October-1998  By Akiya Miyamoto
//*-- Author  : Akiya Miyamoto  5-Octorber-1998

////////////////////////////////////////////////////////////////////////
//
//  JSFLTKCLTrack
//
//  A class to save linked track-cluster track
//  Algorithm and performance of Link-Track-Cluster routines are described
//  in begin_html <a href="http://www-jlc.kek.jp/subg/offl/lib/docs/cmbtrk.ps.gz">New Combined Track Bank(PS 329KB)</a> end_html .
// 
//  Data member in this classes are those created by the subroutine CMBTRK. 
//  Bank data can be obtained by member functions of this class.
// 
//(Warning) 
//  When this class is created when reading the SIMDST data, some information
//  such as fEMC is not given properly as no corresponding information is 
//  available in SIMDST format.  Please use them with care.
//$Id$
//
////////////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFLTKCLTrack.h"


ClassImp(JSFLTKCLTrack)

  const Char_t *gTypeName[14]={"illegal track type",
               "pure gamma", "gamma in mixed EMC cluster",
	       "pure nutral hadron", "hadron in mixed HDC cluster",
	       "pure charged hadron", "unmatched track",
	       "illegal track type",  "illegal track type",
	       "illegal track type",  "illegal track type",
	       "electron candidate",  "illegal track type",
	       "muon candidate"};


//_____________________________________________________________________________
JSFLTKCLTrack::JSFLTKCLTrack(EJSFLTKCLTrackBank bank, TVector& P, Float_t ecl,
         Int_t nemc, Int_t charge, Int_t type, Int_t source, 
 	 Int_t ncdc, Int_t first)
{ 
     fBank = bank ;
     for(Int_t i=0;i<4;i++){ fP[i]=P(i); }
     fEcl=ecl; fNEMC=nemc; fCharge=charge; fType=type ;
     fSource=source; fNCDC=ncdc ; f1stCDC=first ;
     fCDC=NULL;
}

//_____________________________________________________________________________
JSFLTKCLTrack::JSFLTKCLTrack(EJSFLTKCLTrackBank bank, Float_t data[])
{
    fBank=bank;
    fP[1]=data[0] ;  fP[2]=data[1] ;  fP[3]=data[2] ;  fP[0]=data[3] ; 
    fEcl=data[4]  ;  fNEMC=(Int_t)data[6]; 
    fCharge=(Int_t)data[8]; fType=(Int_t)data[9];
    fSource=(Int_t)data[10]; fNCDC=(Int_t)data[11];
    if( fNCDC > 0 ) f1stCDC=(Int_t)data[12];
    else f1stCDC=-1 ;
    fCDC=NULL;
}

//_____________________________________________________________________________
JSFLTKCLTrack::JSFLTKCLTrack(Float_t data[])
{
  // Load data obtained from SIMDST data.
    fP[1]=data[0] ;  fP[2]=data[1] ;  fP[3]=data[2] ;  fP[0]=data[3] ; 
    fCharge=(Int_t)data[4]; fType=(Int_t)data[5];
    fCDC=NULL;
    fSource=0;
    fNCDC=0;
    f1stCDC=-1;
    fEcl=0;
    fNEMC=0;
    if( fType !=1 || fType != 3 ) f1stCDC=(Int_t)data[7];

}

//_____________________________________________________________________________
const Char_t *JSFLTKCLTrack::GetTypeName()
{
  // Return Track type by name
  return gTypeName[fType];
}




