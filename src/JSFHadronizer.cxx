//*LastUpdate:  jsf-1-11  23-July-1999  by A.Miyamoto
//*LastUpdate:  v0.2.01 09/09/1998  by A.Miyamoto
//*-- Author :  Akiya Miyamoto  09/09/1998

//////////////////////////////////////////////////////////////////
//
//  JSFHadronizer
//  
//  Hadronize parton information in JSFParton class and 
//  create JSFGeneratorParticle class.
//  
//$Id$  
//////////////////////////////////////////////////////////////////


#include "TRandom.h"

#include "JSFSteer.h"
#include "JSFLCFULL.h"
#include "JSFHadronizer.h"

extern "C" {
extern void  luhadr_(int *idrec, int *level, int *idebug, int *iret);
extern void  tauint_(int *inut, int *iout, int *jak1, 
		     int *jak2, int *itdkrc, int *keya1, float *xk0dec);
extern void  lutuhl_(int *one, int *ihlon);
};

Int_t JAK1, JAK2, ITDKRC, KEYA1, IHLON;
Float_t XK0DEC;

ClassImp(JSFHadronizer)

typedef struct {
  Int_t mdcy[3][500], mdme[2][2000];
  Float_t brat[2000];
  Int_t kfdp[5][2000];
} COMMON_LUDAT3;
extern COMMON_LUDAT3 ludat3_;

typedef struct {  // Common for JETSET random variables
  Int_t mrlu[6];
  Float_t rrlu[100];
} COMMON_LUDATR;
extern COMMON_LUDATR ludatr_;

typedef struct { // Commo for Tauola random variables
  Float_t  u[98];
  Int_t ij97[2];
} COMMON_RASET1;
extern COMMON_RASET1 raset1_;

//_____________________________________________________________________________
JSFHadronizer::JSFHadronizer(const char *name, const char *title)
       : JSFFULLGenerator(name,title)
{
//  Constructor of JSFHadronizer.  In this constructor, JSFSpring
//  module is searched from the defined JSF module.  last JSFModule
//  which is inherited from JSFSpring class is a class whose Parton
//  data is hadronized.

  fSpring=0;
  fCopySpringClassDataToBank=kTRUE;

  TList *list=gJSF->Modules();
  TIter  next(list);
  JSFModule *module;
  while( (module=(JSFModule*)next()) ){
    if( module->InheritsFrom("JSFSpring") ){ 
      if( fSpring ) { 
	 printf("Find module %s , inherited from JSFSpring\n",fSpring->GetName());
	 printf("More than one JSFSpring are defined.");
	 printf(" Specify correspinding Hadronizer explicityly\n");
      }
      fSpring=(JSFSpring*)module;
    }
  }
  if( !fSpring ){ Error("JSFHadronizer","No JSFSpring class was found"); }

  
  JAK1 = gJSF->Env()->GetValue("JSFHadronizer.JAK1",0);
  JAK2 = gJSF->Env()->GetValue("JSFHadronizer.JAK2",0);
  ITDKRC = gJSF->Env()->GetValue("JSFHadronizer.ITDKRC",1);
  KEYA1 = gJSF->Env()->GetValue("JSFHadronizer.KEYA1",1);
  IHLON = gJSF->Env()->GetValue("JSFHadronizer.IHLON",1);
  sscanf(gJSF->Env()->GetValue("JSFHadronizer.XK0DEC","0.001"),"%g",&XK0DEC);

}

//_____________________________________________________________________________
Bool_t JSFHadronizer::Initialize()
{
// Initialize tauola

   Int_t inut=5;
   Int_t iout=6;
   tauint_(&inut, &iout, &JAK1, &JAK2, &ITDKRC,  &KEYA1, &XK0DEC);
   Int_t one=1;
   lutuhl_(&one, &IHLON);

   ludat3_.mdcy[0][22]=0;
   ludat3_.mdcy[0][23]=0;
   ludat3_.mdcy[0][32]=0;

   return kTRUE ;

}

//_____________________________________________________________________________
Bool_t JSFHadronizer::Process(Int_t ev)
{
//  Create JSFGeneratorParticle from JSFSpringParton.
//  For a time bing, first put JSFSpringParton in to TBS bank,
//  calls luhadr fortran program, and put results into JSFGeneratorParticle
//
 
  JSFFULLGenerator::Process(ev);

  Int_t i;
  for(i=0;i<6;i++){ fMRLU[i]=ludatr_.mrlu[i];}
  for(i=0;i<100;i++){ fRRLU[i]=ludatr_.rrlu[i];}
  for(i=0;i<98;i++){ fRASET1U[i]=raset1_.u[i];}
  for(i=0;i<2;i++){ fRASET1IJ97[i]=raset1_.ij97[i];}

  if( fCopySpringClassDataToBank ) TBPUT(fSpring);

  Int_t idrec=1;
  Int_t level=1;
  Int_t idebug=0;
  Int_t iret;

  luhadr_(&idrec, &level, &idebug, &iret);

  TBGET();  // Put hadronized results into Generator class.

  // Remove Generator banks, since they are not necessary anymore.
  gJSFLCFULL->TBDELB(1,"Spring:Parton_List",iret);
  gJSFLCFULL->TBDELB(1,"Spring:Header",iret);
  gJSFLCFULL->TBDELB(1,"Generator:Particle_List",iret);
  gJSFLCFULL->TBDELB(1,"Generator:Header",iret);

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFHadronizer::EndRun()
{
  Int_t i;
  for(i=0;i<6;i++){ fMRLU[i]=ludatr_.mrlu[i];}
  for(i=0;i<100;i++){ fRRLU[i]=ludatr_.rrlu[i];}
  for(i=0;i<98;i++){ fRASET1U[i]=raset1_.u[i];}
  for(i=0;i<2;i++){ fRASET1IJ97[i]=raset1_.ij97[i];}

  if( fFile->IsWritable() ) {
    if( !JSFFULLGenerator::EndRun() ) return kFALSE;
    Write();
  }
  return kTRUE;
}


//_____________________________________________________________________________
Bool_t JSFHadronizer::GetLastRunInfo()
{
  // Read seed of previous run 

  Read(GetName());

  //  for(Int_t i=0;i<6;i++){ ludatr_.mrlu[i]=fMRLU[i];}
  //for(Int_t i=0;i<100;i++){ ludatr_.rrlu[i]=fRRLU[i];}
  //for(Int_t i=0;i<98;i++){ raset1_.u[i]=fRASET1U[i];}
  //for(Int_t i=0;i<2;i++){ raset1_.ij97[i]=fRASET1IJ97[i];}

  printf("Random seeds for JSFHadronizer were reset by ");
  printf("values from a file.\n");

  return kTRUE;
}

