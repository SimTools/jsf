///////////////////////////////////////////////////////////////////
//
// JSFEventDisplay
//
// Displays event data.
// This is a preliminary version of event display for experiment
// to know how to use ROOT GUI and 3D Graphics.
//
// Type of event display is selected by fDisplayType.  Its meanings are,
//   =0 for Momentum Display, 
//   =1 for event display (All)
//   =2 for event display near VTX.
//   
// Parameters ;  name: default #  description 
// ~~~~~~~~~~~~
//  JSFEventDisplay.[name].Show: 
/*
  Char_t arg[128], args[128];
  sprintf(arg,"JSFEventDisplay.%s.Show",name);
  fShow=gJSF->Env()->GetValue(arg,show);
  sprintf(arg,"JSFEventDisplay.%s.Color",name);
  sprintf(args,"%d",color);
  sscanf(gJSF->Env()->GetValue(arg,args),"%d",&fColor);
  sprintf(arg,"JSFEventDisplay.%s.Type",name);
  fType=gJSF->Env()->GetValue(arg,type);
  sprintf(arg,"JSFEventDisplay.%s.Size",name);
  sprintf(args,"%g",size);
  sscanf(gJSF->Env()->GetValue(arg,args),"%g",&fSize);
*/
//
//$Id$
//
///////////////////////////////////////////////////////////////////

#include <THelix.h>
#include <TPolyMarker3D.h>
#include <TMarker3DBox.h>
#include <TBRIK.h>
#include <TTUBE.h>
#include <TSPHE.h>
#include <TNode.h>
#include <TGeometry.h>

#include "JSFSteer.h"
#include "JSFBasicClasses.h"
#include "JSFGeneratorParticle.h"
#include "JSFQuickSimParam.h"
#include "JSFSIMDST.h"
#include "JSFEventDisplay.h"

enum EJSFEDCommandIdentifiers {
   EDO_B_DRAW,
   EDO_B_SHOW_GENCHARGED,
   EDO_B_REDRAW,
   EDO_B_DISMISS,

   EDO_CB_SHOW_GENNEUTRALS,
   EDO_CB_SHOW_GENCHARGED,
   EDO_CB_SHOW_LTKCLTRACKS,
   EDO_CB_SHOW_CDCTRACKS,
   EDO_CB_SHOW_VTXHITS,
   EDO_CB_SHOW_EMCHITS,
   EDO_CB_SHOW_HDCHITS,

   EDO_RB_DRAW_MOMENTUM,
   EDO_RB_DRAW_SIGNAL,
   EDO_RB_DRAW_VTX

};

ClassImp(JSFEventDisplay)
ClassImp(JSFEDProperty)
ClassImp(JSFEDHelix)

JSFSIMDST *simdst=0;
TPolyMarker3D *pm=0;
TMarker3DBox *box3d=0;
TPolyLine3D *pl=0;
TNode *gvAll=0;
TNode *gvVTX=0;
TNode *gvMomentum=0;
Bool_t  gGeometryIsInitialized;

//---------------------------------------------------------------------------
JSFEDHelix::JSFEDHelix(): THelix()
{
  
}

//---------------------------------------------------------------------------
JSFEDHelix::~JSFEDHelix()
{

  //   if(fRotMat) gGeometry->GetListOfMatrices()->Remove(fRotMat);
}


//---------------------------------------------------------------------------
JSFEDProperty::JSFEDProperty(Char_t *name, 
			     Int_t show, Int_t color, Int_t type, Float_t size )
{
  strncpy(fName, name,24);
  Char_t arg[128], args[128];
  sprintf(arg,"JSFEventDisplay.%s.Show",name);
  fShow=gJSF->Env()->GetValue(arg,show);
  sprintf(arg,"JSFEventDisplay.%s.Color",name);
  sprintf(args,"%d",color);
  sscanf(gJSF->Env()->GetValue(arg,args),"%d",&fColor);
  sprintf(arg,"JSFEventDisplay.%s.Type",name);
  fType=gJSF->Env()->GetValue(arg,type);
  sprintf(arg,"JSFEventDisplay.%s.Size",name);
  sprintf(args,"%g",size);
  sscanf(gJSF->Env()->GetValue(arg,args),"%g",&fSize);
}



