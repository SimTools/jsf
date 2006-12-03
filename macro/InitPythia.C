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

  Int_t iniseed=jsf->Env()->GetValue("JSFGUI.Pythia.InitialSeed",19780503);
  tpy->SetMRPY(1,iniseed);

  //****************************
  //*  ISR on/off
  //****************************
  
  if ( jsf->Env()->GetValue("JSFGUI.Pythia.ISR",1) == 0 ) {
    tpy->SetMSTP(11,0);
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
  //* Set decay mode of particles
  //*******************************************

  SetPythiaDecayMode();

  //*******************************************
  //* Set Other options
  //*******************************************

  if( jsf->Env()->GetValue("JSFGUI.Pythia.Geant3Decay",0) == 1 ) {
    tpy->SetMSTJ(22,2);
  }


  if( gROOT->GetGlobalFunction("UserInitPythia",0,kTRUE) ) UserInitPythia();

}



// ---------------------------------------------------------------------
void SetPythiaDecayMode()
{
  //*******************************************
  //* Suppress decay mode of Z
  //#  -1 : Disable all Z decay mode.
  //#   0 : Enable all Z decay mode.
  //#   n : decay mode of n is alloed, where n is
  //#     1=d-quark, 2=u-quark, 3=s-quark, 4=c-quark, 5=b-quark,
  //#    11=e, 12=nu_e, 13=mu, 14=nu_mu, 15=tau, 16=nu_tau
  //# -2=Z->q qbar
  //# -3=Z->ee/mumu
  //# -4=Z->llepton lepton
  //# -5=Z->nu nu
  //# -6=Z->uds
  //*******************************************

  PythiaGenerator *py=(PythiaGenerator*)jsf->FindModule("PythiaGenerator");

  Int_t ivers=py->GetVersionNumber();
  if( ivers <= 5 ) {
    TPythia *tpy=py->GetPythia();
  }
  else {
    TPythia6 *tpy=py->GetPythia();
  }
  
  Int_t izdcy=jsf->Env()->GetValue("JSFGUI.Pythia.Decay.Z",0);
  Int_t izdcy2=izdcy/100;
  izdcy=izdcy%100;

  std::cerr << " Pythia Z Decay flag=" << izdcy << " izdcy2=" << izdcy2 << std::endl;

  if( izdcy != 0 ) {
    if( ivers <= 5 ) {
      for(Int_t idc=156;idc<=171;idc++)   tpy->SetMDME(idc,1,0);
      if( izdcy > 0 && izdcy < 7 ) tpy->SetMDME(155+izdcy,1,1);
      if( izdcy > 10 && izdcy < 16 ) tpy->SetMDME(153+izdcy,1,1);
    }
    else {
      Int_t kfz=23;
      Int_t kcz=tpy->Pycomp(kfz);
      Int_t mdcy2=tpy->GetMDCY(kcz,2);
      Int_t mdcy3=tpy->GetMDCY(kcz,3);
      for(Int_t i=mdcy2;i<=mdcy2+mdcy3-1;i++){ tpy->SetMDME(i,1,0); }
      if( izdcy > 0 ) {
	if( izdcy > 0 && izdcy < 7 ) { tpy->SetMDME(mdcy2+izdcy-1, 1, 1); }
	if( izdcy > 10 && izdcy < 17 ) { tpy->SetMDME(mdcy2+8+izdcy-11, 1, 1); }
	if( izdcy2 > 0 && izdcy2 < 7 ) { tpy->SetMDME(mdcy2+izdcy2-1, 1, 1); }
	if( izdcy2 > 10 && izdcy2 < 17 ) { tpy->SetMDME(mdcy2+8+izdcy2-11, 1, 1); }
      }
      else if( izdcy == -2 ) {
	Int_t izlist[6]={1, 2, 3, 4, 5, 6 };
	for(Int_t i=0;i<6;i++){
	  Int_t ip=izlist[i];
	  tpy->SetMDME(mdcy2+ip-1, 1, 1);
	}
      }
      else if( izdcy == -3 ) {
	Int_t izlist2[2]={9, 11};
	for(Int_t i=0;i<2;i++){ 
	  Int_t ip=izlist2[i];
	  tpy->SetMDME(mdcy2+ip-1, 1, 1); }
      }
      else if( izdcy == -4 ) {
	Int_t izlist3[3]={9, 11, 13};
	for(Int_t i=0;i<3;i++){ 
	  Int_t ip=izlist3[i];
	  tpy->SetMDME(mdcy2+ip-1, 1, 1); }
      }
      else if( izdcy == -5 ) {
	Int_t izlist4[3]={10, 12, 14};
	for(Int_t i=0;i<3;i++){ 
	  Int_t ip=izlist4[i];
	  tpy->SetMDME(mdcy2+ip-1, 1, 1); 
	}
      }
      else if( izdcy == -6 ) {
	Int_t izlist5[3]={1, 2, 3 };
	for(Int_t i=0;i<3;i++){
	  Int_t ip=izlist5[i];
	  tpy->SetMDME(mdcy2+ip-1, 1, 1);
	}
      }
      else if( izdcy == -31 ) {  // mu mu + u ubar
	Int_t izlist131[2]={13, 1};
	for(Int_t i=0;i<2;i++){
	  Int_t ip=izlist131[i];
	  tpy->SetMDME(mdcy2+ip-1, 1, 4);
	}
      }
      else if( izdcy == -35 ) {  // mu mu + b bbar
	Int_t izlist135[2]={13, 5};
	for(Int_t i=0;i<2;i++){
	  Int_t ip=izlist135[i];
	  tpy->SetMDME(mdcy2+ip-1, 1, 4);
	}
      }
      else if( izdcy == -21 ) {  // nu_e nu_e + u ubar
	Int_t izlist121[2]={12, 1};
	for(Int_t i=0;i<2;i++){
	  Int_t ip=izlist121[i];
	  tpy->SetMDME(mdcy2+ip-1, 1, 4);
	}
      }
      else if( izdcy == -25 ) {  // nu_e nu_e + b bbar
	Int_t izlist125[2]={12, 5};
	for(Int_t i=0;i<2;i++){
	  Int_t ip=izlist125[i];
	  tpy->SetMDME(mdcy2+ip-1, 1, 4);
	}
      }

    }
  }

  //  tpy->Pylist(12);
  // ******************************************
  // Decay Mode of W
  //  1=dbar u   2=dbar c   3=dbar t  
  //  5=sbar u   6=sbar c   7=sbar t
  //  9=bbar u  10=bbar c  11=bbar t
  // 17=e+ nue  18=mu+ nu_mu  19=tau+ nu_tau
  // -2=W->q qbar
  // -3=W->e-nu_e/mu-nu_mu
  // -4=W->lepton nu_lepton
  // ******************************************
  Int_t iwdcy=jsf->Env()->GetValue("JSFGUI.Pythia.Decay.W",0);
  Int_t iwdcy2=iwdcy/100;
  iwdcy=iwdcy%100;
  if( iwdcy != 0 ) {
    if( ivers <= 5 ) {}
    else {
      Int_t kfh=24;
      Int_t kch=tpy->Pycomp(kfh);
      Int_t mdcy2=tpy->GetMDCY(kch,2);
      Int_t mdcy3=tpy->GetMDCY(kch,3);
      for(Int_t i=mdcy2;i<=mdcy2+mdcy3-1;i++){ tpy->SetMDME(i,1,0); }
      if( iwdcy > 0 ) {
	tpy->SetMDME(mdcy2+iwdcy-1, 1, 1);
	if( iwdcy2 > 0 ) {  tpy->SetMDME(mdcy2+iwdcy2-1, 1, 1); }
      }
      else if( iwdcy == -2 ) {
	Int_t iwlist[9]={1, 2, 3, 5, 6, 7, 9, 10, 11};
	for(Int_t i=0;i<9;i++){
	  Int_t ip=iwlist[i];
	  tpy->SetMDME(mdcy2+ip-1, 1, 1);
	}
      }
      else if( iwdcy == -3 ) {
	Int_t iwlist[2]={17, 18};
	for(Int_t i=0;i<2;i++){ 
	  Int_t ip=iwlist[i];
	  tpy->SetMDME(mdcy2+ip-1, 1, 1); }
      }
      else if( iwdcy == -4 ) {
	Int_t iwlist[3]={17, 18, 19};
	for(Int_t i=0;i<3;i++){ 
	  Int_t ip=iwlist[i];
	  tpy->SetMDME(mdcy2+ip-1, 1, 1); }
      }
    }
  }

  // ******************************************
  // Decay Mode of Higgs
  // 1=d dbar, 2=u ubar, 3=s sbar, 4=c cbar, 5=b bar, 6=t tbar, 
  // 9=e- e+,  10=mu- mu+, 11=tau- tau+, 
  // 13=g g, 14=gamma gamma, 15=gamma Z0, 16=Z0 Z0, 17=W+ W-
  // ******************************************
  Int_t ihdcy=jsf->Env()->GetValue("JSFGUI.Pythia.Decay.H",0);
  Int_t ihdcy2=ihdcy/100;
  ihdcy=ihdcy%100;
  if( ihdcy != 0 ) {
    if( ivers <= 5 ) {}
    else {
      Int_t kfh=25;
      Int_t kch=tpy->Pycomp(kfh);
      Int_t mdcy2=tpy->GetMDCY(kch,2);
      Int_t mdcy3=tpy->GetMDCY(kch,3);
      for(Int_t i=mdcy2;i<=mdcy2+mdcy3-1;i++){ tpy->SetMDME(i,1,0); }
      tpy->SetMDME(mdcy2+ihdcy-1, 1, 1);
      if( ihdcy2 > 0 ) {  tpy->SetMDME(mdcy2+ihdcy2-1, 1, 1); }
    }
  }

}
