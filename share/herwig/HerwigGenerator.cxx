///////////////////////////////////////////////////////////////////
//
//  Herwig Generator class
// 
//$Id$
//  
//////////////////////////////////////////////////////////////////

#include <iostream>
#include "JSFConfig.h"

#include <TRandom.h>

#include "HerwigGenerator.h"
#include "JSFSteer.h"
#include "TEnv.h"
#include "TSystem.h"

#include "JSFGenerator.h"

ClassImp(HerwigGenerator)

THerwig *HerwigGenerator::fHerwig=0;

extern "C" {
  void myanal_();
};
	
//_____________________________________________________________________________
HerwigGenerator::HerwigGenerator(const Char_t *name, 
				 const Char_t *title, const Char_t *opt)
       : JSFGenerator(name,title, opt)
{

  fHerwig = new THerwig();

  TEnv *env=gJSF->Env();
  sscanf(env->GetValue("HerwigGenerator.Ecm","300.0"),"%lg",&fEcm);
  fPrintStat=env->GetValue("HerwigGenerator.PrintStat",1);
  fIPROC=env->GetValue("HerwigGenerator.IPROC",100); // e+e- -> hadron

  sscanf(env->GetValue("HerwigGenerator.BeamStrahlung","0"),"%d",&fBeamStrahlung);
  Char_t bspname[32];
  sscanf(env->GetValue("HeerwigGenerator.BSParameter","trc500"),"%s",bspname);
  fBSname=bspname;
  sscanf(env->GetValue("JSFBeamGeneration.Width","-1.0"),"%lg",&fBSwidth);

  sscanf(env->GetValue("HerwigGenerator.BSThreshold","10.0"),"%lg",&fBSThreshold);


  fEventWeight=1.0;
  fBS = 0;
  fBSFile = 0;

  fNBSGen=0;
  fNBSGood=0;


}

// ---------------------------------------------------------------
HerwigGenerator::~HerwigGenerator()
{
  if ( fHerwig ) { delete fHerwig ; fHerwig=0 ;  }
  if ( fBS ) { delete fBS ; fBS=NULL ;}
  if ( fBSFile ) { fBSFile->Close(); delete fBSFile ; fBSFile=NULL; }
}


// ---------------------------------------------------------------
Bool_t HerwigGenerator::Initialize()
{
  // Initializer of Pythia generator

  if( !JSFGenerator::Initialize() ) return kFALSE;
  
  /*
  if( fBeamStrahlung != 0 ) {
    fPythia->SetMSTP(171,1);  // Trun on beamstrahlung
    fPythia->SetMSTP(172,1);  // Generate event at requested energy
    
    Char_t bsfile[256];
    sprintf(bsfile,"%s/data/bsdata/%s.root",gSystem->Getenv("JSFROOT"),fBSname.Data());
    fBSFile=new TFile(bsfile);
    if( fBSFile == 0 ) {
      printf("Error in PythiaGenerator::Initialize\n");
      printf("Unable to open file for Beamstrahlung : %s\n",bsfile);
      exit(-1);
    }
    fBS=(JSFBeamGenerationCain*)fBSFile->Get(fBSname.Data());
    if( fBSwidth < 0.0 ) {
      fBSwidth=fBS->GetIBWidth();
    }      
    fBS->SetIBParameters( fEcm/2.0, fBSwidth , JSFBeamGenerationCain::kUniform ); 
    fBS->MakeBSMap();
    fBS->Print();
    //    fEcmMax=fEcm*(1+2*fBSwidth);
    fEcmMax=fEcm*(1+fBSwidth);
    fPythia->Initialize(fFrame, fBeamParticle, fTargetParticle, fEcmMax);
  }
  else {
    fPythia->Initialize(fFrame, fBeamParticle, fTargetParticle, fEcm);
  }
  */
  Double_t ebeam=fEcm/2.0;
  fHerwig->SetBeams("E-","E+",ebeam, ebeam, fIPROC);
  
  fHerwig->Initialize();

  Int_t maxpr=gJSF->Env()->GetValue("HerwigGenerator.MAXPR",1);
  fHerwig->SetMAXPR(maxpr);

  return kTRUE;
}

// ---------------------------------------------------------------
Bool_t HerwigGenerator::BeginRun(Int_t nrun)
{
   if( !JSFGenerator::BeginRun(nrun) ) return kFALSE;
   return kTRUE;
}

// ---------------------------------------------------------------
Bool_t HerwigGenerator::EndRun()
{
   if( !JSFGenerator::EndRun() ) return kFALSE;

   if( fPrintStat ) {
     printf(" End of Herwig run.\n");
     fHerwig->Terminate();
   }

   // Save random seed
   fIBRN[0]=fHerwig->GetIBRN(1);
   fIBRN[1]=fHerwig->GetIBRN(2);

   if( fFile->IsWritable() ) {  Write();  }

   return kTRUE;
}