//---------------------------------------------------------------------------
void JSFEDProperty::Update(TGPopupMenu *menu, Int_t menuid)
{
  Char_t arg[128], args[128];
  sprintf(arg,"JSFEventDisplay.%s.Show",fName);
  Int_t itmp=fShow;
  fShow=gJSF->Env()->GetValue(arg,itmp);
  sprintf(arg,"JSFEventDisplay.%s.Color",fName);
  sprintf(args,"%d",fColor);
  sscanf(gJSF->Env()->GetValue(arg,args),"%d",&fColor);
  sprintf(arg,"JSFEventDisplay.%s.Type",fName);
  itmp=fType;
  fType=gJSF->Env()->GetValue(arg,itmp);
  sprintf(arg,"JSFEventDisplay.%s.Size",fName);
  sprintf(args,"%g",fSize);
  sscanf(gJSF->Env()->GetValue(arg,args),"%g",&fSize);

  if( menu ) {
    if( fShow ) menu->CheckEntry(menuid);
    else menu->UnCheckEntry(menuid);
  }

}


//---------------------------------------------------------------------------
void JSFEDProperty::ToggleShow()
{
  Char_t arg[128];
  sprintf(arg,"JSFEventDisplay.%s.Show",fName);
  if( fShow ) {
    gJSF->Env()->SetValue(arg,"0");
    fShow=kFALSE;
  }
  else {
    gJSF->Env()->SetValue(arg,"1");
    fShow=kTRUE;
  }

}

//---------------------------------------------------------------------------
JSFEventDisplay::JSFEventDisplay(JSFGUIFrame *gui)
{
   // A class to display event data
  
  fGenNeutral=new JSFEDProperty("GenNeutral",1,6);
  fGenCharged=new JSFEDProperty("GenCharged",1,7);
  fLTKCLTrack=new JSFEDProperty("LTKCLTrack",1,0);
  fCDCTrack  =new JSFEDProperty("CDCTrack",1,5);
  fVTXHit    =new JSFEDProperty("VTXHit",1,2,8,0.5);
  fEMCHit    =new JSFEDProperty("EMCHit",1,2);
  fHDCHit    =new JSFEDProperty("HDCHit",1,3);

  Update();

  fView=NULL;
  fCanvas=NULL;
  fGUIMain=gui;
  
  fRememberDisplayView=kFALSE;

  fWidgets  = new TList();
  fSignals  = new TList();
  fHelixes  = new TList();
  fGeometry = new TGeometry("jlc","jlc");

  gGeometryIsInitialized=kFALSE;

}


//---------------------------------------------------------------------------
void JSFEventDisplay::Update()
{

  if( !fRememberDisplayView ) {
    sscanf(gJSF->Env()->GetValue("JSFEventDisplay.ViewAngle",
				 "0.0 80.0 80.0"),"%lg %lg %lg",
	   &fViewAngle[0], &fViewAngle[1], &fViewAngle[2]);
  }

  fViewNo=gJSF->Env()->GetValue("JSFEventDisplay.ViewNo",1);
  sscanf(gJSF->Env()->GetValue("JSFEventDisplay.ViewRange1",
#if __ROOT_FULLVERSION__ >= 22500
       "-100.0 -100.0 -100.0 100.0 100.0 100.0"),"%lg %lg %lg %lg %lg %lg",
#else
       "-100.0 -100.0 -100.0 100.0 100.0 100.0"),"%g %g %g %g %g %g",
#endif
       &fViewRange[1][0], &fViewRange[1][1], &fViewRange[1][2],
       &fViewRange[1][3], &fViewRange[1][4], &fViewRange[1][5]);
  sscanf(gJSF->Env()->GetValue("JSFEventDisplay.ViewRange2",
#if __ROOT_FULLVERSION__ >= 22500
       "-300.0 -300.0 -300.0 300.0 300.0 300.0"),"%lg %lg %lg %lg %lg %lg",
#else
       "-300.0 -300.0 -300.0 300.0 300.0 300.0"),"%g %g %g %g %g %g",
#endif
       &fViewRange[1][0], &fViewRange[1][1], &fViewRange[1][2],
       &fViewRange[1][3], &fViewRange[1][4], &fViewRange[1][5]);
  sscanf(gJSF->Env()->GetValue("JSFEventDisplay.ViewRange3",
#if __ROOT_FULLVERSION__ >= 22500
       "-10.0 -10.0 -10.0 10.0 10.0 10.0"),"%lg %lg %lg %lg %lg %lg",
#else
       "-10.0 -10.0 -10.0 10.0 10.0 10.0"),"%g %g %g %g %g %g",
