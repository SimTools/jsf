///////////////////////////////////////////////////////////////////
//
//  JSFGeneratedProcessType
// 
//(Function)
//  This class provides a utility function to analize type of 
//  process generated.  It analize the TClonesArray of JSFGeneratorParticle
//  class which is created by PythiaGenerator and saves generated parton 
//  informations so that member function can be used to know what type of 
//  events are generated.
//  
//(Usage)
//  PythiaGenerator *py=(PythiaGenerator*)gJSF->FindModule("PythiaGenerator");
//  JSFGeneratorBuf *buf=(JSFGeneratorBuf*)py->EventBuf();
//  TClonesArray    *particles=buf->GetParticles();
//  JSFGeneratedProcessType proc(particles);
//
//  if( proc->IsQQEN() ) {
//    cout << "This process is e+e-> q qbar e neu_e" << endl ;
//  }
//
//$Id$
//  
//////////////////////////////////////////////////////////////////

#include "JSFGeneratedProcessType.h"

ClassImp(JSFGeneratedProcessType)

//_____________________________________________________________________________
JSFGeneratedProcessType::JSFGeneratedProcessType(TClonesArray *particles):
    TObject()
{
  Reset();
  fGen=particles;
  if( fGen ) {
    AnalizeEvent();
  }
}

//________________________________________________________
void JSFGeneratedProcessType::Reset()
{
  fNd=0;
  fNu=0;
  fNs=0;
  fNc=0;
  fNb=0;
  fNt=0;
  fNe=0;
  fNe=0;
  fNne=0;
  fNmu=0;
  fNnmu=0;
  fNtau=0;
  fNntau=0;

  fNgamma=0;
  fNz=0;
  fNw=0;
  fNh=0;

}


//________________________________________________________
void JSFGeneratedProcessType::AnalizeEvent(TClonesArray *particles)
{
  if( particles ) {
    fGen=particles;
  }

  Bool_t flag=kFALSE;
  for(Int_t i=3;i<fGen->GetEntries();i++){
    JSFGeneratorParticle *gp=(JSFGeneratorParticle*)fGen->UncheckedAt(i);
    Int_t mother=gp->GetMother();
    if( mother > 0 ) break;
    if( mother == 0 ) { flag = kTRUE; }
    if( !flag ) continue;

    Int_t ida=TMath::Abs(gp->GetID());
    switch (ida) {
      case 1: fNd++; break;
      case 2: fNu++; break;
      case 3: fNs++; break;
      case 4: fNc++; break;
      case 5: fNb++; break;
      case 6: fNt++; break;
      case 11: fNe++; break;
      case 12: fNne++; break;
      case 13: fNmu++; break;
      case 14: fNnmu++; break;
      case 15: fNtau++; break;
      case 16: fNntau++; break;
      
      case 22: fNgamma++; break;  
      case 23: fNz++; break;  
      case 24: fNw++; break;  
      case 25: fNh++; break;  
    }
  }
}

