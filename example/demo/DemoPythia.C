// *****************************************
// Initialize Pythia Parameter 
//   for e+e- -> zh event.
//$Id$
// *****************************************

void InitPythia()
{
  Float_t fHiggsmass;       // Higgs mass
  sscanf(jsf->Env()->GetValue("JSFDEMO.HiggsMass","120.0"),"%g",&fHiggsmass);
  TPythia *tpy=py->GetPythia();
  //  tpy->SetMSEL(18);                  // e+e- -> ZH process

  Int_t kf=25;                   // kf code for H0 is 25
  Int_t rootver, rootmver, rootplevel;
  sscanf(gROOT->GetVersion(),"%d.%d/%d",&rootver,&rootmver,&rootplevel);
  if (rootver >= 2 && rootmver >= 23) {
     Int_t kc=tpy->Lucomp(kf);      // Get kc code for Higgs.
  } else {
     Int_t kc=tpy->LuComp(kf);      // Get kc code for Higgs.
  }
  tpy->SetPMAS(kc,1,fHiggsmass); // Set Higgs mass 
  printf(" Higgs mass parameters Mass=%g Width=%g Trancat %g LifeTime=%g\n",
	 tpy->GetPMAS(kc,1), tpy->GetPMAS(kc,2), 
	 tpy->GetPMAS(kc,3), tpy->GetPMAS(kc,4) );

  switch( jsf->Env()->GetValue("JSFDEMO.DemoEvent",1 ) ) {
    case 1:
      tpy->SetMSEL(18);                  // e+e- -> ZH process
      break;
    default:
      tpy->SetMSEL(0);            // Set process manually
      tpy->SetMSUB(24, 1);        // include ee->Z0 + H0

      tpy->SetMSUB(1, 1);        // include ee->gamma/Z0 process
      tpy->SetMSUB(22, 1);        // include ee->Z0 Z0 process
      tpy->SetMSUB(103, 1);       // include  gamma+gamma->H0
      tpy->SetMSUB(123, 1);       // include  f+f' -> f +f' + H0 (ZZ fusion)
      tpy->SetMSUB(124, 1);       // include  f+f' -> f"+f"' + H0 (WW fusion)
      
      tpy->SetMSUB(25, 1);        // include  f+f' -> W+ W-

  // To generate e+e- -> e neu W process.
      tpy->SetMSTP(11,1);         // include initial state beamstrahlung
      tpy->SetMSTP(12,1);        
      tpy->SetMSUB(36,1);        // include e gamma -> gamma W process

      tpy->SetCKIN(1,50.0);
      tpy->SetCKIN(3,2.0);
      
      tpy->SetMSTP(142,2);  // Event weight by method 1.
  }

}

void SetPythiaWeight()
{
  // A function set event weight.

  Int_t isub=py->GetPythia()->GetMINT(1);
  Float_t weight=1.0;
  switch(isub){
    case 1: weight=0.01; break;
    case 22: weight=0.4; break;
    case 24: weight=1.0; break;
    case 25: weight=0.04; break;
    case 36: weight=0.2; break;
  }
  py->SetEventWeight(weight);

}
