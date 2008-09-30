//////////////////////////////////////////////////////////////////
//
//  JSFHadronizer
//  
//  Hadronize parton information in JSFParton class and 
//  create JSFGeneratorParticle class.
//  
//$Id$  
//
//
// (Note)
// 2005/02/28 K.Fujii	pythia calls via fPythia currently broken,
// 			use direct calls.
//			TLorentzVector::Boost does not work, 
//			possibly precision problem, use ubstfd/bk.
//
//////////////////////////////////////////////////////////////////

#include "TRandom.h"
#include "JSFSteer.h"
#include "JSFLCFULL.h"  
#include "JSFHadronizer.h"

#if __PYTHIA_VERSION__ <= 5 
#define pyjoin_ lujoin_
#define pyshow_ lushow_
#define pyprep_ luprep_
#define pychge_ luchge_
#define pylist_ lulist_
#define pycomp_ lucomp_
#define pyexec_ luexec_

#define pydat1_ ludat1_
#define pydat2_ ludat2_
#define pydat3_ ludat3_
#define pydatr_ ludatr_
#define pyjets_ lujets_
#define MRPY   MRLU
#define RRPY   RRLU
#endif
#ifdef KF_DEBUG
#include <iomanip>
#include "TLorentzVector.h"
#endif

extern "C" {
#if __GNUC_VERSION__ >= 4
extern void  pydatawrapper_();
#endif
extern void  tauint_(int *inut, int *iout, int *jak1, 
		     int *jak2, int *itdkrc, int *keya1, float *xk0dec);
extern void  taudec_(int *kto, int *npnt, float *heltau, float p4tau[4]);

extern void  pyjoin_(int *njoin, int ijoin[]);
extern void  pyshow_(int *ip1, int *ip2, double *qmax);
extern void  pyprep_(int *ip);
extern int   pychge_(int *kf);
extern void  pylist_(int *flag);
extern int   pycomp_(int *kf);
extern void  pyexec_();
#ifndef __USE_TPYTHIA__
extern void  pyinit_(char *frame, char *beam, char *target, double *ecm, int len1, int len2, int len3);
#endif

#if __PYTHIA_VERSION__ >= 6 
extern void  pyhepc2_(int *flag, int *n, int *npad, int k[5][4000],
                                double p[5][4000], double v[5][4000],
                                int mstu[200], double paru[200],
                                int mstj[200], double parj[200],
                                int kchg[4][500], double pmas[4][500],
                                double parf[2000], double vckm[4][4]);
#else
extern void  luhepc2_(int *flag, int *n, int k[5][4000], float p[5][4000], 
                                                         float v[5][4000],
                                 int mstu[200], float paru[200],
                                 int mstj[200], float parj[200],
                                 int kchg[3][500], float pmas[4][500],
                                 float parf[2000], float vckm[4][4]);
#endif
extern void  ubstfd_(float pb[4], float pr[4], float pa[4]);
extern void  ubstbk_(float pb[4], float pr[4], float pa[4]);
};


Int_t JAK1, JAK2, ITDKRC, KEYA1;
Float_t XK0DEC;

ClassImp(JSFHadronizer)

#if __PYTHIA_VERSION__ <= 5

typedef struct {
  Int_t   MSTU[200]; 
  Float_t PARU[200];
  Int_t   MSTJ[200]; 
  Float_t PARJ[200];
} Pydat1_t;

typedef struct {
  Int_t   KCHG[3][500]; 
  Float_t PMAS[4][500];
  Float_t PARF[2000];
  Float_t VCKM[4][4];
} Pydat2_t;

typedef struct {
  Int_t   MDCY[3][500];
  Int_t   MDME[2][2000];
  Float_t BRAT[2000];
  Int_t   KFDP[5][2000];
} Pydat3_t;

typedef struct {  // Common for JETSET random variables
  Int_t   MRPY[6];
  Float_t RRPY[100];
} Pydatr_t;

typedef struct {
  Int_t   N;
  Int_t   K[5][4000];
  Float_t P[5][4000];
  Float_t V[5][4000];
} Pyjets_t;

#endif

extern Pydat1_t pydat1_;
extern Pydat2_t pydat2_;
extern Pydat3_t pydat3_;
extern Pydatr_t pydatr_;
extern Pyjets_t pyjets_;

/*
#ifndef __LCLIBRAN_USE_RANMAR__ 
typedef struct { // Commo for Tauola random variables
  Float_t  u[98];
  Int_t ij97[2];
} COMMON_RASET1;
extern COMMON_RASET1 raset1_;
#endif
*/

//_____________________________________________________________________________
JSFHadronizer::JSFHadronizer(const char *name, const char *title)
       : JSFFULLGenerator(name,title)
{
//  Constructor of JSFHadronizer.  In this constructor, JSFSpring
//  module is searched from the defined JSF module.  last JSFModule
//  which is inherited from JSFSpring class is a class whose Parton
//  data is hadronized.

  fSpring=0;
  fCopySpringClassDataToBank=kTRUE;

  TList *list=gJSF->Modules();
  TIter  next(list);
  JSFModule *module;
  while( (module=(JSFModule*)next()) ){
    if( module->InheritsFrom("JSFSpring") ){ 
      if( fSpring ) { 
	 printf("Find module %s , inherited from JSFSpring\n",fSpring->GetName());
	 printf("More than one JSFSpring are defined.");
	 printf(" Specify correspinding Hadronizer explicityly\n");
      }
      fSpring=(JSFSpring*)module;
    }
  }
  if( !fSpring ){ Error("JSFHadronizer","No JSFSpring class was found"); }

  
  JAK1 = gJSF->Env()->GetValue("JSFHadronizer.JAK1",0);
  JAK2 = gJSF->Env()->GetValue("JSFHadronizer.JAK2",0);
  ITDKRC = gJSF->Env()->GetValue("JSFHadronizer.ITDKRC",1);
  KEYA1 = gJSF->Env()->GetValue("JSFHadronizer.KEYA1",1);
  fIHLON = gJSF->Env()->GetValue("JSFHadronizer.IHLON",1);
  sscanf(gJSF->Env()->GetValue("JSFHadronizer.XK0DEC","0.001"),"%g",&XK0DEC);

  fDebug = gJSF->Env()->GetValue("JSFHadronizer.DebugFlag",0);

  fDoesShower = gJSF->Env()->GetValue("JSFHadronizer.PartonShower",1);

#if __PYTHIA_VERSION__ >= 6
#if 0
  fPythia = new TPythia6();
#else
  fPythia = TPythia6::Instance();
#endif
#endif
}

//_____________________________________________________________________________
JSFHadronizer::~JSFHadronizer()
{
#if __PYTHIA_VERSION__ >= 6
  delete fPythia;
#endif

}

