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
////////////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFLTKCLTrack.h"


ClassImp(JSFLTKCLTrack)


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
    else f1stCDC=0 ;
    fCDC=NULL;
}

