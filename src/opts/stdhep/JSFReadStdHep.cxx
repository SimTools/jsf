///////////////////////////////////////////////////////////////////
//
//  JSFReadStdHep
//
//  Interface to HepStd Program
//
//$Id$ 
//  
//////////////////////////////////////////////////////////////////

#include "CLHEP/config/CLHEP.h"
#include "CLHEP/config/fstream.h"

#include "CLHEP/StdHep/StdHep.hh"
#include "CLHEP/StdHep/StdEvent.hh"
#include "CLHEP/StdHep/ReadStdHep.hh"
#include "CLHEP/StdHep/ConversionMethods.hh"
#include <iostream>
#include <string>

#include "JSFSteer.h"
#include "JSFReadStdHep.h"
#include "JSFParticleData.h"

ClassImp(JSFReadStdHep)
ClassImp(JSFReadStdHepBuf)

//_____________________________________________________________________________
JSFReadStdHep::JSFReadStdHep(const char *name, const char *title, const bool makebuf )
       : JSFReadGenerator(name,title, kFALSE)
{
  if( makebuf ) {
    fEventBuf = new JSFReadStdHepBuf("JSFReadStdHepBuf", 
	       "JSFReadStdHeo event buffer",this);
  }
  else {
    fEventBuf=0;
  }

  fInFileName=gJSF->Env()->GetValue("JSFReadStdHep.InputFileName","");
  fNReadBlock=0;
  fRunInfo   =0;
  fEvent     =0;
}

//_____________________________________________________________________________
JSFReadStdHep::~JSFReadStdHep()
{
  if( fEventBuf ) delete fEventBuf;
  fEventBuf=0;
}

//_____________________________________________________________________________
Bool_t JSFReadStdHep::Initialize()
{
  // Initialize


  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFReadStdHep::Process(Int_t nev)
{
// 
  std::string block("first"), empty(" "), etype("StdEvent");
  std::cerr << "Process is called " << std::endl;

  while( fInStream && (block != empty) ) {
    block = StdHep::readBlockType( fInStream );
    fNReadBlock++ ;
    if ( block != etype ) continue;
    fEvent = StdHep::readStdEvent( fInStream);
    break;
    //    fEvent->print();
  }

  if( block == empty ) return kFALSE;

  JSFReadStdHepBuf *buf=(JSFReadStdHepBuf*)EventBuf();
  std::cerr << " will call ReadOneRecord...." << std::endl;
  buf->ReadOneRecord();

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFReadStdHep::BeginRun(Int_t nrun)
{
  if ( !fInStream.is_open() ) {
    std::cerr << "Input file is " << fInFileName << std::endl;
    fInStream.open(fInFileName.data());
    if ( fInStream.fail() ) {
      std::cerr << "Failed to open file " << fInFileName << std::endl;
      return kFALSE;
    }
    std::cerr << "JSFReadStdHep::BeginRun " <<  fInFileName ;
    std::cerr << " is opended " << std::endl;
  }

  std::string block("first"), empty(" "), rtype("StdRunInfo");

  while( fInStream && (block != empty) ) {
    // get the block type
    block = StdHep::readBlockType( fInStream );
    fNReadBlock++ ;
    if( block == rtype ) {
      // run
      fRunInfo = StdHep::readStdRun( fInStream );
      std::cout << "Run " << fRunInfo->runIdentifier() << std::endl;
      break;
    }
  }

  if ( !fRunInfo ) {
    std::cerr << "Error in JSFReadStdHep::BeginRun()" << std::endl;
    std::cerr << "RunInfo block is not found in the input data" << std::endl;
    return kFALSE;
  }
  std::cerr << "End of Begin Run " << std::endl;

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFReadStdHep::EndRun()
{
  if ( fInStream.is_open() ) fInStream.close();
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFReadStdHep::Terminate()
{
  return kTRUE;
}

//_____________________________________________________________________________
JSFReadStdHepBuf::JSFReadStdHepBuf(const char *name, const char *title,
  	     JSFReadGenerator *module) : JSFReadGeneratorBuf(name, title, module)
{
}


// ---------------------------------------------------------------
Bool_t JSFReadStdHepBuf::ReadHepEvent(const Int_t maxhep, Int_t &nevhep, 
	 Int_t &nhep, Int_t isthep[], Int_t idhep[], Int_t jmohep[][2], 
         Int_t jdahep[][2], Double_t phep[][5], Double_t vhep[][4])
{
  // Read Generator data and saved to the class.

  std::cerr << "JSFReadHepStd::ReadHepStd is called" << std::endl;

  //  Int_t nvers=1;
  //  Int_t ivers, endian ;
  //  Int_t nret;

  //  readgenhepevt_(&unit,&endian, &ivers, &nevhep,  &nhep,
  //		 isthep, idhep, jmohep, jdahep, phep, vhep, &nret);

  StdHep::StdEvent *event=((JSFReadStdHep*)Module())->GetEvent();  
 
  HepLorentzVector mom, vh;
  HepMC::GenParticle* part;

  nhep = event->numParticles();
  nevhep = event->eventNumber(); 

  Int_t nrp=0;
  for ( Int_t i = 0; i < event->numParticles(); ++i ) {
    part = event->particle( i );
    if( part ) {
      nrp++;
      idhep[i] = part->ParticleID(); 
      mom = part->Momentum();
      vh = part->CreationVertex();
      isthep[i]    = part->StatusCode();
      jmohep[i][0] = part->Mother()+1;
      jmohep[i][1] = part->SecondMother()+1;
      jdahep[i][0] = part->BeginDaughters()+1;
      jdahep[i][1] = part->EndDaughters()+1;
      phep[i][0]   = mom.px();
      phep[i][1]   = mom.py();
      phep[i][2]   = mom.pz();
      phep[i][3]   = mom.e();
      phep[i][4]   = mom.e();
      phep[i][3]   = part->generatedMass();
      vhep[i][0]   = vh.x();
      vhep[i][1]   = vh.y();
      vhep[i][2]   = vh.z();
      vhep[i][3]   = vh.t();

	std::cerr << "i=" << i << " status=" << isthep[i] ;
	std::cerr << " id=" << idhep[i] << " e=" << phep[i][3] ;
	std::cerr << " mother= " << jmohep[i][0] << std::endl;
    }
  } 
  if ( nhep != nrp ) {
    std::cerr << "Error at JSFReadStdHep::ReadHepEvent ... ";
    std::cerr << "Number of read particles is inconsistent." << std::endl;
    return kFALSE;
  }
  return kTRUE;
}

//_____________________________________________________________________________
JSFReadStdHepBuf::~JSFReadStdHepBuf()
{
}