#endif
       &fViewRange[2][0], &fViewRange[2][1], &fViewRange[2][2],
       &fViewRange[2][3], &fViewRange[2][4], &fViewRange[2][5]);
  sscanf(gJSF->Env()->GetValue("JSFEventDisplay.CanvasSize","600 600"),"%d %d",
	 &fCanvasSize[0], &fCanvasSize[1]);
  fDrawAtNewEvent=gJSF->Env()->GetValue("JSFEventDisplay.DrawAtNewEvent",1);
  fDrawGeometry=gJSF->Env()->GetValue("JSFEventDisplay.DrawGeometry",1);
  fDisplayType=gJSF->Env()->GetValue("JSFEventDisplay.DisplayType",1);

  Int_t i;
  for(i=0;i<14;i++){ fLTKCLTrackColor[i]=1; }
  fLTKCLTrackColor[11]=
    gJSF->Env()->GetValue("JSFEventDisplay.LTKCLTrackColor.Electron",6);
  fLTKCLTrackColor[13]=
    gJSF->Env()->GetValue("JSFEventDisplay.LTKCLTrackColor.Muon",7);
  fLTKCLTrackColor[1]=
    gJSF->Env()->GetValue("JSFEventDisplay.LTKCLTrackColor.PureGamma",2);
  fLTKCLTrackColor[2]=
      gJSF->Env()->GetValue("JSFEventDisplay.LTKCLTrackColor.GammaInMixedEMC",46);
  fLTKCLTrackColor[3]=
    gJSF->Env()->GetValue("JSFEventDisplay.LTKCLTrackColor.NeutralHadron",3);
  fLTKCLTrackColor[4]=
      gJSF->Env()->GetValue("JSFEventDisplay.LTKCLTrackColor.HadronInMixedHDC",30);
  fLTKCLTrackColor[5]=
      gJSF->Env()->GetValue("JSFEventDisplay.LTKCLTrackColor.ChargedHadron",4);
  fLTKCLTrackColor[6]=
      gJSF->Env()->GetValue("JSFEventDisplay.LTKCLTrackColor.Unclassified",5);

  sscanf(gJSF->Env()->GetValue("JSFEventDisplay.Bfield","20.0"),"%g",&fBfield);
  sscanf(gJSF->Env()->GetValue("JSFEventDisplay.VTXViewRadius","10.0"),"%g",&fVTXViewRadius);
  
  sscanf(gJSF->Env()->GetValue("JSFEventDisplay.HDCScale","1.0"),"%g",&fHDCScale);
  sscanf(gJSF->Env()->GetValue("JSFEventDisplay.EMCScale","1.0"),"%g",&fEMCScale);

}


//---------------------------------------------------------------------------
JSFEventDisplay::~JSFEventDisplay()
{
  Clear();
  if( !fGenNeutral ) delete fGenNeutral;
  if( !fGenCharged ) delete fGenCharged;
  if( !fLTKCLTrack ) delete fLTKCLTrack;
  if( !fCDCTrack   ) delete fCDCTrack;
  if( !fVTXHit     ) delete fVTXHit;
  if( !fEMCHit     ) delete fEMCHit;
  if( !fHDCHit     ) delete fHDCHit;
  if( !fView       ) delete fView;
  if( !fCanvas     ) delete fCanvas;
  fWidgets->Delete();
  delete fWidgets;
  delete fSignals;
  delete fGeometry;
}

//---------------------------------------------------------------------------
void JSFEventDisplay::Clear()
{
  if( fHelixes )fHelixes->Delete();
  if ( fSignals ) fSignals->Delete();
  //  fWidgets->Delete();
}


//---------------------------------------------------------------------------
void JSFEventDisplay::DisplayEventData()
{
  // Display Event

  if( !fDrawAtNewEvent ) return;

  if( !fCanvas ) {
    fCanvas = new TCanvas("EventDisplay","EventDisplay",fCanvasSize[0],fCanvasSize[1]);
    fCanvasDirectory=gDirectory;
  } 
  else {
    if( fRememberDisplayView ) { RememberView(); }
    fCanvas->Clear("d");
    Clear();
  }

  TDirectory *olddir=gDirectory;
  if( fCanvasDirectory != gDirectory ) fCanvasDirectory->cd();

  //  TView *evview  = new TView(fViewNo);
  fView  = new TView(fViewNo);
  fSignals->Add(fView);
  Int_t ierr;
  fView->SetView(fViewAngle[0], fViewAngle[1], fViewAngle[2], ierr);
  fView->SetRange(&fViewRange[fDisplayType][0], &fViewRange[fDisplayType][3]);
  if( !simdst ) simdst=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST","quiet");

#if 1
  if (gJSF->Env()->GetValue("JSFGUI.SIMDSTConversion",1) == 0) {
     if( fCanvasDirectory != olddir ) olddir->cd();
     return;
  }
#endif

  if( fDrawGeometry ) DrawGeometry(fDisplayType);

  switch (fDisplayType) {
    case 0:
      if( fLTKCLTrack->fShow ) DisplayLTKCLMomentum();
      break;
    case 1:
      if( fEMCHit->fShow )     DisplayEMCHits();
      if( fHDCHit->fShow )     DisplayHDCHits();

      if( fCDCTrack->fShow )   DisplayCDCTracks();
      //      if( fLTKCLTrack->fShow )   DisplayLTKCLTracks();
      if( fVTXHit->fShow )     DisplayVTXHits();
      if( fGenCharged->fShow || fGenNeutral->fShow) 
      	DisplayGeneratorParticles();
      break;
    case 2:
      if( fCDCTrack->fShow )   DisplayCDCTracks();
      if( fVTXHit->fShow )     DisplayVTXHits();
      if( fGenCharged->fShow || fGenNeutral->fShow ) DisplayGeneratorParticles();
      break;
  }

  fCanvas->Update();
  if( fCanvasDirectory != olddir ) olddir->cd();


}


