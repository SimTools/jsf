// An example script to run quick simulator of WW process
//
//$Id$
//

#include "../../include/JSFConfig.h"

Int_t sim()
{
  gROOT->Reset();
  TFile file("wwsim.root","RECREATE");  // Output file
 
  jsf    = new JSFSteer();
  full   = new JSFLCFULL();
  spring = new WWSpring();
  spring->ReadBases("bases.root");
 
#if __JSF_VERSION__ == 1 && __JSF_MINORVERSION__ <= 13
  printf(" Ebeam is %g\n",((WWBases*)spring->Bases())->GetEbeam());
#else
  printf(" Ebeam is %g\n",((WWBases*)spring->GetBases())->GetEbeam());
#endif

  hdr=new JSFHadronizer();
  sim=new JSFQuickSim();
  
//  full->SetMakeBranch(kFALSE);   // suppress output of EventBuf 
//  hdr->SetMakeBranch(kFALSE);     // suppress output of EventBuf 
//  sim->SetMakeBranch(kFALSE);    // suppress output of EventBuf

  Int_t maxevt=100;      // Number of event is 10.
  jsf->Initialize();

  jsf->BeginRun(30);      // Set run number to 30.
  for(Int_t ev=1;ev<=maxevt;ev++){
    printf(" start event %d\n",ev);
    if( !jsf->Process(ev) ) break;
    jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();

}
