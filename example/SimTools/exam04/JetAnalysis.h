#ifndef __JetAnalysis__
#define __JetAnalysis__

///////////////////////////////////////////////////////////////////////
//
//  JetAnalysis
//
//  Sample program for Jet Analysis
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include "ANL4DVector.h"
#include "JSFModule.h"
#include "JSFZVTOP3.h"

//____________________________________________________________________
class JSFVertex : public TObject
{
 friend class JSFJet;
 protected:
  Int_t   fNTRK;     // Number of input tracks
  Int_t   fNVRT;     // Number of found vertex;
  Int_t   fNISO;     // Number of tracks left isolated
  Float_t fMSPTM;    // Pt corrected mass
  Float_t fPTM2; 
  Float_t fChisq;             // Chisq of vertex fit
  Int_t   fNSec;              // Number of secondary track
  Float_t fDecayLength;       // Decay length
  Float_t fSecondaryMass;     // 
  Float_t fSecondaryMomentum; //

 public:
  JSFVertex(): fNTRK(0), fNVRT(0), fNISO(0), fMSPTM(0.0), fPTM2(0.0), 
    fChisq(0.0), fNSec(0), fDecayLength(0.0), fSecondaryMass(0.0), fSecondaryMomentum(0){}

  virtual ~JSFVertex(){}

  void SetZVTOP3Info(JSFZVTOP3 &zvtop);

  inline Int_t GetNVRT(){ return fNVRT; }
  inline Int_t GetNTRK(){ return fNTRK; }
  inline Int_t GetNISO(){ return fNISO; }
  inline Float_t GetMSPTM(){return fMSPTM; }
  inline Float_t GetPTM2(){return fPTM2; }
  inline Float_t GetChisquare(){ return fChisq; }
  inline Int_t   GetNSec(){ return fNSec; }
  inline Float_t GetDecayLength(){ return fDecayLength; }
  inline Float_t GetSecondaryMass(){ return fSecondaryMass; }
  inline Float_t GetSecondaryMomentum(){ return fSecondaryMomentum; }
 
  inline Int_t NVRT(){ return fNVRT; }
  inline Int_t NTRK(){ return fNTRK; }
  inline Int_t NISO(){ return fNISO; }
  inline Float_t MSPTM(){return fMSPTM; }
  inline Float_t PTM2(){return fPTM2; }
  inline Float_t Chis(){ return fChisq; }
  inline Int_t   NS(){ return fNSec; }
  inline Float_t DL(){ return fDecayLength; }
  inline Float_t SM(){ return fSecondaryMass; }
  inline Float_t SP(){ return fSecondaryMomentum; }
 

  ClassDef(JSFVertex, 1) // Vertex property of jet

}; 

//_____________________________________________________________________
class JSFJet : public TObject
{
 protected:
  TLorentzVector fP;           // 4 momentum of jet
  Int_t       fNTracks;        // Number of tracks in the jet 
  Int_t       fNCharged;       // Number of charged tracks in the jet
  Int_t       fNOffIPTracks;   // Number of OffIP charged tracks 
  JSFVertex   fVertex;         // Vertex information
 public:
  JSFJet(Double_t t=0.0, Double_t x=0.0, Double_t y=0.0, Double_t z=0.0,
	 Int_t nt=0, Int_t nc=0, Int_t noff=0 ):
    fP(x,y,z,t), fNTracks(nt), fNCharged(nc), fNOffIPTracks(noff){}

  JSFJet(const JSFJet &jet):TObject(jet),fP(jet.fP),fNTracks(jet.fNTracks),fNCharged(jet.fNCharged),
    fNOffIPTracks(jet.fNOffIPTracks){}

  virtual ~JSFJet(){}

  inline void SetNTracks(Int_t ntracks){ fNTracks=ntracks; }
  inline void SetNCharged(Int_t ncharged){ fNCharged=ncharged; }
  inline void SetNOffIPTracks(Int_t noffiptracks){ fNOffIPTracks=noffiptracks; }
  void JetVertexing(const TObjArray *particles);

  inline TLorentzVector P(){ return fP; }
  inline ANL4DVector GetANL4D(){ return ANL4DVector(fP.E(), fP.Px(), fP.Py(), fP.Pz()); }
  inline JSFVertex   GetVertex(){ return fVertex; }
  inline Int_t NT(){ return fNTracks;}
  inline Int_t NC(){ return fNCharged;}
  inline Int_t NOff(){ return fNOffIPTracks;}
  inline JSFVertex  V(){ return fVertex; }

  ClassDef(JSFJet, 1)   // Standard JetData

};

//_____________________________________________________________________
class JetEvent : public TObject
{
protected:
  TClonesArray   *fJets;        //->

