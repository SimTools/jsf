//*LastUpdate :  jsf-1-7-2  16-April-1999  A.Miyamoto
//*LastUpdate :  jsf-1-7-1  9-April-1999  A.Miyamoto
//*LastUpdate :  jsf-1-7  8-April-1999  A.Miyamoto
//*LastUpdate :  0.04/04  9-November-1998  By A.Miyamoto
//*-- Author  : A.Miyamoto  9-November-1998

/*
 16-Apr-1999 A.Miyamoto  A bug(mother of particle was not set) was fixed.
  9-Apr-1999 A.Miyamoto  A bug to pickup stable particle from LUJETS is fixed.
  8-Apr-1999 A.Miyamoto  Save all particles in LUJETS common to 
                         JSFGeneratorParticle class
*/

///////////////////////////////////////////////////////////////////
//
//  PythiaGenerator class
// 
//  To run Pythia in JLC Study Framework
//
//  From version JSF-1-7, PythiaGenerator class is modified to save all
//  LUJETS common as JSFGeneratorParticle class.  When the class is prepared,
//  JSFGeneratorParticle::fMother < 0, if it is the documentation line in 
//  LUJETS common ( KS(K(i,1)) code is 21 ), with is absolute value is
//  the pointer to the mother.  First two entries of JSFGeneratorParticle class
//  are initial e- and e+ and their fMother = 0.  Also first particles of the
//  final state have fMother=0.  fNDaughter for them are always 1 and pointer
//  to the first daughter points first particle in the final state.  
//   
//$ID:
//  
//////////////////////////////////////////////////////////////////

#include "PythiaGenerator.h"
#include "JSFSteer.h"
#include "TEnv.h"

#include "JSFGenerator.h"

extern "C" {
extern Int_t luchge_(Int_t *kf);
extern Float_t ulctau_(Int_t *kf);
};

ClassImp(PythiaGenerator)

//_____________________________________________________________________________
PythiaGenerator::PythiaGenerator(const char *name, const char *title)
       : JSFGenerator(name,title)
{

  TEnv *env=gJSF->Env();
  sscanf(env->GetValue("PythiaGenerator.Ecm","500.0"),"%lg",&fEcm);

  fPythia = new TPythia();
}

// ---------------------------------------------------------------
PythiaGenerator::~PythiaGenerator()
{
  if ( fPythia ) delete fPythia ;
}


// ---------------------------------------------------------------
Bool_t PythiaGenerator::Initialize()
{
  // Initializer of Pythia generator

  if( !JSFGenerator::Initialize() ) return kFALSE;
  
  fPythia->Initialize("CMS", "e-", "e+", fEcm);

  return kTRUE;
}

// ---------------------------------------------------------------
Bool_t PythiaGenerator::BeginRun(Int_t nrun)
{
   if( !JSFGenerator::BeginRun(nrun) ) return kFALSE;
   return kTRUE;
}

// ---------------------------------------------------------------
Bool_t PythiaGenerator::EndRun()
{
   if( !JSFGenerator::EndRun() ) return kFALSE;
   return kTRUE;
}

// ---------------------------------------------------------------
Bool_t PythiaGenerator::Process(Int_t ev)
{
//  Generate one pythia event, and results are saved in the 
//  GeneratorParticle class for JSF simulators

  if( !JSFGenerator::Process(ev) ) return kFALSE;

  JSFGeneratorBuf *buf=(JSFGeneratorBuf*)EventBuf();
  Double_t ecm=GetEcm();
  buf->SetEcm(ecm);

  fPythia->GenerateEvent();

// Stable particles are saved in the GeneratorParticle class.

   TClonesArray &tracks = *(buf->GetParticles());
   TVector pv(4);
   TVector xv(4);

   Int_t np=fPythia->GetNumberOfPrimaries();
   TObjArray *pa=fPythia->GetPrimaries();

   Int_t nout=0;
   Int_t ks,kf,kh,kfa;
   Float_t charge, xctau, dl, mass;
   Int_t idhist[4001];
   Int_t i;
   for(i=0;i<4001;i++){ idhist[i]=0 ; }
   Int_t  ndaughter, firstdaughter, mother ;

   for(i=1;i<=np;i++) {
     TMCParticle *p=(TMCParticle*)pa->UncheckedAt(i-1);
     ks=p->GetKS();
     kf=p->GetKF();
     kh=p->GetParent();
     idhist[i]=0;
     kfa=TMath::Abs(kf);

     nout++;
     idhist[i] = nout ;

     pv(0)=p->GetEnergy();
     pv(1)=p->GetPx();
     pv(2)=p->GetPy();
     pv(3)=p->GetPz();
     xv(0)=0.0; xv(1)=0.0 ; xv(2)=0.0 ; xv(3)=0.0 ; 
     charge=((Float_t)luchge_(&kf))/3.0;
     xctau=((Float_t)ulctau_(&kf));
     mass=p->GetMass();
     ndaughter=0;
     firstdaughter=0;
     mother=kh;
     dl=0.0;
     //
     if( ks == 21 ) {
       ndaughter = 1;
       mother = -kh;
       firstdaughter=fPythia->GetMSTI(4)+1;  
     }
     else if( ks != 1 ) {
       firstdaughter=p->GetFirstChild();
       ndaughter=p->GetLastChild()-firstdaughter+1;
     }

     new(tracks[nout-1]) 
       JSFGeneratorParticle(nout, kf,mass,charge, pv, xv , 
	    ndaughter, firstdaughter, mother, xctau, dl ) ;

   }
  buf->SetNparticles(nout);
  
  return kTRUE;
 
}

// ---------------------------------------------------------------
Bool_t PythiaGenerator::Terminate()
{
  if( !JSFGenerator::Terminate() ) return kFALSE;

  return kTRUE;
}



