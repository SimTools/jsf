#ifndef __JSFVTXAnalysis__
#define __JSFVTXAnalysis___

///////////////////////////////////////////////////////////////////////
//
//  Do topological vertex hunting using D.Jackson's algorithm
//
//$Id:
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>
#include "JSFModule.h"
#include "ANLJetFinder.h"
#include "JSFGeneratorVertex.h"
#include "JSFZVTOP3.h"

class JSFVTXAnalysisBuf : public JSFEventBuf 
{
public:

public:
  JSFVTXAnalysisBuf(const char *name="JSFVTXAnalysisBuf", 
	     const char *title="JSFVTXAnalysis buffer",
  	     JSFModule *module=NULL)
    : JSFEventBuf(name, title, module){}
  virtual ~JSFVTXAnalysisBuf(){}

  ClassDef(JSFVTXAnalysisBuf,1)  // JSFVTXAnalyis module
};



// *******************************************************
class JSFVTXAnalysis : public JSFModule {
 protected:
  TObjArray  fZVTOP3;  //! JSFZVTOP3Vertexing object
  ANLJadeEJetFinder fJclust; //! Jets of event
  JSFGeneratorVertexing fGenVTX; //!
  TObjArray  *fJetVTX;      //! Vertices corresponding to the jets.
  Int_t       fCDCFlag[400]; //! !=1 for K0S, =2 for Lambda, otherwise 0
  Int_t       fNCDC;         //! Number of CDC tracks
  void Delete();
public:
  JSFVTXAnalysis(const char *name="JSFVTXAnalysis", 
		  const char *title="JSFVTXAnalysis");
  virtual ~JSFVTXAnalysis();

  virtual Bool_t Initialize();
  virtual Bool_t Process(Int_t ev=1);
  
  Int_t JetAnalysis(TObjArray &particles, ANL4DVector &sum, 
		       ANLJadeEJetFinder *jets);

  void MatchJetWithVTX();
  void FindV0();
  
  inline ANLJetFinder *GetJClust(){ return &fJclust; }
  inline JSFZVTOP3    *GetVertexing(Int_t jetno){ 
    return (JSFZVTOP3*)fZVTOP3.UncheckedAt(jetno); }
  inline TObjArray    *GetVertexing(){ return &fZVTOP3; }

  ClassDef(JSFVTXAnalysis, 1)  // JSFVTXAnalysis class

};

#endif


