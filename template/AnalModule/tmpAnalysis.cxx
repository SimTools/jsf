//*LastUpdate : <<Version>> <<Date>> by <<Author>>
//*-- Author  : <<Author>> <<Date>>

///////////////////////////////////////////////////////////////////
//
//  <<Analysis>>
//
//  <<Comment>>
//
//$Id: 
//  
//////////////////////////////////////////////////////////////////


#include "JSFSteer.h"
#include "JSFSIMDST.h"
#include "<<Analysis>>.h"

#include "TH1.h"

<<+EMAnal>>
TH1F *hEsum;
<<-EMAnal>>

ClassImp(<<Analysis>>)
ClassImp(<<Analysis>>Buf)

//_____________________________________________________________________________
  <<Analysis>>::<<Analysis>>(const char *name, const char *title)
       : JSFModule(name,title)
{
  fEventBuf = new <<Analysis>>Buf("<<Analysis>>Buf", 
	 "<<Analysis>> event uffer",this);

<<+EMAnal>>
  //  Example to get parameter  
  fFlag = gJSF->Env()->GetValue("<<Analysis>>.Flag",3);
  sscanf(gJSF->Env()->GetValue("<<Analysis>>.Parameter","3.14"),"%g",&fParameter);
<<-EMAnal>>

}

//_____________________________________________________________________________
<<Analysis>>::~<<Analysis>>()
{
}

//_____________________________________________________________________________
Bool_t <<Analysis>>::Initialize()
{
  // Initialize

<<+EMAnal>>
  // Put histogram defiition here, if needed.
  hEsum=new TH1F("hEsum","Calorimter toal enegy",100,0,200);
<<-EMAnal>>

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t <<Analysis>>::Process(Int_t nev)
{
// 

<<+EMAnal>>
  JSFSIMDST *sim=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
  JSFSIMDSTBuf *sb=(JSFSIMDSTBuf*)sim->EventBuf();

  TClonesArray *emc=sb->GetEMCHits();

  Int_t nemc=sb->GetNEMCHits();
  Float_t emsum=0;
  for(Int_t ie=0;ie<nemc;ie++){
    JSFCALHit *ch=(JSFCALHit*)emc->UncheckedAt(ie);
    emsum+=ch->GetEnergy();
  }
  hEsum->Fill(emsum);
<<-EMAnal>>

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t <<Analysis>>::BeginRun(Int_t nrun)
{
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t <<Analysis>>::EndRun()
{
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t <<Analysis>>::Terminate()
{
  return kTRUE;
}

//_____________________________________________________________________________
<<Analysis>>Buf::<<Analysis>>Buf(const char *name, const char *title,
  	     JSFModule *module) : JSFEventBuf(name, title, module)
{
}

//_____________________________________________________________________________
<<Analysis>>Buf::~<<Analysis>>Buf()
{
}




