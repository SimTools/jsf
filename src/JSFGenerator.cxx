//*LastUpdate :  0.02/02  11-September-1998  By A.Miyamoto
//*-- Author  : A.Miyamoto  11-September-1998

////////////////////////////////////////////////////////////////////////
//
// JSFGenerator
//
//  JSF Generator
//
////////////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFGenerator.h"
#include "JSFGeneratorParticle.h"


ClassImp(JSFGeneratorBuf)
ClassImp(JSFGenerator)

TClonesArray *gParticles;


//_____________________________________________________________________________
JSFGenerator::JSFGenerator(const char *name, const char *title)
       : JSFModule(name,title)
{
//   Create one JSFGenerator object
//
  fEventBuf = new JSFGeneratorBuf("JSFGeneratorBuf", 
				 "JSFGenerator Event data buffer", this);
}


//_____________________________________________________________________________
JSFGeneratorBuf::JSFGeneratorBuf(const char *name, const char *title, JSFGenerator *generator)
       : JSFEventBuf(name,title, (JSFModule*)generator)
{
//   Create one JSFGenerator object
//   When the constructor is invoked for the first time, the global
//   variable gTracks is NULL. The TClonesArray gTracks is created.
//   The histogram fH is created.
//
  fNparticles=0;
  if( !gParticles ) gParticles= new TClonesArray("JSFGeneratorParticle", 1000);
  fParticles=gParticles;
  fEcm=0;
  fStartSeed=0;
}

//_____________________________________________________________________________
JSFGenerator::~JSFGenerator()
{
}

//_____________________________________________________________________________
Bool_t JSFGenerator::Process(Int_t ev)
{
   fEventBuf->SetHeader();

   return kTRUE;
}

//_____________________________________________________________________________
JSFGeneratorBuf::~JSFGeneratorBuf()
{
   Clear();
}

//_____________________________________________________________________________
void JSFGeneratorBuf::Clear(Option_t *option)
{
   if( fParticles) fParticles->Clear(option);
}


