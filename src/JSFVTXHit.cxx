//*LastUpdate : jsf-1-11  30-July-1999  A.Miyamoto
//*LastUpdate : jsf-1-4  7-Feburary-1999  A.Miyamoto
//*-- Author  : Akiya Miyamoto  7-Feburary-1999  A.Miyamoto

////////////////////////////////////////////////////////////////////////
//
//  JSFVTXHits
//
//  Utilities for VTXHits 
// 
//$Id$
//
////////////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFVTXHit.h"

ClassImp(JSFVTXHit)

//______________________________________________
JSFVTXHit::JSFVTXHit(JSFVTXHit& h)
{
  fR=h.fR;  
  fPhi=h.fPhi; 
  fZ=h.fZ;
  fDphi=h.fDphi;
  fDz=h.fDz;
  fLayer=h.fLayer;
  fLinkedTrack=h.fLinkedTrack;
  fGenTrack=h.fGenTrack;
}
