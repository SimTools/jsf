#ifndef __JSFVTXHit__
#define __JSFVTXHit__

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// JSFVTXHit                                                            //
//                                                                      //
// Vertex Detector Hit information class.                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"
#include "TMath.h"
#include "TVector.h"


const Int_t kMaxVTXHits=500;

class JSFVTXHit : public TObject {

protected:
  Double_t   fR;      // Radius (cm) of vertex hits.
  Double_t   fPhi;    // Azimuthal angle (radian) of vertex hits.
  Double_t   fZ;  // Polar angle (radian) of vertex hits.
  Double_t   fDphi; // Error of fTheta 
  Double_t   fDz;   // Error of fPhi
  Int_t      fLayer;  // Layer number of the hit. ( 1 to n )
  Int_t      fLinkedTrack; // Track number ( 1 to n ) associated with this hit (-1 when no hit)
  Int_t      fGenTrack ;   // GeneratorParticle number ( 1 to n ) who creates the hit.

public:
  JSFVTXHit(){}
  virtual ~JSFVTXHit(){}

  JSFVTXHit(Double_t r, Double_t phi, Double_t z, Double_t dphi,
	    Double_t dz, Int_t layer, Int_t trackid, Int_t gentrack){
            fR=r ; fPhi=phi; fZ=z ;  fDphi=dphi, fDz=dz ;
	    fLayer=layer ; fLinkedTrack=trackid; fGenTrack=gentrack; }

  Double_t GetR(){return fR;}
  Double_t GetPhi(){ return fPhi;}
  Double_t GetZ(){ return fZ; }
  Double_t GetDphi(){ return fDphi;}
  Double_t GetDz(){ return fDz;}
  Int_t    GetLayerNumber(){ return fLayer;}
  Int_t    GetLinkedTrack(){ return fLinkedTrack;}
  Int_t    GetGeneratorTrack(){ return fGenTrack; }

  void SetLinkedTrack(Int_t it){ fLinkedTrack=it; }

  ClassDef(JSFVTXHit,1)  //A JSFVTXHit class
};

#endif






