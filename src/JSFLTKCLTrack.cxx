//*LastUpdate :  jsf-1-11  30-July-1999  By Akiya Miyamoto
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
JSFLTKCLTrack::~JSFLTKCLTrack()
{
  // Load data obtained from SIMDST data.

   fCDCs.Clear();
   fEMGen.Clear();

}
//_____________________________________________________________________________
/*
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
*/


//_____________________________________________________________________________
JSFLTKCLTrack::JSFLTKCLTrack(EJSFLTKCLTrackBank bank, Float_t data[])
{
    fBank=bank;
    fP[1]=data[0] ;  fP[2]=data[1] ;  fP[3]=data[2] ;  fP[0]=data[3] ; 
    fEcl=data[4]  ;  fNEMC=(Int_t)data[6]; 
    fCharge=(Int_t)data[8]; fType=(Int_t)data[9];
    fSource=(Int_t)data[10]; fNCDC=(Int_t)data[11];
    if( fNCDC > 0 ) { 
      f1stCDC=(Int_t)data[12];
      for(Int_t i=0;i<fNCDC;i++){ fIDCDC[i]=(Int_t)data[12+i]; }
    }
    else {
      f1stCDC=-1 ;
      for(Int_t i=0;i<fNCDC;i++){ fIDCDC[i]=-1; }
    }
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
JSFLTKCLTrack::JSFLTKCLTrack(JSFLTKCLTrack& t)
{
  // Load data obtained from SIMDST data.

  fBank=t.fBank;
  for(Int_t i=0;i<4;i++){ fP[i]=t.fP[i]; }
  fEcl=t.fEcl;  fNEMC=t.fNEMC;
  fCharge=t.fCharge ;  fType=t.fType;
  fSource=t.fSource;   fNCDC=t.fNCDC;   f1stCDC=t.f1stCDC;
  fCDC=t.fCDC;
  /*
  for(Int_t i=0;i<t.GetCDCEntries();i++){
    fCDCs.Add(t.GetCDCTrackAt(i));
  }
  for(Int_t i=0;i<t.GetEMGenEntries();i++){
    fCDCs.Add(t.GetEMGenAt(i));
  }
  */
  for(Int_t i=0;i<t.fNCDC;i++){ fIDCDC[i]=t.fIDCDC[i]; }

}

//_____________________________________________________________________________
const Char_t *JSFLTKCLTrack::GetTypeName()
{
  // Return Track type by name
  return gTypeName[fType];
}

#if __ROOT_FULLVERSION__ >= 30000
//______________________________________________________________________________
void JSFLTKCLTrack::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFLTKCLTrack.

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
     if( R__v > 1 ) {
       JSFLTKCLTrack::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
       return;
     }
     TObject::Streamer(R__b);
     R__b >> (Int_t&)fBank;
     R__b.ReadStaticArray(fP);
     R__b >> fEcl;
     R__b >> fNEMC;
     R__b >> fCharge;
     R__b >> fType;
     R__b >> fSource;
     R__b >> fNCDC;
     R__b >> f1stCDC;
     R__b.CheckByteCount(R__s, R__c, JSFLTKCLTrack::IsA());

   } else {
     JSFLTKCLTrack::Class()->WriteBuffer(R__b, this);
   }
}

#else

//______________________________________________________________________________
void JSFLTKCLTrack::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFLTKCLTrack.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> (Int_t&)fBank;
      R__b.ReadStaticArray(fP);
      R__b >> fEcl;
      R__b >> fNEMC;
      R__b >> fCharge;
      R__b >> fType;
      R__b >> fSource;
      R__b >> fNCDC;
      R__b >> f1stCDC;
   } else {
      R__b.WriteVersion(JSFLTKCLTrack::IsA());
      TObject::Streamer(R__b);
      R__b << (Int_t)fBank;
      R__b.WriteArray(fP, 4);
      R__b << fEcl;
      R__b << fNEMC;
      R__b << fCharge;
      R__b << fType;
      R__b << fSource;
      R__b << fNCDC;
      R__b << f1stCDC;
   }
}

#endif


