//****************************************************
//*
//*  This is a sample script to analize e+e- -> Z H process.
//*  Z -> nu nubar , H -> 2 jets events are selected.
//*
//*  To run this script, do
//*    $ jsf -b -q gui.C
//*  
//****************************************************

TCanvas *cHist;
TDirectory *cDir;

TH1F *hHmass, *hMissmass, *hPtmiss, *hStat, *hEvis, *hNjet; 
TH1F *hDrn, *hHmassNoVTX, *hNOffVertex ;
TH1F *hNCDC=0;

//
//   Variables for event property.
//
  Double_t fEcm=300.0;        // CM energy (GeV)
  Int_t    fNcmulti;    // Charged track multiplicity
  Double_t fMaxEP;      // Maximum electron momentum (GeV)
  Double_t fMaxMuP;     // Maximum muon momentum (GeV)			     

  Int_t    fNumjet;     // Number of jets.
  Double_t fEvent4V[4]; // Sum of 4 vector of all tracks.
  Double_t fJet[5][4];  // Jet four momentum ( up to 5 jets )
  Double_t fPtmiss;     // Missing Pt of the event	
  Double_t fMissmass;   // Missing mass (GeV)
  Double_t fCos1 ;      // Costh of 1st jet
  Double_t fCos2 ;      // Costh of 2nd jet
  Double_t fHmass ;     // Higgs mass (GeV)
  Float_t fYcut=0.1;     // Ycut value for event selection.
  Int_t nOffVertex;    // Number of off vertex track of events

// Event selection condition.

Float_t xEvis=170.0 ;         // Maximum visible energy
Float_t xPtmiss=20.0 ;        // Ptmiss minimum;
Int_t   xNumjet=2;            // Number of jets
Float_t xLeptonP=2.0;         // Lepton P maximum
Float_t xCosth=0.85;          // jet Costh maximum
Float_t xHmass1=100.0;        // Higgs mass range to count number of        
Float_t xHmass2=116.0;        //     candidates
Float_t fitMin=104.0;         // Hmass fit range(minimum)
Float_t fitMax=114.0;          // Hmass fit range(maximum)
Int_t   minOffVertex=5;        // Minimum Number of off vertex track.
Float_t fDrcut=5.0;            // Cut in inpact parameter (sigma)

static Int_t nGcall=0;         // Number of good event;
Char_t selName[20][100];       // Character array to save selection name

Float_t gwgt; // Weight of this event.

TPaveText *paveText[3];

