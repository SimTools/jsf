#ifndef __JSFVertexing__
#define __JSFVertexing__

//********************************
//*
//*  General purpose vertexing class.
//*  Finds and fits vertex from JSFHelicalTrack objects.
//*  Currently vertexing of two oposite track is supported.
//*
//*  Example 1) V0 vertex fitting.
//*  TObjArray listOfVertex;
//*  JSFVertexing vtx(2);  // Prepare buffer for two tracks.
//*  for( track 1 ) {
//*    vtx.SetTrack(1, track1);
//*    for( track 2 ) {
//*      vtx.SetTrack(2, track2);
//*      if( vtx.FindVertex() < Your cut1 ) {
//*        if( vtx.FitVertex() < Your cut2 ) {
//*          ... Good pair is found
//*          listOfVertex.Add(new JSFVertexing(vtx));
//*        }
//*      }
//*    } // End of track 2 loop.
//*  }   // End of track 1 loop.
//*
//*
//********************************

#include "TVector2.h"
#include "TVector3.h"

#ifndef __JSFHelicalTrack__
#include "JSFHelicalTrack.h"
#endif

class JSFVertexing : public TObject 
{
 protected:
  Double_t     fV[3]; // Vertex coordinate (cm)
  Double_t  fQuality; // Quality of vertex (Z difference in cm)
  TClonesArray   *fT; // Helical Track to form vertex
  Double_t    fChisq; // Chisq of vertex fit
  Int_t     fEntries; // Number of entries
  Double_t  fPairEpsilon; // epsilon value used for pair finder.

 public:
  JSFVertexing();
  JSFVertexing(Int_t ntracks);
  virtual ~JSFVertexing();

  void Clear();
  void SetTrack(Int_t id, JSFHelicalTrack h);
  
  Double_t FindVertex();
  Double_t FindV0();

  Double_t FitVertex();

  void UCRCLX(Double_t r1, TVector2 xc1, Double_t r2, TVector2 xc2, Double_t eps, 
    	      TVector2 &x1, TVector2 &x2, Int_t &isect);

  Double_t GetQuality(){ return fQuality; }
  Int_t GetEntries(){ return fEntries; }
  Double_t GetChisq(){ return fChisq;}
  void GetVertex(Double_t v[]){ v[0]=fV[0]; v[1]=fV[1]; v[2]=fV[2]; }
  void GetVertex(TVector3 &v){ TVector3 w(fV[0], fV[1], fV[2]); v=w; }

  void SetPairEpsilon(Double_t eps){ fPairEpsilon=eps;}
  Double_t GetPairEpsilon(){ return fPairEpsilon;}

  ClassDef(JSFVertexing, 1) // Do Vertexing of helix tracks.

};

#endif
