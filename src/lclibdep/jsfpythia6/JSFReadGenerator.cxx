///////////////////////////////////////////////////////////////////
//
// JSFReadGenerator
//
// Read a generator data file, and stored into JSFGeneratorParticle class
//
// Currently, HEPEVT format file is supported. 
//
// HEPEVT format:  This file is accessed by subroutines in readgenutil.F.
// The file is a Fortran binary file.  and should be created as follows.
//
// INTEGER*4 NENDIAN
// INTEGER*4 NVERS
// INTEGER*4 NEVHEP
// INTEGER*4 NHEP
// INTEGER*4 ISTHEP(NMXHEP), IDHEP(NMXHEP)
// INTEGER*4 JMOHEP(2,NMXHEP), JDAHEP(2,NMXHEP)
// REAL*8    PHEP(5,NMXHEP), VHEP(4,NMXHEP)
//
//   IU=1
//   NENDIAN=1296651082
//   WRITE(IU) NENDIAN, NVERS, NEVHEP, NHEP, (ITHEP(K),K=1,NHEP),
//  .          (IDHEP(K),K=1,NHEP),
//  .          (JMOHEP(1,K),JMOHEP(2,K),K=1,NHEP),
//  .          (JDAHEP(1,K),JDAHEP(2,K),K=1,NHEP),
//  .          ((PHEP(J,K),J=1,5),K=1,NHEP),
//  .          ((VHEP(J,K),J=1,5),K=1,NHEP)
//
// (Update)
//   16-Apr-2000 A.Miyamoto  Put bug fixes written by I.Nakamura in ReadOneRecord()
//
//   31-Jan-2000 A.Miyamoto  Mange jmohep[i][0]=0 and isthep[i]=1 | 2 case in ReadOneRecord()
//   21-Jan-2003 A.Miyamoto  Create ReadHepEvent method.
//
//$Id$
//
//////////////////////////////////////////////////////////////////

#ifdef __USEISOCXX__
#include <iostream>
#else
#include <iostream.h>
#endif

// #include "TPythia6.h"
#include "JSFSteer.h"
// #include "JSFLCFULL.h"
#include "JSFGenerator.h"
#include "JSFReadGenerator.h"
#include <TMath.h>

ClassImp(JSFReadGenerator)
ClassImp(JSFReadGeneratorBuf)

extern "C" {
extern void readgenopen_(Int_t *unit, Char_t *name, Int_t len);
extern void readgenclose_(Int_t *unit);
extern void readgenhepevt_(Int_t *iunit, 
	Int_t *endian, Int_t *nvers, Int_t *nevhep, Int_t *nhep,
        Int_t isthep[], Int_t idhep[],
        Int_t jmohep[][2], Int_t jdahep[][2],
	Double_t phep[][5], Double_t vhep[][4], Int_t *nret);

#if __PYTHIA_VERSION__ >= 6 
  extern Int_t pychge_(Int_t *kf);
  extern int pycomp_(int *kf);
#else
  extern Int_t luchge_(Int_t *kf);
  extern Float_t ulctau_(Int_t *kf);
#endif

};

#if __PYTHIA_VERSION__ >= 6 
struct COMMON_PYDAT2_t {
  int KCHG[4][500];
  double PMAS[4][500];
  double PARF[2000];
  double VCKM[4][4];
};

extern COMMON_PYDAT2_t pydat2_;
#endif

using namespace std;

//_____________________________________________________________________________
JSFReadGenerator::JSFReadGenerator(const char *name, const char *title)
       : JSFGenerator(name,title)
{
  fEventBuf = new JSFReadGeneratorBuf("JSFReadGeneratorBuf", 
	       "JSFReadGenerator event buffer",this);
 
  sscanf(gJSF->Env()->GetValue("JSFReadGenerator.DataFile","genevent.dat"),
	 "%s",fDataFileName);
  sscanf(gJSF->Env()->GetValue("JSFReadGenerator.Unit","10"),"%d",&fUnit);
  sscanf(gJSF->Env()->GetValue("JSFReadGenerator.Format","HEPEVT"),
	 "%s",fFormat);

}



