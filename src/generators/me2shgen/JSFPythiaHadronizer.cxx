//_____________________________________________________________________________
//
//  JSF's Abstract class for Matrix Element generator
//
//  Based on a ganerator, JSFHEPRUP and JSFHEPEUP class must be
//  Created for subsequent use by Shower and Hadronize Generator
//
//$ID$
//_____________________________________________________________________________


#include "JSFConfig.h"
#include "JSFPythiaHadronizer.h"
#include "TPythia6.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,17,0)
#include "TMCParticle6.h"
#else
#include "TMCParticle.h"
#endif
#include "JSFMEGenerator.h"
#include "TPythia6Calls.h"

#include <iostream>
using namespace std;

ClassImp(JSFPythiaHadronizer)
ClassImp(JSFPythiaHadronizerBuf)

extern "C" {
  void* pythia6_common_block_address_(char* name, int len);
  Int_t pychge_(Int_t *kf);
};
 
//_____________________________________________________________________________
JSFPythiaHadronizer::JSFPythiaHadronizer(const char *name, const char *title) 
  : JSFSHGenerator(name,title)
{
// Create one JSFSpring object
// User class should set addresses to the event buf here, as follows.
//
  fEventBuf = new JSFPythiaHadronizerBuf("JSFPythiaHadronizerBuf",
	    "JSF Pythia Shower/Hadronizer ",this);

  fPythia = new TPythia6();

  fHEPEUPFormat = gJSF->Env()->GetValue("PythiaHadronizer.HEPEUPFormat",0);

}

//_____________________________________________________________________________
JSFPythiaHadronizer::~JSFPythiaHadronizer()
{
  if( fPythia ) delete fPythia;
}

// ---------------------------------------------------------------
Bool_t JSFPythiaHadronizer::Initialize()
{
  if( !JSFSHGenerator::Initialize() ) return kFALSE;

  //  fPythia->SetMDCY(25,1,0);  // suppress higgs decay

  return kTRUE;
}

//__________________________________________________________________________
Bool_t JSFPythiaHadronizer::BeginRun(Int_t nrun)
{

  if( !JSFSHGenerator::BeginRun(nrun) ) return kFALSE;

  JSFMEGenerator *megen=(JSFMEGenerator*)gJSF->FindModule("JSFMEGenerator");
  JSFHEPRUP *heprup=megen->GetJSFHEPRUP();

  void *address=pythia6_common_block_address_("HEPRUP",6);

  heprup->Save(address);

  fPythia->Initialize("USER","","",0.0e0);

  return kTRUE;
}  

//__________________________________________________________________________
Bool_t JSFPythiaHadronizer::Process(Int_t nevent)
{

  if( !JSFSHGenerator::Process(nevent) ) return kFALSE;

  // Copy seed at the begining of this event
  Int_t i;
  for(i=0;i<6;i++){ fMRPY[i]=fPythia->GetMRPY(i+1);}
  for(i=0;i<100;i++){ fRRPY[i]=fPythia->GetRRPY(i+1);}

  JSFMEGenerator *megen=(JSFMEGenerator*)gJSF->FindModule("JSFMEGenerator");
  JSFMEGeneratorBuf *mgb=(JSFMEGeneratorBuf*)megen->EventBuf();
  JSFHEPEUP *hepeup=mgb->GetJSFHEPEUP();

  if( fHEPEUPFormat == 1 ) {  // Special treatment for Higgs sub-group
    JSFHEPRUP *heprup=megen->GetJSFHEPRUP();
    // hepeup->Print();
    Double_t ebeam[2];
    ebeam[0]=heprup->GetEBMUP(1);
    ebeam[1]=heprup->GetEBMUP(2);
    hepeup->ResetBeamParticles(ebeam);
    // hepeup->Print();
  }

  void *address=pythia6_common_block_address_("HEPEUP",6);

  hepeup->Save(address);

  fPythia->GenerateEvent();

  if( fHEPEUPFormat == 1 ) {  // Special treatment for Higgs sub-group
    for(Int_t i=1;i<=4;i++){
      Double_t *p=hepeup->GetIncomming(i-1);
      for(Int_t j=1;j<=5;j++){
	fPythia->SetP(i+2, j, p[j-1]);
      }
    }
  }

  JSFPythiaHadronizerBuf *buf=(JSFPythiaHadronizerBuf*)EventBuf();

  buf->SetParticles(fPythia);

  return kTRUE;
}  

//_____________________________________________________________________________
JSFPythiaHadronizerBuf::JSFPythiaHadronizerBuf(const char *name,  
       const char *title, JSFPythiaHadronizer *module): 
       JSFSHGeneratorBuf(name,title,module)
{
}

//____________________________________________________________________________
void JSFPythiaHadronizerBuf::SetParticles(TPythia6 *fpythia)
{
// Stable particles are saved in the GeneratorParticle class.

   TClonesArray &tracks = *GetParticles();
   TVector pv(4);
   TVector xv(4);

   Int_t np=fpythia->GetNumberOfParticles();
   TObjArray *pa=fpythia->GetPrimaries();

   Int_t nout=0;
   Int_t i,ks,kf,kh,kfa;
   Float_t charge, xctau, dl, mass;
   Int_t idhist[4001];
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
     charge=((Float_t)pychge_(&kf))/3.0;
     Int_t kc=fpythia->Pycomp(kf);
     xctau=fpythia->GetPMAS(kc,4)*0.1;
     mass=p->GetMass();
     ndaughter=0;
     firstdaughter=0;
     mother=kh;
     dl=0.0;
     //
     if( ks == 21 ) {
       ndaughter = 1;
       mother = -kh;
       firstdaughter=fpythia->GetMSTI(4)+1;  
       xctau=0.0;
     }
     else if( ks != 1 ) {
       firstdaughter=p->GetFirstChild();
       ndaughter=p->GetLastChild()-firstdaughter+1;
     }

     new(tracks[nout-1]) 
       JSFGeneratorParticle(nout, kf,mass,charge, pv, xv , 
	    ndaughter, firstdaughter, mother, xctau, dl ) ;
     ((JSFGeneratorParticle*)(tracks[nout-1]))->SetStatus(ks);

   }
  SetNparticles(nout);

}

/*
// ---------------------------------------------------------------
Bool_t JSFPythiaHadronizer::Terminate()
{
  if( !JSFSHGenerator::Terminate() ) return kFALSE;

  return kTRUE;
}
*/

//_____________________________________________________________________________
Bool_t JSFPythiaHadronizer::EndRun()
{
  
  if( !JSFSHGenerator::EndRun() ) return kFALSE;

  Int_t i;
  for(i=0;i<6;i++){ fMRPY[i]=fPythia->GetMRPY(i+1);}
  for(i=0;i<100;i++){ fRRPY[i]=fPythia->GetRRPY(i+1);}

  if( fFile->IsWritable() ) {  Write(); }

  return kTRUE;
}
