//*LastUpdate:  jsf-1-13 25-January-2000  by Akiya Miyamoto
//*LastUpdate:  jsf-1-5 22-Feburary-1999  by Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  22/02/1999

//////////////////////////////////////////////////////////////////
//
//  JSFEventDisplay
//
//  Event Display
//  
//$Id$
//  
//////////////////////////////////////////////////////////////////

#ifndef __JSFEventDisplay__
#define __JSFEventDisplay__

#include "JSFConfig.h"

#include <TROOT.h>
// #include <TCanvas.h>
#include <TView.h>
#if __ROOT_VERSION__ >= 2 && __ROOT_MINORVERSION__ >= 23
#include <TVirtualX.h>
#else
#include <TGXW.h>
#endif

#include <TCanvas.h>
#include <TGeometry.h>
#include <TGListBox.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>

#include <TClonesArray.h>
#include <THelix.h>


class JSFEDHelix : public THelix {
 public:
  JSFEDHelix();
  virtual ~JSFEDHelix();

  ClassDef(JSFEDHelix,1)  // A THelix Class for JSF Event display
};

class JSFEDProperty : public TObject {
public:
  Char_t fName[24]; // Name of detector.
  Bool_t fShow;   // Display the object or not.    
  Int_t  fColor;  // color code.
  Int_t  fType;   // Marker type or line type.
  Float_t  fSize; // Marker size./ line size.
public:
  JSFEDProperty(){}
  JSFEDProperty(Char_t *name, Int_t show=1, Int_t col=1, 
                Int_t type=1, Float_t size=1);
  virtual ~JSFEDProperty(){}
  void Update(TGPopupMenu *menu=NULL, Int_t menuid=0);
  void ToggleShow();
  ClassDef(JSFEDProperty, 1)   // A class to display event
};


class JSFGUIFrame;

// **************************************************************
class JSFEventDisplay : public TObject {
friend class JSFGUIFrame ;
protected:

  TDirectory *fCanvasDirectory;
  TCanvas *fCanvas;
  TView   *fView;
  TList   *fWidgets;
  TList   *fSignals, *fHelixes;
  TGeometry *fGeometry;

  Int_t   fViewNo;
  Float_t fViewAngle[3];
#if __ROOT_VERSION__ >= 2 && __ROOT_MINORVERSION__ >= 25
  Double_t fViewRange[3][6];
#else
  Float_t fViewRange[3][6];
#endif
  Int_t   fCanvasSize[2];
  Bool_t  fDrawAtNewEvent;
  Int_t   fDisplayType; 
  Int_t   fLTKCLTrackColor[14];
  Float_t fBfield;  // Magnetic field in kgauss
  Bool_t  fDrawGeometry;
  Float_t fVTXViewRadius;
  
  Float_t   fHDCScale; // Log10(Energy(GeV))*fHDCScale is box hight.
  Float_t   fEMCScale; // Log10(Energy(GeV))*fEMCScale is box hight.

  JSFEDProperty *fGenNeutral; //!
  JSFEDProperty *fGenCharged; //!
  JSFEDProperty *fLTKCLTrack;  //!
  JSFEDProperty *fCDCTrack;   //!
  JSFEDProperty *fVTXHit;     //!
  JSFEDProperty *fEMCHit;     //!
  JSFEDProperty *fHDCHit;     //!
  JSFGUIFrame   *fGUIMain;    //!

public:
  JSFEventDisplay(JSFGUIFrame *gui=NULL);
  virtual ~JSFEventDisplay();

  void Clear();
  void DisplayEventData();
  void DisplayLTKCLMomentum();

  void DrawGeometry(Int_t type);
  void InitializeGeometry();

  void DisplayGeneratorParticles();
//  void DisplayLTKCLTracks();
  void DisplayCDCTracks();
  void DisplayVTXHits();
  void DisplayEMCHits();
  void DisplayHDCHits();

  void DisplayJIMEMCHits();
  void DisplayJIMHDCHits();

  //  void GetViewRange(Float_t range[]){
  //    Int_t i; for(i=0;i<6;i++){ range[i]=fViewRange[i]; } }
  TCanvas *GetCanvas(){ return fCanvas; }
  Bool_t  DrawAtNewEvent(){ return fDrawAtNewEvent; }  

  void Update();
  void SetGUIMain(JSFGUIFrame *gui){ fGUIMain = gui ; }
  JSFGUIFrame *GetGUIMain(){ return fGUIMain; }

  ClassDef(JSFEventDisplay, 1)   // A class to display event

};

#endif
