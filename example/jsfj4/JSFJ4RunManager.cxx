///////////////////////////////////////////////////////////////////
//
//  Run Manager of Geant4 simulation for JSFJ4
//
//
//$Id$
//
//////////////////////////////////////////////////////////////////


#include "G4Timer.hh"
#include "G4ApplicationState.hh"
#include "G4StateManager.hh"
#include "G4UImanager.hh"
#include "G4Event.hh"

using namespace std;

#include "JSFJ4RunManager.h"
#include "JSFJ4PrimaryGeneratorAction.h"


//_____________________________________________________________________________
JSFJ4RunManager::JSFJ4RunManager() :
   fNSelectEvent(0)
{
}

//_____________________________________________________________________________
JSFJ4RunManager::~JSFJ4RunManager()
{
}

//_____________________________________________________________________________
void JSFJ4RunManager::RunInitialize()
{
  //
  bool cond = ConfirmBeamOnCondition();
  if( !cond ) {
    cout << "Jupiter run manager is not ready for run " ;
    cout << " at JSFG4::Initialize() " << endl;
    exit(0);
  }
 G4RunManager::RunInitialization();

 if( verboseLevel > 0 ) { timer->Start() ; }


}

//_____________________________________________________________________________
bool JSFJ4RunManager::SimulateOneEvent(int ievent)
{
  //

  G4int ie=ievent-1;
  fCurrentEvent = GenerateEvent(ie);

  eventManager->ProcessOneEvent(fCurrentEvent);
  
  AnalyzeEvent(fCurrentEvent);

  ((JSFJ4PrimaryGeneratorAction*)GetUserPrimaryGeneratorAction())->SavePrimaries(fCurrentEvent);

  StackPreviousEvent(fCurrentEvent);
  currentEvent = 0;
  if(runAborted) {
    return false;
  } 

  return true;

}


//_____________________________________________________________________________
void JSFJ4RunManager::RunTermination()
{

  if( verboseLevel > 0 ) {
    timer->Stop();
    G4cout << "JSFJ4 Run terminated." << G4endl;
    G4cout << "JSFJ4 Run Summary" << G4endl;
    //  if(runAborted)
    //    { G4cout << "  Run Aborted after " << i_event << " events processed." << G4endl; }
    //    else
    //    { G4cout << "  Number of events processed : " << n_event << G4endl; }
    G4cout << "  "  << *timer << G4endl;
  }

  G4RunManager::RunTermination();
}

