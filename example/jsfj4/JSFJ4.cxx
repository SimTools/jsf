///////////////////////////////////////////////////////////////////
//
//  Geant4 execution in JSF framework
//
//
//$Id$
//
//////////////////////////////////////////////////////////////////


#include <strstream.h>


#include "JSFJ4RunManager.h"
#include "G4UImanager.hh"
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4VisManager.hh"
#include "G4VVisManager.hh"

#include "G4UIXm.hh"


#include "J4DetectorConstruction.hh"
#include "J4PhysicsList.hh"
#include "J4PrimaryGeneratorAction.hh"
#include "J4RunAction.hh"
#include "J4EventAction.hh"
#include "J4TrackingAction.hh"
#include "TBookKeeper.hh"

#ifdef G4VIS_USE
#include "J4VisManager.hh"
#endif

//#include "CLHEP/Random/Random.h"

#include "J4IR.hh"
#include "J4BD.hh"
#include "J4VTX.hh"
#include "J4IT.hh"
#include "J4CDC.hh"
#include "J4CAL.hh"

TBookKeeper* TBookKeeper::fgBookKeeper = new TBookKeeper();

#include "JSFSteer.h"
#include "JSFJ4.h"

#include "JSFJ4TOutput.h"
#include "JSFJ4VTXHit.h"
#include "J4VTXPixelHit.hh"
#include "JSFJ4CDCHit.h"
#include "J4CDCDriftRegionHit.hh"
#include "JSFJ4PrimaryGeneratorAction.h"

ClassImp(JSFJ4)
ClassImp(JSFJ4Buf)
ClassImp(JSFJ4BufferMap)

#include <hash_map>

typedef hash_map<char*, J4Object*> JSFJ4Detectors_t;  // Used to swich on/off detectors
typedef hash_map<char*, J4Output*> JSFJ4Outputs_t;    // used to manage output
JSFJ4Detectors_t gJSFJ4Detectors;
JSFJ4Outputs_t gJSFJ4Outputs;

//_____________________________________________________________________________
JSFJ4::JSFJ4(const char *name, const char *title)
  : JSFModule(name,title), fpDetector(0)
{

  fEventBuf=new JSFJ4Buf("JSFJ4Buf", "JSFJ4 event data", this);

  if( IsWritable() ) { BuildJupiter() ; }

}

//_____________________________________________________________________________
JSFJ4::~JSFJ4()
{
}

