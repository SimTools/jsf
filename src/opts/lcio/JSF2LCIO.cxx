//*LastUpdate : v.01.01 undefined by undefined
//*-- Author  : undefined undefined

///////////////////////////////////////////////////////////////////
//
//  JSF2LCIO
//
//  JSF to LCIO Converer
//
//$Id: 
//  
//////////////////////////////////////////////////////////////////
 
#include "lcio.h"

#include "IO/LCWriter.h"
#include "EVENT/LCIO.h"
#include "DATA/LCFloatVec.h"
#include "DATA/LCIntVec.h"
 
#include "IMPL/LCEventImpl.h"
#include "IMPL/LCRunHeaderImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "IMPL/SimCalorimeterHitImpl.h"
#include "IMPL/SimTrackerHitImpl.h"
#include "IMPL/MCParticleImpl.h"
#include "IMPL/LCFlagImpl.h"
#include "IMPL/LCTOOLS.h"
#include "IMPL/TPCHitImpl.h"
#include "UTIL/LCRelationNavigator.h"
 
#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace std;
using namespace lcio;

#include "JSFSteer.h"
#include "JSF2LCIO.h"

IO::LCWriter *flcWrt=0;        
IMPL::LCRunHeaderImpl* frunHdr=0; 

ClassImp(JSF2LCIO)

//_____________________________________________________________________________
  JSF2LCIO::JSF2LCIO(const char *name, const char *title)
       : JSFModule(name,title)
{
//  fEventBuf = new JSF2LCIOBuf("JSF2LCIOBuf", 
//	 "JSF2LCIO event uffer",this);
  flcWrt=0;
  frunHdr=0;

}

//_____________________________________________________________________________
JSF2LCIO::~JSF2LCIO()
{
}

//_____________________________________________________________________________
Bool_t JSF2LCIO::Initialize()
{
  fOutputFileName=gJSF->Env()->GetValue("JSF2LCIO.OutputFileName","jsf.slcio");

  // create sio writer
  flcWrt = LCFactory::getInstance()->createLCWriter() ;
       
  flcWrt->open( fOutputFileName.Data() , LCIO::WRITE_NEW )  ;

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSF2LCIO::Process(Int_t nev)
{
// 

   // we need to use the implementation classes here
   LCEventImpl*  evt = new LCEventImpl() ;
 
   evt->setRunNumber(  gJSF->GetRunNumber()   ) ;
   evt->setEventNumber( nev ) ;
   evt->setDetectorName( fDetectorName.Data() ) ;


   LCCollectionVec* extFVec = new LCCollectionVec( LCIO::LCFLOATVEC )  ;
   LCCollectionVec* extIVec = new LCCollectionVec( LCIO::LCINTVEC )  ;

          LCFloatVec* extF = new LCFloatVec ;
          LCIntVec*   extI = new LCIntVec ;

          // fill the extension vectors (4 floats, 2 ints)
          extF->push_back( 3.14159 ) ;
          for(int k=0;k<3;k++) extF->push_back(   0.1*k*k  ) ;
 
          extI->push_back( 123456789 ) ;
          extI->push_back( 987654321 ) ;

          extFVec->push_back( extF ) ;
          extIVec->push_back( extI ) ;

        evt->addCollection( extFVec , "UserFloatExtension" ) ;
        evt->addCollection( extIVec , "UserIntExtension" ) ;

        // write the event to the file
        flcWrt->writeEvent( evt ) ;
 
        // dump the event to the screen
        LCTOOLS::dumpEvent( evt ) ;

        delete evt;

/*
//  <<+EMAnal>>
  JSFSIMDST *sim=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
  JSFSIMDSTBuf *sb=(JSFSIMDSTBuf*)sim->EventBuf();

  TClonesArray *emc=sb->GetEMCHits();

  Int_t nemc=sb->GetNEMCHits();
  Float_t emsum=0;
  for(Int_t ie=0;ie<nemc;ie++){
    JSFCALHit *ch=(JSFCALHit*)emc->UncheckedAt(ie);
    emsum+=ch->GetEnergy();
  }
  hEsum->Fill(emsum);
//  <<-EMAnal>>
*/

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSF2LCIO::BeginRun(Int_t nrun)
{

   frunHdr = new LCRunHeaderImpl ;
   frunHdr->setRunNumber( nrun ) ;
                                                                                
   string detName("GLD_V1")  ;
   frunHdr->setDetectorName( detName ) ;
                                                                                
   stringstream description ;
   description << " run: " << nrun <<" First implementation of lcio in JSF" ;
   frunHdr->setDescription( description.str()  ) ;

   string ecalName("GLD_ECAL") ;
   frunHdr->addActiveSubdetector( ecalName ) ;
       
   string tpcName("GLD_TPC") ;
   frunHdr->addActiveSubdetector( tpcName ) ;

   frunHdr->parameters().setValue( "SimulationProgram" , "jsfj4" ) ;
   IntVec iv(3) ;
   iv[0] = 1 ;
   iv[1] = 2 ;
   iv[2] = 3 ;
   frunHdr->parameters().setValues( "SomeIndices" , iv ) ;
                                                                                
   flcWrt->writeRunHeader( frunHdr ) ;

   return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSF2LCIO::EndRun()
{
  delete frunHdr;
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSF2LCIO::Terminate()
{
  flcWrt->close();  
  delete flcWrt;
  return kTRUE;
}

