//_____________________________________________________________________________
//
//  JSF's Abstract class for Matrix Element generator
//
//  Based on a ganerator, JSFHEPRUP and JSFHEPEUP class must be
//  Created for subsequent use by Shower and Hadronize Generator
//
//$ID$
//_____________________________________________________________________________



#include "JSFConfig.h"
#include "JSFSHGenerator.h"
#include "JSFMEGenerator.h"

#include <iostream>
using namespace std;

ClassImp(JSFSHGenerator)
ClassImp(JSFSHGeneratorBuf)

//_____________________________________________________________________________
JSFSHGenerator::JSFSHGenerator(const char *name, const char *title) 
  : JSFGenerator(name,title)
{
// Create one JSFSpring object
// User class should set addresses to the event buf here, as follows.
//
//  fEventBuf = new JSFSHGeneratorBuf("JSFSHGeneratorBuf",
//	    "JSF Matrix Element Generator EVent Data",this);
}

//_____________________________________________________________________________
JSFSHGenerator::~JSFSHGenerator()
{

}

//_____________________________________________________________________________
Bool_t JSFSHGenerator::BeginRun(Int_t nrun)
{
  Bool_t st=JSFModule::BeginRun(nrun);
  if( !st ) return kFALSE;
 
  return kTRUE;
  
}

//_____________________________________________________________________________
Bool_t JSFSHGenerator::Process(Int_t nevent)
{

  Bool_t st=JSFModule::Process(nevent);
  if( !st ) return kFALSE;

  JSFMEGenerator  *meg=(JSFMEGenerator*)gJSF->FindModule("JSFMEGenerator");
  JSFHEPRUP *heprup=meg->GetJSFHEPRUP();

  JSFSHGeneratorBuf *buf=(JSFSHGeneratorBuf*)EventBuf();
  buf->Clear();
  buf->SetEcm(heprup->GetEBMUP(1) + heprup->GetEBMUP(2));

  return kTRUE;
  
}

//_____________________________________________________________________________
JSFSHGeneratorBuf::JSFSHGeneratorBuf(const char *name,  const char *title,
	    JSFSHGenerator *module): JSFGeneratorBuf(name,title,module)
{
}
