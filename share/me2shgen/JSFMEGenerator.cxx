//_____________________________________________________________________________
//
//  JSF's Abstract class for Matrix Element generator
//
//  Based on a ganerator, JSFHEPRUP and JSFHEPEUP class must be
//  Created for subsequent use by Shower and Hadronize Generator
//
//$ID$
//_____________________________________________________________________________

#include <iostream>
using namespace std;

#include "JSFConfig.h"
#include "JSFMEGenerator.h"

ClassImp(JSFMEGenerator)
ClassImp(JSFMEGeneratorBuf)

//_____________________________________________________________________________
JSFMEGenerator::JSFMEGenerator(const char *name, const char *title) 
  : JSFModule(name,title)
{
}

//_____________________________________________________________________________
JSFMEGenerator::~JSFMEGenerator()
{
}

//_____________________________________________________________________________
JSFMEGeneratorBuf::~JSFMEGeneratorBuf()
{
  cout << "Destructor of JSFMEGeneratorBuf is called" << endl;
}

//_____________________________________________________________________________
Bool_t JSFMEGenerator::BeginRun(Int_t nrun)
{
  if( !JSFModule::BeginRun(nrun) ) return kFALSE;

  return SetHEPRUP();
}

//_____________________________________________________________________________
Bool_t JSFMEGenerator::Process(Int_t nev)
{
  if( !JSFModule::Process(nev) ) return kFALSE;

  JSFHEPEUP hepeup;

  JSFReturnCode_t st=SetHEPEUP(hepeup);
  
  if( st != JSFSteer::kJSFOK ) {
    gJSF->SetReturnCode(st);
    return kTRUE;
  }

  ((JSFMEGeneratorBuf*)fEventBuf)->SetHEPEUP(hepeup);
  
  return kTRUE;
}

//_____________________________________________________________________________
void JSFMEGeneratorBuf::SetHEPEUP(JSFHEPEUP &hepeup)
{

  fHEPEUP = hepeup ; 

}