//_____________________________________________________________________________
void JSFJ4::BuildJupiter()
{
  // Construct the default run manager
  fRunManager = new JSFJ4RunManager();
  fUImanager  = G4UImanager::GetUIpointer();

  //*--------------------------------------------
  //* Install detector components...
  //*--------------------------------------------

  fpDetector = new J4DetectorConstruction;
  JSFJ4Buf *buf=(JSFJ4Buf*)EventBuf();

  //* beam line

  if( gJSF->Env()->GetValue("JSFJ4.HasIR",1) == 1 ) {
    J4IR *pIR = new J4IR();
    pIR->SetMother(fpDetector->GetExpHall());
    fpDetector->AddComponent(pIR);
    gJSFJ4Detectors["IR"]=(J4Object*)pIR;
  }

  if( gJSF->Env()->GetValue("JSFJ4.HasBD",1) == 1 ) {
    J4BD *pBD = new J4BD();
    pBD->SetMother(fpDetector->GetExpHall());
    fpDetector->AddComponent(pBD);
    gJSFJ4Detectors["BD"]=(J4Object*)pBD;
  }

  //* vtx
  if( gJSF->Env()->GetValue("JSFJ4.HasVTX",1) == 1 ) {
    J4VTX *pVTX = new J4VTX();
    pVTX->SetMother(fpDetector->GetExpHall());
    fpDetector->AddComponent(pVTX);

    TObjArray *vtxhits=buf->AddComponent("VTXHits");
    gJSFJ4Detectors["VTX"]=(J4Object*)pVTX;
    gJSFJ4Outputs["VTX"]=new JSFJ4TOutput<JSFJ4VTXHit,J4VTXPixelHit>(vtxhits);
  }

  //* intermediate tracker

  if( gJSF->Env()->GetValue("JSFJ4.HasIT",1) == 1 ) {
    J4IT *pIT = new J4IT();
    pIT->SetMother(fpDetector->GetExpHall());
    fpDetector->AddComponent(pIT);
    gJSFJ4Detectors["IT"]=(J4Object*)pIT;
  }

  //* cdc
  if( gJSF->Env()->GetValue("JSFJ4.HasCDC",1) == 1 ) {
    J4CDC *pCDC = new J4CDC();
    pCDC->SetMother(fpDetector->GetExpHall());
    fpDetector->AddComponent(pCDC);

    TObjArray *cdchits=buf->AddComponent("CDCHits");
    gJSFJ4Detectors["CDC"]=(J4Object*)pCDC;
    gJSFJ4Outputs["CDC"]=new JSFJ4TOutput<JSFJ4CDCHit,J4CDCDriftRegionHit>(cdchits);
  }

  //* calorimeter
  if( gJSF->Env()->GetValue("JSFJ4.HasCAL",1) == 1 ) {
    J4CAL *pCAL = new J4CAL();
    pCAL->SetMother(fpDetector->GetExpHall());
    fpDetector->AddComponent(pCAL);
    gJSFJ4Detectors["CAL"]=(J4Object*)pCAL;
  }

  //*--------------------------------------------
  //* Installation of detectors end
  //*--------------------------------------------

  fRunManager-> SetUserInitialization(fpDetector);

  // particles and physics processes
  J4PhysicsList *physicsList=new J4PhysicsList();
  fRunManager->SetUserInitialization(physicsList);
  //  fPhysicsListLevel=gJSF->Env()->GetValue("JSFJ4.PhysicsListLevel",0);
  //  physicsList->SetLevel(fPhysicsListLevel);

 //---------------------------------------------
  // set mandatory user action class...

  //* primary generator
  JSFJ4PrimaryGeneratorAction *primaryGenerator=new JSFJ4PrimaryGeneratorAction();
  primaryGenerator->SetEventBuf(buf);
  buf->AddComponent("PrimaryVertices");
  
  fUseJupiterGenerator=gJSF->Env()->GetValue("JSFJ4.UseJupiterGenerator",1);
  primaryGenerator->SetUseJupiterGenerator(fUseJupiterGenerator);
  fRunManager->SetUserAction(primaryGenerator);

  //* user action classes... (optional)
  fRunManager-> SetUserAction(new J4RunAction);
  fRunManager-> SetUserAction(new J4EventAction);
  fRunManager-> SetUserAction(new J4TrackingAction);

  fVisManager=0;
#ifdef G4VIS_USE
  // initialize visualization package
  fVisManager= new J4VisManager;
  fVisManager-> Initialize();
  G4cout << endl;
#endif

  istrstream sin(gJSF->Env()->GetValue("JSFJ4.MacroFile","run.mac"));
  fInitMacro.ReadLine(sin);


}

