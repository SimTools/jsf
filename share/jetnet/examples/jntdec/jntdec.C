//
// jntdec program written by ROOT macro
// To run, do
//    $ jsf -b jntdec.C
//
//$Id$
//

Int_t jntdec(Int_t method=0)
{
//C...JetNet subroutine Test-DECk


//C...Runs a test-program using data from two overlapping Gaussian
//C...distributions in the input space. The test-program uses the
//C...method specified by METHOD.

  static const Int_t indim=5;
  static const Int_t hidden=10;
  static const Int_t ntrain=5000;
  static const Int_t ntest=10000;
  static const Int_t nepoch=100;

  static const Float_t wid1=1.0;
  static const Float_t wid2=2.0;
  static const Float_t xi=0.00;
  static const Float_t bayes=85.2;

  gSystem->Load("libJSFJETNET.so");
  JSFJETNET jn;
  
  Float_t tin[indim][ntrain+ntest];
  Float_t tout[ntrain+ntest];

  Char_t blk15[]="               ";

  printf("%s%sJETNET Test-Deck\n",blk15);
  printf("%sTwo overlapping Gaussian distributions in %2d dimensions.\n",blk15,indim);
  printf("%sTheir standard deviations are %3.1f and %f3.1\n",blk15,wid1,wid2);
  printf("%sTheir mean values are separated by %4.2\n",blk15,xi);

  printf("\n");
  
  //C...Generate data:
  printf("%sGenerating training and test patterns...\n");
  for(Int_t ipat=1;ipat<=ntrain+ntest;ipat++){
    Int_t idum=ipat;
    if( gRandom->Rndm() > 0.5 ) {
      for(Int_t i=1;i<=indim;i++){
	tin[i-1][ipat-1]=wid1*gRandom->Gaus();
      }
      tout[ipat-1]=1.0;
    }
    else {
      tin[0][ipat-1]=wid2*gRandom->Gaus()+xi;
      for(Int_t i=2;i<=indim;i++){
	tin[i-1][ipat-1]=wid2*gRandom->Gaus();
      }
      tout[ipat-1]=0.0;
    }
  }
  
  printf("%s...done generating data.\n",blk15);

  //C...Set network architecture: MSTJN(1)-layered network with 
  //C...MSTJN(11) hidden nodes, MSTJN(12) output nodes and 
  //C...MSTJN(10) inputs.
  jn.SetMSTJN(1,3);
  jn.SetMSTJN(10,indim);
  jn.SetMSTJN(11,hidden);
  jn.SetMSTJN(12,1);

  //C...Set sigmoid function: 
  jn.SetMSTJN(3,1);

  //C...Initial width of weights:
  jn.SetPARJN(4,0.5);

  //C...Choose updating method
  jn.SetMSTJN(5,method);

  if( ( jn.GetMSTJN(5) == 8 ) || ( jn.GetMSTJN(5) == 9 )
    ||( jn.GetMSTJN(5) == 14) || ( jn.GetMSTJN(5) < 0  )
    ||( jn.GetMSTJN(5) > 15 ) ) {
    printf(" Undefined training algorithm in call to JNTDEC\n");
    return 0;
  }

  //C...Initialize network:
  jn.JNINIT();

  //C...Set parameters suitable for the given method of updating
  if( jn.GetMSTJN(5) == 0 ) {
    //C...Normal Backprop
    jn.SetPARJN( 1,2.0);
    jn.SetPARJN( 2,0.5);
    jn.SetPARJN(11,0.999);
  }
  else if( jn.GetMSTJN(5) == 1 ) {
    //C...Manhattan
    jn.SetPARJN( 1,0.05);
    jn.SetPARJN( 2,0.5);
    jn.SetPARJN(11,-0.99);
  }
  else if( jn.GetMSTJN(5) == 2 ) {
    //C...Langevin
    jn.SetPARJN( 1,1.0);
    jn.SetPARJN( 2,0.5);
    jn.SetPARJN( 6,0.01);
    jn.SetPARJN(11,0.999);
    jn.SetPARJN(20,0.99);
  }
  else if( jn.GetMSTJN(5) == 3 ) {
    //C...Quickprop
    jn.SetPARJN( 1,2.0);
    jn.SetPARJN( 2,0.0);
    jn.SetPARJN( 6,0.0);
    jn.SetPARJN(11,1.0);
    jn.SetPARJN(20,1.0);
    jn.SetMSTJN( 2,ntrain);
  }
  else if( jn.GetMSTJN(5) >= 4 && jn.GetMSTJN(5) <= 7 ) {
    //C...Conjugate Gradient
    jn.SetPARJN( 1,1.0);
    jn.SetMSTJN( 2,ntrain);
  }
  else if( jn.GetMSTJN(5) >= 10 && jn.GetMSTJN(5) <= 13 ) {
    //C...Scaled Conjugate Gradient
    jn.SetMSTJN( 2,ntrain);
  }
  else if( jn.GetMSTJN(5) == 15 ) {
    //C...Rprop
    jn.SetPARJN( 1,1.0);
    jn.SetMSTJN( 2,ntrain);
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
    }
    Float_t train=(Float_t)nright/(Float_t)ntrain;

    if( iepoch%10 == 0 ) {
//C...Testing loop:
      nright=0;
      for(Int_t ipat=ntrain+1;ipat<=ntrain+ntest;ipat++){

//C...Put pattern into OIN:
	for(Int_t i=1;i<jn.GetMSTJN(10);i++){
	  jn.SetOIN(i,tin[i-1][ipat-1]);
	}

//C...Get network output:
	jn.JNTEST();
//C...Calculate performance on test set (=generalization):
	if( TMath::Abs(jn.GetOUT(1)-tout[ipat-1]) < 0.5 ) {
	  nright++;
	}
      }
      Float_t test=(Float_t)nright/(Float_t)ntest;
      if( jn.GetMSTJN(5)>3 && jn.GetMSTJN(5)<15 ) {
	if( train > trnmx ) {
	  trnmx=train;
	  testmx=test;
	}
	test=testmx;
	train=trnmx;
      }

//C...Display performance:
      printf("%8d   /%9.3f   /%f9.3  \n",iepoch,train,test);
    }

//C...Terminate CG and SCG training:
    if( iepoch == nepoch-1 ) {
      if( jn.GetMSTJN(5) > 3 && jn.GetMSTJN(5) < 15 ) {
	if( jn.GetMSTJN(5) < 9 ) {
	  jn.SetMSTJN(5,8);
	}
	else {
	  jn.SetMSTJN(5,14);
	}
	trnmx=0.0;
	testmx=0.0;
      }
    }
  }
      
  printf("\n");
  printf(" The optimal generalization performance is %4.1f\n",bayes);

  switch(method){
  case 0:
    printf("' Backprop should reach (81.0 +- 2.2)%% in 100 epochs\n");
    break;
  case 1:
    printf(" Manhattan should reach (84.3 +- 0.6)%% in 100 epochs\n");
    break;
  case 2:
    printf(" Langevin should reach (82.9 +- 1.8)%% in 100 epochs\n");
    break;
  case 3:
    printf(" Quickprop should reach (82.8 +- 8.8)%% in 100 epochs\n");
    break;
  case 4:
    printf(" Polak-Ribiere CG should reach (79.0 +- 7.0)%% in 100 epochs\n");
    break;
  case 5:
    printf(" Hestenes-Stiefel CG should reach (79.8 +- 5.6)%% in 100 epochs\n");
    break;
  case 6:
    printf(" Fletcher-Reeves CG should reach (79.6 +- 5.6)%% in 100 epochs\n");
    break;
  case 7:
    printf("Shanno CG should reach (71.7 +- 11.6)%% in 100 epochs\n");
    break;
  case 10:
    printf(" Polak-Ribiere SCG should reach (84.0 +- 1.6)%% in 100 epochs\n");
    break;
  case 11:
    printf(" Hestenes-Stiefel SCG should reach (84.1 +- 2.6)%% in 100 epochs\n");
    break;
  case 12:
    printf(" Fletcher-Reeves SCG should reach (81.4 +- 5.2)%% in 100 epochs\n");
    break;
  case 13:
    printf(" Shanno SCG should reach (70.7 +- 8.1)%% in 100 epochs\n");
    break;
  case 15:
    printf(" Rprop should reach (83.5 +- 2.2)%% in 100 epochs\n");
    break;
  }
  
  return 0;
}
