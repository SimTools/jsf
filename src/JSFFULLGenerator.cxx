//*LastUpdate :  0.02/02  11-September-1998  By A.Miyamoto
//*-- Author  : A.Miyamoto  11-September-1998

////////////////////////////////////////////////////////////////////////
//
// JSFFULLGenerator
//
//  Convert FULL data stored in TBS bank to/from JSF classes.
//
////////////////////////////////////////////////////////////////////////


#include "TRandom.h"

#include "JSFLCFULL.h"
#include "JSFFULLGenerator.h"
#include "JSFSpring.h"

ClassImp(JSFFULLGenerator)


//_____________________________________________________________________________
JSFFULLGenerator::JSFFULLGenerator(const char *name, const char *title)
       : JSFGenerator(name,title)
{
}

//_____________________________________________________________________________
JSFFULLGenerator::~JSFFULLGenerator()
{
}

// ---------------------------------------------------------------
void JSFFULLGenerator::TBPUT(JSFSpring *spring)
{
//  Create TBS banks, 
//      Spring:Parton_List, Spring:Header,
//      Generator:Particle_List, Generator:Header
//  and save parton information saved in JSFSpringParton class
//  into Spring:Parton_List bank.
//
  Int_t iret;
  gJSFLCFULL->TBCRTE(1,"Spring:Parton_List",0,0, iret);
  gJSFLCFULL->TBCRTE(1,"Spring:Header",0,0, iret);
  gJSFLCFULL->TBCRTE(1,"Generator:Particle_List",0,0, iret);
  gJSFLCFULL->TBCRTE(1,"Generator:Header",0,0, iret);

  Float_t data[20];
  JSFSpringBuf *spevt=(JSFSpringBuf*)spring->EventBuf();
  TClonesArray *ps=spevt->GetPartons();
  for(Int_t j=0;j<spevt->GetNpartons();j++){
    JSFSpringParton *p=(JSFSpringParton*)ps->At(j);
    data[0]=p->fSer;
    data[1]=p->fID;
    data[2]=p->fMass;
    data[3]=p->fCharge;
    data[4]=p->fP[1];
    data[5]=p->fP[2];
    data[6]=p->fP[3];
    data[7]=p->fP[0];
    data[11]=p->fNdaughter;
    data[12]=p->fFirstDaughter;
    data[13]=p->fMother;
    data[16]=p->fHelicity;
    data[17]=p->fColorID;
    data[18]=p->fShowerInfo;
    Int_t ielm=j+1;
 
    gJSFLCFULL->TBPUT(1,"Spring:Parton_List",ielm,20,(Int_t*)data,iret);
  }
}

// ---------------------------------------------------------------
void JSFFULLGenerator::TBGET()
{
// Copy Generator:Particle_List Banks of TBS to PythiaGeneratorParticle class.
//
  Int_t nelm, neary[1000];
  Int_t iret, nw;
  Float_t data[20];
  Int_t   idata[20];
  JSFLCFULL *lcfull;
  lcfull=gJSFLCFULL;
  JSFGeneratorBuf *buf=(JSFGeneratorBuf*) EventBuf();

  // Save Header information
  lcfull->TBGET(1,"Generator:Header",1,nw,idata,iret);
  buf->SetStartSeed(idata[4]);
  buf->SetEcm(idata[6]*0.002);

  // Save Particle information
  lcfull->TBNOEL(1,"Generator:Particle_List",nelm, neary);
  
  TClonesArray *temp = buf->GetParticles();
  TClonesArray &particles = *temp;
  Int_t npart=0;


  for(Int_t j=0;j<nelm;j++){
     lcfull->TBGET(1,"Generator:Particle_List",neary[j],nw,(Int_t*)data,iret);
     new( particles[npart++] ) JSFGeneratorParticle(data);
  }
  buf->SetNparticles(npart);


}
