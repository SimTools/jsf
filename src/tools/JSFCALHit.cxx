//*LastUpdate : jsf-1-15  7-Jul-2000  A.Miyamoto
//*LastUpdate : jsf-1-6  30-March-1999  A.Miyamoto
//*LastUpdate : jsf-1-4  6-Feburary-1999  A.Miyamoto
//*-- Author  : Akiya Miyamoto  6-Feburary-1999  A.Miyamoto


////////////////////////////////////////////////////////////////////////
//
//  JSFCALHitCell
//
//  Class for Calorimeters hits.
//
//(Update)
//  30-March-1999  A.Miyamoto
//   JSFCALHit class is modified to save energy deposited by EM particle
//   and those by HD particle separately.  This is not backword compatible.
//  7-Jul-2000 A.Miyamoto  Add JSFSMHit (Shower Max counter's hit)
//
//$Id$
//
////////////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFCALHit.h"

ClassImp(JSFCALHit)
ClassImp(JSFEMCHit)
ClassImp(JSFHDCHit)
ClassImp(JSFSMHit)

