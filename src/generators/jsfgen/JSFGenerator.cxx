////////////////////////////////////////////////////////////////////////
//
// JSFGenerator
//
//  JSF Generator
//
//$Id$
////////////////////////////////////////////////////////////////////////

#ifdef __USEISOCXX__
#include <iostream>
using  namespace std;
#else
#include <iostream.h>
using  namespace std;
#endif

#include "JSFSteer.h"
#include "JSFGenerator.h"
#include "JSFGeneratorParticle.h"


ClassImp(JSFGeneratorBuf)
ClassImp(JSFGenerator)


//_____________________________________________________________________________
JSFGenerator::JSFGenerator(const Char_t *name, const Char_t *title, const Char_t *opt, Bool_t makebuf)
       : JSFModule(name,title, opt)
{
//   Create one JSFGenerator object
//
  if( makebuf ) {
    fEventBuf = new JSFGeneratorBuf("JSFGeneratorBuf", 
				 "JSFGenerator Event data buffer", this);
  }
  else {
    fEventBuf = 0 ;
  }

}


//_____________________________________________________________________________
JSFGeneratorBuf::JSFGeneratorBuf()
{
//   Create one JSFGenerator object
//
  fNparticles=0;
  fParticles=NULL;
  fEcm=0;
  fStartSeed=0;
}

//_____________________________________________________________________________
JSFGeneratorBuf::JSFGeneratorBuf(const char *name, const char *title, JSFGenerator *generator)
       : JSFEventBuf(name,title, (JSFModule*)generator)
{
//   Create one JSFGenerator object
//
  fNparticles=0;
#if 1
  fParticles= new TClonesArray("JSFGeneratorParticle", 1000);
#else
  if( !fParticles ) fParticles= new TClonesArray("JSFGeneratorParticle", 1000);
#endif
  fEcm=0;
  fStartSeed=0;
}

//_____________________________________________________________________________
JSFGenerator::~JSFGenerator()
{
  if( fEventBuf ) { delete fEventBuf; }
  fEventBuf=0;
}

//_____________________________________________________________________________
Bool_t JSFGenerator::Process(Int_t ev)
{

  fEventBuf->Clear();
  fEventBuf->SetHeader();

  return kTRUE;
}

//________________________
const Char_t *JSFGenerator::GetEventTypeString()
{
   return "JSFGeneraor::GetEventTypeStrin - dummy" ;
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

//___________________________________________________________________________
void JSFGeneratorBuf::Print(const Option_t *opt)
{
  // Print particles in fParticles

  printf(" *** Event# %d ",gJSF->GetEventNumber());
  printf(" %d particles in JSFGeneratorBuf object\n",fNparticles);
  printf(" Ser   PId  Mo.  #D 1stD Charge Mass (E,Px,Py,Pz) ");
  if( strcmp(opt,"full")==0 ) {
    printf(" (T,X,Y,Z) ");
  }
  printf("\n");

  for(Int_t i=0;i<fParticles->GetEntries();i++){
    JSFGeneratorParticle *p=(JSFGeneratorParticle*)fParticles->UncheckedAt(i);
    printf("%4d",p->GetSerial());
    printf("%5d",p->GetID());
    printf("%4d",p->GetMother());
    printf("%4d",p->GetNDaughter());
    printf("%4d",p->GetFirstDaughter());
    printf("%6.3f",p->GetCharge());
    printf("%10.5f",p->GetMass());
    printf("%12.5f",p->GetE());
    printf("%12.5f",p->GetPx());
    printf("%12.5f",p->GetPy());
    printf("%12.5f",p->GetPz());
    if( strcmp(opt,"full")==0 ) {
      printf(" %12.5f",p->GetT());
      printf(" %12.5f",p->GetX());
      printf(" %12.5f",p->GetY());
      printf(" %12.5f",p->GetZ());
    }
    printf("\n");
  }

}

//______________________________________________________________________________
void JSFGeneratorBuf::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFGeneratorBuf.

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v=R__b.ReadVersion(&R__s, &R__c);
     if( R__v > 1 ) {
       JSFGeneratorBuf::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
       return;
     }

     JSFEventBuf::Streamer(R__b);
     R__b >> fStartSeed;
     R__b >> fEcm;
     R__b >> fNparticles;
     fParticles->Streamer(R__b);
     R__b.CheckByteCount(R__s, R__c, JSFGeneratorBuf::IsA());
   } else {
     JSFGeneratorBuf::Class()->WriteBuffer(R__b, this);
   }
}








