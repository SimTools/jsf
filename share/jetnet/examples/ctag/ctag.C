//
// A sample program to train jetnet. 
// Input data are prepared by a script, mkdata.
// The mkdata command creates root files, each containing
// 1000 events of e+e- -> Z -> bbar, ccbar, ssbar.
// First 500 jet information saved in ntuple are used
// to train network.  The next 500 jet information are
// used to test performance of the neural network. 
//
// Please note that this is a very simple-minded program to 
// show an example of using JETNET for  physics study.
// Due to limitted number of variables used for the training,
// c/b separation obtained by this program is not perfect.
// I hope further improvements are implemented by JSF users.
//
// Originally coded by Akiya Miyamoto   14-January-2002
//
//$Id$
//

TFile **ftrain=0;

static const Int_t indim=4;
static const Int_t nevtrain=5000;
static const Int_t hidden=10;
static const Int_t nftrain=3;
static const Int_t kinsize=nevtrain*nftrain;
static const Int_t nftest=3;
static const Int_t nepoch=500;

//_____________________________________________________________
Int_t ctag()
{

  gSystem->Load("libJSFJETNET.so");

  ftrain=(TFile**)new void[3];  // data for training.
  ftrain[0]=new TFile("train.cc.root"); 
  ftrain[1]=new TFile("train.bb.root");
  ftrain[2]=new TFile("train.ss.root");

  TH1F *htrain=new TH1F("htrain","Eff vs epoch",nepoch,0.,(Float_t)nepoch);

  //
  // Prepare input data for jetnet.
  //
  // We use, (msptm,dl,msec,nsec ), for JETNET

  Float_t tin[indim][kinsize];    // Data for training.
  Float_t tout[kinsize];          // 
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
      //      if( var[knsec] > 1 && var[kdl] > 0.01) {
      //      if( var[knsec] > 1 ) {
      if( var[kmsptm]>0.1 && var[kmsptm]<7.0 ) {
	if( nread%2 == 0 ) {
	  tin[0][ntrain]=var[kmsptm];
	  tin[1][ntrain]=var[kdl];
	  tin[2][ntrain]=var[kmsec];
	  tin[3][ntrain]=var[knsec];
	  tout[ntrain]=0.0;
	  if( nf==0 ) { tout[ntrain]=1.0; }
	}
	else {
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

  //C...Set network architecture: MSTJN(1)-layered network with 
  //C...MSTJN(11) hidden nodes, MSTJN(12) output nodes and 
  //C...MSTJN(10) inputs.
  JSFJETNET jn;
  jn.SetMSTJN(1,3);
  jn.SetMSTJN(10,indim);
  jn.SetMSTJN(11,hidden);
  jn.SetMSTJN(12,1);

 
  //C...Set sigmoid function: 
  jn.SetMSTJN(3,1);

  //C...Initial width of weights:
  jn.SetPARJN(4,0.5);

  //C...Choose updating method
  Int_t method=0;
  jn.SetMSTJN(5,method);

  //C...Initialize network:
  jn.JNINIT();

  //C...Set parameters suitable for the given method of updating
  if( jn.GetMSTJN(5) == 0 ) {
    //C...Normal Backprop
    jn.SetPARJN( 1,2.0);
    jn.SetPARJN( 2,0.5);
    jn.SetPARJN(11,0.999);
  }


//C...Define the size of one epoch. Note that for batch training, the
//C...number of patterns per update, MSTJN(2), must be set to the
//C...total number of training patterns, and hence MSTJN(9), the
//C...number of updates per epoch must be set to one.

  jn.SetMSTJN( 9, TMath::Max(1,ntrain/jn.GetMSTJN(2)));

//C...Other parameters keep their default values.

  printf("\n");
  printf("   Epoch   /  Training  /  General. \n");
    
  Float_t testmx=0.0;
  Float_t trnmx=0.0;

//C...Main loop over epochs:
  for(Int_t iepoch=1;iepoch<=nepoch;iepoch++){

//C...Training loop:
    Int_t nright=0;
    Int_t ipat;
    for(Int_t ip=1;ip<=ntrain;ip++){
      if( jn.GetMSTJN(5) <= 2 ) {
//C...Note that for non-batch training it is often a good idea to pick
//C...training patterns at random
	ipat=1+(Int_t)(gRandom->Rndm()*(Float_t)ntrain);
      }
      else {
	ipat=ip;
      }

//C...Put pattern into OIN:
      for(Int_t i=1;i<=jn.GetMSTJN(10);i++){
	jn.SetOIN(i, tin[i-1][ipat-1]);
      }
//C...Put target output value into OUT:
      jn.SetOUT(1,tout[ipat-1]);

//C...Invoke training algorithm:
      jn.JNTRAL();

//C...Calculate performance on training set:
      if( TMath::Abs(jn.GetOUT(1)-tout[ipat-1]) < 0.5 ) {
	nright++;
      }
      //      printf(" jnout=%g\n",jn.GetOUT(1));
      
    }
    Float_t train=(Float_t)nright/(Float_t)ntrain;
    htrain->SetBinContent(iepoch, train);

    printf(" iepoch=%d train=%g\n",iepoch, train);
  }

// Plot results after training.
  TH1F **hrsl=(TH1F**)new void[3];
  hrsl[0]=new TH1F("hsig","hsig",50,0.,1.);
  hrsl[1]=new TH1F("hbkg1","hbkg1",50,0.,1.);
  hrsl[2]=new TH1F("hbkg2","hbkg2",50,0.,1.);
  for(Int_t i=0;i<nftrain;i++){
    for(Int_t j=0;j<nevtrain;j++){
      Int_t jp=nevtrain*i+j;
      for(Int_t k=1;k<=jn.GetMSTJN(10);k++){
	jn.SetOIN(k,testin[k-1][jp]);
      }
      jn.JNTEST();
      hrsl[i]->Fill(jn.GetOUT(1));
    }
  }

  TCanvas *cn1=new TCanvas();
  hrsl[0]->SetLineColor(2);
  hrsl[1]->SetLineColor(3);
  hrsl[2]->SetLineColor(4);
  hrsl[0]->Draw();
  hrsl[1]->Draw("same");
  hrsl[2]->Draw("same");

  TCanvas *cn2=new TCanvas();
  htrain->SetMarkerStyle(8);
  htrain->SetLineColor(2);
  htrain->SetMinimum(0.5);
  htrain->Draw("p");
  

  return 0;
}
  




