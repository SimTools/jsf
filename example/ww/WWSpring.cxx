//*LastUpdate:  v.01.01 12-December-1998 Akiya Miyamoto
//*-- Author :  12-December-1998  Akiya Miyamoto

//////////////////////////////////////////////////////////////////
//
//  WWSpring
//  
//  e+e- -> w+w-
//  
//  In this program, meaning of integration variables are as follows.
//
//  Definition of vairables
//   1 ; Polarization variable.             0 --> 1
//   2 ; Invariant mass of W-               0 --> 1
//   3 ; Invariant mass of W+               0 --> 1
//   4 ; Cos(th) of W-                     -1 --> 1
//   5 ; Cos(th)-hat of f in W- system     -1 --> 1
//   6 ; Cos(th)-hat of f in W+ system     -1 --> 1
//   7 ; Parameter for Bremstrulung         0 --> 1
//   8 ; E- beam                            0 --> 1
//   9 ; E+ beam                            0 --> 1
//  10 ; Phi/2pi of W-                      0 --> 1
//  11 ; Phi-hat/2pi of f in W- system      0 --> 1
//  12 ; phi-hat/2pi of f in W+ system      0 --> 1
//  13 ; Decay mode of W-                   0 --> 1
//  14 ; Decay mode of W+                   0 --> 1
//
//      NDIM  = 14
//      NGDIM = 10
//////////////////////////////////////////////////////////////////

#include "JSFModule.h"
#include "JSFLCFULL.h"
#include "JSFSpring.h"
#include "JSFSteer.h"
#include "JSFBases.h"
#include "JSFHadronizer.h"
#include "WWSpring.h"

ClassImp(WWSpring)
ClassImp(WWSpringBuf)
ClassImp(WWBases)

extern "C" {
extern void usrout_();
extern Double_t func_(double x[]);
extern void spevnt_(Int_t *nret);
};

//_____________________________________________________________________________
WWSpring::WWSpring(const char *name, const char *title,
			 WWBases *bases)
  : JSFSpring(name, title, bases)
{
  fEventBuf = new WWSpringBuf("WWSpringBuf", 
	 "WWSpring event buffer", this);
  if( !bases ) { 
    WWBases *bs=new WWBases();
    SetBases(bs);
  }
}


//_____________________________________________________________________________
WWSpring::~WWSpring()
{
  if( !fEventBuf ) delete fEventBuf;
}


//_____________________________________________________________________________
Bool_t WWSpring::Initialize()
{
  // Make sure to set JSFHadronize::fCopySpringClassDataToBank = kFALSE

  JSFSpring::Initialize();

  JSFHadronizer *had=(JSFHadronizer*)gJSF->FindModule("JSFHadronizer");
  if(had) had->SetCopySpringClassDataToBank(kFALSE);
  return kTRUE;
}


//_____________________________________________________________________________
void WWSpringBuf::Spevnt(Int_t &iret) { spevnt_(&iret); }

//_____________________________________________________________________________
WWBases::WWBases(const char *name, const char *title)
           : JSFBases(name, title)
{
//  Constructor of bases.  Default parameter should be initialized here
//

   fACC1  = 0.4 ;   // precision at 1st step.
   fACC2  = 0.2 ;   // precision at 2nd step.
   fITMX1 = 10 ;  // max number of iteration at 1st step.
   fITMX2 = 10 ;  // max number of iteration at 1st step.
   fNCALL = 100000 ;

   fNDIM =14 ;
   fNWILD=10;
// Get parameters from jsf.conf, if specified.
  Char_t pname[40];
  for(Int_t i=0;i<fNDIM;i++){
    sprintf(pname,"WWBases.X%i2.2Range",i+1);
    if( i==3 || i==4 || i== 5 ) {
      sscanf(gJSF->Env()->GetValue(pname,"-1.0 1.0"),"%lg%lg",&fXL[i],&fXU[i]);
    } else {
      sscanf(gJSF->Env()->GetValue(pname,"0.0 1.0"),"%lg%lg",&fXL[i],&fXU[i]);
    }
  }

  sscanf(gJSF->Env()->GetValue("WWBases.Ebeam","250.0"),"%g",&fEbeam);
  sscanf(gJSF->Env()->GetValue("WWBases.NBSON","0"),"%d",&fNBSON);
  sscanf(gJSF->Env()->GetValue("WWBases.NBTYPE","0"),"%d",&fNBTYPE);

  fPrintInfo = gJSF->Env()->GetValue("WWBases.PrintInfo",kTRUE);
  fPrintHist = gJSF->Env()->GetValue("WWBases.PrintHist",kTRUE);

}


