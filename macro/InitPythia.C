// *****************************************
// Initialize Pythia Parameter 
// Type of event are controled by environment parameters.
// Note that CM energy is defined in InitGenSim function defined
// by GUIMainMacro.C, using a value defined by JSFGUI.ECM parameter.
//$Id$
// *****************************************


void InitPythia()
{

  PythiaGenerator *py=(PythiaGenerator*)jsf->FindModule("PythiaGenerator");

  Int_t ivers=py->GetVersionNumber();
  if( ivers <= 5 ) {
    TPythia *tpy=py->GetPythia();
  }
  else {
    TPythia6 *tpy=py->GetPythia();
  }

  //****************************
  // Set ISUB according to parameters.
  //****************************
  tpy->SetMSEL(0);

  static const Int_t nproc=9;
  Char_t *pname[nproc]={"ZH","gammagammaH", "eeH","nnH",
			"gammaZ", "ZZ","WW","enW","twophoton"};
  Int_t isub[nproc]={24, 103, 123, 124,   1, 22, 25,36,-1};
  Char_t valname[64];
  Int_t i;
  Int_t ihiggs=0;
  for(i=0;i<nproc;i++){
    sprintf(valname,"JSFGUI.Pythia.Process.%s",pname[i]);
    if( jsf->Env()->GetValue(valname,0) ) {
      switch(isub[i]) {
        case -1:
	  tpy->SetMSTP(12,1);  // Use e-,gamma,g,e+ distribution function inside an electron.
	  tpy->SetMSTP(14,10); // VMD, direct anomalous, etc.
	  tpy->SetMSUB(33,1);
	  tpy->SetMSUB(34,1);
	  tpy->SetMSUB(54,1);
	  tpy->SetMSUB(58,1);
	  tpy->SetMSUB(80,1);
	  tpy->SetMSUB(84,1);
	  tpy->SetMSUB(85,1);
          break;

        default:
          tpy->SetMSUB(isub[i],1);
      }
      if( i < 4 ) ihiggs++;
    }
  }

  //*******************************************
  //* Set Higgs mass if Higgs process is selected.
  //*******************************************
  if( ihiggs > 0 ) {
    Float_t hmass;
    sscanf(jsf->Env()->GetValue("JSFGUI.Pythia.Higgsmass","120.0"),"%g",&hmass);
    Int_t kf=25;              // kf code for H0 is 25

    Int_t kc;
    if ( ivers <= 5 ) {
      kc=tpy->Lucomp(kf);
    }
    else {
      kc=tpy->Pycomp(kf);
    }

    tpy->SetPMAS(kc,1,hmass); // Set Higgs mass 
    printf(" Higgs mass Mass=%g (GeV)\n",tpy->GetPMAS(kc,1));
  }

  //*******************************************
  //* Suppress decay mode of Z
  //#  -1 : Disable all Z decay mode.
  //#   0 : Enable all Z decay mode.
  //#   n : decay mode of n is alloed, where n is
  //#     1=d-quark, 2=u-quark, 3=s-quark, 4=c-quark, 5=b-quark,
  //#    11=e, 12=nu_e, 13=mu, 14=nu_mu, 15=tau, 16=nu_tau
  //*******************************************
  
  Int_t izdcy=jsf->Env()->GetValue("JSFGUI.Pythia.Decay.Z",0);
  if( izdcy != 0 ) {
    for(Int_t idc=156;idc<=171;idc++)   tpy->SetMDME(idc,1,0);
    if( izdcy > 0 && izdcy < 7 ) tpy->SetMDME(155+izdcy,1,1);
    if( izdcy > 10 && izdcy < 16 ) tpy->SetMDME(153+izdcy,1,1);
  }

  //*******************************************
  //* Set Other options
  //*******************************************

  if( jsf->Env()->GetValue("JSFGUI.Pythia.Geant3Decay",0) == 1 ) {
    tpy->SetMSTJ(22,2);
  }

}


