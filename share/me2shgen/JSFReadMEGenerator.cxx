//----------------------------------------------------------------------------
//
//  JSFReadMEGenerator class
//
//  Read ASCII file of Matrix Element Event genrator,
//  and store them into the data member of JSFMEGeneratorBuf
//  for subsequent shower and hadronize by JSFSHGenerator
//
//$ID$
//_____________________________________________________________________________


#include "JSFSteer.h"
#include "JSFReadMEGenerator.h"

#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;


ClassImp(JSFReadMEGenerator)
ClassImp(JSFReadMEGeneratorBuf)

//_____________________________________________________________________________
JSFReadMEGenerator::JSFReadMEGenerator(const char *name, const char *title) 
  : JSFMEGenerator(name,title)
{

  fEventBuf=new JSFReadMEGeneratorBuf("JSFReadMEGeneratorBuf",
				      "JSF Read Matrix Element Generator Event data", this);

  fEventFileName=TString(gJSF->Env()->GetValue("JSFReadMEGenerator.EventFileName","event.dat"));
  fRunFileName=TString(gJSF->Env()->GetValue("JSFReadMEGenerator.RunFileName","run.dat"));
}

//_____________________________________________________________________________
JSFReadMEGenerator::~JSFReadMEGenerator()
{

}

//_____________________________________________________________________________
Bool_t JSFReadMEGenerator::Initialize()
{

  if( !JSFMEGenerator::Initialize() ) return kFALSE;
  
  fRunInput=new ifstream(fRunFileName.Data(), ios::in);
  if ( !fRunInput ) {
    cout << "Fatal Error at JSFReadMEGenerator::Initialize" << endl;
    cout << "Unable to open HEPRUP file. File name is " << fRunFileName << endl;
    return kFALSE;
  }
  cout << "JSFReadMEGenerator read run info. from the file, " << fRunFileName << endl;

  fEventInput=new ifstream(fEventFileName.Data(), ios::in);
  if ( !fEventInput ) {
    cout << "Fatal Error at JSFReadMEGenerator::Initialize" << endl;
    cout << "Unable to open HEPEUP file. File name is " << fEventFileName << endl;
    return kFALSE;
  }
  cout << "JSFReadMEGenerator read event info. from the file, " << fEventFileName << endl;

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFReadMEGenerator::SetHEPRUP()
{

  Bool_t st=fHEPRUP.ReadFile(*fRunInput);
  
  if( !st ) {
    cout << "In JSFReadMEGenerator::SetHEPRUP ..." << endl;
    cout << "End-of-File detected while reading HEPRUN file.  " ;
    cout << "File name is " << fRunFileName.Data() << endl;
   return kFALSE;
  }

  return kTRUE;

}

//_____________________________________________________________________________
JSFReturnCode_t JSFReadMEGenerator::SetHEPEUP(JSFHEPEUP &hepeup)
{

  Bool_t iret=hepeup.ReadFile(*fEventInput);

  if( !iret ) return JSFSteer::kJSFEOF;

  return JSFSteer::kJSFOK;

}