//---------------------------------------------------------------------------
void JSFEventDisplay::DrawGeometry(Int_t type)
{

  if( !gGeometryIsInitialized ) { InitializeGeometry(type); }

  switch (type) {
    case 0:
      gvVTX->SetVisibility(-1);
      gvAll->SetVisibility(-1);	
      if( gvMomentum ) {
        gvMomentum->SetVisibility(3);
        gvMomentum->Draw("same");
      }
      break;
    case 1:
      if( gvMomentum ) gvMomentum->SetVisibility(-1);
      gvVTX->SetVisibility(-1);
      gvAll->SetVisibility(3);	
      gvAll->Draw("same");

      break;
    case 2:
      if( gvMomentum ) gvMomentum->SetVisibility(-1);
      gvAll->SetVisibility(-1);	
      gvVTX->SetVisibility(3);
      gvVTX->Draw("same");
      break;
  }

}


//---------------------------------------------------------------------------
void JSFEventDisplay::InitializeGeometry(Int_t type)
{
  if( gJSF == 0 ) { printf("JSFSteer is not defined yet.\n"); return; }
  JSFQuickSimParam *p=(JSFQuickSimParam*)simdst->Param();

  TRotMatrix *rotx=new TRotMatrix("XDIR", "XDIR", 0.0, 0.0, 90.0, 90.0, 90.0, 0.0);
  TRotMatrix *roty=new TRotMatrix("YDIR", "YDIR", 90.0, 0.0, 0.0, 0.0, 90.0, 90.0);
  fWidgets->Add(rotx) ; fWidgets->Add(roty);

  //  View for full detector
  Double_t zshift=p->GetHDCZPlus()/2;
  TTUBE *tube, *beampipe, *pipe;
  TNode *vAll2, *vAll3, *vAll4, *vAll5;
  TTUBE *vtx0, *vtx1, *vtxaxis;
  TNode *vVTXin, *vVTXxa, *vVTXya;
  TSPHE *momframe;
  TTUBE *momaxis;
  TNode *vMomxa, *vMomya;
  Double_t zvtx;
  Int_t nl;

  if( fCanvas != 0 ) fCanvas->cd();
      tube=new TTUBE("EMC","EMC", "void", p->GetEMCRMaximum(), 
			    p->GetHDCRMaximum(), p->GetHDCZPlus());
      beampipe=new TTUBE("BEAMPIPE","BEAMPIPE", "void", 0.01, 0.1,zshift);
      //			p->GetVTXRadius(0),p->GetVTXRadius(0)+0.1,zshift);
      pipe=new TTUBE("PIPE", "PIPE", "void", 0.0,1.0, 10.0);
      fWidgets->Add(tube) ; fWidgets->Add(beampipe); fWidgets->Add(pipe);
      
      gvAll=new TNode("ALLVIEW","ALLVIEW","EMC");
      gvAll->cd();
      if( gJSF->Env()->GetValue("JSFGUI.ShowALLVIEWAxis",1) == 1 ) {
      vAll2=new TNode("ALLVIEW2","ALLVIEW2","BEAMPIPE",0,0,zshift,"");
      vAll3=new TNode("ALLVIEW3","ALLVIEW3","BEAMPIPE",0,0,-zshift,"");
      vAll4=new TNode("ALLVIEW4","ALLVIEW4","PIPE",10.0,0,2*zshift,"XDIR");
      vAll5=new TNode("ALLVIEW5","ALLVIEW5","PIPE",0.0, 10.0,2*zshift,"YDIR");
      fWidgets->Add(gvAll) ; fWidgets->Add(vAll2); fWidgets->Add(vAll3);
      fWidgets->Add(vAll4) ; fWidgets->Add(vAll5);
	
      vAll2->SetLineColor(2);
      vAll3->SetLineColor(4);
      vAll4->SetLineColor(7);
      vAll5->SetLineColor(6);
      }
    
  if( fCanvas != 0 ) fCanvas->cd();
      //  View for vertex view
      nl=p->GetVTXNLayer();
      vtx0=new TTUBE("VTX","VTX", "void",p->GetVTXRadius(nl-1),
		     p->GetVTXRadius(nl-1)+0.01, p->GetVTXZplus(nl-1));
      gvVTX=new TNode("VTXVIEW", "VTXVIEW","VTX");
      gvVTX->cd();


      vtx1=new TTUBE("VTXIN","VTXIN", "void",p->GetVTXRadius(1),
		     p->GetVTXRadius(1)+0.01, p->GetVTXZplus(1));
      vtxaxis=new TTUBE("VTXAXIS", "VTXAXIS", "void", 0.0,0.02, 0.5);
      fWidgets->Add(vtx0) ; fWidgets->Add(vtx1); fWidgets->Add(vtxaxis);

   
      if( gJSF->Env()->GetValue("JSFGUI.ShowVTXAxis",1) == 1 ) { 
      vVTXin=new TNode("VTXIN", "VTXIN", "VTXIN");
      zvtx= p->GetVTXZplus(1);
      vVTXxa=new TNode("VTXXAXIS","VTXXAXIS","VTXAXIS",0.30,0,zvtx,"XDIR");
      vVTXya=new TNode("VTXYAXIS","VTXYAXIS","VTXAXIS",0.0, 0.30,zvtx,"YDIR");
      vVTXin->SetLineColor(1);
      vVTXxa->SetLineColor(7);
      vVTXya->SetLineColor(6);
      fWidgets->Add(gvVTX) ; fWidgets->Add(vVTXxa); fWidgets->Add(vVTXya);
      }

  if( fCanvas != 0 ) fCanvas->cd();
  
  if( gJSF->Env()->GetValue("JSFGUI.MomentumView",1) == 1 ) {
      momframe=new TSPHE("MOMV", "MOMV","void", 0.99, 1.0, 0.0, 180.0, 0.0, 360.0);
      gvMomentum=new TNode("MOMENTUMVIEW","MOMENTUMVIEW","MOMV",0.0,0.0,0.0,"");
      gvMomentum->cd();
      momaxis=new TTUBE("MOMAXIS", "MOMAXIS", "void", 0.0,0.002, 0.1);
      fWidgets->Add(momframe);  fWidgets->Add(momaxis);
      vMomxa=new TNode("MOMXAXIS","MOMXAXIS","MOMAXIS",0.1,0,0.25,"XDIR");
      vMomya=new TNode("MOMYAXIS","MOMYAXIS","MOMAXIS",0.0, 0.1,0.25,"YDIR");
      gvMomentum->SetLineColor(43);
      vMomxa->SetLineColor(7);
      vMomya->SetLineColor(6);
      fWidgets->Add(momframe) ; fWidgets->Add(momaxis); fWidgets->Add(gvMomentum);
      fWidgets->Add(vMomxa) ; fWidgets->Add(vMomya);
   }

    gGeometryIsInitialized=kTRUE;
}


