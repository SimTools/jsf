///////////////////////////////////////////////////////////////////
//
//  JSFReadStdHep
//
//  Interface to StdHep data.  
//  
//  (29-Nov-2007: Interface using CLHEP library is removed.)
//
//$Id$ 
//  
//////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

#include "JSFSteer.h"
#include "JSFReadStdHep.h"
#include "JSFParticleData.h"
#include "stdhep.h"


ClassImp(JSFReadStdHep)
ClassImp(JSFReadStdHepBuf)

extern "C" {
  extern void jsf_mcfio_init_();
  extern void stdxropen_(const char *fname, int *nentries, 
			int *istream,int *iok, int lfname);
  extern void stdxrd_(int *iflag, int *istream, int *iok);
  extern void stdxend_(int *istream);
};

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
  fMCFIOStream=0;
  fDebugLevel = gJSF->Env()->GetValue("JSFReadStdHep.DebugLevel",0);
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
  if ( IsWritable() ) {
    jsf_mcfio_init_(); 
  }
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFReadStdHep::Process(Int_t nev)
{
// 
  int lok=0;
  int flag=0;

  while( lok == 0 ) {
    stdxrd_(&flag, &fMCFIOStream, &lok);
    if( lok != 0 ) return kFALSE;
    else if ( flag == 1 || flag == 4) {
      JSFReadStdHepBuf *buf=(JSFReadStdHepBuf*)EventBuf();
      buf->ReadOneRecord();
      if( fDebugLevel > 0 ) {
	std::cerr << "JSFReadStdHep::Process " 
		  << " Event# " << nev 
		  << " Evt#_in_File=" << buf->GetEventNumber()
		  << " num_part.=" << buf->GetNumberOfParticles()
		  << " " ;
	buf->GetHEPEV4().Print();
      }
      return kTRUE;
    }
    else {
      std::cerr << "read one record .. flag=" << flag << ") not recognized " << std::endl;
    }
  }
  return kFALSE;
}

//_____________________________________________________________________________
Bool_t JSFReadStdHep::BeginRun(Int_t nrun)
{
  if ( IsWritable() ) {
  int lenfn=fInFileName.size();
  int ntries;
  int iok;
  std::cerr << fInFileName << " will be opened" << std::endl;
  stdxropen_(fInFileName.data(),&ntries, &fMCFIOStream, &iok, lenfn);
  if( iok != 0 ) { 
    std::cerr << "Error  JSFReadStdHep::BeginRun failed to open file ";
    std::cerr << fInFileName << std::endl;
    return kFALSE;
    }
  }
  return kTRUE;
} 

//_____________________________________________________________________________
Bool_t JSFReadStdHep::EndRun()
{
  if ( IsWritable() ) {
    stdxend_(&fMCFIOStream);
  }
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


  if( maxhep != NMXHEP ) {
    std::cerr << "In JSFReadStdHepBuf::ReadHepEvent.." << std::endl;
    std::cerr << "  maxhep (" << maxhep << ") is not consistent with NMXHEP " << std::endl;
    exit(-1);
  }

  // Set HEPEV4
  
  fEventNumber=hepevt_.nevhep;

  nevhep=hepevt_.nevhep;
  nhep=hepevt_.nhep;
  for (Int_t i=0;i<nhep;i++){
    isthep[i]=hepevt_.isthep[i];
    idhep[i]=hepevt_.idhep[i];
    jmohep[i][0]=hepevt_.jmohep[i][0];
    jmohep[i][1]=hepevt_.jmohep[i][1];
    jdahep[i][0]=hepevt_.jdahep[i][0];
    jdahep[i][1]=hepevt_.jdahep[i][1];
    for(Int_t k=0;k<4;k++) {
      phep[i][k]=hepevt_.phep[i][k];
      vhep[i][k]=hepevt_.vhep[i][k];
    }
    phep[i][4]=hepevt_.phep[i][4];
  }

  fHEPEV4.SetHEPEV4(nhep);

  return kTRUE;
}

//_____________________________________________________________________________
JSFReadStdHepBuf::~JSFReadStdHepBuf()
{
}
