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
#include "G4VUserPrimaryParticleInformation.hh"

class G4Event;
class JSFJ4Buf;
class JSFGeneratorParticle;
  

class JSFJ4PrimaryParticleInformation : public G4VUserPrimaryParticleInformation 
{
  protected:
     int fGeneratorIndex;
     JSFGeneratorParticle *fGeneratorParticle;
     
  public:
     JSFJ4PrimaryParticleInformation(int index=-1, JSFGeneratorParticle *gp=0){
         fGeneratorIndex=index; 
         fGeneratorParticle=gp; 
     }
     ~JSFJ4PrimaryParticleInformation(){;}
     
  public:
      void Print() const {}
      int GetIndex(){ return fGeneratorIndex;}
      JSFGeneratorParticle *GetGeneratorParticle(){ return fGeneratorParticle; }
      void SetIndex(int index=-1) {
          fGeneratorIndex=index; 
      }
      void SetParticle(JSFGeneratorParticle *gp) {
          fGeneratorParticle = gp ;
      }

};
 



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










