#ifndef __JSFJ4RunManager__
#define __JSFJ4RunManager__

// *******************************************************************
// Interface to Geant Run manager
//
// $Id$
// *******************************************************************

#include "G4RunManager.hh"

class JSFJ4;
class G4StateManager;

class JSFJ4RunManager : public G4RunManager 
{
  friend class JSFJ4;
 protected:
  G4int           fNSelectEvent;  //!
  G4Event        *fCurrentEvent;  //!

 public:
  JSFJ4RunManager();
  virtual ~JSFJ4RunManager();

  void RunInitialize();
  bool SimulateOneEvent(int nev);
  void RunTermination();

  void SetNSelectEvent(G4int i){ fNSelectEvent=i; }

  inline G4Event *GetCurrentEvent(){ return fCurrentEvent; }

};

#endif