//---------------------------------------------------------------------------
void JSFEventDisplay::DisplayEMCHits()
{
  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();
  TClonesArray *emc=sdb->GetEMCHits(); 
  JSFQuickSimParam *par=simdst->Param();
  JSFCALGeoParam *geo=par->GetEMCGeom();

  Float_t dybarrel = 2*TMath::Pi()*par->GetEMCRMaximum()/par->GetEMCNPhi()/2;
  Float_t dzbarrel = 10;
  Float_t dxend    = ( par->GetEMCRMaximum() - par->GetEMCRMinimum() ) /
                       par->GetEMCNRadial()/2;
  
  Int_t i;
  Double_t todegree=180.0/TMath::Pi();

  for(i=0;i<sdb->GetNEMCHits();i++){
     JSFEMCHit *t=(JSFEMCHit*)emc->UncheckedAt(i);
     Int_t cel=t->GetCellID();
     Double_t energy=t->GetEnergy();
     if( energy <= 0 ) continue;
     JSFRThPhi hit=geo->HitPosition(cel);
     Double_t x,y,z;
     Double_t ang=0;
     Int_t izee=TMath::Abs(cel)/1000000;
     Double_t dy=dybarrel;  
     //     Double_t dx=TMath::Log10(energy)*fEMCScale;  
     Double_t dx=energy*fEMCScale;  
     Double_t dz=dzbarrel;
     if( izee == 0 ) { 
       x=(hit.r+dx)*TMath::Cos(hit.phi) ;
       y=(hit.r+dx)*TMath::Sin(hit.phi) ;
       z=hit.r/TMath::Tan(hit.th) ;
       ang = hit.phi*todegree; }
     else { 
       x=hit.r*TMath::Cos(hit.phi) ;
       y=hit.r*TMath::Sin(hit.phi) ;
       z=hit.r/TMath::Tan(hit.th) ;
       dz = dx;
       dy = hit.r*TMath::Pi()/par->GetEMCNPhi();
       dx = dxend;
       ang=hit.phi*todegree; 
       z += TMath::Sign(dz,z);
     }
     box3d=new TMarker3DBox(x, y, z, dx, dy, dz, 0, ang);
     box3d->SetLineColor(fEMCHit->fColor);
     box3d->SetLineWidth((Width_t)fEMCHit->fSize);
     box3d->Draw();
     fSignals->Add(box3d);
  } 




}


