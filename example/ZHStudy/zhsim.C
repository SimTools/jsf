// ************************************************************
// zhsim.C
//  
// An example to simulate e+e- -> Zh events using Pythia,
// JLC QUick Simulator, then output mini-DST information 
// for latter analysis by zha.C 
// 
// ************************************************************

Int_t maxevt=200;              // Number of events to generate
Char_t *outputfile="zhsim.root"; // Output file name

Float_t fEcm=300.0;              // Center of mass energy
Float_t fHiggsmass=110.0;        // Higgs Mass

PythiaGenerator *py;
TFile *file;

// *****************************************
// SetPythiaParameters
// *****************************************
void SetPythiaParameters()
{
  TPythia *tpy=py->GetPythia();
  tpy->SetMSEL(18);                  // e+e- -> ZH process

  Int_t kf=25;                   // kf code for H0 is 25
  Int_t rootver, rootmver, rootplevel;
  sscanf(gROOT->GetVersion(),"%d.%d/%d",&rootver,&rootmver,&rootplevel);
  if (rootver > 2 || (rootver == 2 && rootmver >= 23)) {
     Int_t kc=tpy->Lucomp(kf);      // Get kc code for Higgs.
  } else { 
     Int_t kc=tpy->LuComp(kf);      // Get kc code for Higgs.
  }
  tpy->SetPMAS(kc,1,fHiggsmass); // Set Higgs mass 
  printf(" Higgs mass parameters Mass=%g Width=%g Trancat %g LifeTime=%g\n",
	 tpy->GetPMAS(kc,1), tpy->GetPMAS(kc,2), 
	 tpy->GetPMAS(kc,3), tpy->GetPMAS(kc,4) );
}

//*************************************************
//* Main part to generate qqbar event
//*************************************************
int zhsim()
{
  //  gROOT->Reset();
  file = new TFile(outputfile,"RECREATE");  // Output file

  jsf   = new JSFSteer();
  full  = new JSFLCFULL();
  py    = new PythiaGenerator();
  sim   = new JSFQuickSim();
  anal  = new MyAnalysis();

  
  full->SetMakeBranch(kFALSE);   // suppress output of EventBuf 
  py->SetMakeBranch(kFALSE);     // suppress output of EventBuf 
  sim->SetMakeBranch(kFALSE);    // suppress output of EventBuf
 
  py->SetEcm(fEcm);             // Center of mass energy (GeV)
  SetPythiaParameters();         // Set Pythia parameters.
  jsf->Initialize();

  jsf->BeginRun(1);              // Set run number to 1.
  for(Int_t ev=1;ev<=maxevt;ev++){
    if( !jsf->Process(ev)) break ;
    jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();

}