  Float_t         fEcm;
  Int_t           fNumJets;
  Float_t         fYmax;
  Float_t         fThrust;
  Int_t           fEventNumber;
  Int_t           fNTracks;
  Int_t           fNCharged;
  Float_t         fPtSum;
  Float_t         fPlSum;
  Float_t         fESum;
  Float_t         fMissingMass;

  static TClonesArray *fgJets;  //

public:
  JetEvent();
  virtual ~JetEvent();

  static void Reset(Option_t *opt="");
  void Clear(Option_t *opt="");

  inline void SetEcm(Double_t ecm){ fEcm=ecm; }
  inline void SetEventNumber(Int_t eventnumber){ fEventNumber=eventnumber; }
  inline void SetNTracks(Int_t ntracks){ fNTracks=ntracks; }
  inline void SetNCharged(Int_t ncharged){ fNCharged=ncharged; }
  inline void SetNumJets(Int_t numjets){ fNumJets=numjets; }
  inline void SetYmax(Double_t ymax){ fYmax=ymax;}
  inline void SetThrust(Double_t thrust){ fThrust=thrust;}
  inline void SetMissingMass(Double_t missingmass){ fMissingMass=missingmass; }
  inline void SetPtSum(Double_t ptsum){ fPtSum=ptsum; }
  inline void SetPlSum(Double_t plsum){ fPlSum=plsum; }
  inline void SetESum(Double_t esum){ fESum=esum; }

  inline TClonesArray *GetJets() const { return fJets;}
  inline JSFJet  *GetJet(Int_t i) const { return (JSFJet*)(fJets->UncheckedAt(i)); }

  inline TLorentzVector Pair(Int_t i, Int_t j) const {
    TLorentzVector p1=((JSFJet*)(fJets->UncheckedAt(i)))->P();
    TLorentzVector p2=((JSFJet*)(fJets->UncheckedAt(j)))->P();
    TLorentzVector psum=p1+p2;
    return psum;
  }

  //  inline JSFJet J(Int_t i){ return *(JSFJet*)fJets->UncheckedAt(i); }
  inline JSFJet *J(Int_t i){ return (JSFJet*)(fJets->UncheckedAt(i)); }
  inline Double_t Ecm(){ return fEcm; }
  inline Int_t EventNumber(){ return fEventNumber; }
  inline Int_t NT(){ return fNTracks; }
  inline Int_t NC(){ return fNCharged; }
  inline Int_t NJ(){ return fNumJets;}
  inline Double_t Ymax(){ return fYmax; }
  inline Double_t Thrust(){ return fThrust; }
  inline Double_t Pt(){ return fPtSum; }
  inline Double_t Pl(){ return fPlSum; }
  inline Double_t E(){ return  fESum; }
  inline Double_t MM(){ return  fMissingMass; }


  ClassDef(JetEvent,1)  // Jet event data buffer
};



// *******************************************************
class JetAnalysis : public JSFModule 
{
 protected:
  TTree *fJetTree;     // Jet Event information is saved
  JetEvent *fJetEvent; // 

  static Int_t    fForcedNJets;  // Event is clustered to this number of jets
  static Double_t fYcut;         // Initial Y Cut value
  static Int_t    fJetFinderAlgorithm;  // =0(JadeFinder), 1=(JadeEJetFinder),2=(DurhamJetFinder)
  static Double_t fVTXNSigCut;   // Cut value for Track to count NOffIPVertex Track.
  static Double_t fCutCosth;     // Cut on costh of track for vertexing
  static Int_t    fCutNDF;       // Cut on NDF of track for vertexing.


 public:
  JetAnalysis(const char *name="JetAnalysis", 
		  const char *title="JetAnalysis");
  virtual ~JetAnalysis();

  static Int_t    GetForcedNJets(){ return fForcedNJets;}
  static Double_t GetYcut(){ return fYcut; }
  static Int_t    GetJetFinderAlgorithm(){ return fJetFinderAlgorithm; }
  static Double_t GetVTXNSigCut(){ return fVTXNSigCut; }
  static Double_t GetCutCosth(){ return fCutCosth; }
  static Double_t GetCutNDF(){ return fCutNDF; }

  //  virtual Bool_t Initialize();
  //  virtual Bool_t BeginRun(Int_t runno=1);
  //  virtual Bool_t EndRun();
  //  virtual Bool_t Terminate();

  virtual Bool_t Process(Int_t ev=1);
  void JetClustering(TObjArray *particles);
  void ShapeAnalysis(TObjArray *particles);

  ClassDef(JetAnalysis, 1)  // JetAnalysis class

};

#endif