//_____________________________________________________________________
void UserInitialize()
{
  //  This function is called at the begining of the job or when
  //  "reset hist" action is selected in the gui menu.
  //  This is used to define/reset histograms.

  if( hNCDC ) {
    hStat->Reset();  hEvis->Reset(); hMissmass->Reset();
    hPtmiss->Reset(); hNjet->Reset(); hDrn->Reset();
    hNCDC->Reset();
    return;
  }

  hStat     = new TH1F("hStat", "Selection Statistics",20,0,20);
  hEvis     = new TH1F("hEvis","Visible energy",100,0.0, 350.0);
  hMissmass = new TH1F("hMissmass","Missing Mass",100,-50.0, 250.0);
  hPtmiss = new TH1F("hPtmiss","Missing Pt",100,0.0, 100.0);
  hHmass  = new TH1F("hHmass", "Higgs mass", 100, 100.0, 140.0);
  hHmassNoVTX  = new TH1F("hHmassNoVTX", "Higgs mass without VTX", 
			  100, 60.0, 160.0);
  hNjet   = new TH1F("hNjet", "Number of jets",20,0.0,10.0);
  hDrn   = new TH1F("hDrn", "Normalized Dr of particles",50, -25, 25);
  hNOffVertex   = new TH1F("hNOffVertex", 
		   "Number of Off Vertex Tracks",50, 0, 50);
  hNCDC=new TH1F("hNCDC","Number of CDC Tracks",100,0,100);

  TH1F *htmp=0;
  TDirectory *pwd=gDirectory;
  TFile *fold=new TFile("demo.root","READ");

    hEvis->Add((TH1F*)fold->Get("hEvis"));
    hNCDC->Add((TH1F*)fold->Get("hNCDC"));
    hMissmass->Add((TH1F*)fold->Get("hMissmass"));
    hHmass->Add((TH1F*)fold->Get("hHmass"));
  
    fold->Close();
    delete fold;
    pwd->cd();
    printf("Histogram data of previous run is not loaded.\n");


  hEvis->SetFillColor(38);
  hNCDC->SetFillColor(38);
  hHmass->SetFillColor(2);
  
  hEvis->SetNdivisions(705);
  hEvis->SetNdivisions(505,"Y");
  hEvis->SetLabelSize(0.1);
  hEvis->SetLabelSize(0.1,"Y");
  hNCDC->SetLabelSize(0.1);
  hNCDC->SetLabelSize(0.1,"Y");
  hNCDC->SetNdivisions(505);
  hNCDC->SetNdivisions(505,"Y");
  hHmass->SetLabelSize(0.1);
  hHmass->SetLabelSize(0.1,"Y");
  hHmass->SetNdivisions(505);
  hHmass->SetNdivisions(505,"Y");

  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetPadLeftMargin(0.2);
  
  Int_t i;
  for(i=0;i<3;i++){
    paveText[i]=new TPaveText(0.1,0.85,0.6,1.0);
    paveText[i]->SetTextFont(42);
    paveText[i]->SetTextSize(0.1);
    paveText[i]->SetBorderSize(1);
  }
  paveText[0]->AddText(hEvis->GetTitle());
  paveText[1]->AddText(hNCDC->GetTitle());
  paveText[2]->AddText(hHmass->GetTitle());

  cDir=gDirectory;

}

//_________________________________________________________
void DrawHist(Int_t id=1)
{
  //  This function is called to draw histograms during the interactive 
  //  session.  Thus you can see the accumulation of the histogram
  //  interactively.  
/*

  TDirectory *old=gDirectory;
  if( !cHist ) {
    //    cHist=new TCanvas("cHist","Canvas 1",100, 100, 400, 400);
    cHist=new TCanvas("cHist","Canvas 1",100, 100, 800, 800);
    cHist->Divide(2,2);
  } 
  else {
    cHist->cd();
  }
*/

  switch(id) {
    case 1:
        hEvis->Draw();
	break;
    case 2:
        hNCDC->Draw();
	break;
    case 3:
        if( hHmass->Integral() > 0 )  gPad->SetLogy();
        hHmass->Draw();
	break;
  }
  paveText[id-1]->SetX1NDC(0.25); paveText[id-1]->SetX2NDC(0.85);
  paveText[id-1]->SetY1NDC(0.85); paveText[id-1]->SetY2NDC(0.97);
  paveText[id-1]->Draw();
/*
  cHist->cd(1);   hEvis->Draw();
  cHist->cd(2);   hNOffVertex->Draw();
  cHist->cd(3);   hHmassNoVTX->Draw();
  cHist->cd(4);   hHmass->Draw();

  cHist->Update();
  old->cd();
*/
}