//---------------------------------------------------------------------------
void JSFEventDisplay::DisplayHDCHits()
{
  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();
  TClonesArray *hdc=sdb->GetHDCHits(); 
  JSFQuickSimParam *par=simdst->Param();
  JSFCALGeoParam *geo=par->GetHDCGeom();

  Float_t dybarrel = 2*TMath::Pi()*par->GetHDCRMaximum()/par->GetHDCNPhi()/2;
  Float_t dzbarrel = 10;
  Float_t dxend    = ( par->GetHDCRMaximum() - par->GetHDCRMinimum() ) /
                       par->GetHDCNRadial()/2;
  

  Int_t i;
  Double_t todegree=180.0/TMath::Pi();
  for(i=0;i<sdb->GetNHDCHits();i++){
     JSFHDCHit *t=(JSFHDCHit*)hdc->UncheckedAt(i);
     Int_t cel=t->GetCellID();
     Double_t energy=t->GetEnergy();
     if( energy <= 0 ) continue;
     JSFRThPhi hit=geo->HitPosition(cel);
     Double_t ang=0;
     Double_t x,y,z;
     Int_t izee=TMath::Abs(cel)/1000000;
     Double_t dy=dybarrel;  
     Double_t dx=energy*fHDCScale;  
     Double_t dz=dzbarrel;
     if( izee == 0 ) { 
       x=(hit.r+dx)*TMath::Cos(hit.phi) ;
       y=(hit.r+dx)*TMath::Sin(hit.phi) ;
       z=hit.r/TMath::Tan(hit.th) ;
       ang = hit.phi*todegree;  }
     else { 
       x=hit.r*TMath::Cos(hit.phi) ;
       y=hit.r*TMath::Sin(hit.phi) ;
       z=hit.r/TMath::Tan(hit.th) ;
       dz = dx;
       dx = dxend;
       dy = hit.r*TMath::Pi()/par->GetHDCNPhi();
       ang=hit.phi*todegree; 
       z += TMath::Sign(dz,z);
     }
     box3d=new TMarker3DBox(x, y, z, dx, dy, dz, 0, ang);
     box3d->SetLineColor(fHDCHit->fColor);
     box3d->SetLineWidth((Width_t)fHDCHit->fSize);
     box3d->Draw();
     fSignals->Add(box3d);
  } 

}

//---------------------------------------------------------------------------
void JSFEventDisplay::DisplayCDCTracks()
{

   JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();
   TClonesArray *cdc=sdb->GetCDCTracks(); 
   Int_t i;

   JSFQuickSimParam *par=(JSFQuickSimParam*)simdst->Param();
   for(i=0;i<sdb->GetNCDCTracks();i++){
     JSFCDCTrack *t=(JSFCDCTrack*)cdc->UncheckedAt(i);
     Double_t hp[3], hx[3];
     hx[0]=t->GetX() ; hx[1]=t->GetY(); hx[2]=t->GetZ();
     Double_t pt=TMath::Sqrt(t->GetPx()*t->GetPx()+t->GetPy()*t->GetPy());
     Double_t ptor= 333.5640952*10.0/fBfield;
     Double_t r   = ptor*pt;
     Double_t w   = pt/r;
     Double_t vt  = r*w;
     Double_t vz  = t->GetPz()*ptor*w;
     hp[0]=vt*t->GetPx()/pt;  
     hp[1]=vt*t->GetPy()/pt;  
     hp[2]=vz;
     w *= t->GetCharge();

     Double_t range[2]; 
     Double_t zlast;
     Double_t rcyl=fVTXViewRadius;
     Double_t zcyl=rcyl;
     if( fDisplayType == 1 ) {
       rcyl= par->GetHDCRMaximum();
       zcyl= par->GetHDCZPlus();
     }
     else {
       if( fVTXViewRadius > 5.0 ) {
	 rcyl = par->GetVTXRadius(par->GetVTXNLayer()-1)+0.2;
	 zcyl = par->GetVTXZplus(par->GetVTXNLayer()-1)+0.2;
       }
     }
     // printf(" rcyl=%g zcyl=%g \n",rcyl,zcyl);
     JSFHelicalTrack ht=t->GetHelicalTrack();
     ht.SetBfield(par->GetBField());
     Double_t phi0, phi1;
     ht.OriginToCylinder(rcyl, zcyl, phi0, phi1);
     // printf(" phi0=%g phi1=%g \n",phi0,phi1);

     JSF3DV end=ht.GetCoordinate(phi1);
     zlast = end.z;     if( zlast > 0.0 ) { range[0]=hx[2] ; range[1]=zlast ;}
     else {  range[1]=hx[2] ; range[0]=zlast; }

     JSFEDHelix *thelix=new JSFEDHelix();
     fHelixes->Add(thelix);
     thelix->SetLineColor(fCDCTrack->fColor);
     thelix->SetLineWidth((Width_t)(fCDCTrack->fSize));
     thelix->SetHelix(hx, hp, w, range, kHelixZ, 0);
     thelix->Draw();

   }
}

