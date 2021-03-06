// *****************************************
// Initialize Pythia Parameter 
//   for e+e- -> zh event.
//(Parameters) 
//  There is one global parameter, fHiggsmass,  for this macro.
//  It must be defined prior to the execution.
// *****************************************
void InitPythia()
{
  TPythia *tpy=py->GetPythia();
  tpy->SetMSEL(18);                  // e+e- -> ZH process

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
}

