//*LastUpdate :  JSF-1-11  29-July-1999 Akiya Miyamoto
//*LastUpdate :  0.02/02  11-September-1998  By A.Miyamoto
//*-- Author  : A.Miyamoto  11-September-1998

/*
  29-July-1999 A.Miyamoto  Add opt option in constructor.
  30-July-1999 A.Miyamoto  Do not use gParticles for TClonesArray.
                           It is not good if there are more than one generator object.
			   Add Apend();
*/

////////////////////////////////////////////////////////////////////////
//
// JSFGenerator
//
//  JSF Generator
//
//$Id$
////////////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFGenerator.h"
#include "JSFGeneratorParticle.h"


ClassImp(JSFGeneratorBuf)
ClassImp(JSFGenerator)


//_____________________________________________________________________________
JSFGenerator::JSFGenerator(const Char_t *name, const Char_t *title, const Char_t *opt)
       : JSFModule(name,title, opt)
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
//
  fNparticles=0;
  if( !fParticles ) fParticles= new TClonesArray("JSFGeneratorParticle", 1000);
  fEcm=0;
  fStartSeed=0;
}

//_____________________________________________________________________________
JSFGenerator::~JSFGenerator()
{
  if( fEventBuf ) { delete fEventBuf; }
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
   if( fParticles ) { delete fParticles; }
}

//_____________________________________________________________________________
void JSFGeneratorBuf::Clear(Option_t *option)
{
   if( fParticles) fParticles->Clear(option);
}

//--------------------------------------------------------------
void JSFGeneratorBuf::Append(JSFGeneratorBuf *src)
{
  //
  // Append JSFGeneratorBuf objects of src to current object.
  // When appended, fSer, fFirstDaughter, and fMother are modified.
  //

  TClonesArray &part=*(fParticles);
  Int_t norg=fNparticles;
  TClonesArray *sp=src->GetParticles();
  Int_t nsrc=src->GetNparticles();
  if( nsrc <= 0 ) return;

  for(Int_t i=0;i<nsrc;i++){
    JSFGeneratorParticle *p=(JSFGeneratorParticle*)sp->UncheckedAt(i);
    p->fSer+=fNparticles;
    p->fFirstDaughter+=fNparticles;
    if( p->fMother > 0 ) {
      p->fMother+=fNparticles;
    }
    else if( p->fMother < 0 ) {
      p->fMother-=fNparticles;
    }
    new(part[norg+i]) JSFGeneratorParticle(*p);
  }
  fNparticles+=nsrc;

}

