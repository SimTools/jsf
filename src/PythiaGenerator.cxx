//*LastUpdate :  0.04/04  9-November-1998  By A.Miyamoto
//*-- Author  : A.Miyamoto  9-November-1998
///////////////////////////////////////////////////////////////////
//
//  PythiaGenerator class
//
//  To run Pythia in JLC Study Framework
//  
//////////////////////////////////////////////////////////////////

#include "PythiaGenerator.h"
#include "JSFSteer.h"
#include "TEnv.h"

#include "JSFGenerator.h"

// extern "C" { 
// };

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

  fPythia->GenerateEvent();

// Stable particles are saved in the GeneratorParticle class.

   JSFGeneratorBuf *buf=(JSFGeneratorBuf*)EventBuf();
   TClonesArray &tracks = *(buf->GetParticles());
   TVector pv(4);
   TVector xv(4);

   Int_t np=fPythia->GetNumberOfPrimaries();
   TObjArray *pa=fPythia->GetPrimaries();
   Int_t n0=fPythia->GetMSTI(4);

   Int_t nout=0;
   Int_t ks,kf,kh,kfa;
   Float_t charge, xctau, life, mass;
   Int_t idhist[4001];
   for(Int_t i=0;i<4001;i++){ idhist[i]=0 ; }
   Int_t  ndaughter, firstdaughter, mother ;

   for(i=n0+1;i<=np;i++) {
     TMCParticle *p=(TMCParticle*)pa->UncheckedAt(i-1);
     ks=p->GetKS();
     kf=p->GetKF();
     kh=p->GetParent();
     idhist[i]=0;
     kfa=TMath::Abs(kf);
     if( (kfa >= 1 && kfa <= 10) || kfa == 21 ) continue ;
     if(  kfa >=23 && kfa <= 100 ) continue ;
     Float_t pt2=p->GetPx()*p->GetPx() + p->GetPy()*p->GetPy(); 
     if( pt2 < 1.e-12 ) continue ;

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
     mother=0;
     life=0.0;
     if( kh != 0 ) {
       Int_t ipar=idhist[kh];
       if( ipar > 0 ) {
       JSFGeneratorParticle *gp=
	 (JSFGeneratorParticle*)tracks.UncheckedAt(ipar-1);
	 gp->fNdaughter++;
	 if( gp->fNdaughter == 1 ) gp->fFirstDaughter=nout;
	 mother=ipar;
       }
     }
     new(tracks[nout-1]) 
       JSFGeneratorParticle(nout, kf,(Double_t)mass,(Double_t)charge, pv, xv , 
	    ndaughter, firstdaughter, mother, (Double_t)life, (Double_t)xctau ) ;

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



