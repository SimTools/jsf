#ifndef __JSFZVTOP3__
#define __JSFZVTOP3__

///////////////////////////////////////////////////////////////////////
//
//  Do topological vertex hunting using D.Jackson's algorithm
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include "ANLJetFinder.h"
#include "JSFVirtualVertex.h"
#include "JSFSIMDST.h"
#include "JSFQuickSimParam.h"


// *********************************************************************
class JSFZVTOP3Track : public TObject
{
 protected:
  Int_t fZID; // ID of track in ZXTRKS bank.
  Int_t fVNO; // Vertex number where this track belongs.
  Float_t fXIST; // Chisqare contribution of the track to the vertex
  Float_t fXISG[7]; // Chisqare contribution of the track to the vertex
  Float_t fPRBG[7]; // vertex prob. if this track added to VRTS
  Float_t fTRDI; // Distance in cm from the track to the IP
  Float_t fLODI; // distance from IP along 2.5 cm line to TRDI
 public:
  JSFZVTOP3Track(Int_t id=0, Int_t jetno=0);
  virtual ~JSFZVTOP3Track(){}

  ClassDef(JSFZVTOP3Track, 1) // JSFZVTOP3 track

};


// *********************************************************************
class JSFZVTOP3Vertex : public JSFVirtualVertex
{
 protected:
  Float_t fVSIG;  // Vertex significance func. value
  Float_t fXISQ; // Chisq of vertex fit
  Float_t fPRBV;  // Probabilifty of vertex fit.
  Float_t fALPHA; // Angular displacement of vertex
  
  Float_t fDPOS[6]; // Error matrix of vertex position.

 public:
  JSFZVTOP3Vertex(Int_t vno=0,Int_t jetno=0);
  virtual ~JSFZVTOP3Vertex();

  inline Double_t GetProbability(){ return fPRBV; }
  inline Double_t GetChisquare(){ return fXISQ; }
  inline Double_t GetVertexSignificance(){ return fVSIG; }
    
  ClassDef(JSFZVTOP3Vertex,1)  // ZVTOP3 Vertex information.

};


// *********************************************************************
class JSFZVTOP3 : public JSFVirtualVertexing
{
protected:
  Int_t fNTRK; // number of input tracks.
  Int_t fNVRT; // number of found vertices.
  Int_t fNISO; // number of tracks left isolated.
  Float_t fPTKS[4]; // X,Y,Z,magnitude ot summed track momenta
  Float_t fIPX[3];  // XYZ location of IP
  Float_t fAXI[3];  // Ghost axis direction
  Float_t fGWID;    // Fitted ghost track gaussian width /um

  Float_t fMSPTM;   // 
  Float_t fMSPT;
  Float_t fPTM2;

  Float_t fChisq; //
  Float_t fProb;  // 
  Float_t fProb2; // Vfit probability of first fit.
  Float_t fProbo ;// Probability of original vertex
  Int_t   fNseco ;
  Int_t   fNsec; //
  Float_t fDecayLength; //
  Float_t fDecayLength0; //
  Float_t fSecondaryMass; //
  Float_t fSecondaryMomentum; //

  static JSFCDCTrack **fTrackPointers;

public:
  JSFZVTOP3();
  virtual ~JSFZVTOP3();

  void MakeVertices(TObjArray *tracks=0);
  void CalculateMSPTM();
  void DebugPrint();

  static void ZXSWIM(Int_t *mode, Float_t *ok, Float_t vpos[], Int_t *id);
  static Int_t ZXFIT(Int_t *mode, Int_t *jj, Int_t vid[], Float_t vpos[], 
     Float_t vpossg[], Float_t *chisq, Float_t xvtx[],Float_t xvtxsg[], 
     Float_t chisqtk[], Float_t pxyz[][3], Int_t *ier);
  static Int_t GUFLD(Float_t pos[], Float_t b0[]);
  
  inline Float_t GetMSPTM(){return fMSPTM; }
  inline Float_t GetMSPT(){return fMSPT; }
  inline Float_t GetPTM2(){return fPTM2; }
  inline Int_t GetNVRT(){ return fNVRT; }
  inline Int_t GetNTRK(){ return fNTRK; }

  inline Float_t GetChisquare(){ return fChisq; }
  inline Float_t GetProbability(){ return fProb; }
  inline Float_t GetProbability2(){ return fProb2; }
  inline Int_t   GetNsec(){ return fNsec; }
  inline Float_t GetDecayLength(){ return fDecayLength; }
  inline Float_t GetDecayLength0(){ return fDecayLength0; }
  inline Float_t GetSecondaryMass(){ return fSecondaryMass; }
  inline Float_t GetSecondaryMomentum(){ return fSecondaryMomentum; }

  inline Float_t GetProbabilityOriginalVertex(){ return fProbo; }
  inline Int_t   GetNsecOriginalVertex(){ return fNseco; }

  ClassDef(JSFZVTOP3,1)  // JSFZVTOP3 event buffer
};

#endif


