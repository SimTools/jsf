//*LastUpdate :  jsf-1-12  26-August-1999  A.Miyamoto
//*LastUpdate :  jsf-1-11  23-July-1999  A.Miyamoto
//*LastUpdate :  jsf-1-9   15-May-1999  A.Miyamoto
//*LastUpdate :  jsf-1-7-2  16-April-1999  A.Miyamoto
//*LastUpdate :  jsf-1-7-1  9-April-1999  A.Miyamoto
//*LastUpdate :  jsf-1-7  8-April-1999  A.Miyamoto
//*LastUpdate :  0.04/04  9-November-1998  By A.Miyamoto
//*-- Author  : A.Miyamoto  9-November-1998

/*
 26-August-1999 A.Miyamoto Add pyevwt(wtxs) to provide weight for events.
                           This is a global function, since this is to replace dummy 
                           PYEVWT in Pythia library. (see p.142 of manual)
 23-July-1999 A.Miyamoto Add GetLastRunInfo() to save/restore random seed.
 15-May-1999 A.Miyamoto  Lifetime of particles in documentation line is set 
                         to 0, otherwise initial mu and tau are swimed.
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
//  About generated event information.
//  At the end of run, total cross section and gnumber of generated events are
//  saved togather with random seed into the data member of PythiaGenerator class.
//  Seed of random number are used to continue the random number sequence in 
//  the next run.  Cross section and number of generated events are used
//  to merge events to simulate piled up events.
//
//  Environment parameters and default values used for this classes are as follows.
//  
//  PythiaGenerator.Ecm              500.0  # Center of mass energy (GeV)
//  PythiaGenerator.Frame            CMS    # Event coordinate system
//  PythiaGenerator.BeamParticle     e-     # Name of beam particle.
//  PythiaGenerator.TargetParticle   e+     # Name of target particle.
//  PythiaGenerator.PrintStat        1      # 1: call PYSTAT at the end run, =0 not call.
//
//  Valid values of Frame, BeamParticle, and TragetParticle are described in 
//  the Pythia 5.7 manual page 140. (T.Sjostrand, CPC 82(1994) 74 ).
//
//$Id$
//  
//////////////////////////////////////////////////////////////////

#include "JSFConfig.h"

#include "PythiaGenerator.h"
#include "JSFSteer.h"
#include "TEnv.h"

#include "JSFGenerator.h"

extern "C" {
extern Int_t luchge_(Int_t *kf);
extern Float_t ulctau_(Int_t *kf);
extern void pyevwt_(Float_t *wtxs);
};

ClassImp(PythiaGenerator)

PythiaGenerator *lPythiaGenerator=NULL;

//_____________________________________________________________________________
PythiaGenerator::PythiaGenerator(const Char_t *name, 
				 const Char_t *title, const Char_t *opt)
       : JSFGenerator(name,title, opt)
{

  TEnv *env=gJSF->Env();
  sscanf(env->GetValue("PythiaGenerator.Ecm","500.0"),"%lg",&fEcm);
  sscanf(env->GetValue("PythiaGenerator.Frame","CMS"),"%s",fFrame);
  sscanf(env->GetValue("PythiaGenerator.BeamParticle","e-"),"%s",fBeamParticle);
  sscanf(env->GetValue("PythiaGenerator.TargetParticle","e+"),"%s",fTargetParticle);
  fPrintStat=env->GetValue("PythiaGenerator.PrintStat",1);

  fNUMSUB=0;
  fNGEN=NULL;
  fXSEC=NULL;

  fPythia = new TPythia();
  lPythiaGenerator = this;
}

// ---------------------------------------------------------------
PythiaGenerator::~PythiaGenerator()
{
  if ( fPythia ) { delete fPythia ; fPythia=0 ;  }
  if ( fNGEN ) { delete fNGEN ; fNGEN=NULL ; }
  if ( fXSEC ) { delete fXSEC ; fXSEC=NULL ;}
  fNUMSUB=0;
}


// ---------------------------------------------------------------
Bool_t PythiaGenerator::Initialize()
{
  // Initializer of Pythia generator

  if( !JSFGenerator::Initialize() ) return kFALSE;
  
  fPythia->Initialize(fFrame, fBeamParticle, fTargetParticle, fEcm);

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

   if( fPrintStat ) {
     printf(" End of Pythia run.\n");
     fPythia->PyStat(fPrintStat);
   }

   // Save random seed
   for(Int_t i=0;i<6;i++){ fMRLU[i]=fPythia->GetMRLU(i+1); }
   for(Int_t i=0;i<100;i++){ fRRLU[i]=fPythia->GetRRLU(i+1); }

   // Save PYINT5 information.
   Int_t ns[201];
   ns[0]=0;
   fNUMSUB=1;
   for(Int_t isub=1;isub<=200;isub++){
     if( fPythia->GetMSUB(isub) ) { ns[fNUMSUB]=isub; fNUMSUB++; }
   }
   if( fISUB ) { delete fISUB; }
   if( fNGEN ) { delete fNGEN; }
   if( fXSEC ) { delete fXSEC; }
   
   fISUB=new Int_t[fNUMSUB];
   fNGEN=new Int_t[fNUMSUB];
   fXSEC=new Double_t[fNUMSUB];
   for(Int_t i=0;i<fNUMSUB;i++){
     fISUB[i]=ns[i];
     fNGEN[i]=fPythia->GetNGEN(ns[i], 3);
     fXSEC[i]=fPythia->GetXSEC(ns[i], 3);
   }

   if( fFile->IsWritable() ) {  Write();  }

   return kTRUE;
}

// ---------------------------------------------------------------
Bool_t PythiaGenerator::Process(Int_t ev)
{
//  Generate one pythia event, and results are saved in the 
//  GeneratorParticle class for JSF simulators

  if( !JSFGenerator::Process(ev) ) return kFALSE;

  for(Int_t i=0;i<6;i++){ fMRLU[i]=fPythia->GetMRLU(i+1); }
  for(Int_t i=0;i<100;i++){ fRRLU[i]=fPythia->GetRRLU(i+1); }

  JSFGeneratorBuf *buf=(JSFGeneratorBuf*)EventBuf();
  Double_t ecm=GetEcm();
  buf->SetEcm(ecm);

  fPythia->GenerateEvent();

// Stable particles are saved in the GeneratorParticle class.

   TClonesArray &tracks = *(buf->GetParticles());
   TVector pv(4);
   TVector xv(4);

#if __ROOT_VERSION__ <= 2 &&  __ROOT_MINORVERSION__ <= 21 
   Int_t np=fPythia->GetNumberOfPrimaries();
#else
   Int_t np=fPythia->GetNumberOfParticles();
#endif

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
       xctau=0.0;
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

// ---------------------------------------------------------------
Bool_t PythiaGenerator::GetLastRunInfo()
{

  Float_t ecm=fEcm;
  Read(GetName());

  if( fEcm != ecm ) {
    printf("Error in PythiaGenerator::GetLastRunInfo() \n");
    printf(" Ecm of last run(%g) and current run(%g) is inconsistent.\n",fEcm, ecm);
    return kFALSE;
  }

  for(Int_t i=0;i<6;i++){ fPythia->SetMRLU(i+1, fMRLU[i]); }
  for(Int_t i=0;i<100;i++){ fPythia->SetRRLU(i+1, fRRLU[i]); }
  
  printf("Random seeds for PythiaGenerator were reset by ");
  printf("values from a file.\n");
  return kTRUE;
}

//______________________________________________________________________________
void PythiaGenerator::Streamer(TBuffer &R__b)
{
   // Stream an object of class PythiaGenerator.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      JSFGenerator::Streamer(R__b);
      R__b >> fEcm;
      R__b.ReadStaticArray(fFrame);
      R__b.ReadStaticArray(fBeamParticle);
      R__b.ReadStaticArray(fTargetParticle);
      R__b.ReadStaticArray(fMRLU);
      R__b.ReadStaticArray(fRRLU);
      R__b >> fNUMSUB;

      if( fISUB ) { delete fISUB; } ; 
      fISUB = new Int_t[fNUMSUB];
      R__b.ReadStaticArray(fISUB);

      if( fNGEN ) { delete fNGEN; } ; 
      fNGEN = new Int_t[fNUMSUB];
      R__b.ReadStaticArray(fNGEN);

      if( fXSEC ) { delete fXSEC; } ; 
      fXSEC = new Double_t[fNUMSUB];
      R__b.ReadStaticArray(fXSEC);

   } else {
      R__b.WriteVersion(PythiaGenerator::IsA());
      JSFGenerator::Streamer(R__b);
      R__b << fEcm;
      R__b.WriteArray(fFrame, 8);
      R__b.WriteArray(fBeamParticle, 8);
      R__b.WriteArray(fTargetParticle, 8);
      R__b.WriteArray(fMRLU, 6);
      R__b.WriteArray(fRRLU, 100);
      R__b << fNUMSUB;
      R__b.WriteArray(fISUB, fNUMSUB);
      R__b.WriteArray(fNGEN, fNUMSUB);
      R__b.WriteArray(fXSEC, fNUMSUB);
      
   }
}

//------------------------------------------------------
void pyevwt_(float *wtxs)
{
  // This routine is called when MSTP(142) != 0
  // User should prepare a macro, SetPythiaWeight(), to calculate the weight.
  // A sample SetPythiaWeight() is as follows.
  //  void SetPythiaWeight()
  //  {
  //      PythiaGenerator *py=(PythiaGenerator*)jsf->FindModule("PythiaGenerator");
  //      Float_t weight=1.0;
  //      switch (py->GetPythia()->GetMINT(1)) {  // Decide weight according to the ISUB
  //        case 1:   weight=0.1;  break ;  
  //        case 22:   weight=0.5;  break ;   
  //      }
  //      py->SetEventWeight(weight);   
  //      ...........
  //      return;
  //   }   
  // According to the p.142 of Pythia Manual, the weight should be calculated
  // by using variables MINT and VINT in PYINT1 common block.
  // When the weight is calculated, set its value by PythiaGenerator::SetEventWeight() 
  // function.

  if( gROOT->GetGlobalFunction("SetPythiaWeight",0,kTRUE) ) 
    gROOT->ProcessLine("SetPythiaWeight();");
  else {
    printf("Error .. PYEVWT is called, but SetPythiaWeight macro is not exist.\n");
    printf("Use default event weight of 1.\n");
    lPythiaGenerator->SetEventWeight(1.0);
  }
  *wtxs=lPythiaGenerator->GetEventWeight();
}
