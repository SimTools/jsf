// Read a weight file of JETNET and make a efficiency plot.
//
// $Id$

static const Int_t indim=4;
static const Int_t nftrain=3;
static const Int_t nevtrain=5000;
static const Int_t kinsize=nevtrain*nftrain;
TFile **ftrain=0;

Int_t ctagplot()
{


  gSystem->Load("libJSFJETNET.so");

  // Read data for test plot

  ftrain=(TFile**)new void[3];  // data for training.
  ftrain[0]=new TFile("train.cc.root"); 
  ftrain[1]=new TFile("train.bb.root");
  ftrain[2]=new TFile("train.ss.root");

  Float_t testin[indim][kinsize]; // Data for test.
  Float_t testout[kinsize];
  const Int_t kmsptm=5;
  const Int_t kdl=6;
  const Int_t kmsec=7;
  const Int_t knsec=10;
  Int_t ntrain=0;  // Number of events used for training.
  Int_t ntest=0;   // Number of events used for test.
  for(Int_t nf=0;nf<nftrain;nf++){
    ftrain[nf]->cd();
    printf("Reading a file, %s\n",ftrain[nf]->GetName());
    TNtuple *nt=(TNtuple*)ftrain[nf]->Get("ntuple");
    Int_t nev=0;
    Int_t nread=0;
    while(nread<2*nevtrain&&nt->GetEvent(nev)){
      Float_t *var=nt->GetArgs();
      if( var[kmsptm]>0.1 && var[kmsptm]<7.0 ) {
	if( nread%2 != 0 ) {
	  testin[0][ntrain]=var[kmsptm];
	  testin[1][ntrain]=var[kdl];
	  testin[2][ntrain]=var[kmsec];
	  testin[3][ntrain]=var[knsec];
	  testout[ntrain]=0.0;
	  if( nf==0 ) { testout[ntrain]=1.0; }
	  ntrain++;
	}
	nread++;
      }
      nev++;
    }
  }
  printf(" Obtained %d events in total\n",ntrain);



  TFile *cnout=new TFile("ctag-weight.root");
  JSFJETNET *jn=(JSFJETNET*)cnout->Get("JSFJETNET");


// Plot results after training.
  TH1F **hrsl=(TH1F**)new void[3];
  hrsl[0]=new TH1F("hsig","hsig",50,0.,1.);
  hrsl[1]=new TH1F("hbkg1","hbkg1",50,0.,1.);
  hrsl[2]=new TH1F("hbkg2","hbkg2",50,0.,1.);
  for(Int_t i=0;i<nftrain;i++){
    for(Int_t j=0;j<nevtrain;j++){
      Int_t jp=nevtrain*i+j;
      for(Int_t k=1;k<=jn->GetMSTJN(10);k++){
	jn->SetOIN(k,testin[k-1][jp]);
      }
      jn->JNTEST();
      hrsl[i]->Fill(jn->GetOUT(1));
    }
  }

  TCanvas *cn1=new TCanvas();
  hrsl[0]->SetLineColor(2);
  hrsl[1]->SetLineColor(3);
  hrsl[2]->SetLineColor(4);
  hrsl[0]->Draw();
  hrsl[1]->Draw("same");
  hrsl[2]->Draw("same");

}
