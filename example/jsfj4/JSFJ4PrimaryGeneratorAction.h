#ifndef __JSFJ4PrimaryGeneratorAction__
#define __JSFJ4PrimaryGeneratorAction__

///////////////////////////////////////////////////////////////////////
//
//  Interface of JSFGenerator to J4PrimaryGeneratorAction
//
//
//$Id$
//
///////////////////////////////////////////////////////////////////////

#include "J4PrimaryGeneratorAction.hh"

class G4Event;
class JSFJ4Buf;
  
// *******************************************************
class JSFJ4PrimaryGeneratorAction : public J4PrimaryGeneratorAction
{
 protected:
  G4bool fUseJupiterGenerator;
  JSFJ4Buf  *fEventBuf;              // 

  void LoadJSFGenerator(G4Event *anEvent);

 public:
  JSFJ4PrimaryGeneratorAction(): fUseJupiterGenerator(false), fEventBuf(0){}
  virtual ~JSFJ4PrimaryGeneratorAction(){}

  void SavePrimaries(G4Event *anEvent);
  void SetUseJupiterGenerator(G4bool yesno){ fUseJupiterGenerator=yesno; }
  void SetEventBuf(JSFJ4Buf *buf){ fEventBuf=buf; }
  JSFJ4Buf *GetEventBuf(){ return fEventBuf; }

  void GeneratePrimaries(G4Event *anEvent);
};

#endif










