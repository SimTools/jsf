// *****************************************
// Initialize Pythia Parameter 
//   for e+e- -> zh event.
//$Id$
// *****************************************
void InitPythia()
{
  Float_t fHiggsmass=110.0;       // Higgs mass

  TPythia *tpy=py->GetPythia();
  tpy->SetMSEL(18);                  // e+e- -> ZH process

  Int_t kf=25;                   // kf code for H0 is 25
  Int_t kc=tpy->LuComp(kf);      // Get kc code for Higgs.
  tpy->SetPMAS(kc,1,fHiggsmass); // Set Higgs mass 
  printf(" Higgs mass parameters Mass=%g Width=%g Trancat %g LifeTime=%g\n",
	 tpy->GetPMAS(kc,1), tpy->GetPMAS(kc,2), 
	 tpy->GetPMAS(kc,3), tpy->GetPMAS(kc,4) );
}

