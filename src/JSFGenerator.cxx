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
JSFGenerator::JSFGenerator(const Char_t *name, const Char_t *title, const Char_t *opt)
       : JSFModule(name,title, opt)
{
//   Create one JSFGenerator object
//
  fEventBuf = new JSFGeneratorBuf("JSFGeneratorBuf", 
				 "JSFGenerator Event data buffer", this);
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
}

//_____________________________________________________________________________
Bool_t JSFGenerator::Process(Int_t ev)
{

  fEventBuf->Clear();
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
Int_t JSFGenerator::HepevtToGeneratorParticles(COMMON_HEPEVT_t *hepevt)
{
// Converts particles information of TParticle format (HEPEVT) to
// JSFGeneratorParticles format
// 

  const Int_t kNMXHEP=__HEPEVT_NMXHEP__;

  Int_t nhep=hepevt->NHEP;

  cout << " nhep= " << nhep << endl;

  // ***************************************
  // Scan data and reassign mother-daughter map
  // ***************************************
  
  Int_t i=0;
  struct modau {
    Int_t mother;
    Int_t ndau;
    Int_t dau1st;
    Int_t nser;
  } map[kNMXHEP];
  Int_t nser=0;
  Int_t n1stfinal=0;
  Int_t n1stdoc=0;
  Int_t jlist[kNMXHEP];

  for(i=0;i<nhep;i++){

    map[i].nser=0;
    if( hepevt->ISTHEP[i] == 0 ) continue;

    nser++;
    jlist[nser]=i;

    Int_t thestatus= hepevt->ISTHEP[i];

    switch ( thestatus ) {
      case 1:
        map[i].ndau=0;
        map[i].dau1st=0;
	if( hepevt->JMOHEP[i][0] <= 0 ) map[i].mother=0;
        else map[i].mother=map[hepevt->JMOHEP[i][0]-1].nser; // modified by I.Nakamura
        map[i].nser=nser;
	if( n1stfinal == 0 ) n1stfinal=nser;
	if( map[i].mother == 0 ) map[i].mother=n1stdoc;
	break;
      case 2:
        map[i].ndau=hepevt->JDAHEP[i][1] - hepevt->JDAHEP[i][2] + 1;
        map[i].dau1st=-2;
	if(  hepevt->JMOHEP[i][0] <= 0 ) map[i].mother=0;
        else map[i].mother=map[hepevt->JMOHEP[i][0]-1].nser; // modified by I.Nakamura
        map[i].nser=nser;
	if( n1stfinal == 0 ) n1stfinal=nser;
	if( map[i].mother == 0 ) map[i].mother=n1stdoc;
	break;
      case 3:
	if( n1stdoc == 0 ) n1stdoc=nser;
        map[i].ndau=1;
        map[i].dau1st=-3;
        map[i].mother=-3;
        map[i].nser=nser;
	break;
      default:
        map[i].ndau=1;
        map[i].dau1st=-4;
        map[i].mother=-hepevt->ISTHEP[i];
        map[i].nser=nser;
    }
  }

  for(i=0;i<nhep;i++){
    if( map[i].nser == 0 ) continue;
    switch (map[i].dau1st){
      case -2:
	map[i].dau1st=map[hepevt->JDAHEP[i][0]-1].nser; // modified by I.Nakamura
	break;
      case -3:
      case -4:
	map[i].dau1st=n1stfinal;
	break;
    }
  }

  // ***************************************
  // Fill GeneratorParticle Array
  // ***************************************
  
  JSFGeneratorBuf *buf=(JSFGeneratorBuf*)EventBuf();
  TClonesArray &ps=*(buf->GetParticles());
  ps.Clear();

  TVector px(4);
  TVector vp(4);
  Int_t iser=0;
  Int_t is;
  for(is=1;is<=nser;is++){
    i=jlist[is];

    if( hepevt->ISTHEP[i] == 0 ) continue;

    Int_t id=hepevt->IDHEP[i];
    Float_t mass, charge, xctau;
    GetChargeCtau(id, charge, xctau);
    Int_t mother=map[i].mother;
    Int_t firstdaughter=map[i].dau1st;
    Int_t ndaughter=map[i].ndau;

    Float_t dl=0.0;
    //    if( mother < 0 ) xctau=0.0;
    mass = hepevt->PHEP[i][4];

    px(0)=hepevt->PHEP[i][3]; px(1)=hepevt->PHEP[i][0]; 
    px(2)=hepevt->PHEP[i][1]; px(2)=hepevt->PHEP[i][2]; 

    vp(0)=hepevt->VHEP[i][3]; vp(1)=hepevt->VHEP[i][0]; 
    vp(2)=hepevt->VHEP[i][1]; vp(2)=hepevt->VHEP[i][2]; 
    
    iser++;
    new(ps[is-1]) JSFGeneratorParticle(iser, id, mass, charge,
	              px, vp, ndaughter, firstdaughter, mother,
		      xctau, dl);
  }

  buf->SetNparticles(iser);

  return kTRUE;
}

//______________________________________________________________________________
void JSFGenerator::GetChargeCtau(Int_t i, Float_t &charge, Float_t &ctau)
{
  // Returns charge of i-th particle in HEPEVT buffer
  // i=(0,n-1) 
  printf("Error .. Dummy JSFGenerator::GetChargeCtau( , ,) was called.\n");
  printf("Implement the function for %s\n",GetName());

}

#if __ROOT_FULLVERSION__ >= 30000
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

#else

//______________________________________________________________________________
void JSFGeneratorBuf::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFGeneratorBuf.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      JSFEventBuf::Streamer(R__b);
      R__b >> fStartSeed;
      R__b >> fEcm;
      R__b >> fNparticles;
      fParticles->Streamer(R__b);
   } else {
      R__b.WriteVersion(JSFGeneratorBuf::IsA());
      JSFEventBuf::Streamer(R__b);
      R__b << fStartSeed;
      R__b << fEcm;
      R__b << fNparticles;
      fParticles->Streamer(R__b);
   }
}

#endif








