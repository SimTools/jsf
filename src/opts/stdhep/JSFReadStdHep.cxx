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
  extern void stdxrinit_(const char *fname, int *nentries, 
			int *istream,int *iok, int lfname);
  extern void stdxrd_(int *iflag, int *istream, int *iok);
  extern void stdxend_(int *istream);
};

Bool_t JSFReadStdHep::fIsInitialized=kFALSE;

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
  fPrintSkipMessage = gJSF->Env()->GetValue("JSFReadStdHep.PrintSkipMessage",1000);
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
  if ( IsWritable() ) {
    jsf_mcfio_init_(); 
    fIsInitialized=kTRUE;
  }

  int lenfn=fInFileName.size();

  if( fInFileName.substr(0,1) != std::string("@") ) { 
    fFNStack.push_back( fInFileName.c_str() );
  }
  else { // Load file names from a file.
    std::cerr << "JSFReasStdHep::Initialize  "
	      <<"input file names are obtained from" 
	      << fInFileName.substr(1,lenfn-1) << std::endl;
    std::ifstream fin(fInFileName.substr(1,lenfn-1).c_str());
    Char_t infile[1024];
    while( ! fin.eof() ) {
      fin >> infile ;
      if( fin.eof() ) break;      
      fFNStack.push_back( infile );
    }
  }
  fFNIter=fFNStack.begin();

  return kTRUE;
}

//_____________________________________________________________________________
void JSFReadStdHep::Print()
{
  std::vector<std::string>::iterator iv;
  std::cout << "JSFReadStdHep Input file" << std::endl; 
  for(iv=fFNStack.begin();iv!=fFNStack.end();++iv) {
    std::cout << *iv << std::endl;
  }
  std::cout << " ... " << fFNStack.size() << " files. " << std::endl;
}

//_____________________________________________________________________________
Bool_t JSFReadStdHep::Process(Int_t nev)
{
// 
  JSFGenerator::Process(nev);

  int lok=0;
  int flag=0;

  Bool_t skip=kTRUE;
  Int_t iloop=0;
  static Int_t nrec=0;
  while( lok == 0 && skip ) {
retry:  stdxrd_(&flag, &fMCFIOStream, &lok);
    if( lok != 0 ) {
      std::cerr << "JSFReadStdHep::Process ... Read end of file." << std::endl;
      fFNIter++;
      if( fFNIter >= fFNStack.end() ) {
	std::cerr << "JSFReadStdHep::Process ... No more input files." <<std::endl; 
        return kFALSE;
      }
      stdxend_(&fMCFIOStream);
      std::string infile=*fFNIter;
      std::cerr << infile << " will be opened" << std::endl;
      Int_t lenfn=infile.size();
    //    stdxrinit_(infile.Data(),&ntries, &fMCFIOStream, &iok, lenfn);
      Int_t ntries;
      Int_t iok;
      stdxropen_(infile.c_str(),&ntries, &fMCFIOStream, &iok, lenfn);
      if( iok != 0 ) { 
        std::cerr << "Error  JSFReadStdHep::Process failed to open file ";
        std::cerr << infile << std::endl;
        return kFALSE;
      }
      goto retry;

    }
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
      nrec++;
      if( nrec >= nev ) {
	skip = kFALSE;
	return kTRUE;
      }
      if( nrec%fPrintSkipMessage == 1 ) {
      std::cerr << "JSFReadStdHep .. Skipping Record# " << nrec << std::endl;
      }  
    }
    else if ( flag == 100 ) {
      std::cerr << "JSFReadStdHep .. Get StdHep BeginRun " << std::endl;
      fStdHepCM.LoadStdHepCM();
      fStdHepCM.Print();
    }
    else if ( flag == 200 ) {
      std::cerr << "JSFReadStdHep .. Get StdHep EndRun " << std::endl;
      fStdHepCM.LoadStdHepCM();
      fStdHepCM.Print();
    }
    else {
      std::cerr << "read one record .. flag=(" << flag << ") not recognized " << std::endl;
    }
    iloop++;
    if( iloop > 10000000 ) {
      std::cerr << "JSFReadStdHep .. Too many loops ( 10000000 ) . Forced termination" << std::endl;
      skip=kFALSE;
    }
  }
  return kFALSE;
}

//_____________________________________________________________________________
Bool_t JSFReadStdHep::BeginRun(Int_t nrun)
{
  if ( IsWritable() ) {

    fFNIter=fFNStack.begin();

    std::string infile=*fFNIter;

    int ntries;
    int iok;
    std::cerr << infile << " will be opened" << std::endl;
  //  stdxropen_(fInFileName.data(),&ntries, &fMCFIOStream, &iok, lenfn);
    Int_t lenfn=infile.size();
    stdxrinit_(infile.c_str(),&ntries, &fMCFIOStream, &iok, lenfn);
    if( iok != 0 ) { 
      std::cerr << "Error  JSFReadStdHep::BeginRun failed to open file ";
      std::cerr << infile << std::endl;
      return kFALSE;
    }
  }
  //  std::cerr << "instream=" << fMCFIOStream << std::endl;
  fIsInitialized=kTRUE;

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

//____________________________________________________________________________
Bool_t JSFReadStdHepBuf::ReadOneRecord()
{
  Bool_t ret=JSFReadGeneratorBuf::ReadOneRecord();
  if( !ret ) { return ret; }

  if( fHEPEV4.GetNHEP() == GetNParticles() ) {
    TClonesArray *ps=GetParticles();
    Float_t spin[3];
    Int_t colorflow[2];
    for(Int_t i=0;i<GetNParticles();i++){
      JSFGeneratorParticle *p=(JSFGeneratorParticle*)ps->At(i);
      for(Int_t k=0;k<3;k++){ spin[k]=fHEPEV4.GetSpin(i,k); }
      for(Int_t k=0;k<2;k++){ colorflow[k]=fHEPEV4.GetColorFlow(i,k); }
      p->SetSpinAndColorflow(spin, colorflow);
    }
  }
  return kTRUE;
}

//_____________________________________________________________________________
JSFReadStdHepBuf::~JSFReadStdHepBuf()
{
}