//_____________________________________________________
void  JSFEventDisplay::DisplayLTKCLMomentum()
{
   JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();
   TObjArray *lts=sdb->GetLTKCLTracks(); 
   Int_t i;

   //  First scan to get pmax;
   Float_t pmax=0;
   for(i=0;i<sdb->GetNLTKCLTracks();i++){
     JSFLTKCLTrack *lt=(JSFLTKCLTrack*)lts->UncheckedAt(i);
     if( pmax < lt->GetE() ) pmax=lt->GetE();
   }

   Float_t wscale=1.0/pmax;;

   Float_t xp[2], yp[2], zp[2];
   xp[0]=0.0 ; yp[0]=0.0 ; zp[0]=0.0;
   for(i=0;i<sdb->GetNLTKCLTracks();i++){
     JSFLTKCLTrack *lt=(JSFLTKCLTrack*)lts->UncheckedAt(i);
     xp[1]=lt->GetPx()*wscale;
     yp[1]=lt->GetPy()*wscale;
     zp[1]=lt->GetPz()*wscale;
     Int_t col=fLTKCLTrackColor[lt->GetType()];
     pl=new TPolyLine3D(2,xp,yp,zp);
     pl->SetLineColor(col);
     pl->Draw();
     fSignals->Add(pl);
   }
}

