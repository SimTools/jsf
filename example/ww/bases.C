// Macro example for bases calculation.
//
//$Id$
//

#include "../../include/JSFConfig.h"

Int_t bases()
{

  gROOT->Reset();
  TFile file("bases.root","RECREATE");

  jsf   = new JSFSteer();    // required to read parameter from jsf.conf
  bases = new WWBases();

#if __JSF_VERSION__ == 1 && __JSF_MINORVERSION__ <= 13
//  bases->SetNCALL(5000);
//  bases->SetITMX1(1);
//  bases->SetITMX2(1);
  bases->fPrintInfo=kTRUE;
  bases->fPrintHist=kTRUE;
  bases->DoBases();
#else
  bases->SetNoOfSample(10000);
  bases->SetIteration1( 0.2, 10);
  bases->SetIteration2( 0.1, 10);
  bases->Bases();
  bases->Bh_plot();
  bases->Userout();
#endif

  bases->Write();
  file->Write();
}