// ---------------------------------------------------------------
Bool_t JSFReadGenerator::BeginRun(Int_t nrun)
{
  if( !IsWritable() ) return kTRUE; // Quit when not output mode.
  Bool_t rc=kTRUE;
// Open a file 
  Int_t lenf=strlen(fDataFileName);
  readgenopen_(&fUnit, fDataFileName, lenf);

  return rc;
}

// ---------------------------------------------------------------
Bool_t JSFReadGenerator::EndRun()
{
  if( !IsWritable() ) return kTRUE;
  readgenclose_(&fUnit);
  return kTRUE;
}


// ---------------------------------------------------------------
Bool_t JSFReadGenerator::Process(Int_t nev)
{
//

  JSFReadGeneratorBuf *simb=(JSFReadGeneratorBuf*)EventBuf();
  return simb->ReadOneRecord();
}

// ---------------------------------------------------------------
JSFReadGeneratorBuf::JSFReadGeneratorBuf(const char *name,
     const char *title, JSFReadGenerator *module)
  :JSFGeneratorBuf(name, title, (JSFGenerator*)module)
{
  SetStartSeed(12345);
}



// ---------------------------------------------------------------
Bool_t JSFReadGeneratorBuf::ReadHepEvent(const Int_t maxhep, Int_t &nevhep, Int_t &nhep, 
	 Int_t isthep[], Int_t idhep[], Int_t jmohep[][2], Int_t jdahep[][2],
	 Double_t phep[][5], Double_t vhep[][4])
{
// Read Generator data and saved to the class.

// Due to imcompatibility in JSFGeneratorParticle class and HEPEVT,
// second mother information is not saved.
//
// 

  Int_t nvers=1;
  Int_t ivers, endian ;
  Int_t nret;
  Int_t unit=((JSFReadGenerator*)Module())->GetUnit();
  readgenhepevt_(&unit,&endian, &ivers, &nevhep,  &nhep,
	isthep, idhep, jmohep, jdahep, phep, vhep, &nret);

  if( nvers != ivers ) {
    printf("Errror at JSFReadGenerator.. Version number of ");
    printf("data is %d, while program can process only ",ivers);
    printf("version %d\n",nvers);
    return kFALSE;
  }
  else if(nret == -1) { 
    printf("In JSFReadGeneratorBuf::UnpackDST .. ");
    printf("read end-of-file of the input file.\n");
    return kFALSE;
  }
  else if( nret == -2 ) {
    printf("In JSFReadGeneratorBuf::UnpackDST .. ");
    printf("read error detected.\n");
    return kFALSE;
  }

  return kTRUE;
}