//_____________________________________________________________________________
void WWBases::PrintParameters()
{
//  Print parameters
//
  
  printf("Parameters for ee->WW generator\n");
  printf("  Ebeam =%g (GeV)\n",fEbeam);
  printf("  Flag for Beamstruhlung(NBSON) =%d\n",fNBSON);
  printf("       = 0 ; Neither Beamstrulung nor Bremstrulung\n");
  printf("       = 1 ; With Beamstrulung\n");
  printf("       = 2 ; With Bremstrulung\n");
  printf("       = 3 ; With Beamstrulung and Bremstrlung\n");
  printf("  Type of Beamstrahlung spectrum(NBTYPE) = %d\n",fNBTYPE);

  printf("  Bases integration parameters..\n");
  printf("  ITMX1=%d  ITMX2=%d  NCALL=%d\n",fITMX1, fITMX2, fNCALL);
  printf("  ACC1 =%g  ACC2 =%g\n",fACC1,fACC2);

  return ;

}

//_____________________________________________________________________________
Double_t WWBases::Func(Double_t x[])
{
//  Bases Integrand
//
  double val=func_(x);
  return val;

}

//_____________________________________________________________________________
void WWBases::Userin()
{
//
//   Initialize User parameters for Bases
//
  JSFBases::Userin();  // Call JSFBases::Userin() for standard setup.

  // Copy class data member into common /wwzprm/
  wwzprm_.ebeam  = fEbeam   ;
  wwzprm_.rs     = 2.*fEbeam;
  wwzprm_.nbson  = fNBSON   ;
  wwzprm_.nbtype = fNBTYPE ;

  //
  PrintParameters();

  // Define histograms

  Xhinit(1, 0.0,  1.0,  2,  "ds/hel");
  Xhinit(2, fXL[1], fXU[1], 50, "ds/d(W-mass)");
  Xhinit(3, fXL[2], fXU[2], 50, "ds/d(W+mass)");
  Xhinit(4, fXL[3], fXU[3], 50, " ds/dcosth(W-)");
  Xhinit(5, fXL[4], fXU[4], 50, " ds/dcosth(f) of W-");
  Xhinit(6, fXL[5], fXU[5], 50, " ds/dcosth(f) of W+");
  Xhinit(7, fXL[6], fXU[6], 50, " ds/dX(Brems) ");
  Xhinit(8, fXL[7], fXU[7], 50, " ds/dX(Beamst(E-)");
  Xhinit(9, fXL[8], fXU[8], 50, " ds/dX(Beamst(E+)");
  Xhinit(10, fXL[9], fXU[9], 50, " ds/dphi of W-");
  Xhinit(11, fXL[10], fXU[10], 50, " ds/dphi of f of W-");
  Xhinit(12, fXL[11], fXU[11], 50, " ds/dphi of f of W+");
  Xhinit(13, fXL[12], fXU[12], 50, " ds/ddecay mode of W-");
  Xhinit(14, fXL[13], fXU[13], 50, " ds/ddecay mode of W+");
  //
  Xhinit(15, 1.0, 12.0, 12, "Decay mode of W- ");
  Xhinit(16, 1.0, 12.0, 12, "Decay mode of W+ ");
  Xhinit(17, 0.0, fEbeam, 50, "W- invariant mass ");
  Xhinit(18, 0.0, fEbeam, 50, "W+ invariant mass ");
  Xhinit(19, 0.0, 1.0,    50,  "Effective root-S ");

  return ;
}

//_____________________________________________________________________________
void WWBases::Userout()
{
  usrout_();
}