//_____________________________________________________
void  JSFEventDisplay::DisplayGeneratorParticles()
{
   
   JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();
   TClonesArray *gen=sdb->GetGeneratorParticles(); 
   JSFQuickSimParam *par=simdst->Param();

   Int_t i;
   //   printf(" There are %d Generator particles\n",sdb->GetNGeneratorParticles());
   Float_t xp[2],yp[2],zp[2];
   TVector pv(4); TVector xv(4); TVector xvd(4);
   JSFGeneratorParticle *gt;
   Int_t nogen=0;
   for(i=0;i<sdb->GetNGeneratorParticles();i++){
     gt=(JSFGeneratorParticle*)gen->UncheckedAt(i);
     xv=gt->GetXV();
     pv=gt->GetPV();
     Int_t ndau=gt->GetNDaughter();
     if( ndau > 0 ) continue;

     Double_t zcyl=par->GetHDCZPlus();
     Double_t rcyl=par->GetHDCRMaximum();
     if( fDisplayType == 2 ) {
         rcyl=par->GetVTXRadius(par->GetVTXNLayer()-1)+0.2;
	 zcyl=par->GetVTXZplus(par->GetVTXNLayer()-1)+0.2;
     }
     //  Omit tracks, whose star point outside and out going.
     if( ( TMath::Sqrt(xv(1)*xv(1)+xv(2)*xv(2)) > rcyl ||
	   TMath::Abs(xv(3)) > zcyl ) &&
	 xv(1)*pv(1)+xv(2)*pv(2)+xv(3)*pv(3) > 0.0 ) continue; 
     
     //printf(" xv=%g %g %g pv=%g %g %g\n",xv(1),xv(2),xv(3),pv(1),pv(2),pv(3));

     if( gt->GetCharge() == 0 ) { 
       if( fGenNeutral->fShow ) {
         xp[0]=xv(1); yp[0]=xv(2); zp[0]=xv(3);
	 JSF2DV xbeg(xp[0], yp[0]);
	 Double_t xsq = xbeg*xbeg;
	 if( xsq >= rcyl ) continue;  // Draw tracks only from inside.
	 JSF2DV pbeg(pv(1), pv(2));
	 Double_t vsq = pbeg*pbeg;
	 Double_t xpbeg = xbeg*pbeg;
	 Double_t vlen = ( xpbeg + TMath::Sqrt(xpbeg*xpbeg-vsq*(xsq-rcyl*rcyl) ))/vsq;
	 JSF2DV xend=xbeg + pbeg*vlen ;
	 Double_t zend=zp[0] + pv(3)*vlen;
	 if( TMath::Abs(zend) < zcyl ) { xp[1]=xend.x ; yp[1]=xend.y ; zp[1]=zend; }
	 else {
	   zp[1]= TMath::Sign(zcyl,(Double_t)pv(3));
	   vlen = ( zp[1]-zp[0] )/pv(3);
	   xp[1]=xp[0]+pv(1)*vlen;
	   yp[1]=yp[0]+pv(2)*vlen;
	 }
	 pl=new TPolyLine3D(2,xp,yp,zp);
         pl->SetLineColor(fGenNeutral->fColor);
         pl->Draw();
	 fSignals->Add(pl);
       }
     }
     else if( fGenCharged->fShow ) {
       Double_t hp[3], hx[3];
       hx[0]=xv(1) ; hx[1]=xv(2); hx[2]=xv(3);
       Double_t pt=gt->GetPt();
       Double_t ptor= 333.5640952*10.0/fBfield;
       Double_t r   = ptor*pt;
       Double_t w   = pt/r;
       Double_t vt  = r*w;
       Double_t vz  = gt->GetPz()*ptor*w;
       hp[0]=vt*gt->GetPx()/pt;  
       hp[1]=vt*gt->GetPy()/pt;  
       hp[2]=vz;
       Double_t range[2];
       w *=gt->GetCharge();
       nogen++;
       JSFHelicalTrack ht(pv(1), pv(2), pv(3), par->GetBField(), 
			  gt->GetCharge(), xv(1), xv(2), xv(3));
       Double_t phi0, phi1;
       ht.OriginToCylinder(rcyl, zcyl, phi0, phi1, 3, hx[0], hx[1]);
       JSF3DV tend=ht.GetCoordinate(phi1);

       JSFEDHelix *thelix=new JSFEDHelix();
       fHelixes->Add(thelix);

       EHelixRangeType iht=kHelixZ;
       if( TMath::Abs(gt->GetPz()) < 0.0001 ) { 
	 if( TMath::Abs(gt->GetPx()) < 0.0001 ) {
 	   if( gt->GetPy() > 0 ) { range[0]=hx[1];range[1]=tend.y; }
	   else { range[1]=hx[1];range[0]=tend.y; }
  	   iht=kHelixY;
	 }
	 else {
	   if( gt->GetPx() > 0 ) { range[0]=hx[0];range[1]=tend.x; }
	   else { range[1]=hx[0];range[0]=tend.x; }
	   iht=kHelixX;
	 }
       }
       else if( gt->GetPz() > 0 ) { range[0]=hx[2]; range[1]=tend.z; }
       else  { range[1]=hx[2]; range[0]=tend.z; }

       thelix->SetHelix(hx, hp, w, range, iht);

       if( fGenCharged->fColor < 0 ) {
         if( nogen < 50 ) { thelix->SetLineColor(nogen); }
         else { thelix->SetLineColor(50); }
       }
       else {  thelix->SetLineColor(fGenCharged->fColor); }

       thelix->Draw();
     }
   }

}

//_______________________________________________________________________
void  JSFEventDisplay::DisplayVTXHits()
{

   JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();
   TClonesArray *vtx=sdb->GetVTXHits(); 
   Int_t i;
   JSFVTXHit *vh;
   Int_t nvtx=sdb->GetNVTXHits();

   //   if( pm ) { delete pm;}
   pm=new TPolyMarker3D(nvtx);
   fSignals->Add(pm);
   for(i=0;i<nvtx;i++){
     vh=(JSFVTXHit*)vtx->UncheckedAt(i);
     Float_t r=vh->GetR();
     Float_t phi=vh->GetPhi();
     Float_t x=r*TMath::Cos(phi);
     Float_t y=r*TMath::Sin(phi); 
     Float_t z=vh->GetZ();
     //     pm->SetPoint(i,x,y,z);
     Float_t angle=phi/TMath::Pi()*180.0;
     box3d=new TMarker3DBox(x, y, z,0.002,0.002,0.002,0,angle);
     fSignals->Add(box3d);
     if( vh->GetGeneratorTrack() != 0 ){
       box3d->SetLineColor(fVTXHit->fColor);
       box3d->Draw();
       box3d=new TMarker3DBox(x, y, z,0.002,0.02,0.02,0,angle);
       fSignals->Add(box3d);
       box3d->SetLineColor(fVTXHit->fColor);
       box3d->Draw();
     }
     else {
       box3d->SetLineColor(fVTXHit->fColor+1);
       box3d->Draw();
     }
   }

}