//_____________________________________________________________________________
Bool_t JSFHadronizer::Initialize()
{
#if __GNUC_VERSION__  >= 4
   pydatawrapper_();
#endif

// Initialize tauola

   Int_t inut=5;
   Int_t iout=6;
   tauint_(&inut, &iout, &JAK1, &JAK2, &ITDKRC,  &KEYA1, &XK0DEC);

   Double_t  mh     = gJSF->Env()->GetValue("JSFHadronizer.HiggsMass",120.);
#ifdef __USE_TPYTHIA__
#if 0
   fPythia->SetMDCY(23,1,0);
   fPythia->SetMDCY(24,1,0);
   fPythia->SetMDCY(33,1,0);
#endif
   // Invoke Pyinit with dummy arguments to initialize decay table
   Char_t   *frame  = const_cast<Char_t *>("CMS");
   Char_t   *beam   = const_cast<Char_t *>("e+");
   Char_t   *target = const_cast<Char_t *>("e-");
   Double_t  ecm    = 1000.;
   fPythia->SetPMAS(25,1,mh);
   fPythia->Pyinit(frame, beam, target, ecm);
#else
   // Int_t one=1;
#if 0
   pydat3_.MDCY[0][22]=0; // Z
   pydat3_.MDCY[0][23]=0; // W
   pydat3_.MDCY[0][32]=0; // Z'
#endif
   // Invoke Pyinit with dummy arguments to initialize decay table
   Char_t   *frame  = const_cast<Char_t *>("CMS");
   Char_t   *beam   = const_cast<Char_t *>("e+");
   Char_t   *target = const_cast<Char_t *>("e-");
   Double_t  ecm    = 1000.;
   pydat2_.PMAS[0][24] = mh;
   pyinit_(frame, beam, target, &ecm, 3, 2, 2);
#endif
   //--
   // Select H decay mode if requested
   //--
   Int_t mdcyh = gJSF->Env()->GetValue("JSFHadronizer.DecayModeForH",0);
   if (mdcyh) {
     //--
     // mdcyh =  1;	// H --> d dbar
     //       =  2;	// H --> u ubar
     //       =  3;	// H --> s sbar
     //       =  4;	// H --> c cbar
     //       =  5;	// H --> b bbar
     //       =  6;	// H --> t tbar
     //       =  7;	// H --> b' b'bar
     //       =  8;	// H --> t' t'bar
     //       =  9;	// H --> e- e+
     //       = 10;	// H --> mu- mu+
     //       = 11;	// H --> tau- tau+
     //       = 12;	// H --> tau'- tau'+
     //       = 13;	// H --> glue glue
     //       = 14;	// H --> gamma gamma
     //       = 15;	// H --> gamma Z0
     //       = 16;	// H --> Z0 Z0
     //       = 17;	// H --> W+ W-
     //--
     Int_t kh     = 25;
#ifdef __USE_TPYTHIA__
     Int_t kc    = fPythia->Pycomp(kh);
     Int_t mdcy2 = fPythia->GetMDCY(kc,2);
     Int_t mdcy3 = fPythia->GetMDCY(kc,3);
     for (Int_t i=mdcy2; i<=mdcy2+mdcy3-1; i++) fPythia->SetMDME(i,1,0);
     fPythia->SetMDME(mdcy2+mdcyh-1,1,1);
#else
     Int_t kc = pycomp_(&kh);
     Int_t mdcy2 = pydat3_.MDCY[1][kc-1];
     Int_t mdcy3 = pydat3_.MDCY[2][kc-1];
     for (Int_t i=mdcy2; i<=mdcy2+mdcy3-1; i++) pydat3_.MDME[0][i-1] = 0;
     pydat3_.MDME[0][mdcy2+mdcyh-2] = 1;
#endif
     cerr << " ---------------------------------------------" << endl
          << " H decay restricted to mode: mdcyh = " << mdcyh << endl
          << " mdcy2 = " << mdcy2                             << endl
          << " mdcy3 = " << mdcy3                             << endl
          << " ---------------------------------------------" << endl;
   }
   Int_t iprint = gJSF->Env()->GetValue("JSFHadronizer.PrintDecayModeTable",0);
   if (iprint) {
     iprint = 12;
     pylist_(&iprint);
   }

   return kTRUE ;

}

//_____________________________________________________________________________
Bool_t JSFHadronizer::Process(Int_t ev)
{
//  Create JSFGeneratorParticle from JSFSpringParton.
//  For a time bing, first put JSFSpringParton in to TBS bank,
//  calls luhadr fortran program, and put results into JSFGeneratorParticle
//
 
  JSFFULLGenerator::Process(ev);

  Int_t i;
  for(i=0;i<6;i++){ fMRPY[i]=pydatr_.MRPY[i];}
  for(i=0;i<100;i++){ fRRPY[i]=pydatr_.RRPY[i];}
/*
#ifndef __LCLIBRAN_USE_RANMAR__
  for(i=0;i<98;i++){ fRASET1U[i]=raset1_.u[i];}
  for(i=0;i<2;i++){ fRASET1IJ97[i]=raset1_.ij97[i];}
#endif
*/

  Int_t iret;

  Hadronize(fSpring, iret);

  if ( iret < 0 ) return kFALSE;

  if( fDebug > 0 ) {
    printf(" End of JSFHadronizer::Process()\n");
  }

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFHadronizer::EndRun()
{
  Int_t i;
  for(i=0;i<6;i++){ fMRPY[i]=pydatr_.MRPY[i];}
  for(i=0;i<100;i++){ fRRPY[i]=pydatr_.RRPY[i];}
/*
#ifndef __LCLIBRAN_USE_RANMAR__
  for(i=0;i<98;i++){ fRASET1U[i]=raset1_.u[i];}
  for(i=0;i<2;i++){ fRASET1IJ97[i]=raset1_.ij97[i];}
#endif
*/

  if( fFile->IsWritable() ) {
    if( !JSFFULLGenerator::EndRun() ) return kFALSE;
    Write();
  }
  return kTRUE;
}


//_____________________________________________________________________________
Bool_t JSFHadronizer::GetLastRunInfo()
{
  // Read seed of previous run 

  Read(GetName());

  printf("Random seeds for JSFHadronizer were reset by ");
  printf("values from a file.\n");

  return kTRUE;
}

#if __ROOT_FULLVERSION__ >=30000
//___________________________________________________________________________
void JSFHadronizer::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFHadronizer.

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v=R__b.ReadVersion(&R__s, &R__c);
     if( R__v > 1 ) {
       JSFHadronizer::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
       return;
     }
     JSFFULLGenerator::Streamer(R__b);
     R__b.ReadStaticArray(fMRPY);
     R__b.ReadStaticArray(fRRPY);
#ifndef __LCLIBRAN_USE_RANMAR__
     R__b.ReadStaticArray(fRASET1U);
     R__b.ReadStaticArray(fRASET1IJ97);
#endif
     R__b.CheckByteCount(R__s, R__c, JSFHadronizer::IsA());

   } else {
     JSFHadronizer::Class()->WriteBuffer(R__b, this);
   }

}

#else
//______________________________________________________________________________
void JSFHadronizer::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFHadronizer.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      JSFFULLGenerator::Streamer(R__b);
      R__b.ReadStaticArray(fMRPY);
      R__b.ReadStaticArray(fRRPY);
#ifndef __LCLIBRAN_USE_RANMAR__
      R__b.ReadStaticArray(fRASET1U);
      R__b.ReadStaticArray(fRASET1IJ97);
