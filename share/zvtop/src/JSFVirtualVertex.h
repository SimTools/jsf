#ifndef __JSFVirtualVertex__
#define __JSFVirtualVertex__

///////////////////////////////////////////////////////////////////////
//
//  Virtual Vertex class
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

#include "TObjArray.h"
#include "JSFGeneratorParticle.h"
#include "ANL4DVector.h"
#include "TVector3.h"


// *********************************************************
class JSFVirtualVertex : public TObject
{
 protected:
  TObjArray  *fTracks;// Pointer to track obj array belong to this vertex.
  Float_t fMass;    // vertex mass
  Float_t fCharge;  // sum of charge
  Int_t   fNcharged; // number of charged track.
  Float_t fPos[3]; // vertex position.
 public:
  JSFVirtualVertex();
  virtual ~JSFVirtualVertex();

  virtual Float_t GetMass(){ return fMass; }     // Vertex mass
  virtual Float_t GetCharge(){ return fCharge; }   // Get Virtex charge
  virtual Int_t GetNcharged(){return fNcharged; } // Get number of charged track
  virtual void AddTrack(TObject *t=0){}
  //  virtual void SetTrack(TObjArray *t, Bool_t reset=kFALSE);
  virtual void MakeVertex();
  virtual TObjArray *GetTracks(){ return fTracks; }
  virtual TVector3 GetVertexPosition(){  return TVector3(fPos[0], fPos[1], fPos[2]);}

  virtual Double_t GetProbability(){return 0.0;}
  virtual Double_t GetChisquare(){return 1.E30;}
  /*
  virtual Bool_t FitVertex();
  virtual void SetVertex();
  virtual Bool_t HasParent();
  virtual TVector3 &GetPosition();
  */

  ClassDef(JSFVirtualVertex, 1)  // Vertex class

};

// *********************************************************
class JSFVirtualVertexing : public TObjArray
{
 public:
  JSFVirtualVertexing(){}
  virtual ~JSFVirtualVertexing(){}

  virtual void  MakeVertices(TObjArray *t=0)=0;
  virtual Int_t GetNVertices(){ return GetEntries(); }
  
  ClassDef(JSFVirtualVertexing, 1)  // Vertex class

};


#endif