//_________________________________________________________
void SoundMessage(Int_t id=0)
{
  if( strcmp(gSystem->HostName(),"jlcam.kek.jp") != 0 ) return;

  Int_t itype=0;
  switch (id) {
    case -2:
      gSystem->Exec("/usr/bin/esdplay /usr/share/sounds/startup1.wav");
      break;
    case -1:
      gSystem->Exec("/usr/bin/esdplay /usr/share/sounds/startup2.wav");
      break;
    case -99:
      gSystem->Exec("/usr/bin/esdplay /usr/share/sounds/shutdown1.wav");
      break;
    case 0:
      itype=py->GetPythia()->GetMSTI(1);
      switch (itype) {
        case 24:
          gSystem->Exec("/usr/bin/esdplay /usr/share/sounds/gnobots2/victory.wav
");
          break;
        case 25:
          gSystem->Exec("/usr/bin/esdplay /usr/share/sounds/gnibbles/gobble.wav"
);
          break;
        case 22:
          gSystem->Exec("/usr/bin/esdplay /usr/share/sounds/gnibbles/pop.wav");
          break;
        case 36:
          gSystem->Exec("/usr/bin/esdplay /usr/share/sounds/gnobots2/bad.wav");
          break;
        default:
          gSystem->Exec("/usr/bin/esdplay /usr/share/sounds/question.wav");
      }
  }
}



//_________________________________________________________
void UserSetOptions()
{
  // This function is called only once, soon after jsf is started.
  // This function can be used to define parameters which is not 
  // defined in jsf.conf file.


}


//_________________________________________________________
void UserAnalysis()
{
  // This function is called when the processing of one event is completed.
  // Any data processing of the event can be performed in this function.
  // 

  //  if( jsf->GetEventNumber()%10 == 9 ) {
    printf("Processing event# %d\n",jsf->GetEventNumber());
  //  }

  // This can be used only when generating event on the fly.
  // PrintEventInfo();  

  CalculateEventParameters();

  OffVertexTracks();

  HAccumulate();

}



//_______________________________________________________________
Bool_t HAccumulate()
{
  // Variables are accumulated in histograms, applying event selection cuts.

  Char_t msg[60];
  Float_t weight=1.0;
  Float_t wgt=1.0;
  if( py->GetPythia()->GetMSTP(142) != 1 ) {
    Int_t isub=py->GetPythia()->GetMSTI(1);
    switch(isub){
      case 1: weight=0.005; break;
      case 22: weight=0.2; break;
      case 24: weight=1.0; break;
      case 25: weight=0.02; break;
      case 36: weight=0.1; break;
    }
    wgt=1.0/weight;
  }
  gwgt=wgt;

  Float_t selid=-0.5;
  hStat->Fill(++selid,wgt);
  if( nGcall == 0 ) strcpy(&selName[(Int_t)selid][0],"Total event\00");

  hEvis->Fill(fEvent4V[0],wgt);

  // Cut by visible energy
  if( fEvent4V[0] > xEvis )       return kFALSE; 
  hStat->Fill(++selid,wgt);
  if( nGcall == 0 ) {
    sprintf(msg,"Visible Energy < %g (GeV)\00", xEvis);
    strcpy(&selName[(Int_t)selid][0],msg);
  }
  hMissmass->Fill(fMissmass,wgt);
  hPtmiss->Fill(fPtmiss,wgt);

  // Cut by missing Pt
  if( fPtmiss < xPtmiss )  return kFALSE ;
  hStat->Fill(++selid,wgt);
  if( nGcall == 0 ) {
    sprintf(msg,"Missing Pt > %g (GeV)\00", xPtmiss);
    strcpy(&selName[(Int_t)selid][0],msg);
  }

  // Cut by number of jet
  hNjet->Fill(fNumjet,wgt);
  if( fNumjet != xNumjet )    return kFALSE ;
  hStat->Fill(++selid,wgt);
  if( nGcall == 0 ) {
    sprintf(msg,"# jets = 2 (ycut=%g)\00", fYcut);
    strcpy(&selName[(Int_t)selid][0],msg);
  }

  // Electron/Muon P < 2.0 
  if( fMaxEP > xLeptonP )    return kFALSE ;
  hStat->Fill(++selid,wgt);
  if( nGcall == 0 ) {
    sprintf(msg,"Electron Momentum < %g (GeV)\00", xLeptonP);
    strcpy(&selName[(Int_t)selid][0],msg);
  }
  if( fMaxMuP > xLeptonP )   return kFALSE ;
  hStat->Fill(++selid,wgt);
  if( nGcall == 0 ) {
    sprintf(msg,"Muon Momentum < %g (GeV)\00", xLeptonP);
    strcpy(&selName[(Int_t)selid][0],msg);
  }

  // Costh of jets
  if( TMath::Abs(fCos1) > xCosth ) return kFALSE;
  hStat->Fill(++selid,wgt);
  if( nGcall == 0 ) {
    sprintf(msg,"|Costh-jet1| < %g\00",xCosth);
    strcpy(&selName[(Int_t)selid][0],msg);
  }
  if( TMath::Abs(fCos2) > xCosth ) return kFALSE;
  hStat->Fill(++selid,wgt);
  if( nGcall == 0 ) {
    sprintf(msg,"|Costh-jet2| < %g\00",xCosth);
    strcpy(&selName[(Int_t)selid][0],msg);
  }

  // Cut by number of off vertex track.
  hHmassNoVTX->Fill(fHmass,wgt);
  hNOffVertex->Fill(nOffVertex,wgt);
  if( nOffVertex < minOffVertex ) return kFALSE;
  hStat->Fill(++selid,wgt);
  if( nGcall == 0 ) {
    sprintf(msg,"Number of Off vertex track < %d\00",minOffVertex);
    strcpy(&selName[(Int_t)selid][0],msg);
  }


  // Count number of candidates in the suitable mass range.
  hHmass->Fill(fHmass,wgt);
  if( fHmass < xHmass1 || fHmass > xHmass2 ) return kFALSE;
  hStat->Fill(++selid,wgt);
  if( nGcall == 0 ) {
    sprintf(msg,"Higgs mass from %g to %g\00",xHmass1, xHmass2);
    strcpy(&selName[(Int_t)selid][0],msg);
    selid++;
    strcpy(&selName[(Int_t)selid][0],"END\00");
  }

  nGcall++;

  return kTRUE;
}  

//_________________________________________________________
void OffVertexTracks()
{
  // Count number of off-vertex tracks.

  JSFSIMDST *sds=(JSFSIMDST*)jsf->FindModule("JSFSIMDST");
  JSFSIMDSTBuf *evt=(JSFSIMDSTBuf*)sds->EventBuf();
  JSFQuickSimParam *spar=sds->Param();
  TClonesArray *cdc=evt->GetCDCTracks();

  Int_t ncdc=evt->GetNCDCTracks();
  hNCDC->Fill((Float_t)ncdc,gwgt);

  Double_t err[15];  
  Float_t  trk[30];
  nOffVertex=0;
  //
  //  Count a number of off vertex tracks.
  //  Tracks with very long decay length are not counted, since they are
  //  most likely strang particles such as K0.
  //
  for(Int_t i=0;i<ncdc;i++){
     JSFCDCTrack *t=(JSFCDCTrack*)cdc->UncheckedAt(i);
     t->GetTrackParameter(trk);     

     // Skip, if track does not have enough VTX hits
     if( t->GetNVTX() < 3 )  continue;	

     Float_t rclosest=TMath::Sqrt(trk[4]*trk[4]+trk[5]*trk[5]);
     if( rclosest > 1.0 ) {     // Does not count track which miss beam pipe.
       //       nOffVertex++;
       continue;
     }
     else {
       t->MovePivotToIP(spar);
       t->GetErrorMatrix(err);
       t->GetTrackParameter(trk);
       Float_t drn=TMath::Sqrt(trk[9]*trk[9]/err[0] +
			       trk[12]*trk[12]/err[9]);
       hDrn->Fill(drn,gwgt);
       if( drn > fDrcut ) nOffVertex++;
     }
  }

}

//_________________________________________________________
void CalculateEventParameters()
{
  // Calculate event parameters such as Visible energy, etc.

  JSFSIMDST *sds=(JSFSIMDST*)jsf->FindModule("JSFSIMDST");
  JSFSIMDSTBuf *evt=(JSFSIMDSTBuf*)sds->EventBuf();

  //  Accumulate information in the histogram

   // ***************************************
   // Make histograms of simulator information
   // ***************************************

   Int_t nt=evt->GetNLTKCLTracks();
   TClonesArray *tracks=evt->GetLTKCLTracks();

   // Accumulate 4 momentum of the event

   TVector ssum(4);
   TVector st(4);
   static const Int_t maxprt=500;
   static const Int_t lngdat=4;
   Float_t prtlst[maxprt][lngdat];
   Int_t nprt=0;  Int_t ipx0=0;
   Float_t maxEP=0.0;
   Float_t maxMuP=0.0;
   for(Int_t i=0;i<nt;i++){
     JSFLTKCLTrack *t=(JSFLTKCLTrack*)tracks->UncheckedAt(i);
     if( t->GetType() == 11 && t->GetECL() > maxEP) { maxEP= t->GetECL(); } 
     if( t->GetType() == 13 && t->GetE() > maxMuP ){ maxMuP=t->GetE(); }
     st=t->GetPV();
     prtlst[nprt][0]=t->GetPx();      prtlst[nprt][1]=t->GetPy(); 
     prtlst[nprt][2]=t->GetPz();      prtlst[nprt][3]=t->GetE(); 
     nprt++;
     ssum += st;  // Sum 4 momentum
   }

   //   
   fMaxEP=maxEP;nt;
   fMaxMuP=maxMuP;
   fNcmulti=nt;
   fEvent4V[0]=ssum(0); 
   fEvent4V[1]=ssum(1);
   fEvent4V[2]=ssum(2);
   fEvent4V[3]=ssum(3);

   // Clear other variables
   ClearJetInfo();

   // Does jet analysis
   if( nt > 1 )  {
     static const Int_t maxcut=1;
     Float_t ycut[maxcut] ;    ycut[0]=fYcut;
     Int_t numcls[maxcut] ;   
     Int_t numjet[maxprt]; Int_t jettbl[maxprt] ; 
     Float_t jetlst[maxprt][4]; Int_t status;
     JSFLCFULL::JCLUST(nprt, &prtlst[0][0], lngdat, ipx0, maxcut, ycut, 
		   numcls, numjet, &jettbl[0], &jetlst[0][0], status);


     CalculateVariables((numcls[0]), jetlst);
   }

  return;
 
}


// ---------------------------------------------------------------
void CalculateVariables(Int_t njet, Float_t jetlst[][4])
{
//  Calculate costh of jets, missing mass of the event, and 
//  and invariant mass of the event.

  fNumjet=njet; 
  for(Int_t i=0;i<TMath::Min(fNumjet, 5);i++){
     fJet[i][0]=jetlst[i][3] ; fJet[i][1]=jetlst[i][0];
     fJet[i][2]=jetlst[i][1] ; fJet[i][3]=jetlst[i][2];
   }

   fPtmiss=TMath::Sqrt(
		   fEvent4V[1]*fEvent4V[1] + fEvent4V[2]*fEvent4V[2]);
   
   //   printf(" e/mu Pmax=%g %g \n",fMaxEP, fMaxMuP);
   //printf(" ptmiss=%g\n",fPtmiss);
   // printf(" event 4 vector is %g %g %g %g\n",
   //	  fEvent4V[0], fEvent4V[1], fEvent4V[2], fEvent4V[3]);

   fCos1=1.0; 
   fCos2=-1.0;
  if( fNumjet > 0 ) {	
    fCos1 = fJet[0][3]/TMath::Sqrt( fJet[0][1]*fJet[0][1] + 
	    fJet[0][2]*fJet[0][2] + fJet[0][3]*fJet[0][3] );
  }
  if( fNumjet > 1) {  
     fCos2 = fJet[1][3]/TMath::Sqrt( fJet[1][1]*fJet[1][1] + 
	  fJet[1][2]*fJet[1][2] + fJet[1][3]*fJet[1][3] );
  }   

  Double_t missmass2 = (fEcm-fEvent4V[0])*(fEcm-fEvent4V[0]) 
     - fEvent4V[1]*fEvent4V[1] - fEvent4V[2]*fEvent4V[2] 
     - fEvent4V[3]*fEvent4V[3];
  if( missmass2 > 0.0 ) fMissmass=TMath::Sqrt(missmass2);
  else fMissmass=-TMath::Sqrt(-missmass2);


  Double_t hmass2 = fEvent4V[0]*fEvent4V[0] -  fEvent4V[1]*fEvent4V[1] 
     - fEvent4V[2]*fEvent4V[2] - fEvent4V[3]*fEvent4V[3] ;
  if( hmass2 > 0.0 ) fHmass=TMath::Sqrt(hmass2);
  else fHmass=-TMath::Sqrt(hmass2);
    
}

//__________________________________________________________
void ClearJetInfo()
{ 
  fNumjet=0 ; fPtmiss=0.0; fMissmass=0.0;
  fCos1=1.0 ; fCos2=-1.0 ; fHmass=0.0;
  fJet[0][0]=0.0;   fJet[0][1]=0.0;
  fJet[0][2]=0.0;   fJet[0][3]=0.0;
}


//__________________________________________________________
void UserTerminate()
{ 
  printf("\n Summary of event selection .\n");
  for(Int_t i=1;i<hStat->GetNbinsX();i++){
    if( strcmp(&selName[i-1][0],"END")==0 ) break; 
    printf("%40.40s : %7d \n",&selName[i-1][0],hStat->GetBinContent(i));
  }

  PythiaGenerator *py
     =(PythiaGenerator*)jsf->FindModule("PythiaGenerator");
  py->GetPythia()->PyStat(1);

}



//____________________________________________________________
void PrintEventInfo()
{
  // Get information about geneterated event from Generator Class

   PythiaGenerator *py
     =(PythiaGenerator*)jsf->FindModule("PythiaGenerator");
   TPythia *tpy=py->GetPythia();

   // See page 140. of the Pythia document 
   //**********************************************
   // GetMSTI(1)
   // specifies the general type of subprocess that has occured,
   // According to the ISUB code.
 
   Int_t msti1=tpy->GetMSTI(1);
   printf("Generated process is %s\n",MSTIInfo(1,msti1));

   printf("Ecm=%g\n",py->GetEcm());

   //**********************************************
   JSFSIMDST *sds=(JSFSIMDST*)jsf->FindModule("JSFSIMDST");
   JSFSIMDSTBuf *evt=(JSFSIMDSTBuf*)sds->EventBuf();
   
   Int_t ngen=evt->GetNGeneratorParticles();
   TClonesArray *gen=(TClonesArray*)evt->GetGeneratorParticles();
   /*
   printf(" ngen=%d\n",ngen);
   for(Int_t i=0;i<ngen;i++){
     JSFGeneratorParticle *p=(JSFGeneratorParticle*)gen->UncheckedAt(i);
     //     if( p->GetNDaughter() != 0 ) continue;
     printf(" Serial %d",p->GetSerial());
     printf(" Id=%d",p->GetID());
     printf(" Mass=%g",p->GetMass());
     printf(" NDau=%d",p->GetNDaughter());
     printf(" 1stD=%d",p->GetFirstDaughter());
     printf(" Mother=%d",p->GetMother());

     printf("\n");
     printf(" LifeTime=%g\n",p->GetLifeTime());
     printf(" DecayLength=%g\n",p->GetDecayLength());
     p->ls();
   }
   */
   PrintGeneratedEventInfo(ngen, gen);

}

//_________________________________________________________________
void PrintGeneratedEventInfo(Int_t ngen, TClonesArray *gen)
{
  // Scan TClonesArray of JSFGeneratorParticle and
  // print initial parton information.

  const Int_t maxp=20;
  JSFGeneratorParticle *p[maxp];
  // Save pointers to documentation line particles ( Mother <= 0 )
  Int_t n=0;
  Int_t i;
  Int_t ndau[maxp]={0};
  //  for(i=0;i<maxp;i++){ ndau[i]=0; }

  for(i=0;i<ngen;i++){
    JSFGeneratorParticle *pt=(JSFGeneratorParticle*)gen->UncheckedAt(i);
    if(pt->GetMother()>0) break;
    p[n++]=pt;
    ndau[TMath::Abs(pt->GetMother())]++;
  }

  // printf(" There are %d documentation lines\n",n);

  Int_t nprim=0;
  Int_t nppnt[maxp]={0};

  printf(" e+e- -> ");
  for(i=2;i<n;i++){
    if( p[i]->GetMother() != 0 ) continue;
    printf(" %s ",ParticleName(p[i]->GetID()));
    nppnt[nprim++]=i;
  }

  printf(",");

  for(i=0;i<nprim;i++){
    if( ndau[p[nppnt[i]]->GetSerial()] ==0 ) continue;
    printf(" %s -> ",ParticleName(p[nppnt[i]]->GetID()));
    for(Int_t j=2;j<n;j++){
      if( TMath::Abs(p[j]->GetMother()) == p[nppnt[i]]->GetSerial()) {
	printf(" %s ",ParticleName(p[j]->GetID()));
      }
    }
    if( i < nprim-1 ) printf(",");
  }

  printf("\n");

}



//_________________________________________________________________
Char_t *ParticleName(Int_t id)
{
  // Return name of particle names


  const Int_t maxp=300;
  static Char_t *pname[maxp]={0};
  pname[1]="d"   ; pname[2]="u"; 
  pname[3]="s"   ; pname[4]="c" ; 
  pname[5]="b"   ; pname[6]="t";
  pname[7]="D"   ; pname[8]="U";
  pname[11]="e"  ; pname[12]="neu_e";
  pname[13]="mu" ; pname[14]="neu_mu";
  pname[15]="tau"; pname[16]="neu_tau";
  pname[17]=" L"  ; pname[18]="neu_L";
  pname[21]="gluon" ; pname[22]="gamma";
  pname[23]="Z0" ; pname[24]="W+";  pname[25]="H0";
  static Char_t str[20];

  if( id > 0 && id < 26 )  return pname[id];
  elseif( id > -20&& id < 0 )  {
    sprintf(str,"%s-bar",pname[TMath::Abs(id)]);
    return str;
  }
  elseif( id == -24 ) return "W-";    
  
  return "Undefined pid";
}  

//_________________________________________________________________
Char_t *MSTIInfo(Int_t id, Int_t ival)
{
  // Returns strings explaining meanings of MSTI array.
  // See page 141 of Pythia document.
  // ISUB code is described in p.98 of Pythia Document 

  const Int_t maxprocess=200;
  Char_t *processname[maxprocess]={0};
  processname[1]="e+e- -> gamma*/Z0 ";
  processname[22]="e+e- -> Z0 Z0";
  processname[24]="e+e- -> Z0 H0";
  processname[25]="e+e- -> W+ W-";
  processname[103]="e+e- -> e+e- H0 (gamma+gamma->H0)";
  processname[123]="e+e- -> e+e- H0 (ZZ fusion)";
  processname[124]="e+e- -> e+e- H0 (WW fusion)";

  switch (id) {
    //*****************************************************
    case 1:
      switch (ival) {
        case 0 :
	  printf("ISUB code and process name\n");
	  for(Int_t i=0;i<maxprocess;i++){
	    if( processname[i] == 0 ) continue;
	    printf(" ISUB=%3d : %s\n",i,processname[i]);
	  } 
	  break;  // end of id=1, ival=0

        default:  // default of id=1, ival switch
	  if( processname[ival] == 0 ) {
	    printf("No process name available for ISUB=%d\n",ival);
	  }
	  else {
	    return processname[ival];
	  }
      } 
      break;  // End of id=1 


    //*****************************************************
    default:  // Default of id switch
      printf("MSTIInfo .. id=%d is invalid.\n");
 }

  return "\00";

}