#endif
   } else {
      R__b.WriteVersion(JSFHadronizer::IsA());
      JSFFULLGenerator::Streamer(R__b);
      R__b.WriteArray(fMRPY, 6);
      R__b.WriteArray(fRRPY, 100);
#ifndef __LCLIBRAN_USE_RANMAR__
      R__b.WriteArray(fRASET1U, 98);
      R__b.WriteArray(fRASET1IJ97, 2);
#endif
   }
}
#endif

// ____________________________________________________________
void JSFHadronizer::Hadronize(JSFSpring *spring, Int_t &nret)
{
  //(Function)
  //  Hadronize partons in JSFSpringParton objects
  //(Input)
  //  spring: pointer to the input spring objects
  //(Output)
  //  nret : =0 on sucess
  //       : =-1 on error
  //CC**********************************************************************
  //C*
  //C*===========================================----===
  //C*  Subroutine LUHADR( IDREC, LEVEL, IDEBUG, NRET )
  //C*===========================================----===
  //C*
  //C* (Function)
  //C*    LUND hadronizer.
  //C*    Hadronize particle in 'Spring:Particle_List' bank.
  //C* (Inputs)
  //C*    IDREC  ; record ID, use only ID=1
  //C*    LEVEL  ; not used.
  //C*    IDEBUG ; not used.
  //C* (Output)
  //C*    NRET  ; Return code.
  //C* (Update Record)
  //C*   91/08/27  K.Fujii		Derived from LUHADR by TAYM.
  //C*				Heavily modified.
  //C*   95/01/11  K.Fujii		Another big changes for implementation
  //C*				of tau polarization.
  //C*				ID=220000 is the LSP.
  //C*
  //CC**********************************************************************
 
  //      SUBROUTINE LUHADR( IDREC, LEVEL, IDEBUG, NRET )
 
  //      INTEGER*4       IDREC, LEVEL, IDEBUG, NRET
  //      COMMON /SSWORK/ IHEAD(20), NOELM(30), RBUF(20,30), RINLST(10,30),
  //     .                ROTLST(20, 400), KSTAT(30), RTMP(20),
  //     .                ISHUFL(30), ISHPR(2,30), ITMP(20),
  //     .                NSGRP(10), ISGRP(2,30,10), JSTAT(30),
  //     .                NPTLV(10), IPTLV(30,10), IDIDIT(30),
  //     .                NDKPTN(10), IDKPTN(30,10), INOSHW(30)
  //      COMMON /LUSEED/ NLSEED
  //      DATA  ILEVT/0/
  //C
  //C========< Entry Point >================================================
  //C
  //C--
  //C  Skip if non-event record.
  //C--
  //    NRET = 0
  //      IF ( IDREC.NE.1 ) RETURN
  //      ILSAVE = NLSEED
  //C--
  //C  Get 'Spring:Parton_List' into local common.
  //C--
  //C  The parton showering is handled level by level, since showers
  //C  from parent parton pairs might modify the momenta of their
  //C  daughter partons.
  //C  The level is defined by
  //C     RBUF(19,*) = ISLEV*100 + ISPAIR
  //C  where partons with the same ISLEV are passed to LUFRAG at one
  //C  time (that means they comprise a set of color singlet states and
  //C  that there is no parent-daughter relation among the set) and
  //C  partons with the same ISPAIR are paired in LUSHOW calls in
  //C  LUFRAG. 
  //C  Lepton pairs are required to have ISLEV = 0 and NDAU = 0 for
  //C  proper polarization handling when the pair contains a tau.
  //C  If you are satisfied with the default tau decay treatment, you
  //C  do not have to distinguish leptons from quarks in the decay
  //C  daughters from W or Z, etc.
  //C  Color singlet groups are distinguished by
  //C     RBUF(18,*) = ICF
  //C  where ICF must have different values for different color singlet
  //C  groups.
  //C  Helicities of the primary partons should be stored in
  //C     RBUF(17,*) = IHEL
  //C  when necessary as with tau leptons.
  //C--
  //      CALL TBNOEL(1, 'Spring:Parton_List', NPART, NOELM )
  //      IF ( NPART.GT.30 ) THEN
  //         PRINT *,'%Fatal Error in LUHADR...'
  //         PRINT *,'# of partons in ''SPring:Parton_list'' ',
  //     .           ' bank exceeds buffer size.'
  //         STOP
  //      ENDIF
  //      DO 100 IP = 1, NPART
  //         IE = NOELM(IP)
  //         CALL TBGET(1,'Spring:Parton_List',IE, NW, RBUF(1,IP), IRET )
  //100   CONTINUE
  //
#ifdef __USE_TPYTHIA__
  Int_t mdcyzs = fPythia->GetMDCY(23,1);
  Int_t mdcyws = fPythia->GetMDCY(24,1);
  Int_t mdcyhs = fPythia->GetMDCY(25,1);
#else
  Int_t mdcyzs =  pydat3_.MDCY[0][22];
  Int_t mdcyws =  pydat3_.MDCY[0][23];
  Int_t mdcyws =  pydat3_.MDCY[0][24];
#endif

  JSFGeneratorBuf *gbuf=(JSFGeneratorBuf*)EventBuf();
  TClonesArray &part=*(gbuf->GetParticles());
  nret=0;
  Int_t npgen=0;


  JSFSpringBuf *spevt=(JSFSpringBuf*)spring->EventBuf();
  TClonesArray *ps=spevt->GetPartons();
  Int_t npart=spevt->GetNpartons();
  
  Float_t rbuf[30][20];
  for(Int_t j=0;j<npart;j++){
    JSFSpringParton *p=(JSFSpringParton*)ps->At(j);
    rbuf[j][0]=p->fSer;
    rbuf[j][1]=p->fID;
    rbuf[j][2]=p->fMass;
    rbuf[j][3]=p->fCharge;
    rbuf[j][4]=p->fP[1];
    rbuf[j][5]=p->fP[2];
    rbuf[j][6]=p->fP[3];
    rbuf[j][7]=p->fP[0];
    rbuf[j][11]=p->fNdaughter;
    rbuf[j][12]=p->fFirstDaughter;
    rbuf[j][13]=p->fMother;
    rbuf[j][16]=p->fHelicity;
    rbuf[j][17]=p->fColorID;
    rbuf[j][18]=p->fShowerInfo;
  }

  //C--
  //C  Scan through Spring:Parton_List and classify them according to
  //C  required showing scheme. 
  //C--
  //C     NSLVL               = # levels of showering
  //C     NPTLV(ISLV)         = # partons in the level ISLV(=1,..,NSLVL)
  //C     IPTLV(IQ,ISLV)      = IP pointer to RBUF of IQ-th parton in
  //C			    the ISLV-th level (IQ=1,...,NPTLV(ISLV))
  //C     NSGRP(ISLV)         = # showering pairs in the ISLV-th level
  //C     ISGRP(1,IPSHW,ISLV) = IQ pointer of the 1st parton 
  //C          (2,IPSHW,ISLV) = IQ pointer of the 2nd parton 
  //C                           in IPSHW-th pair (IPSHW=1,..,NSGRP(ISLV))
  //C     NDKPTN(ISLV)	  = # unstable partons (NDAU>0) in the
  //C			    ISLV-th level.
  //C     IDKPTN(IMTH,ISLV)   = IQ pointer of IMTH-th unstable parton
  //C                           in the ISLV-th level
  //C--
  //C     NNOSHW       	  = # non-showering partons
  //C     INOSHW(IQ)	  = IP pointer of IQ-th non-showering 
  //C			    partons (IQ=1,..,NNOSHW)
  //C  Notice that non-showering partons might be affected by parton
  //C  showering, if they are daughters of some parton in a showering
  //C  pair: their momenta have to be readjusted when the parent
  //C  momentum is modified by the parton showering. 
  //C--
  Int_t nslvl = -999;
  Int_t nnoshw=0 ;
  Int_t nptlv[10], iptlv[10][30], nsgrp[10], idkptn[10][30];
  Int_t isgrp[10][30][2], ndkptn[10];
  for(Int_t i=0;i<10;i++){
    nptlv[i]=0;
    nsgrp[i]=0;
    ndkptn[i]=0;
    for(Int_t k=0;k<30;k++){
      iptlv[i][k]=0;
      isgrp[i][k][0]=0;
      isgrp[i][k][1]=0;
    }
  }

  Int_t inoshw[30];

  //C--
  for(Int_t ip=1;ip<=npart;ip++){
    Int_t ndau  = (Int_t)rbuf[ip-1][11];
    Int_t ifshw = (Int_t)rbuf[ip-1][18];
    Int_t islv  = ifshw/100;
    Int_t ipshw = ifshw%100;
    if( fDebug > 0 ) {
      printf(" Looking for shower pair\n");
      printf("  ip=%d  ndau=%d  ifshw=%d islv=%d ipshw=%d\n",
	     ip, ndau, ifshw, islv, ipshw);
    }
    nslvl = TMath::Max(nslvl, islv);
    if( islv <= 0 ) {
      if ( ndau <= 0 ) {
	nnoshw = nnoshw + 1 ;
	inoshw[nnoshw-1] = ip;
      }
    }
    else {
      nptlv[islv-1]++;
      iptlv[islv-1][nptlv[islv-1]-1] = ip;
      if( ipshw > 0 ) {
	if( nsgrp[islv-1] > 0 && isgrp[islv-1][ipshw-1][0] > 0 ) {
	  isgrp[islv-1][ipshw-1][1] = nptlv[islv-1];
	}
	else {
	  nsgrp[islv-1]++;
	  isgrp[islv-1][ipshw-1][0]=nptlv[islv-1];
	}
	if( ndau > 0 ) {
	  ndkptn[islv-1] ++;
	  idkptn[islv-1][ndkptn[islv-1]-1]=nptlv[islv-1];
	}
      }
    }
  }

  if( fDebug > 0 ) {
    printf(" nslvl=%d \n",nslvl);
    for(Int_t klvl=0;klvl<nslvl;klvl++){
      printf(" nsgrp[%d]=%d",klvl,nsgrp[klvl]);
      printf(" isgrp[%d][0][0:1]=%d,%d",klvl,isgrp[klvl][0][0],isgrp[klvl][0][1]);
    }
    printf("\n");
  }


  //C--
  //C  Now start creating Generator:Particle_List bank.
  //C--
  //C  Data format of Generator:Particle_List bank.
  //C    Generator:Particle_List
  //C      Elm# n  RBUF( 1) = particle serial #
  //C                  ( 2) = Particle ID (PDG).
  //C                  ( 3) = mass (GeV)
  //C                  ( 4) = charge
  //C                  ( 5) = Px (GeV)
  //C                  ( 6) = Py (GeV)
  //C                  ( 7) = Pz (GeV)
  //C                  ( 8) = E  (GeV)
  //C                  ( 9) = X  (cm) of vertex point.
  //C                  (10) = Y  (cm)
  //C                  (11) = Z  (cm)
  //C                  (12) = # of daughter particle (=0 in this hadronizer)
  //C                  (13) = Pointer to the daughter (=0)
  //C                  (14) = Pointer to the mother (-n n=element# of
  //C                                               SPRING:Parton_List )
  //C                  (15) = trigger timing.
  //C                  (16) = particle life time (c*nsec)
  //C                  (18) = not used
  //C                  (19) = not used
  //C                  (20) = not used.
  //C--
  //C  Loop over levels of parton showers.
  //C--

  Double_t rinlst[30][10];
  Double_t rotlst[4000][20];
  Int_t inelm=0;
  Int_t ididit[30];
  Int_t ishpr1[30], ishpr2[30];
  Int_t kstat[30], jstat[30], ishufl[30];
  Int_t nout = 0;

  for(Int_t islv=1;islv<=nslvl;islv++){
    if( nptlv[islv-1] <= 0 ) { continue; }

    Int_t nin=0;
    for(Int_t k=0;k<30;k++){ ididit[k]=0; }

    Int_t nzdk = 0;
    Int_t nwdk = 0;
    Int_t nhdk = 0;

    //C--
    //C  Loop over partons in this level and store their information
    //C  in RINLST and invoke LUFRAG to carry out parton showering 
    //C  and fragmentation and decay.
    //C--
    //C     NIN           = # partons in RINLST
    //C     KSTAT(IIN)    = color flow flag
    //C     NSG           = NSGRP(ISLV) = # showering pairs in the
    //C		      current showering level
    //C     ISHPR(1,IS)   = IIN pointer of the 1st parton
    //C          (2,IS)   = IIN pointer of the 2nd parton
    //C		      in IS-th showering pair (IS=1,..,NSG)
    //C     RINLST(1,IIN) = ID
    //C           (2,IIN) = Px
    //C	    (3,IIN) = Py
    //C	    (4,IIN) = Pz
    //C	    (5,IIN) = E
    //C	    (6,IIN) = mass
    //C	    (7,IIN) = helicity
    //C--
    //C  Partons in RINLST must be orderd to make a planar tree in
    //C  terms of color flow. (e.g.) When this level consists of b, 
    //C  W+, bbar, and W- from a t-tbar pair, they must be ordered,
    //C  for instance, as b, bbar, W+, and W- and
    //C     KSTAT(1-4) = 1 0 0 0
    //C     JSTAT(1-4) = 0 0 2 2
    //C--
    //C     ISHUFL(IQ)    = Pointer to RINLST of IQ-th parton in the
    //C                     current showering level (IIN=1,..,NIN)
    //C--
    for(Int_t iq=1;iq<=nptlv[islv-1];iq++){

      if( ididit[iq-1] > 0 ) { continue; }

      Int_t ip   = iptlv[islv-1][iq-1];
      Int_t id   = (Int_t)rbuf[ip-1][1];
      Int_t ida  = TMath::Abs(id);
      Int_t ndau = (Int_t)rbuf[ip-1][11];

      if (ndau && ida==23) nzdk++;
      if (ndau && ida==24) nwdk++;
      if (ndau && ida==25) nhdk++;

      nin++;
      Int_t icf  = (Int_t)rbuf[ip-1][17];
      for(Int_t k=0;k<10;k++){ rinlst[nin-1][k]=0.0; }
      rinlst[nin-1][0] = rbuf[ip-1][1];
      rinlst[nin-1][1] = rbuf[ip-1][4];
      rinlst[nin-1][2] = rbuf[ip-1][5];
      rinlst[nin-1][3] = rbuf[ip-1][6];
      rinlst[nin-1][4] = rbuf[ip-1][7];
      rinlst[nin-1][5] = rbuf[ip-1][2];
      rinlst[nin-1][6] = rbuf[ip-1][16];
      ishufl[iq-1]     = nin;
      ididit[iq-1]     = 1;
      kstat[nin-1]     = 0;
      jstat[nin-1]     = ndau;
      if( ida > 9 && ida != 21 ) { continue; }
      kstat[nin-1]     = 1;
      //C--
      //C  Look for color singlet partner.
      //C--
      for(Int_t jq=iq+1;jq<=nptlv[islv-1];jq++){
	if( ididit[jq-1] > 0 ) { continue; }
	Int_t jp  = iptlv[islv-1][jq-1];
	Int_t jd  = (Int_t)rbuf[jp-1][1];
	Int_t jda = TMath::Abs(jd);
	Int_t ndau= (Int_t)rbuf[jp-1][11];
	if( jda > 9  && jda != 21 ) { continue; }
	Int_t jcf = (Int_t)rbuf[jp-1][17];
	if( jcf != icf ) { continue; }
	nin++;
	for(Int_t k=0;k<10;k++){ rinlst[nin-1][k]=0; }
	rinlst[nin-1][0] = rbuf[jp-1][1];
	rinlst[nin-1][1] = rbuf[jp-1][4];
	rinlst[nin-1][2] = rbuf[jp-1][5];
	rinlst[nin-1][3] = rbuf[jp-1][6];
	rinlst[nin-1][4] = rbuf[jp-1][7];
	rinlst[nin-1][5] = rbuf[jp-1][2];
	rinlst[nin-1][6] = rbuf[jp-1][16];
	ishufl[jq-1]     = nin ;
	ididit[jq-1]     = 1;
	kstat[nin-1]     = 1;
	jstat[nin-1]     = ndau;
      }
      kstat[nin-1] = 0;
    }
    //C--
    //C  Do parton shower and fragmentation.
    //C--
    Int_t nsg=nsgrp[islv-1];
    for(Int_t is=1;is<=nsg;is++){
      ishpr1[is-1]=ishufl[isgrp[islv-1][is-1][0]-1];
      ishpr2[is-1]=ishufl[isgrp[islv-1][is-1][1]-1];
    }
    //    Int_t maxout=400;
    Int_t maxout=4000;
    Int_t iret;
    if( fDebug > 0 ) {
      printf(" Before JSFHadronize::Fragmentaion call \n");
      printf(" nsg=%d  ishpr=",nsg);
      for(Int_t k=0;k<nsg;k++){
	printf(" (%d,%d) ",ishpr1[k],ishpr2[k]);
      }
      printf("\n");
    }

#ifdef __USE_TPYTHIA__
    if (nzdk) fPythia->SetMDCY(23,1,0);
    if (nwdk) fPythia->SetMDCY(24,1,0);
    if (nhdk) fPythia->SetMDCY(33,1,0);
#else
    if (nzdk) pydat3_.MDCY[0][22] = 0;
    if (nwdk) pydat3_.MDCY[0][23] = 0;
    if (nhdk) pydat3_.MDCY[0][24] = 0;
#endif

    Fragmentation(nin, rinlst, maxout, nsg, ishpr1, ishpr2,
		  kstat, jstat, nout, rotlst, iret);

#ifdef __USE_TPYTHIA__
    if (nzdk) fPythia->SetMDCY(23,1,mdcyzs);
    if (nwdk) fPythia->SetMDCY(24,1,mdcyws);
    if (nhdk) fPythia->SetMDCY(33,1,mdcyhs);
#else
    if (nzdk) pydat3_.MDCY[0][22] = mdcyzs;
    if (nwdk) pydat3_.MDCY[0][23] = mdcyws;
    if (nhdk) pydat3_.MDCY[0][24] = mdcyhs;
#endif

    if( iret < 0 ) {
      nret = -1;
      return;
    }
    if( fDebug > 0 ) {
      printf(" After Fragmentation .. nout=%d\n",nout);
    }

    //C--
    //C  Put particles in this level to bank.
    //C--
    for(Int_t i=1;i<=nout;i++){
      rotlst[i-1][0] = i + inelm;
      if (rotlst[i-1][11] > 0.0) rotlst[i-1][12] = (Int_t)(rotlst[i-1][12]+0.1) + inelm;
      if( rotlst[i-1][13] > 0.0 ) {
	rotlst[i-1][13] = (Int_t)(rotlst[i-1][13]+0.1) + inelm;
      }
      else {
        rotlst[i-1][13] = -iptlv[islv-1][ishufl[i-1]-1];
      }
      Int_t ie=i+inelm;
      new(part[ie-1]) JSFGeneratorParticle(&rotlst[i-1][0]);
      npgen++;
    }
    inelm += nout;
    //C--
    //C  Modify daughter momenta if parent momentum got modified.
    //C--
    for(Int_t imth=1;imth<=ndkptn[islv-1];imth++){
      Int_t iq  = idkptn[islv-1][imth-1];
      Int_t ip  = iptlv[islv-1][iq-1];
      Int_t in  = ishufl[iq-1];
      Int_t ndau= (Int_t)rbuf[ip-1][11];
      Int_t idau= (Int_t)rbuf[ip-1][12]-1;
#if 0
      TLorentzVector pref(rbuf[ip-1][4], rbuf[ip-1][5], 
        	          rbuf[ip-1][6], rbuf[ip-1][7]); 
      TVector3 b=-pref.BoostVector();

      TLorentzVector ppar(rinlst[in-1][1], rinlst[in-1][2], 
			  rinlst[in-1][3], rinlst[in-1][4]);
      TVector3 bpar=ppar.BoostVector();

      for(Int_t jdau=1;jdau<=ndau;jdau++){
	TLorentzVector pdau(rbuf[idau][4], rbuf[idau][5], 
			    rbuf[idau][6], rbuf[idau][7]); 
	pdau.Boost(b);
	pdau.Boost(bpar);
	rbuf[4][idau]=pdau.Px();
	rbuf[5][idau]=pdau.Py();
	rbuf[6][idau]=pdau.Pz();
	rbuf[7][idau]=pdau.E();
	idau = idau+1;
      }
#else
      Float_t pin[4];
      for (Int_t k=0; k<4; k++) pin[k]  = rbuf[ip-1][k+4];
      Float_t pot[4];
      for (Int_t k=0; k<4; k++) pot[k]  = rinlst[in-1][k+1];
      for(Int_t jdau=1;jdau<=ndau;jdau++){
        idau++;
        Float_t pdau[4];
        for (Int_t k=0; k<4; k++) pdau[k] = rbuf[idau-1][k+4];
        ubstfd_(pdau, pin, pdau);
        ubstbk_(pdau, pot, pdau);
        for (Int_t k=0; k<4; k++) rbuf[idau-1][k+4] = pdau[k];
      }
#endif
    }
  }

  //C--
  //C  Then output non-showering particles.
  //C  Notice that IDA = 220000 is the LSP(chi^0_1) which is
  //C  assumed to be stable.
  //C--
  Double_t rtmp[20];
  for(Int_t jp=1;jp<=nnoshw;jp++){
    Int_t ip = inoshw[jp-1];
    Int_t id = (Int_t)rbuf[ip-1][1];
    Int_t ida = TMath::Abs(id);
    if( ida > 25 && ida != 220000 && ida != 1000022 ) {
      printf("Warning in JSFHadronizer::Hadronize");
      printf(" Particle ID=%d is not recognized.\n",id);
      continue;
    }
    //C--
    //C  Neutrinos, e, mu, gamma, and LSP.
    //C--
    if( ida==12 || ida==14 || ida==16 ||
	ida==11 || ida==13 || ida==22 ||
	ida==220000 || ida==1000022 ) {
      inelm++;
      for(Int_t k=0;k<20;k++){ rtmp[k]=0; }
      rtmp[0]=inelm;
      rtmp[1]=id;
      rtmp[2]=rbuf[ip-1][2];
      rtmp[3]=-id%2;
      rtmp[4]=rbuf[ip-1][4];
      rtmp[5]=rbuf[ip-1][5];
      rtmp[6]=rbuf[ip-1][6];
      rtmp[7]=rbuf[ip-1][7];
      rtmp[13]=-ip;
      new(part[inelm-1]) JSFGeneratorParticle(rtmp);
      npgen++;
    }
    //C--
    //C  Tau (15), Z(23), W(24), and H(25).
    //C--
    else if( ida==15 || ida==23 || ida==24 || ida==25 ) {
      for(Int_t k=0;k<10;k++){ rinlst[k][0]=0; }
      rinlst[0][0] = rbuf[ip-1][1];
      rinlst[0][1] = rbuf[ip-1][4];
      rinlst[0][2] = rbuf[ip-1][5];
      rinlst[0][3] = rbuf[ip-1][6];
      rinlst[0][4] = rbuf[ip-1][7];
      rinlst[0][5] = rbuf[ip-1][2];
      rinlst[0][6] = rbuf[ip-1][16];
      kstat[0]     = 0 ;
      jstat[0]     = 0 ;
      Int_t nin = 1;
      //      Int_t n400=400;
      Int_t n400=4000;
      Int_t zero=0;
      Int_t iret;

      // When 4th argument is zero, parton shower is not performed.
      // 5th and 6th argumnet is dummy 

      Fragmentation(nin, rinlst, n400, zero,  ishpr1, ishpr2,
      		  kstat, jstat, nout, rotlst, iret);

      if( iret < 0 ) {
	nret = -1;
	return;
      }
      for(Int_t  i=1;i<=nout;i++){
	rotlst[i-1][0] = i+inelm ;
	if (rotlst[i-1][11] > 0.0) {
	  rotlst[i-1][12] = (Int_t)(rotlst[i-1][12]+0.1) + inelm;
	}
	if( rotlst[i-1][13] > 0.0 ) {
	  rotlst[i-1][13] = (Int_t)(rotlst[i-1][13]+0.1) + inelm;
	}
	else {
	  rotlst[i-1][13] = -ip;
	}
	Int_t ie=i+inelm;
	new(part[ie-1]) JSFGeneratorParticle(&rotlst[i-1][0]);
	npgen++;
      }
      inelm = inelm + nout;
    }
  }
  gbuf->SetNparticles(npgen);

  if( fDebug > 0 ) {
    printf(" JSFHadronizer::Hadronize() .. Number of generator particles");
    printf(" is %d\n",npgen);
  }
}