// ---------------------------------------------------------------
Bool_t JSFReadGeneratorBuf::ReadOneRecord()
{
// Read Generator data and saved to the class.

// Due to imcompatibility in JSFGeneratorParticle class and HEPEVT,
// second mother information is not saved.
//
// 

  const Int_t kNMXHEP=4000;
  Int_t nevhep, nhep;
  Int_t isthep[kNMXHEP], idhep[kNMXHEP];
  Int_t jmohep[kNMXHEP][2], jdahep[kNMXHEP][2];
  Double_t phep[kNMXHEP][5], vhep[kNMXHEP][4];


  if( !ReadHepEvent(kNMXHEP, nevhep, nhep, isthep, idhep, jmohep, jdahep,
		    phep, vhep) ) return kFALSE;

  // ***************************************
  // Scan data and reassign mother-daughter map
  // ***************************************

  Int_t i;
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
    if( isthep[i] == 0 ) continue;

    nser++;
    jlist[nser]=i;
    /*
    if( jmohep[i][0] < 1 && (isthep[i]==1 || isthep[i]==2 )) {
      printf("Fatal error in JSFReadGenerator::ReadOneRecord().\n");
      printf("jmohep should not be 0 when isthep=1 or 2 but \n");
      printf("jmohep[%d][0]=%d, isthep[%d]=%d\n",i,jmohep[i][0],i,isthep[i]);
      return kFALSE;
    }
    */

    switch ( isthep[i] ) {
      case 1:
        map[i].ndau=0;
        map[i].dau1st=0;
	if( jmohep[i][0] <= 0 ) map[i].mother=0;
        else map[i].mother=map[jmohep[i][0]-1].nser; // modified by I.Nakamura
        map[i].nser=nser;
	if( n1stfinal == 0 ) n1stfinal=nser;
	if( map[i].mother == 0 ) map[i].mother=n1stdoc;
	break;
      case 2:
        map[i].ndau=jdahep[i][1]-jdahep[i][0]+1;
        map[i].dau1st=-2;
	if( jmohep[i][0] <= 0 ) map[i].mother=0;
        else map[i].mother=map[jmohep[i][0]-1].nser; // modified by I.Nakamura
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
        map[i].mother=-isthep[i];
        map[i].nser=nser;
    }
  }

  for(i=0;i<nhep;i++){
    if( map[i].nser == 0 ) continue;
    switch (map[i].dau1st){
      case -2:
	map[i].dau1st=map[jdahep[i][0]-1].nser; // modified by I.Nakamura
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
  
  TClonesArray &particles=*fParticles;

  TVector p(4);
  TVector v(4);
  Int_t iser=0;
  
  Int_t is;
  for(is=1;is<=nser;is++){
    i=jlist[is];
    if( isthep[i] == 0 ) continue;

    Int_t id=idhep[i];
    Float_t mass=phep[i][4];
    Int_t mother=map[i].mother;
    Int_t firstdaughter=map[i].dau1st;
    Int_t ndaughter=map[i].ndau;

#if __PYTHIA_VERSION__ >= 6
    Float_t charge=((Float_t)pychge_(&id))/3.0;
    Int_t kc=pycomp_(&id);
    Float_t xctau=pydat2_.PMAS[3][kc-1]*0.1;
#else
    Float_t charge=((Float_t)luchge_(&id))/3.0;
    Float_t xctau=((Float_t)ulctau_(&id));
#endif
    Float_t dl=0.0;
    //    if( mother < 0 ) xctau=0.0;

    p(0)=phep[i][3]; p(1)=phep[i][0]; p(2)=phep[i][1]; p(3)=phep[i][2];
    v(0)=vhep[i][3]; v(1)=vhep[i][0]; v(2)=vhep[i][1]; v(3)=vhep[i][2];

    iser++;
    new(particles[is-1]) JSFGeneratorParticle(iser, id, mass, charge,
	              p, v, ndaughter, firstdaughter, mother,
		      xctau, dl);
  }

  SetNparticles(iser);

  return kTRUE;
}

#if __ROOT_FULLVERSION >= 30000
//______________________________________________________________________________
void JSFReadGenerator::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFReadGenerator.

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v=R__b.ReadVersion(&R__s, &R__c);
     if( R__v > 1 ) {
       JSFReadGenerator::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
       return;
     }
     JSFGenerator::Streamer(R__b);
     R__b >> fUnit;
     R__b.ReadStaticArray(fFormat);
     R__b.CheckByteCount(R__s, R__c, JSFReadGenerator::IsA());

   } else {
     JSFReadGenerator::Class()->WriteBuffer(R__b, this);
   }

}

#else
//______________________________________________________________________________
void JSFReadGenerator::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFReadGenerator.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      JSFGenerator::Streamer(R__b);
      R__b >> fUnit;
      R__b.ReadStaticArray(fFormat);
   } else {
      R__b.WriteVersion(JSFReadGenerator::IsA());
      JSFGenerator::Streamer(R__b);
      R__b << fUnit;
      R__b.WriteArray(fFormat, 32);
   }
}

#endif
