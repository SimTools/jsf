//*LastUpdate:  v0.2.01 09/09/1998  by A.Miyamoto
//*-- Author :  Akiya Miyamoto  09/09/1998

//////////////////////////////////////////////////////////////////
//
//  JSFHadronizer
//  
//  Hadronize parton information in JSFParton class and 
//  create JSFGeneratorParticle class.
//  
//////////////////////////////////////////////////////////////////


#include "TRandom.h"

#include "JSFSteer.h"
#include "JSFLCFULL.h"
#include "JSFHadronizer.h"

extern "C" {
extern void  luhadr_(int *idrec, int *level, int *idebug, int *iret);
};


ClassImp(JSFHadronizer)

//_____________________________________________________________________________
JSFHadronizer::JSFHadronizer(const char *name, const char *title)
       : JSFFULLGenerator(name,title)
{
//  Constructor of JSFHadronizer.  In this constructor, JSFSpring
//  module is searched from the defined JSF module.  last JSFModule
//  which is inherited from JSFSpring class is a class whose Parton
//  data is hadronized.

  fSpring=0;
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
}

//_____________________________________________________________________________
Bool_t JSFHadronizer::Process(Int_t ev)
{
//  Create JSFGeneratorParticle from JSFSpringParton.
//  For a time bing, first put JSFSpringParton in to TBS bank,
//  calls luhadr fortran program, and put results into JSFGeneratorParticle
//
 
  JSFFULLGenerator::Process(ev);

  TBPUT(fSpring);

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
