//*LastUpdate :  jsf-1-15  6-August-2000  By Akiya Miyamoto
//*-- Author  : A.Miyamoto  6-August-2000 


///////////////////////////////////////////////////////////////////
//
// JSFJIMEventDisplay 
//
//  Displays JIM event data
//
//$Id: 
//
///////////////////////////////////////////////////////////////////

#include <TMarker3DBox.h>

#include "JSFSteer.h"
#include "JSFBasicClasses.h"
#include "JSFGeneratorParticle.h"
#include "JSFQuickSimParam.h"
#include "JSFSIMDST.h"
#include "JSFJIMEventDisplay.h"

#include "JSFJIMParam.h"

ClassImp(JSFJIMEventDisplay)

TMarker3DBox *box3d;

//---------------------------------------------------------------------------
void JSFJIMEventDisplay::DisplayEMCHits()
{
  JSFSIMDST *simdst=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();
  TClonesArray *emc=sdb->GetEMCHits(); 
  JSFJIMParam *par=(JSFJIMParam*)simdst->Param();

  //  Float_t dybarrel;
  //  Float_t dzbarrel;
  //  Float_t dxend   ;
  
  Int_t i;
  Double_t todegree=180.0/TMath::Pi();
  Float_t pos[3], width[3];
  Double_t x=0; Double_t y=0; Double_t z=0;

  for(i=0;i<sdb->GetNEMCHits();i++){
     JSFEMCHit *t=(JSFEMCHit*)emc->UncheckedAt(i);
     Int_t cel=t->GetCellID();
     Double_t energy=t->GetEnergy();
     if( energy <= 0 ) continue;

     par->GetEMPosition(cel, pos, width);

     Double_t ang=0;
     Double_t ath=0;
     Int_t izee=cel/1000000;
     Double_t dy=pos[0]*width[1]*0.5;
     Double_t dx=energy*fEMCScale;  
     Double_t dz=width[2]*0.5;
     if( izee == 0 ) { 
       x=(pos[0]+dx)*TMath::Cos(pos[1]) ;
       y=(pos[0]+dx)*TMath::Sin(pos[1]) ;
       z=pos[2]*(1+dx/(pos[0]+par->GetPointingOffset())) ;
       ang = pos[1]*todegree; 
       ath=TMath::ATan2((pos[0]+par->GetPointingOffset()),z)*todegree + 90;
     }
     else { 
       dy=TMath::Abs(pos[0]*width[1]*0.5);
       dx=TMath::Abs(width[0]*0.5);
       dz=TMath::Abs(izee*energy*fEMCScale);
       x=(pos[0]+dx)*TMath::Cos(pos[1]) ;
       y=(pos[0]+dx)*TMath::Sin(pos[1]) ;
       z=pos[2]+izee*dz;
       ang = 0 ; 
       ath= TMath::ATan2(pos[0], pos[2])*todegree;
   }
     
     box3d=new TMarker3DBox(x, y, z, dx, dy, dz, ath, ang);
     box3d->SetLineColor(fEMCHit->fColor);
     box3d->SetLineWidth((Width_t)fEMCHit->fSize);
     box3d->Draw();
     fSignals->Add(box3d);
  } 
}


//---------------------------------------------------------------------------
void JSFJIMEventDisplay::DisplayHDCHits()
{
  JSFSIMDST *simdst=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();
  TClonesArray *hdc=sdb->GetHDCHits(); 
  JSFJIMParam *par=(JSFJIMParam*)simdst->Param();
  //  JSFCALGeoParam *geo=par->GetHDCGeom();

  Int_t i;
  Double_t todegree=180.0/TMath::Pi();
  Double_t x=0; Double_t y=0; Double_t z=0;
  Float_t pos[3], width[3];

  for(i=0;i<sdb->GetNHDCHits();i++){
     JSFHDCHit *t=(JSFHDCHit*)hdc->UncheckedAt(i);
     Int_t cel=t->GetCellID();
     Double_t energy=t->GetEnergy();
     if( energy <= 0 ) continue;
     par->GetHDPosition(cel, pos, width);

     Double_t ang=0;
     Double_t ath=0;
     Int_t izee=cel/1000000;
     Double_t dy=pos[0]*width[1]*0.5;
     Double_t dx=energy*fEMCScale;  
     Double_t dz=width[2]*0.5;
     if( izee == 0 ) { 
       x=(pos[0]+dx)*TMath::Cos(pos[1]) ;
       y=(pos[0]+dx)*TMath::Sin(pos[1]) ;
       z=pos[2]*(1+dx/(pos[0]+par->GetPointingOffset())) ;
       ang = pos[1]*todegree;
       ath=TMath::ATan2((pos[0]+par->GetPointingOffset()),z)*todegree + 90;
     }
     else { 
       dy=pos[0]*width[1]*0.5;
       dx=width[0]*0.5;
       dz=izee*energy*fEMCScale;
       x=(pos[0]+dx)*TMath::Cos(pos[1]) ;
       y=(pos[0]+dx)*TMath::Sin(pos[1]) ;
       z=pos[2]+izee*dz;
       ang = 0 ; 
       ath= TMath::ATan2(pos[0], pos[2])*todegree;
     }

     box3d=new TMarker3DBox(x, y, z, dx, dy, dz, ath, ang);
     box3d->SetLineColor(fHDCHit->fColor);
     box3d->SetLineWidth((Width_t)fHDCHit->fSize);
     box3d->Draw();
     fSignals->Add(box3d);

  } 

}






