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

#include <TROOT.h>
#include <TCanvas.h>
#include <TView.h>
#include <TGXW.h>

#include <TGListBox.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>


class JSFEDProperty : public TObject {
public:
  Bool_t fShow;   // Display the object or not.    
  Int_t  fColor;  // color code.
  Int_t  fType;   // Marker type or line type.
  Float_t  fSize; // Marker size.
public:
  JSFEDProperty(){}
  JSFEDProperty(Char_t *name, Int_t show=1, Int_t col=1, 
                Int_t type=1, Float_t size=1);
  ~JSFEDProperty(){}
  ClassDef(JSFEDProperty, 1)   // A class to display event
};


// **************************************************************
class JSFEventDisplay : public TObject {
friend class JSFGUIFrame ;
private:

  TDirectory *fCanvasDirectory;
  TCanvas *fCanvas;
  TView   *fView;
  TList   *fWidgets;

  Int_t  fViewNo;
  Float_t fViewRange[6];
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

public:
  JSFEventDisplay();
  ~JSFEventDisplay();

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

  void GetViewRange(Float_t range[]){
      Int_t i; for(i=0;i<6;i++){ range[i]=fViewRange[i]; } }
  TCanvas *GetCanvas(){ return fCanvas; }
  Bool_t  DrawAtNewEvent(){ return fDrawAtNewEvent; }  

  ClassDef(JSFEventDisplay, 1)   // A class to display event

};

#endif
