
///////////////////////////////////////////////////////////////////
//
//  JSF Generator
//
//  The first example to generate simple generator track 
//  in the JLC Study Framework.
//  
//////////////////////////////////////////////////////////////////

#include "PythiaGenerator.h"
#include "JSFLCFULL.h"
#include "JSFSteer.h"
#include "TEnv.h"


extern "C" { 
extern void p55evt_(int *recid, int* level, int *debug, int *qualit);
extern void p55com_rooti_(float *ecm);
extern void p55com_root_(float *ecm, int *plund, int *iseed, int *isavelu);
extern void p55bru_(int *level, int *debug);
extern void p55eru_(int *level, int *debug);
extern void p55ini_();
extern void p55trm_();
};

ClassImp(PythiaGenerator)

//_____________________________________________________________________________
PythiaGenerator::PythiaGenerator(const char *name, const char *title)
       : JSFFULLGenerator(name,title)
{

  TEnv *env=gJSF->Env();

  sscanf(env->GetValue("PythiaGenerator.Ecm","500.0"),"%lg",&fEcm);

  Char_t process[100];

  sscanf(env->GetValue("PythiaGenerator.Process","WW"),"%s",process);

  // Set parameters for some frequently used process.
  if( strcmp(process,"WW")==0) {
     SetParam("PYSUBS","MSEL",1);
     SetParam("PYSUBS","CKIN",0,20.0);
     SetParam("PYSUBS","CKIN",2, 0.0);
     SetParam("PYPARS","MSTP",1,1);
     SetParam("PYPARS","MSTP",121,1);
     SetParam("PYPARS","MSTP",10,1);
     SetParam("PYPARS","MSTP",32,1);
     SetParam("LUDAT1","MSTU",10,6);
     SetParam("LUDAT1","MSTU",11,1);
  }

}

// ---------------------------------------------------------------
PythiaGenerator::~PythiaGenerator()
{
}


// ---------------------------------------------------------------
Bool_t PythiaGenerator::Initialize()
{
  // Initializer of Pythia generator

  if( !JSFFULLGenerator::Initialize() ) return kFALSE;
  Float_t ecm=(Double_t)fEcm;
  p55com_rooti_(&ecm);
  p55ini_();
  return kTRUE;
}

// ---------------------------------------------------------------
Bool_t PythiaGenerator::BeginRun(Int_t nrun)
{
   if( !JSFGenerator::BeginRun(nrun) ) return kFALSE;
   Int_t level=1;
   Int_t debug=0;
   p55bru_(&level, &debug);
   return kTRUE;
}

// ---------------------------------------------------------------
Bool_t PythiaGenerator::EndRun()
{
   if( !JSFFULLGenerator::EndRun() ) return kFALSE;
   Int_t level=1;
   Int_t debug=0;
   p55eru_(&level, &debug);
   return kTRUE;
}

// ---------------------------------------------------------------
Bool_t PythiaGenerator::Process(Int_t ev)
{
  if( !JSFFULLGenerator::Process(ev) ) return kFALSE;

  Int_t ir;
  Int_t level=1;
  Int_t debug=0;
  int recid=1;
  ir=0;
  p55evt_(&recid, &level, &debug, &ir);

  TBGET();
  
  return kTRUE;
 
}

// ---------------------------------------------------------------
Bool_t PythiaGenerator::Terminate()
{
  if( !JSFFULLGenerator::Terminate() ) return kFALSE;

  p55trm_();
  return kTRUE;
}



// ---------------------------------------------------------------
void PythiaGenerator::SetParam(const Char_t *common, const Char_t *vname,
      Int_t value)
{
  if( strcmp(common,"PYSUBS") == 0 ) {
    if( strcmp(vname,"MSEL") == 0 ) pysubs_.msel=value;
  }
  else {
    Warning("SetParam","Common %s or Variable %s is unrecognized",
	    common, vname);
  }
}


// ---------------------------------------------------------------
void PythiaGenerator::SetParam(const Char_t *common, const Char_t *vname,
      Int_t index, Double_t value)
{
  if( strcmp(common,"PYSUBS") == 0 ) {
    if( strcmp(vname,"CKIN") == 0 ) pysubs_.ckin[index]=(Float_t)value;
  }
  else if( strcmp(common,"PYPARS") == 0 ) {
    if( strcmp(vname,"PARP") == 0 ) pypars_.parp[index]=(Float_t)value;
    if( strcmp(vname,"PARI") == 0 ) pypars_.pari[index]=(Float_t)value;
  }
  else if( strcmp(common,"LUDAT1") == 0 ) {
    if( strcmp(vname,"PARJ") == 0 ) ludat1_.parj[index]=(Float_t)value;
  }
  else {
    Warning("SetParam","Common %s or Variable %s is unrecognized",
	    common, vname);
  }

}


// ---------------------------------------------------------------
void PythiaGenerator::SetParam(const Char_t *common, const Char_t *vname,
      Int_t index, Int_t value)
{
  if( strcmp(common,"PYSUBS") == 0 ) {
    if( strcmp(vname,"MSUB") == 0 ) pysubs_.msub[index]=value;
  }
  else if( strcmp(common,"PYPARS") == 0 ) {
    if( strcmp(vname,"MSTP") == 0 ) pypars_.mstp[index]=value;
    if( strcmp(vname,"MSTI") == 0 ) pypars_.msti[index]=value;
  }
  else if( strcmp(common,"LUDAT1") == 0 ) {
    if( strcmp(vname,"MSTU") == 0 ) ludat1_.mstu[index]=value;
    if( strcmp(vname,"PARU") == 0 ) ludat1_.paru[index]=value;
    if( strcmp(vname,"MSTJ") == 0 ) ludat1_.mstj[index]=value;
  }
  else {
    Warning("SetParam","Common %s or Variable %s is unrecognized",
	    common, vname);
  }

}


// ---------------------------------------------------------------
void PythiaGenerator::SetParam(const Char_t *common, const Char_t *vname,
      Int_t ind1, Int_t ind2, Int_t value)
{
  if( strcmp(common,"PYSUBS") == 0 ) {
    if( strcmp(vname,"KFIN") == 0 ) pysubs_.kfin[ind1][ind2]=value;
  }
  else {
    Warning("SetParam","Common %s or Variable %s is unrecognized",
	    common, vname);
  }

}



