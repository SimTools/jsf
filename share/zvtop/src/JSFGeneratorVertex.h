#ifndef __JSFGeneratorVertex__
#define __JSFGeneratorVertex__

///////////////////////////////////////////////////////////////////////
//
//  Store vertex information of generator's vertex
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include "JSFQuickSim.h"
#include "TLorentzVector.h"
#include "JSFVirtualVertex.h"
#include "JSFGeneratorParticle.h"

class JSFGeneratorVertexing;

enum EJSFGeneratorVertexType {kNormalVertex=0, kInitialParton=-1, kInitialBeam=-2};

// *********************************************************************
class JSFGeneratorVertex : public JSFVirtualVertex
{
 friend class JSFGeneratorVertexing;

 protected:
  Int_t fType;// Type of vertex
  JSFGeneratorParticle *fParentTrack; // Parent track of this vertex.
  TLorentzVector fLV;  // 4 momentum of vertex.
  JSFGeneratorVertex *fParentVertex;  // Pointer to the parent vertex
  JSFGeneratorVertexing *fVertexing;

  void SetDataMember(); // Set data member

 public:
  JSFGeneratorVertex(Int_t type=kNormalVertex, JSFGeneratorVertexing *vx=0);
  virtual ~JSFGeneratorVertex();

  inline Bool_t IsNormalVertex(){ return ( fType == kNormalVertex ? kTRUE : kFALSE ); }
  inline Bool_t IsInitialParton(){ return ( fType == kInitialParton ? kTRUE : kFALSE ); }
  inline Bool_t IsInitialBeam(){ return ( fType == kInitialBeam ? kTRUE : kFALSE ); }

  inline TLorentzVector GetLorentz(){ return fLV; }
  inline JSFGeneratorParticle *GetParent(){ return fParentTrack; }
  inline JSFGeneratorVertex *GetParentVertex(){ return fParentVertex;}
  inline JSFGeneratorVertexing *GetVertexing(){ return fVertexing; }
  inline GetType(){ return fType; }
  Double_t GetDecayLength(){ 
    return TMath::Sqrt(fPos[0]*fPos[0] + fPos[1]*fPos[1] + fPos[2]*fPos[2]);
  }

  void AddTrack(JSFGeneratorParticle *t);

  Int_t GetNGeneration();
  Int_t GetParentFlavourID();
  JSFGeneratorParticle *GetInitialParton();
  
  ClassDef(JSFGeneratorVertex,1)  // JSF Generator Vertex

};


// *********************************************************************
class JSFGeneratorVertexing : public JSFVirtualVertexing
{
 friend class JSFGeneratorVertex;
protected:
  Int_t fNgen;  //! number of generator particle
  TObjArray *fGen; //! TObjArray of input generator particles
  Int_t fKbgn[1000]; //!
  Int_t fKend[1000]; //!
public:
  JSFGeneratorVertexing();
  virtual ~JSFGeneratorVertexing();

  void MakeVertices(TObjArray *tracks=0);
  void MakeMCPART();
  void DebugPrint();

  JSFGeneratorVertex *GetGeneratorVertex(const ANL4DVector jet);
  JSFGeneratorVertex *GetGeneratorVertex2(const ANL4DVector jet);

  JSFCDCTrack *GetCDCTrack(TClonesArray *tracks, Int_t genid);

  void Clear();

  void PHHEAVY(Int_t &nbhad, Int_t bhind[8], 
	       Int_t &nbchad, Int_t bcind[8],
	       Int_t bcpar[8], Int_t &nchad, Int_t chind[8]);
  void PHUDLT(Int_t imc, Int_t &multa, Int_t &multv);
  void PHTMTYP(Int_t imc, Int_t &ipmc, Int_t &ipflav, Int_t &ipvtx);



  ClassDef(JSFGeneratorVertexing,1)  // JSF Vertexing
};

#endif