//_____________________________________________________________________________
Bool_t JSFJ4::Initialize()
{
  if( !IsWritable() ) { return kTRUE; }

  fNcall=0;
  fNgood=0;

  // Initialize G4 kernel
  fRunManager->Initialize();
  if( fVisManager ) fVisManager->Initialize();


  //*--------------------------------------------
  //* SwitchOn/Off your detector...
  //*--------------------------------------------
  //* If you wants call SwitchOn/Off recursively,
  //* call virtual function with resolver, as
  //* "J4VDetectorComponent::SwitchOn()".
  //* SwitchOn/(Off() requires option flag
  //* "recursive" or other strings, however,
  //* default value is set as "recursive".

  JSFJ4Detectors_t::iterator  idet;
  idet=gJSFJ4Detectors.find("VTX");
  if( idet != gJSFJ4Detectors.end() ) {
  	((J4VTX*)((*idet).second))->J4VDetectorComponent::SwitchOn();
  }

  idet=gJSFJ4Detectors.find("CDC");
  if( idet != gJSFJ4Detectors.end() ) {
    ((J4CDC*)((*idet).second))->J4VDetectorComponent::SwitchOn();

    // Then, SwitchOff only SenseWires.
    for (G4int i=0; i<10; i++) {
      for (G4int j=0; j<5; j++) {
        strstream str;
        str << "ExpName:CDC:Layer0" << i << ":Cell:DriftRegion"
            << j << ":SenseWire" ;
        G4String name;
        str >> name;
        J4VDetectorComponent::GetComponentPtrByName(name)->SwitchOff();
      }
    }
  }


  G4String command = "/control/execute ";
  G4String filename = fInitMacro.Data();

  fUImanager->ApplyCommand(command+filename);

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFJ4::Terminate()
{
  if( fVisManager )  delete fVisManager;
  if( fRunManager )  delete fRunManager;
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFJ4::BeginRun(Int_t irun)
{

  if( !IsWritable() ) return kTRUE;

  fRunManager->RunInitialization();

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFJ4::EndRun()
{

  if( !IsWritable() ) { return kTRUE; }

  fRunManager->RunTermination();

  return kTRUE;
}



//_____________________________________________________________________________
Bool_t JSFJ4::Process(Int_t nev)
{
//
  JSFJ4Outputs_t::iterator id;
  for( id=gJSFJ4Outputs.begin() ; id!=gJSFJ4Outputs.end(); id++ ) {
    ((*id).second)->Clear();
  }

  Bool_t runstat=fRunManager->SimulateOneEvent(nev);

  JSFJ4Buf *buf=(JSFJ4Buf*)EventBuf();
  buf->fEventID=fRunManager->GetCurrentEvent()->GetEventID();

  if ( runstat ) {
    return kTRUE;
  }
  else {
    gJSF->SetReturnCode(JSFSteer::kJSFDoEndRun) ;
    return kTRUE;
  }
}

// ______________________________________________________________________________
JSFJ4Buf::JSFJ4Buf(const Char_t *name, const Char_t *title, JSFModule *module)
  : JSFEventBuf( name, title, module ),
    fEventID(0)
{
}

// ______________________________________________________________________________
JSFJ4Buf::JSFJ4Buf()
{
}

// ______________________________________________________________________________
JSFJ4Buf::~JSFJ4Buf()
{
  fEvent.Delete();

}

// ______________________________________________________________________________
TObjArray *JSFJ4Buf::AddComponent(Char_t *name)
{
  JSFJ4BufferMap *data=new JSFJ4BufferMap(name);
  fEvent.Add(data);

  return data->GetObjArray();
}


// ______________________________________________________________________________
TObjArray *JSFJ4Buf::GetComponent(Char_t *name) const
{
  for(Int_t i=0;i<fEvent.GetEntries();i++){
    JSFJ4BufferMap *m=(JSFJ4BufferMap*)fEvent.At(i);
    if( strcmp(name, m->GetName() ) == 0 ) {
      return m->GetObjArray();
    }
  }
  return 0;

}

// ______________________________________________________________________________
void JSFJ4Buf::Print(Option_t *opt="")
{
  cout << "List of JSFJ4 data" << endl;
  for(Int_t i=0;i<fEvent.GetEntries();i++){
    JSFJ4BufferMap *m=(JSFJ4BufferMap*)fEvent.At(i);
    cout << m->GetName() << endl;
  }
}

// --------------------------------------------------
JSFJ4BufferMap::JSFJ4BufferMap(Char_t *name)
  : fName(TString(name))
{
  fObjArray=new TObjArray();
}
// --------------------------------------------------
JSFJ4BufferMap::~JSFJ4BufferMap()
{
  fObjArray->Delete();
  delete fObjArray;
}