// ---------------------------------------------------------------
Bool_t HerwigGenerator::Process(Int_t ev)
{
//  Generate one pythia event, and results are saved in the 
//  GeneratorParticle class for JSF simulators


  if( !JSFGenerator::Process(ev) ) return kFALSE;

  fHerwig->GenerateEvent();

  JSFGeneratorBuf *buf=(JSFGeneratorBuf*)EventBuf();

  Double_t ecm=GetEcm();
  buf->SetEcm(ecm);

  Int_t np=HepevtToGeneratorParticles(fHerwig->GetHEPEVT());

  if( np < 0 ) { return kFALSE; }
 
  return kTRUE;
 
}

//______________________________________________________________________________
void HerwigGenerator::GetChargeCtau(Int_t i, Float_t &charge, Float_t &xctau)
{
  // Returns charge of i-th particle in HEPEVT buffer

  THerwigParticle pp=fHerwig->GetParticleProperties(i);
  charge = pp.GetTrueCharge();
  Double_t ctau = pp.GetCtau();
  if( ctau > 1.e10 ) {
    xctau=1.e10;
  }
  else {
    xctau=ctau;
  }
}


//______________________________________________________________________________
Int_t HerwigGenerator::HepevtToGeneratorParticles(COMMON_HEPEVT_t *hepevt)
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
    if( hepevt->ISTHEP[i] >= 100 && hepevt->ISTHEP[i] <= 103 ) continue;
    if( hepevt->ISTHEP[i] == 3 ) continue;

    nser++;
    jlist[nser]=i;

    Int_t tst= hepevt->ISTHEP[i];

    if( tst == 1 ) {
      map[i].ndau=0;
      map[i].dau1st=0;
      if( hepevt->JMOHEP[i][0] <= 0 ) map[i].mother=0;
      else map[i].mother=map[hepevt->JMOHEP[i][0]-1].nser; // modified by I.Nakamura
      map[i].nser=nser;
      if( n1stfinal == 0 ) n1stfinal=nser;
      if( map[i].mother == 0 ) map[i].mother=n1stdoc;
    }
    else {
      if( hepevt->JDAHEP[i][0] == 0 || hepevt->JDAHEP[i][1] == 0 ) {
	cout << "Error  in HerwigGenerator::HepevtToGeneratorParticles" ;
	cout << " Unexpected status code " << tst << endl;
      }
      map[i].ndau=TMath::Abs(hepevt->JDAHEP[i][1] - hepevt->JDAHEP[i][0]) + 1;
      map[i].dau1st=-2;
      if(  hepevt->JMOHEP[i][0] <= 0 ) map[i].mother=0;
      else map[i].mother=map[hepevt->JMOHEP[i][0]-1].nser; // modified by I.Nakamura
      map[i].nser=nser;
      if( n1stfinal == 0 ) n1stfinal=nser;
      if( map[i].mother == 0 ) map[i].mother=n1stdoc;
    }
  }

  for(i=0;i<nhep;i++){
    if( map[i].nser == 0 ) continue;
    switch (map[i].dau1st){
      case -2:
	if( hepevt->JDAHEP[i][0] < hepevt->JDAHEP[i][1] ) {
	  map[i].dau1st=map[hepevt->JDAHEP[i][0]-1].nser; // modified by I.Nakamura
	}
	else {
	  map[i].dau1st=map[hepevt->JDAHEP[i][1]-1].nser; // modified by I.Nakamura
	}
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
    px(2)=hepevt->PHEP[i][1]; px(3)=hepevt->PHEP[i][2]; 

    vp(0)=hepevt->VHEP[i][3]; vp(1)=hepevt->VHEP[i][0]; 
    vp(2)=hepevt->VHEP[i][1]; vp(3)=hepevt->VHEP[i][2]; 

    iser++;
    new(ps[is-1]) JSFGeneratorParticle(iser, id, mass, charge,
	              px, vp, ndaughter, firstdaughter, mother,
		      xctau, dl);
  }

  buf->SetNparticles(iser);

  return kTRUE;
}



// ---------------------------------------------------------------
Bool_t HerwigGenerator::Terminate()
{
  if( !JSFGenerator::Terminate() ) return kFALSE;

  if( fBeamStrahlung != 0 ) {
    cout << " *********************** " << endl;
    cout << " Number of call to BSGenerator =" << fNBSGen << endl;
    cout << " Number of good BSGencall      =" << fNBSGood << endl;    
    cout << " Fraction :                    =" << 
      (Double_t)fNBSGood/(Double_t)fNBSGen << endl;
  }

  return kTRUE;
}

