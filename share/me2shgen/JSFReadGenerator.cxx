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


#include "JSFConfig.h"
#include "JSFReadMEGenerator.h"

ClassImp(JSFReadMEGenerator)
ClassImp(JSFReadMEGeneratorBuf)

//_____________________________________________________________________________
JSFReadMEGenerator::JSFReadMEGenerator(const char *name, const char *title) 
  : JSFMEGenerator(name,title)
{
//  fEventBuf = new JSFMEGeneratorBuf("JSFMEGeneratorBuf",
//	    "JSF Matrix Element Generator EVent Data",this);
}

//_____________________________________________________________________________
JSFReadMEGenerator::~JSFReadMEGenerator()
{

}

//_____________________________________________________________________________
void JSFReadMEGenerator::AllocateEventBuffer()
{
  fEventBuf=new JSFReadMEGeneratorBuf("JSFReadMEGeneratorBuf",
				      "JSF Read Matrix Element Generator Event data", this);
}

//_____________________________________________________________________________
Bool_t JSFReadMEGenerator::SetHEPRUP()
{
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFReadMEGenerator::SetHEPEUP()
{
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFReadMEGenerator::Initialize()
{
  return kTRUE;
}