//______________________________________________________________
void JSFHadronizer::Fragmentation(const Int_t nin, Double_t inlist[][10],
				  const Int_t mxxout, const Int_t nspar,
				  const Int_t ispar1[], const Int_t ispar2[], 
				  const Int_t kstat[],  const Int_t jstat[],
				  Int_t &nout, Double_t outlst[][20],
				  Int_t &nret)
{
/*
CC**********************************************************************
C*
C*==============================================================
C* Subroutine LUFRAG(NIN,INLIST,MXxOUT,NSPAR,ISPAR,KSTAT,JSTAT,
C*                   NOUT,OUTLST, NRET )
C*===================------------------=========================
C*
C* (Function)
C*    Parton-shower, decay, and hadronize partons with 
C*    LUND-PS and string fragmentation.
C* (Inputs)
C*    NIN          :(I*4): # of input partons.
C*    INLIST( 1,i) :(R*4): Particle ID (PDG format).
C*          ( 2,i) :(R*4): Px (GeV).
C*          ( 3,i) :(R*4): Py (GeV).
C*          ( 4,i) :(R*4): Pz (GeV).
C*          ( 5,i) :(R*4): E  (GeV).
C*          ( 6,i) :(R*4): m  (GeV).
C*          ( 7,i) :(R*4): helicity.
C*    MXxOUT       :(I*4): Size of OUTLST.
C*    NSPAR        :(I*4): # showering pairs.
C*    ISPAR ( 1,j) :(I*4): pointer to 1-st parton in j-th pair.
C*          ( 2,j) :(I*4): pointer to 2-nd parton in j-th pair.
C*    KSTAT (   i) :(I*4): color singlet group flag.
C*    JSTAT (   i) :(I*4): # daughters.
C* (Outputs)
C*    NOUT         :(I*4): # output particles.
C*    OUTLST( 1,i) :(R*4): 0.
C*          ( 2,i) :(R*4): Particle ID ( PDG format)
C*          ( 3,i) :(R*4): mass (GeV)
C*          ( 4,i) :(R*4): charge
C*          ( 5,i) :(R*4): Px
C*          ( 6,i) :(R*4): Py
C*          ( 7,i) :(R*4): Pz
C*          ( 8,i) :(R*4): E
C*          ( 9,i) :(R*4): 0.
C*          (10,i) :(R*4): 0.
C*          (11,i) :(R*4): 0.
C*          (12,i) :(R*4): # of daughter
C*          (13,i) :(R*4): Pointer to the daughter (relative)
C*          (14,i) :(R*4): Pointer to the mother (relative)
C*          (15,i) :(R*4): 0.
C*          (16,i) :(R*4): life time  (c*t (cm))
C*          (17,i) :(R*4): 0.
C*          (18,i) :(R*4): 0.
C*          (19,i) :(R*4): 0.
C*          (20,i) :(R*4): 0.
C*    NRET         :(I*4): return flag.
C* (Update Records)
C*    95/02/10  K.Fujii		New version for JETSET7.3.
C*
CC**********************************************************************
*/
#ifdef __USE_TPYTHIA__
  Pyjets_t *pyjets=fPythia->GetPyjets();
#else
  Pyjets_t *pyjets = &pyjets_;
#endif

  //C--
  //C  Pointer from /LUJETS/ to OUTLST.
  //C     IDHIST ( i ) : Address in OUTLST for particles in /LUJETS/
  //C--

  Int_t njoin, ijoin[4000];
  nret = 0 ;

  //C
  //C ======< Entry Point >=============================================
  //C
  //C--
  //C  Reset #particles in /LUJETS/.
  //C--
  pyjets->N=0;
  //C--
  //C  Check NSPAR to decide whether parton shower is required or not.
  //C--
  Int_t kf=0;
  Int_t kfa=0;
  Int_t two=2;
  Int_t one=1;
  if( nspar <= 0 ) {
    kf=(Int_t)inlist[0][0];
    kfa=TMath::Abs(kf);
    if( nin==1 && kfa==15 ) {

  //C--
  //C  Decay taus using TAUOLA.
  //C  Notice that TAUOLA initializatin is carried out in FLNPAR
  //C  and TAUOLA summary is printed out in FLUERU.
  //C  TAUOLA parameters should be read in by FLNPAR from FT05.
  //C--
     
      Int_t kto;
      if( kf <= 0 ) {
	kto=1;
      }
      else {
	kto=2;
      }
      Float_t pol=inlist[0][6]*fIHLON;
      Float_t tinlst[10];
      for(Int_t i=0;i<10;i++){ tinlst[i]=(Float_t)inlist[0][i]; }

      taudec_(&kto,&one, &pol, &tinlst[1]);
#if __PYTHIA_VERSION__ >= 6
      pyhepc2_(&two, 
               &pyjets_.N, &pyjets_.NPAD, pyjets_.K, pyjets_.P, pyjets_.V,
                pydat1_.MSTU, pydat1_.PARU, pydat1_.MSTJ, pydat1_.PARJ,
                pydat2_.KCHG, pydat2_.PMAS, pydat2_.PARF, pydat2_.VCKM);
#else
      luhepc2_(&two, 
               &pyjets_.N, pyjets_.K, pyjets_.P, pyjets_.V,
                pydat1_.MSTU, pydat1_.PARU, pydat1_.MSTJ, pydat1_.PARJ,
                pydat2_.KCHG, pydat2_.PMAS, pydat2_.PARF, pydat2_.VCKM);
#endif
      if( fDebug > 0 ) {
	printf(" Tau decay .. \n");
	pylist_(&one);
      }
    }
    //C--
    //C  If input particles do not require parton shower generations,
    //C  fragment them according to the standard procedure.
    //C--
    else {
      for(Int_t i=0;i<nin;i++){
	pyjets->N++;
	kf=(Int_t)inlist[i][0];
	kfa=TMath::Abs(kf);
	if( kfa == 220000) { kf=TMath::Sign(33,kf); }

	Int_t ip=pyjets->N - 1;
	pyjets->K[0][ip]=kstat[i]+1;
	pyjets->K[1][ip]=kf;
	pyjets->K[2][ip]=0;
	pyjets->K[3][ip]=0;
	pyjets->K[4][ip]=0;
	pyjets->P[0][ip]=inlist[i][1];
	pyjets->P[1][ip]=inlist[i][2];
	pyjets->P[2][ip]=inlist[i][3];
	pyjets->P[3][ip]=inlist[i][4];
	pyjets->P[4][ip]=inlist[i][5];
	pyjets->V[0][ip]=0;
	pyjets->V[1][ip]=0;
	pyjets->V[2][ip]=0;
	pyjets->V[3][ip]=0;
	pyjets->V[4][ip]=0;
      }
    }
  }
  else {
  //C--
  //C  If input particles require parton shower generations,
  //C  Do PARTON SHOWER first.
  //C--
    njoin=0;
    for(Int_t i=1;i<=nin;i++){
      pyjets->N++;
      kf=(Int_t)inlist[i-1][0];
      kfa=TMath::Abs(kf);
      if( kfa == 220000) { kf=TMath::Sign(33,kf); }

      if( kstat[i-1] != 0 ) {
	njoin++;
	ijoin[njoin-1]=i;
      }
      else if ( njoin > 0 ) {
	if( kstat[i-2] != 0 ) {
	  njoin++;
	  ijoin[njoin-1] = i;
	}
      }    

      Int_t ip=pyjets->N - 1;
      pyjets->K[0][ip]=kstat[i-1]+1;
      pyjets->K[1][ip]=kf;
      pyjets->K[2][ip]=0;
      pyjets->K[3][ip]=0;
      pyjets->K[4][ip]=0;
      pyjets->P[0][ip]=inlist[i-1][1];
      pyjets->P[1][ip]=inlist[i-1][2];
      pyjets->P[2][ip]=inlist[i-1][3];
      pyjets->P[3][ip]=inlist[i-1][4];
      pyjets->P[4][ip]=inlist[i-1][5];
      pyjets->V[0][ip]=0;
      pyjets->V[1][ip]=0;
      pyjets->V[2][ip]=0;
      pyjets->V[3][ip]=0;
      pyjets->V[4][ip]=0;
    }
    //C--
    //C  Do LUJOIN.
    //C--
    pyjoin_(&njoin, ijoin);

    if( fDebug > 0 ) {
      printf(" After join  njoin=%d  ijoin=",njoin);
      for(Int_t k=0;k>njoin;k++){
	printf(" %d ",ijoin[k]);
      }
      printf("\n");
    }
    //C--
    //C  Do LUSHOW for each showering pair.
    //C--
    if( fDoesShower) {
      for(Int_t is=0;is<nspar;is++){
	Int_t ip1=ispar1[is];
	Int_t ip2=ispar2[is];
	Double_t qmx=TMath::Power(pyjets->P[3][ip1-1]+pyjets->P[3][ip2-1],2) 
	  - TMath::Power(pyjets->P[0][ip1-1]+pyjets->P[0][ip2-1],2) 
	  - TMath::Power(pyjets->P[1][ip1-1]+pyjets->P[1][ip2-1],2) 
	  - TMath::Power(pyjets->P[2][ip1-1]+pyjets->P[2][ip2-1],2);
	qmx = TMath::Sqrt(qmx)-pyjets->P[4][ip1-1]-pyjets->P[4][ip2-1];
	qmx = TMath::Max(qmx, 1.);

	pyshow_(&ip1, &ip2, &qmx);

#ifdef __USE_TPYTHIA__
	if( fPythia->GetMSTU(23) != 0 || fPythia->GetMSTU(24) != 0 
	    || fPythia->GetMSTU(28) != 0 ) {
	  printf(" Warning .. JSFHadronizer::Fragmentation\n");
	  printf(" Possible error in PYSHOW  detected.");
	  printf(" MSTU(23)=%d MSTU(24)=%d MSTU(28)=%d\n",
		 fPythia->GetMSTU(23), fPythia->GetMSTU(24), fPythia->GetMSTU(28)),
	    printf(" ip1=%d,ip2=%dqmx=%g",ip1,ip2,qmx);
	    printf(" This event will be skipped.\n");
	  fPythia->SetMSTU(23,0);
	  fPythia->SetMSTU(24,0);
	  fPythia->SetMSTU(28,0);
	  nret=-1;
	  return; 
	}
#endif
      }
    }
    //C--
    //C  Store modified momenta for unstable particles.
    //C  When one such particle has a daughter of the same flavor,
    //C  take it as the one after gluon emission and test if further
    //C  gluon emission occured or not. If not, consider the last one
    //C  as the parent parton after gluon radiations.
    //C-- 
    //C    MDF = the line# of the 1-st daughter
    //C    MDL = the line# of the last daughter
    //C--
    for(Int_t i=1;i<=nin;i++){
      if( jstat[i-1] > 0 ) {
	kf=pyjets->K[1][i-1];
	Int_t mp=i;
      label35:
	Int_t mdf=pyjets->K[3][mp-1];
	Int_t mdl=pyjets->K[4][mp-1];
	if( mdf*mdl != 0 ) {
	  for(Int_t m=mdf;m<=mdl;m++){
	    if( pyjets->K[1][m-1] == kf ) {
	      mp=m;
	      if( pyjets->K[0][m-1] > 3 ) { goto label35; }
	    }
	  }
	}
        inlist[i-1][1]=pyjets->P[0][mp-1];
        inlist[i-1][2]=pyjets->P[1][mp-1];
        inlist[i-1][3]=pyjets->P[2][mp-1];
        inlist[i-1][4]=pyjets->P[3][mp-1];
      }
    } // end of loop   for(Int_t i=1;i<=nin;i++){

    Int_t one=1;

    pyprep_(&one);
  }
  //C--
  //C  Do LUEXEC for fragmentation.
  //C-
  pyexec_();

  if( fDebug > 0 ) {
    printf(" After pyexec...\n");
    pylist_(&one);
  }
#ifdef __USE_TPYTHIA__
  if( fPythia->GetMSTU(23) != 0 || fPythia->GetMSTU(24) != 0 
      || fPythia->GetMSTU(28) != 0 ) {
    printf(" Warning .. JSFHadronizer::Fragmentation\n");
    printf(" Possible error in PYEXEC detected.");
    printf(" MSTU(23)=%d MSTU(24)=%d MSTU(28)=%d",
	   fPythia->GetMSTU(23), fPythia->GetMSTU(24), fPythia->GetMSTU(28)),
    printf(" This event will be skipped.\n");
    fPythia->SetMSTU(23,0);
    fPythia->SetMSTU(24,0);
    fPythia->SetMSTU(28,0);
    nret=-1;
    return; 
  }
#else
  if( pydat1_.MSTU[22] != 0 || pydat1_.MSTU[23] != 0 
      || pydat1_.MSTU[27] != 0 ) {
    printf(" Warning .. JSFHadronizer::Fragmentation\n");
    printf(" Possible error in LUEXEC detected.");
    printf(" MSTU(23)=%d MSTU(24)=%d MSTU(28)=%d",
	   pydat1_.MSTU[22], pydat1_.MSTU[23], pydat1_.MSTU[27]),
    printf(" This event will be skipped.\n");
    pydat1_.MSTU[22] = 0;
    pydat1_.MSTU[23] = 0;
    pydat1_.MSTU[27] = 0;
    //nret=-1;
    return; 
  }
#endif
  //C--
  //C  Output particles after fragmentations and decays.
  //C--
  nout=0;
  if( fDebug > 0 ) {
    printf("  After fragmentation...pyjets->N=%d\n",pyjets->N);
  }
  for(Int_t i=1;i<=pyjets->N;i++){
    Int_t ks = pyjets->K[0][i-1]; // status code
          kf = pyjets->K[1][i-1]; // flavor code
    kfa=TMath::Abs(kf);
    //C--
    //C  Now store this particle in OUTLST.
    //C--
    nout++;
    if( nout > mxxout ) {
      printf(" Warning in JSFHadronizer::Fragment");
      printf("Output buffer overflowed. Trancated");
      printf(" nout=%d\n",nout);
      //Int_t two=2;
      pylist_(&two);
      nout=mxxout;
      goto label900;
    }
    for(Int_t k=0;k<20;k++){ outlst[nout-1][k]=0; }
    //C--
    Double_t chrg=0.0;
    Double_t xctau=0.0;
    if( kfa == 33 ) {
      chrg=0.0;
      kf=TMath::Sign(220000,kf);
      xctau=0.0;
    }
    else {
      chrg = ((Double_t)pychge_(&kf))/3.0;
#ifdef __USE_TPYTHIA__
      Int_t kc=fPythia->Pycomp(kf);
      xctau=fPythia->GetPMAS(kc,4)*0.1;
#else
      Int_t kc = pycomp_(&kf);
      xctau    = pydat2_.PMAS[3][kc-1]*0.1;
#endif
#if 0
std::cerr << "hadr: &pychge = " << (void *)&pychge_ << std::endl;
std::cerr << "    : ichg = " << pychge_(&kf) << std::endl;
std::cerr << "root: ichg = " <<fPythia->Pychge(kf) << std::endl;
std::cerr << "hadr: &pycomp = " << (void *)&pycomp_ << std::endl;
std::cerr << "root: kf = " << kf << std::endl;
std::cerr << "root: kc = " << kc << std::endl;
std::cerr << "    : kc = " << pycomp_(&kf) << std::endl;
std::cerr << "root: xctau = " << xctau << std::endl;
std::cerr << "    : xctau = " << pydat2_.PMAS[3][kc-1]*0.1 << std::endl;
std::cerr << "root: xctau = " << fPythia->GetPMAS(pycomp_(&kf),4)*0.1 << std::endl;
std::cerr << "    : xctau = " << pydat2_.PMAS[3][pycomp_(&kf)-1]*0.1 << std::endl;
#endif
    }
    //C--         
    outlst[nout-1][1]=kf;
    outlst[nout-1][2]=pyjets->P[4][i-1];
    outlst[nout-1][3]=chrg;
    outlst[nout-1][4]=pyjets->P[0][i-1];
    outlst[nout-1][5]=pyjets->P[1][i-1];
    outlst[nout-1][6]=pyjets->P[2][i-1];
    outlst[nout-1][7]=pyjets->P[3][i-1];
    outlst[nout-1][11]=0.0;
    outlst[nout-1][12]=0.0;
    //C--
    Int_t firstchild = pyjets->K[3][i-1];
    Int_t lastchild  = pyjets->K[4][i-1];
    Int_t ndau       = lastchild - firstchild + 1;
    //C--
    //C  Make sure that unstable partons are flagged as unstable
    //C--
    if ((kfa >= 1 && kfa <= 10) || kfa==21 || // quarks & gluons
        (kfa >= 1100 && kfa <= 5509 && kfa%100 < 10) || // diquarks
         ks == 14) { // partiocles that radiate
      outlst[nout-1][11] = ndau > 0 ? ndau : 999999;
      outlst[nout-1][12] = ndau > 0 ? firstchild : 999999;
    } else if (firstchild || lastchild) {
      outlst[nout-1][11] = ndau > 0 ? ndau : 999999;
      outlst[nout-1][12] = ndau > 0 ? firstchild : 999999;
    } else if (kfa == 23 || kfa == 24) { // undecayed Z/W
      outlst[nout-1][11] = ndau > 0 ? ndau : 999999;
      outlst[nout-1][12] = ndau > 0 ? firstchild : 999999;
    }

    outlst[nout-1][13] = pyjets->K[2][i-1];
    outlst[nout-1][15] = xctau;
  }
  //C--
  //C  That's it.
  //C--
 label900: 
  if( fDebug > 0 ) {
    printf(" End of JSFHadronizer::Fragmentation  nout=%d\n",nout);
  }
  return ;
}
