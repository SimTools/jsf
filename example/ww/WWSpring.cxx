//*LastUpdate:  jsf-1-14 29-Feburary-2000 Akiya Miyamoto
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
//
//$Id$
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

// Variable conversion.
#if __JSF_VERSION__ == 1 && __JSF_MINORVERSION__ <= 13

#else
#define fNDIM  ndim 
#define fNWILD nwild
#define fXL    xl
#define fXU    xu
#define fNCALL ncall
#define fACC1  acc1
#define fACC2  acc2
#define fITMX1 itmx1
#define fITMX2 itmx2
#endif

// =====================================================================
//  COMMONs for WW calculation, see WWZPRM.inc for more details.
// =====================================================================
typedef struct {
  Double_t rs, emins, eplus, ebeam, xg[25], ahmas, ahwid ;
  Int_t    nbson, nfbmas;
  Double_t pgen[10][4];
  Int_t    ngdcy[4], nbsspr, nbtype;
} COMMON_WWZPRM;             //  Common /WWZPRM/

extern COMMON_WWZPRM wwzprm_;


extern "C" {
#if __JSF_VERSION__ == 1 && __JSF_MINORVERSION__ <= 13
extern void usrout_();
#endif
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

   fACC1  = 0.04 ;   // precision at 1st step.
   fACC2  = 0.01 ;   // precision at 2nd step.
   fITMX1 = 100 ;  // max number of iteration at 1st step.
   fITMX2 = 100 ;  // max number of iteration at 1st step.
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

#if __JSF_VERSION__ == 1 && __JSF_MINORVERSION__ <= 13
  fPrintInfo = gJSF->Env()->GetValue("WWBases.PrintInfo",kTRUE);
  fPrintHist = gJSF->Env()->GetValue("WWBases.PrintHist",kTRUE);
#elsif __JSF_VERSION__ == 1 && __JSF_MINORVERSION__ == 14
  Userin();
#endif


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


#if __JSF_VERSION__ >= 1 ||  __JSF_MINORVERSION__ > 13
  Char_t hn[8];
  for(Int_t i=0;i<14;i++) {
    sprintf(hn,"x%2.2d",i+1);
    H1Fill(hn,x[i],val);
  }
  /*
      CALL XHFILL( 15,DFLOAT(NDMOD(1)), FUNC )
      CALL XHFILL( 16,DFLOAT(NDMOD(2)), FUNC )
      XMWM = UH4MAS( PFMOM(0,1), PFMOM(0,2) )
      XMWP = UH4MAS( PFMOM(0,3), PFMOM(0,4) )
      CALL XHFILL( 17, DBLE(XMWM), FUNC )
      CALL XHFILL( 18, DBLE(XMWP), FUNC )
      XRSCOL = RSCOL/EBEAM*0.5
      CALL XHFILL( 19,DBLE(XRSCOL), FUNC )
  */
#endif

  return val;

}

//_____________________________________________________________________________
void WWBases::Userin()
{
//
//   Initialize User parameters for Bases
//
#if __JSF_VERSION__ == 1 && __JSF_MINORVERSION__ <= 13
  JSFBases::Userin();  // Call JSFBases::Userin() for standard setup.
#endif

  // Copy class data member into common /wwzprm/
  wwzprm_.ebeam  = fEbeam   ;
  wwzprm_.rs     = 2.*fEbeam;
  wwzprm_.nbson  = fNBSON   ;
  wwzprm_.nbtype = fNBTYPE ;

  //
  PrintParameters();

  // Define histograms

#if __JSF_VERSION__ == 1 && __JSF_MINORVERSION__ <= 13
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
#else
  H1Init("x01","ds/hel", 2, 0.0,  1.0 );
  H1Init("x02", "ds/d(W-mass)", 50, fXL[1], fXU[1] );
  H1Init("x03", "ds/d(W+mass)", 50, fXL[2], fXU[2]);
  H1Init("x04", " ds/dcosth(W-)", 50, fXL[3], fXU[3]);
  H1Init("x05", " ds/dcosth(f) of W-", 50, fXL[4], fXU[4]);
  H1Init("x06", " ds/dcosth(f) of W+", 50, fXL[5], fXU[5]);
  H1Init("x07", " ds/dX(Brems) ", 50, fXL[6], fXU[6]);
  H1Init("x08", " ds/dX(Beamst(E-)", 50, fXL[7], fXU[7]);
  H1Init("x09", " ds/dX(Beamst(E+)", 50, fXL[8], fXU[8]);
  H1Init("x10", " ds/dphi of W-", 50, fXL[9], fXU[9]);
  H1Init("x11", " ds/dphi of f of W-", 50, fXL[10], fXU[10]);
  H1Init("x12", " ds/dphi of f of W+", 50, fXL[11], fXU[11]);
  H1Init("x13", " ds/ddecay mode of W-", 50, fXL[12], fXU[12]);
  H1Init("x14", " ds/ddecay mode of W+", 50, fXL[13], fXU[13]);
  //
  /*
  H1Init("x15", "Decay mode of W- ", 12, 1.0, 12.0);
  H1Init("x16", "Decay mode of W+ ", 12, 1.0, 12.0);
  H1Init("x17", "W- invariant mass ", 50, 0.0, fEbeam);
  H1Init("x18", "W+ invariant mass ", 50, 0.0, fEbeam);
  H1Init("x19",  "Effective root-S ", 50, 0.0, 1.0);
  */
#endif

  return ;
}

//_____________________________________________________________________________
void WWBases::Userout()
{
#if __JSF_VERSION__ == 1 && __JSF_MINORVERSION__ <= 13
  usrout_();
#else
  printf("End of Bases of ee --> WW process\n");
  printf("NBSON(Beamstruhlung)     =%d\n",fNBSON);
  printf("NBTYPE(BeamstruhlungType =%d\n",fNBTYPE);
  printf("Ebeam                    =%g (GeV)\n",fEbeam);
  printf("Total Cross section      =%g +- %g (fb)\n",GetEstimate(),GetError());
  printf("Number of iteration      =%d\n",GetNoOfIterate());  
#endif

}




