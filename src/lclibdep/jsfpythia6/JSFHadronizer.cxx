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

#include <iomanip>
#include "TRandom.h"
#include "JSFSteer.h"
#include "JSFLCFULL.h"  
#include "JSFHadronizer.h"

#ifdef KF_DEBUG
#include "TLorentzVector.h"
#endif

extern "C" {
#if __GNUC_VERSION__ >= 4
extern void  pydatawrapper_();
#endif
#if __TAUOLA_MINOR_VERSION__ < 7
extern void  tauint_(int *inut, int *iout, int *jak1, 
		     int *jak2, int *itdkrc, int *keya1, float *xk0dec);
extern void  taudec_(int *kto, int *npnt, float *heltau, float p4tau[4]);
#else
extern void  tauola_(int *mode, int *keypol);
#endif
extern void  pyjoin_(int *njoin, int ijoin[]);
extern void  pyshow_(int *ip1, int *ip2, double *qmax);
extern void  pyprep_(int *ip);
extern int   pychge_(int *kf);
extern void  pylist_(int *flag);
extern int   pycomp_(int *kf);
extern void  pyexec_();
extern void  pyhepc_(int *mconv);
extern void  py6frm_(double *p12, double *p13, double *p21, double *p23, double *p31, double *p32,
                     double *ptop, int *irad, int *irau, int *icom);
extern void  py2frm_(int *irad, int *itau, int *icom);
extern void  py4frm_(double *atotsq, double *a1sq, double *a2sq, int *istrat, 
	  	    int *irad, int *itau, int *icom);
extern void  pygive_(const char* cmd, int len);
extern void  fmessage_(const char *msg, int len);
extern void  dexay_(int *kto, float pol[4]);
extern double pyr_(int *dummy);

extern void  pyhepc2_(int *flag, int *n, int *npad, int k[5][4000],
                                double p[5][4000], double v[5][4000],
                                int mstu[200], double paru[200],
                                int mstj[200], double parj[200],
                                int kchg[4][500], double pmas[4][500],
                                double parf[2000], double vckm[4][4]);
extern void  ubstfd_(float pb[4], float pr[4], float pa[4]);
extern void  ubstbk_(float pb[4], float pr[4], float pa[4]);
};

#if __TAUOLA_MINOR_VERSION__ < 7 
Int_t JAK1, JAK2, ITDKRC, KEYA1;
Float_t XK0DEC;
#endif

ClassImp(JSFHadronizer)

typedef struct { 
int nevhep;             /* The event number */
int nhep;               /* The number of entries in this event */
int isthep[4000];     /* The Particle id */
int idhep[4000];      /* The particle id */
int jmohep[4000][2];    /* The position of the mother particle */
int jdahep[4000][2];    /* Position of the first daughter... */
double phep[4000][5];    /* 4-Momentum, mass */
double vhep[4000][4];    /* Vertex information */
} Hepevt_t ;

typedef struct {
  int np1;
  int np2;
} Taupos_t;

typedef struct {
  float p4tau[4];
} P4tau_t;
extern P4tau_t  p4tau_;

extern Taupos_t taupos_;
extern Hepevt_t hepevt_;
extern Pydat1_t pydat1_;
extern Pydat2_t pydat2_;
extern Pydat3_t pydat3_;
extern Pydatr_t pydatr_;
extern Pyjets_t pyjets_;

extern "C" void pytaud_(int *itau, int *iorig, int *kforig, int *ndecay)
{
  JSFHadronizer::Instance()->Pytaud(itau, iorig, kforig, ndecay);
};

JSFHadronizer *JSFHadronizer::fInstance=0;

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
  fInstance=this;

  int helsize=sizeof(pyjets_.K)/20;
  fHelicity=new Float_t[helsize];
  fColor   = new Int_t[helsize];
  fAntiColor = new Int_t[helsize]; 

  TList *list=gJSF->Modules();
  TIter  next(list);
  JSFModule *module;
  while( (module=(JSFModule*)next()) ){
    if( module->InheritsFrom("JSFSpring") ){ 
      if( fSpring ) { 
	 std::cout << "Find module " << fSpring->GetName() << " inherited from JSFSpring" << std::endl;
	 std::cout << "More than one JSFSpring are defined." << std::endl;
	 std::cout << " Specify correspinding Hadronizer explicityly" << std::endl;
      }
      fSpring=(JSFSpring*)module;
//      std::cout << fSpring->GetName() << " is used by JSFHadronizer " << std::endl;
    }
  }
  if( !fSpring ){ Error("JSFHadronizer","No JSFSpring class was found"); }

#if __TAUOLA_MINOR_VERSION__ < 7   
  JAK1 = gJSF->Env()->GetValue("JSFHadronizer.JAK1",0);
  JAK2 = gJSF->Env()->GetValue("JSFHadronizer.JAK2",0);
  ITDKRC = gJSF->Env()->GetValue("JSFHadronizer.ITDKRC",1);
  KEYA1 = gJSF->Env()->GetValue("JSFHadronizer.KEYA1",1);
  sscanf(gJSF->Env()->GetValue("JSFHadronizer.XK0DEC","0.001"),"%g",&XK0DEC);
#endif

  fIHLON = gJSF->Env()->GetValue("JSFHadronizer.IHLON",1);
  fDebug = gJSF->Env()->GetValue("JSFHadronizer.DebugFlag",0);

  fDoesShower = gJSF->Env()->GetValue("JSFHadronizer.PartonShower",1); // Obsolute 
  fHadronizerType = gJSF->Env()->GetValue("JSFHadronizer.Type",1); //(1,2)=(old,new)
// KEYPOL=1 to include polarization effect
//  fTauola_Keypol  = gJSF->Env()->GetValue("JSFHadronizer.Tauola_Keypol",1);
  fFSQEDRadiation = gJSF->Env()->GetValue("JSFHadronizer.FSQEDRadiation",1); // (0,1)=(0ff,on)
  fTauola_Keypol  = fIHLON ; 

  fPythia = TPythia6::Instance();
}

//_____________________________________________________________________________
JSFHadronizer::~JSFHadronizer()
{
  if ( fHelicity )  { delete fHelicity; }
  if ( fColor )     { delete fColor; }
  if ( fAntiColor ) { delete fAntiColor; }

}

//_____________________________________________________________________________
Bool_t JSFHadronizer::Initialize()
{
#if __GNUC_VERSION__  >= 4
#if __JSF_HOSTTYPE__ != KEKCC
   pydatawrapper_();
#endif
#endif

// Print message
   int tvers=__TAUOLA_MINOR_VERSION__ ;
   std::cout << "### JSFHadronizer initialization ##########################" << std::endl
	<< "   Spring Name           : " << fSpring->GetName() << std::endl
	<< "   JSFHadronizer.Type    : " << fHadronizerType << "            : (1,2,3)=(old,new,JSFSpring)" << std::endl
        << "   Final state QED rad.  : " << fFSQEDRadiation << "            : (0,1)=(off,on)"  << std::endl
	<< "   Tau polarization      : " << fIHLON          << "            : (0,1)=(off,on)"  << std::endl
        << "   Tauola m.version used : " << tvers << std::endl;
#if __TAUOLA_MINOR_VERSION__ < 7 
   std::cout 
	<< "   Tauola parameter    : JAK1=" << JAK1 << " JAK2=" << JAK2 
	<< " ITDKRC=" << ITDKRC << " KEYA1=" << KEYA1 << " XK0DEC=" << XK0DEC << std::endl;
#else
   std::cout 
        << "   Tau polarization      : " << fTauola_Keypol << "            : (0,1) = (off, on)" << std::endl;
#endif
//   std::cout 
//	<< "   PYGIVE command given by Spring : " << fSpring->GetPygiveCommandForHadronizer() << std::endl;        
   std::cout << std::endl;

// Initialize tauola
#if __TAUOLA_MINOR_VERSION__ < 7
   Int_t inut=5;
   Int_t iout=6;
   tauint_(&inut, &iout, &JAK1, &JAK2, &ITDKRC,  &KEYA1, &XK0DEC);
#endif
   Double_t  mh     = gJSF->Env()->GetValue("JSFHadronizer.HiggsMass",120.);
   Double_t  wh     = gJSF->Env()->GetValue("JSFHadronizer.HiggsWidth",0.3605e-2);

   Int_t hparameter_set=gJSF->Env()->GetValue("JSFHadronizer.ParameterSet",0);
   std::cout << "   Pythia parameter set  : " << hparameter_set 
	     << "            : (0,1)=(default, DBD_standard) " << std::endl;
   if ( hparameter_set == 1 ) {
      SetPythiaDBDStandard(mh, wh);
   }
   else {
     fPythia->SetPMAS(25,1,mh);
     fPythia->SetPMAS(25,2,wh);
     std::cout << "   Pythia Higgs mass    : " << mh << std::endl;
     std::cout << "          Higgs width   : " << wh << std::endl;

   }

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
   fPythia->Pyinit(frame, beam, target, ecm);

   const std::string pygivecmd=fSpring->GetPygiveCommandForHadronizer();
   if ( pygivecmd.size() > 0 ) {
     pygive_(pygivecmd.c_str(),pygivecmd.size());
   }
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

     Int_t kc    = fPythia->Pycomp(kh);
     Int_t mdcy2 = fPythia->GetMDCY(kc,2);
     Int_t mdcy3 = fPythia->GetMDCY(kc,3);
     if( mdcyh < 100 ) {
       for (Int_t i=mdcy2; i<=mdcy2+mdcy3-1; i++) fPythia->SetMDME(i,1,0);
       fPythia->SetMDME(mdcy2+mdcyh-1,1,1);
       cout << " ---------------------------------------------" << endl
            << " H decay restricted to mode: mdcyh = " << mdcyh << endl
            << " mdcy2 = " << mdcy2                             << endl
            << " mdcy3 = " << mdcy3                             << endl
            << " ---------------------------------------------" << endl;
     }
     else {
       Int_t kdcyh=mdcyh%100;
       fPythia->SetMDME(mdcy2+kdcyh-1,1,0);
       cout << " ---------------------------------------------" << endl ;
       cout << " H decay to mode: mdcyh =" << kdcyh << " is suppressed." << endl;
     }
     cout << " H Decay mode definition" << endl;
     cout << "   1(d dbar), 2(u ubar), 3(s sbar), 4(c cbar), 5(b bbar), 6(ttbar)" << endl;
     cout << "   7(b'b'bar), 8(t't'bar), 9(e-e+), 10(mu-mu+), 11(tau-tau+), 12(tau'-tau'+)" << endl;
     cout << "   13(glue glue), 14(gamma gamma), 15(gamma Z0), 16(Z0 Z0), 17(W+W-)" << endl;
   }

   Int_t iprint = gJSF->Env()->GetValue("JSFHadronizer.PrintDecayModeTable",0);
   if (iprint) {
     iprint = 12;
     pylist_(&iprint);
   }
#if __TAUOLA_MINOR_VERSION__ >= 7
   fPythia->SetMSTJ(28,1); 
   Int_t tauola_mode=-1;
   tauola_(&tauola_mode, &fTauola_Keypol);
#endif

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

  Int_t iret;


  if( fHadronizerType == 1 ) {    // Use Old Hadronizer code
    Hadronize(fSpring, iret);
  }
  else if ( fHadronizerType == 3 ) { // Use Hadronizer provided by JSFSpring module
    fSpring->DoHadronize(iret);
  }
  else {
    HadronizeNew(fSpring, iret);  // Use new hadronizer
  }


  if ( iret < 0 ) {  
    if ( gJSF->Env()->GetValue("JSFHadronizer.ExitOnError",1) == 1 ) {
      return kFALSE;
    }
    else {
      gJSF->SetReturnCode(JSFSteer::kJSFSkipRestModules);
      std::cout << "Warning  JSFHadronizer::Process requested the event " 
	<< gJSF->GetEventNumber() << " to be skipped" << std::endl;
      return kTRUE;
    }
  }

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
  Int_t mdcyzs = fPythia->GetMDCY(23,1);
  Int_t mdcyws = fPythia->GetMDCY(24,1);
  Int_t mdcyhs = fPythia->GetMDCY(25,1);

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
#if 1
    rbuf[j][15]=p->fLifeTime;
#endif
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
#if 0
      if ( ndau <= 0 ) {
#else
      if ( ndau <= 0 || rbuf[ip-1][15] > 0.) {
#endif
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

      //ishpr1[is-1]=ishufl[isgrp[islv-1][is-1][0]-1];
      //ishpr2[is-1]=ishufl[isgrp[islv-1][is-1][1]-1];

      // changed by daniel jeans to allow fragmentation of single particles
      ishpr1[is-1] = isgrp[islv-1][is-1][0]-1>=0 ? ishufl[isgrp[islv-1][is-1][0]-1] : 0;
      ishpr2[is-1] = isgrp[islv-1][is-1][1]-1>=0 ? ishufl[isgrp[islv-1][is-1][1]-1] : 0;
      // ensure that zero entry is the second one (in case of passing single particle)
      if (ishpr1[is-1]==0) {
        int temp = ishpr1[is-1];
        ishpr1[is-1] = ishpr2[is-1];
        ishpr2[is-1] = temp;
      }



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

    if (nzdk) fPythia->SetMDCY(23,1,0);
    if (nwdk) fPythia->SetMDCY(24,1,0);
    if (nhdk) fPythia->SetMDCY(33,1,0);

    Fragmentation(nin, rinlst, maxout, nsg, ishpr1, ishpr2,
		  kstat, jstat, nout, rotlst, iret);

    if (nzdk) fPythia->SetMDCY(23,1,mdcyzs);
    if (nwdk) fPythia->SetMDCY(24,1,mdcyws);
    if (nhdk) fPythia->SetMDCY(33,1,mdcyhs);

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
    if( ida > 25 && ida != 220000 && ida != 1000022 && ida != 1000015 && ida != 1000039) {
      printf("Warning in JSFHadronizer::Hadronize");
      printf(" Particle ID=%d is not recognized.\n",id);
      continue;
    }
    //C--
    //C  Neutrinos, e, mu, gamma, LSP, neutralino 1,
    //C  stau 1 (~L): 1000015, gravitino: 1000039
    //C--
    if( ida==12 || ida==14 || ida==16 ||
	ida==11 || ida==13 || ida==22 ||
	ida==220000 || ida==1000022 || ida==1000015 || ida==1000039) {
      inelm++;
      for(Int_t k=0;k<20;k++){ rtmp[k]=0; }
      rtmp[0]=inelm;
      rtmp[1]=id;
      rtmp[2]=rbuf[ip-1][2];
      rtmp[3]=(ida != 1000039 ? -id%2 : 0);
      rtmp[4]=rbuf[ip-1][4];
      rtmp[5]=rbuf[ip-1][5];
      rtmp[6]=rbuf[ip-1][6];
      rtmp[7]=rbuf[ip-1][7];
#if 0
      rtmp[13]=-ip;
#else
      rtmp[11]=rbuf[ip-1][11];  // # daughters
      rtmp[12]=-rbuf[ip-1][12]; // 1st daughter
      rtmp[13]=-ip;             // mother
      Double_t xctau=rbuf[ip-1][15];  // ctau
      rtmp[15]=xctau;
      if (rtmp[15]>0.) {
        rtmp[16]=gRandom->Exp(xctau);
      }
#endif
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
#if 1
  // Fix daughter pointers
  for(Int_t  i=1;i<=npgen;i++){
    Int_t idau = static_cast<JSFGeneratorParticle *>(part[i-1])->fFirstDaughter;
    if( idau < 0 ){
      Int_t ndau = static_cast<JSFGeneratorParticle *>(part[i-1])->fNdaughter;
      for(Int_t j=i+1;j<=npgen;j++){
        Int_t ip = static_cast<JSFGeneratorParticle *>(part[j-1])->fMother;
	if( ip == idau ){
          static_cast<JSFGeneratorParticle *>(part[i-1])->fFirstDaughter=j;
	}
	if( -ip >=-idau && -ip <= -idau+ndau-1  ){
          static_cast<JSFGeneratorParticle *>(part[j-1])->fMother=i;
	}
      }
    }
  }
#endif
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
  Pyjets_t *pyjets=fPythia->GetPyjets();

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
     
#if __TAUOLA_MINOR_VERSION__ < 7
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
#else
      std::cerr << "Fatal error in JSFHadronize::Fragmentation(...) :: Old hadronizer was used with newer tauola version.";
      exit(-1);
#endif

      pyhepc2_(&two, 
               &pyjets_.N, &pyjets_.NPAD, pyjets_.K, pyjets_.P, pyjets_.V,
                pydat1_.MSTU, pydat1_.PARU, pydat1_.MSTJ, pydat1_.PARJ,
                pydat2_.KCHG, pydat2_.PMAS, pydat2_.PARF, pydat2_.VCKM);
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

	Double_t qmx = 1;
	if (ip2>0) {
	  qmx=TMath::Power(pyjets->P[3][ip1-1]+pyjets->P[3][ip2-1],2) 
	    - TMath::Power(pyjets->P[0][ip1-1]+pyjets->P[0][ip2-1],2) 
	    - TMath::Power(pyjets->P[1][ip1-1]+pyjets->P[1][ip2-1],2) 
	    - TMath::Power(pyjets->P[2][ip1-1]+pyjets->P[2][ip2-1],2);
	  qmx = TMath::Sqrt(qmx)-pyjets->P[4][ip1-1]-pyjets->P[4][ip2-1];
	  qmx = TMath::Max(qmx, 1.);
	} 

	pyshow_(&ip1, &ip2, &qmx);

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
      Int_t kc=fPythia->Pycomp(kf);
      xctau=fPythia->GetPMAS(kc,4)*0.1;
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

    // Durty edit to avoid error 
    if ( firstchild == 0 && lastchild != 0 ) {
       if( ks == 1 && ( abs(kf) == 11 || abs(kf) == 13 ) ) { 
	  std::cout << "Warning JSFHadronizer::Fragmentation encountered strange pointers to first and last child" << std::endl;
          std::cout << "  firstchild=" << firstchild << " lastchild=" << lastchild << " at event number " << gJSF->GetEventNumber() << std::endl;
          std::cout << "  Errorneous line is " << i << " ks=" << ks << " kf=" << kf << " E=" << pyjets->P[3][i-1] << std::endl;
          std::cout << "  To fix, lastchild and ndau are set to 0" << std::endl;
          std::cout << "  Event listing of this Fragmentation will follow " << std::endl;
          pylist_(&two);
	  ndau=0;
	  lastchild=0;
        }
     }

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

// ___________________________________________________________
void JSFHadronizer::HadronizeNew(JSFSpring *spring, Int_t &nret)
{
//(Function)
// Hadronize spring partons, using py2frm, py4frm, py6frm of pythia 
//  stored them as JSFGenerator particles.
//  JSFSpringParton::fPyjetsPositon are used to feed /PYJETS/
//  
//  fPyjetsPosition should be filled by JSFSpring::Pyevnt as
//  real*4 value.  It's format is 
//     fPyjetsPosition = LEVEL*1000 + NFERMIONS*100 + LOC
//  where
//     LOC is the line number of this partons in /PYJETS/
//     NFERMION is either 2, 4, or 6 corresponding to 
//         py2frm, py4frm, py6frm.
//     LEVEL is a number from 0 to N. Particles with same level
//         are saved to /PYJETS/ and then hadronized by pyNfrm.
//       
//  When LEVEL=0, SpringPartons are copied to /PYJETS/ without changing 
//  the partile ordering and setting status=21 ( documentation line ).
//
//  Particles decayed by Spring, top for example, 
//  should have fPyjetsPosition < 0 so that they are not hadronized by pyNfrm routines. 
//  Stable particles which need to be decayed and hadronized by JSFHadronizer
//  should be a part of LEVEL=0 particles
//
//  In order to use py4frm and py6frm, Spring module should provide arguments 
//  to these subroutine by overriding JSFSpring::GetPy6frmProb(...) or 
//  JSFSpring::GetPy4frmArgs(...)
//  

  Pyjets_t *pyjets=fPythia->GetPyjets();
  JSFGeneratorBuf *gbuf=(JSFGeneratorBuf*)EventBuf();
//   TClonesArray &part=*(gbuf->GetParticles());
  nret=0;
  pyjets->N=0;
  hepevt_.nevhep=gJSF->GetEventNumber();
  hepevt_.nhep=0;
  int npgen=0;
  int ncid=100;
  int lastColor=0;

  JSFSpringBuf *spevt=(JSFSpringBuf*)spring->EventBuf();
  TClonesArray *ps=spevt->GetPartons();
  Int_t npart=spevt->GetNpartons();

  std::vector<GenP_t> sparticles;

//  std::cout << "JSFHadronizer .. npart=" << npart << std::endl;
  for(Int_t j=0;j<npart;j++){
    JSFSpringParton *p=(JSFSpringParton*)ps->At(j);
    GenP_t sp;
    for(int i=0;i<20;i++){ sp.rbuf[i]=0; }
    sp.rbuf[0]=p->fSer;
    sp.rbuf[1]=p->fID;
    sp.rbuf[2]=p->fMass;
    sp.rbuf[3]=p->fCharge;
    sp.rbuf[4]=p->fP[1];
    sp.rbuf[5]=p->fP[2];
    sp.rbuf[6]=p->fP[3];
    sp.rbuf[7]=p->fP[0];
    sp.rbuf[11]=p->fNdaughter;
    sp.rbuf[12]=p->fFirstDaughter;
    sp.rbuf[13]=p->fMother;
    sp.rbuf[15]=p->fLifeTime;
    sp.rbuf[16]=p->fHelicity;
    sp.rbuf[17]=p->fColorID;
    sp.rbuf[18]=p->fShowerInfo;
    sp.rbuf[19]=p->fPyjetsPosition;
    sp.ibuf[0]=21;   // status
    sp.ibuf[1]=p->fMother; // first mother;
    sp.ibuf[2]=0;  // second mother;
    sp.ibuf[3]=p->fFirstDaughter ; // first daughter;
    sp.ibuf[4]=p->fFirstDaughter==0 ? 0 : p->fFirstDaughter+p->fNdaughter-1 ; // last daughter
    sp.ibuf[5]=0;  // colorID a la hepeup
    sp.ibuf[6]=0;  // anti-colorID a la hepeup
    if( p->fID > 0 && p->fID < 10 ) {   // ColorID of quark
      sp.ibuf[5]=p->fColorID < 100 ? p->fColorID+100 : p->fColorID;
      ncid=ncid<sp.ibuf[5] ? sp.ibuf[5] : ncid;
      if ( ncid > lastColor ) { lastColor=ncid; }
    }
    else if ( p->fID > -10 && p->fID < 0 ) { // ColorID of anti-quark
      sp.ibuf[6]=p->fColorID < 100 ? p->fColorID+100 : p->fColorID;
      ncid=ncid<sp.ibuf[6] ? sp.ibuf[6] : ncid;
      if ( ncid > lastColor ) { lastColor=ncid; }
    }
    sp.done=false;
    sparticles.push_back(sp);
//
// Fill /pyjets/ by SpringParton info.
//
    int ip=pyjets->N;
    pyjets->K[0][ip]=21; // status code
    pyjets->K[1][ip]=sp.rbuf[1];
    pyjets->K[2][ip]=sp.ibuf[1];
    pyjets->K[3][ip]=sp.ibuf[3];
    pyjets->K[4][ip]=sp.ibuf[4];
    for(int i=0;i<4;i++) { pyjets->P[i][ip]=sp.rbuf[i+4]; }
    pyjets->P[4][ip]=sp.rbuf[2];
    for(int i=0;i<5;i++) { pyjets->V[i][ip]=0.0 ; }
    fHelicity[ip]=sp.rbuf[16];
    fColor[ip] = sp.ibuf[5];
    fAntiColor[ip]=sp.ibuf[6];
    pyjets->N++;
  }
  int ipspring=pyjets->N;

//  int two=2;
//  int five=5;
//
// Pick lv-th partons, fill /PYJETS/, then hadronize 
// by py2frm, py4frm, py6frm.

  for(int lv=0;lv<20;lv++) {
    int nfound=0;
    int nfermi=0;
    if( lv != 0 ) { pyjets->N=0; }
    int morig[100];
    int iplast=0;
    int jmin=lv==0 ? sparticles.size() : 0 ; 
    for(unsigned int j=jmin; j<sizeof(pyjets_.K)/20; j++) { 
	fHelicity[j]=0.0 ;
	fColor[j]=0.0 ;
	fAntiColor[j]=0.0 ;
    }
    for(unsigned int j=0; j < sparticles.size() ; j++) {
      int jpos=sparticles[j].rbuf[19];
//      std::cout << " j=" << j << " jpos=" << jpos << std::endl;
      if( jpos < 0 ) { continue; }
      int iv=jpos/1000;
      if( iv != lv ) { continue ; }
      nfound++;
      nfermi=(jpos%1000)/100;
      int ip=lv==0 ? jpos%100+ipspring : jpos%100 ;
      pyjets->N++;
      pyjets->K[0][ip]=1; // status code
      pyjets->K[1][ip]=sparticles[j].rbuf[1];
      pyjets->K[2][ip]=0;
      pyjets->K[3][ip]=0;
      pyjets->K[4][ip]=0;
      for(int i=0;i<4;i++) { pyjets->P[i][ip]=sparticles[j].rbuf[i+4]; }
      pyjets->P[4][ip]=sparticles[j].rbuf[2];
      for(int i=0;i<5;i++) { pyjets->V[i][ip]=0.0 ; }
      fHelicity[ip]=sparticles[j].rbuf[16];
      fColor[ip]=sparticles[j].ibuf[5] == 0 ? 0 : sparticles[j].ibuf[5] ;
      fAntiColor[ip]=sparticles[j].ibuf[6] == 0 ? 0 : sparticles[j].ibuf[6] ;
      morig[ip]=j;
      iplast=ip;
    }
//    if( lv != 0 ) { continue ; }

    if( nfound == 0 ) { break; }
    int irad=fFSQEDRadiation;
    irad=0;
    int itau=1; // (0,1)=(no pytaud, pytaud call)
    int icom=1; // (0,1)=(hepevt, pyjets)
    std::string msg;
    if( nfermi == 2 ) { 
      py2frm_(&irad, &itau, &icom);
    }
    else if( nfermi == 4 ) {
      double atotsq;
      double a1sq;
      double a2sq;
      int istrat;
      fSpring->GetPy4frmArgs(lv, atotsq, a1sq, a2sq, istrat);
      py4frm_(&atotsq, &a1sq, &a2sq, &istrat, &irad, &itau, &icom);
    }
    else if( nfermi == 6 ) {
      double pcomb[7];
      fSpring->GetPy6frmProb(lv, pcomb);
      py6frm_(&pcomb[0],&pcomb[1],&pcomb[2],&pcomb[3],&pcomb[4],&pcomb[5],&pcomb[6],
              &irad,&itau,&icom);
    }
    else {
      std::cout << "JSFHadronizer::HadronizerNew .. nfermi=" << nfermi << " not supported yet." 
 	  << std::endl;
      exit(-1);
    }

    if( fPythia->GetMSTU(23) != 0 || fPythia->GetMSTU(24) != 0
      || fPythia->GetMSTU(28) != 0 ) {
      std::cout <<" Warning .. JSFHadronizer::Fragmentation" << std::endl;
      std::cout << "Possible error in PYEXEC detected."
              << " MSTU(23)=" << fPythia->GetMSTU(23)
              << " MSTU(24)=" << fPythia->GetMSTU(24)
              << " MSTU(28)=" << fPythia->GetMSTU(28)
              << " This event will be skipped." << std::endl;
      std::cout << " Event number is " << gJSF->GetEventNumber() << std::endl;
      fPythia->SetMSTU(23,0);
      fPythia->SetMSTU(24,0);
      fPythia->SetMSTU(28,0);
      int two=2;
      nret=-1;
      pylist_(&two);
      return;
    }
    int one=1;
//  Correct pointer to mother"
    if( lv > 0 ) {
      for(int ip=0;ip<iplast;ip++) {
        int j=morig[ip];
        pyjets->K[2][ip]=sparticles[j].ibuf[1];
      }
    }

    SetColorFlowInfo(&lastColor);
    pyhepc_(&one);
    if( pyjets->N != hepevt_.nhep ) {
      std::cout << "Error in JSFHadronizer::HadronizeNew ... "
	<< " Entryies in /PYJETS/ and /HEPEVT/ are different." << std::endl;
      std::cout << "  pyjets->N=" << pyjets->N << "  hepevt_.nhep=" << hepevt_.nhep << std::endl;
      std::cout << "  Program will stop here." << std::endl;
      exit(-1);
    }
    Hepevt2GeneratorParticle(npgen, gbuf);
  }
}

//___________________________________________________________________________
void JSFHadronizer::SetColorFlowInfo(int *lastColor) 
{
// Analyize /PYJETS/ and set color flow info.
// Particles with status=3, 13, 14 are scanned and corresponding fColor[], 
// fAntiColor[] of quarks are set, if it is not set yet.
  int nc=*lastColor;
  Pyjets_t *pyjets=fPythia->GetPyjets();
  for(int ip=0;ip<pyjets->N;ip++) {
    if( pyjets->K[0][ip] == 42 || pyjets->K[0][ip] == 52 ) {
      std::cout << "Warning!! Evt#" << gJSF->GetEventNumber() 
		<< ".  JSFHadronizer::SetColorFlowInfo( ) .. status code " << pyjets->K[0][ip] 
	 	<< " is not supported. Not color flow info is set. ip= " << ip << std::endl;
    }
    if( pyjets->K[0][ip] != 3 && pyjets->K[0][ip] != 13 && pyjets->K[0][ip] != 14 ) { continue ; }

    int k4=pyjets->K[3][ip];
    int k5=pyjets->K[4][ip];
    if( k4==0 || k5 == 0 ) { continue; }
    int pid=pyjets->K[1][ip];
    if( abs(pid) > 10 && pid != 21 ) { continue ; }
    int icfrom=(k4%100000000)/10000;
    int icto  = k4%10000;
    int iafrom=(k5%100000000)/10000;
    int iato  = k5%10000; 

    int ncnow=pid>0 ? fColor[ip] : fAntiColor[ip] ;
    if ( ncnow == 0 ) {
      if ( pid > 0 && pid < 10 ) {
         ncnow=fAntiColor[icfrom-1]!=0 ? fAntiColor[icfrom-1] : nc+1 ;
         fColor[ip]=ncnow;
      }
      else if ( pid > -10 && pid < 0 ) {
         ncnow=fColor[iafrom-1]!=0 ? fColor[iafrom-1] : nc+1 ;
         fAntiColor[ip]=ncnow;
      }
    }
// loop over daughter particles.
    while( icto != 0 ) {
      fColor[icto-1]=ncnow;
      if ( pyjets->K[1][icto-1] == 21 ) { fAntiColor[icto-1]=ncnow; }
      k4=pyjets->K[3][icto-1];
      k5=pyjets->K[4][icto-1];
      icfrom=(k4%100000000)/10000;
      icto  = k4%10000;
      if( pyjets->K[0][icto-1] != 3 && pyjets->K[0][icto-1] != 13 
       && pyjets->K[0][icto-1] != 14 ) { icto=0; }
    }  
    while( iato != 0 ) {
      fAntiColor[iato-1]=ncnow;
      if( pyjets->K[1][iato-1] == 21) { fColor[iato-1]=ncnow; }
      k4=pyjets->K[3][iato-1];
      k5=pyjets->K[4][iato-1];
      iafrom=(k5%100000000)/10000;
      iato  = k5%10000;
      if( pyjets->K[0][iato-1] != 3 && pyjets->K[0][iato-1] != 13 
       && pyjets->K[0][iato-1] != 14 ) { iato=0; }
    }  
    nc=ncnow;
  }

  *lastColor=nc;

}


// __________________________________________________________________________
void JSFHadronizer::Hepevt2GeneratorParticle(Int_t &npgen, JSFGeneratorBuf *gbuf)
{
// Copy /HEPEVT/ to JSFGeneratorParticles
  TClonesArray &part=*(gbuf->GetParticles());
  int npgen0=npgen;
  for(int j=0;j<hepevt_.nhep;j++){
    double rbuf[JSFGeneratorParticle::kRdataSize];
    int    ibuf[JSFGeneratorParticle::kIdataSize];
    for(unsigned int i=0;i<20;i++) { rbuf[i]=0.0; }
    rbuf[JSFGeneratorParticle::kSerial]=j+1+npgen0;
    int pid=hepevt_.idhep[j];  // pid
    rbuf[JSFGeneratorParticle::kID]=pid;  // PID
    rbuf[JSFGeneratorParticle::kMass]=hepevt_.phep[j][4]; // Mass
    double charge=fPythia->Pychge(pid)/3.0;
    int    kc=fPythia->Pycomp(pid);
    double xctau=fPythia->GetPMAS(kc,4)*0.1;
    rbuf[JSFGeneratorParticle::kCharge]=charge;
    for(unsigned int i=0;i<4;i++) { 
        rbuf[i+JSFGeneratorParticle::kPx]=hepevt_.phep[j][i]; 
    }
    for(unsigned int i=0;i<4;i++) { 
        rbuf[i+JSFGeneratorParticle::kX]=hepevt_.vhep[j][i]*0.1; }
    int    firstchild=hepevt_.jdahep[j][0];
    int    lastchild=hepevt_.jdahep[j][1];
    int    firstmother=hepevt_.jmohep[j][0];
    int    lastmother=hepevt_.jmohep[j][1];
    rbuf[JSFGeneratorParticle::kNDaughter]
	= firstchild==0 ? 0 : lastchild-firstchild+1;
    rbuf[JSFGeneratorParticle::kFirstDaughter]
        = firstchild==0 ? 0 : firstchild+npgen0;
    rbuf[JSFGeneratorParticle::kMother]
        = firstmother==0 ? 0 : firstmother+npgen0;
    rbuf[JSFGeneratorParticle::kLifeTime]=xctau;
// Set polarization vector
    float pa=sqrt(rbuf[JSFGeneratorParticle::kPx]*rbuf[JSFGeneratorParticle::kPx]
		+ rbuf[JSFGeneratorParticle::kPy]*rbuf[JSFGeneratorParticle::kPy]
		+ rbuf[JSFGeneratorParticle::kPz]*rbuf[JSFGeneratorParticle::kPz]);
    rbuf[JSFGeneratorParticle::kSpinX]=fHelicity[j]*rbuf[JSFGeneratorParticle::kPx]/pa;
    rbuf[JSFGeneratorParticle::kSpinY]=fHelicity[j]*rbuf[JSFGeneratorParticle::kPy]/pa;
    rbuf[JSFGeneratorParticle::kSpinZ]=fHelicity[j]*rbuf[JSFGeneratorParticle::kPz]/pa;

    ibuf[JSFGeneratorParticle::kStatus]=hepevt_.isthep[j];
    ibuf[JSFGeneratorParticle::kSecondMother]
        = lastmother==0 ? 0 : lastmother+npgen0;
    ibuf[JSFGeneratorParticle::kColorFlow0]=fColor[j];
    ibuf[JSFGeneratorParticle::kColorFlow1]=fAntiColor[j];
//
    new(part[npgen]) JSFGeneratorParticle(&rbuf[0], &ibuf[0]);
    npgen++;
  }

  gbuf->SetNparticles(npgen);

}

//_____________________________________________________________
void JSFHadronizer::SetColorFlow(int ip, int npgen0, int ibuf[])
{
  Pyjets_t *pyjets=fPythia->GetPyjets();
  int status=pyjets->K[0][ip];
  int pid=pyjets->K[1][ip];
  ibuf[JSFGeneratorParticle::kColorFlow0]=0;
  ibuf[JSFGeneratorParticle::kColorFlow1]=0;
  int ki4=pyjets->K[3][ip];
  int ki5=pyjets->K[4][ip];

  if ( status == 3 || status == 13 || status == 14 ) {
    if( pid > 0 && pid < 10 ) { 
      ibuf[JSFGeneratorParticle::kColorFlow0]=(ki4%100000000)/10000+npgen0; // color flow from
      ibuf[JSFGeneratorParticle::kColorFlow1]=ki4%10000+npgen0;             // color flow to
    }
    else if ( pid > -10 && pid < 0 ) {
      ibuf[JSFGeneratorParticle::kColorFlow0]=(ki5%100000000)/10000+npgen0; // anti-color flow from
      ibuf[JSFGeneratorParticle::kColorFlow1]=ki5%10000+npgen0;             // anti-color flow to
    } 
  }
  else if ( status == 42 || status == 52 ) {
      ibuf[JSFGeneratorParticle::kColorFlow0]=ki4%10000+npgen0;   // IC1
      ibuf[JSFGeneratorParticle::kColorFlow1]=ki5%10000+npgen0;   // IC3
  }
}

//______________________________________________________________
JSFHadronizer *JSFHadronizer::Instance()
{
  if( !fInstance ) { fInstance=new JSFHadronizer(); }
  return fInstance;
}

//_______________________________________________________________
void JSFHadronizer::Pytaud(int *itau, int *iorig, int *kforig, int *ndecay)
{
// Decays tau using tauola.  for Pythia. Just for a single tau decay
// Pyhita information is store in /PYJETS/.  Copy tau info to /HEPEVT/
// Insert the result to /PYJETS/

  int ip=*itau-1;

  int kp=0;
  hepevt_.nhep=1;
  hepevt_.isthep[kp]=1;
  int  pid=pyjets_.K[1][ip];
  hepevt_.idhep[kp]=pyjets_.K[1][ip];
  hepevt_.jmohep[kp][0]=0;
  hepevt_.jmohep[kp][1]=0;
  hepevt_.jdahep[kp][0]=0;
  hepevt_.jdahep[kp][1]=0;
  for(int i=0;i<5;i++) { hepevt_.phep[kp][i]=pyjets_.P[i][ip]; }
  for(int i=0;i<4;i++) { hepevt_.vhep[kp][i]=pyjets_.V[i][ip]; }

// Set polarization vector.
  float pol[4];
  for(int i=0;i<4;i++) { pol[i]=0.0 ; }
  float ap=sqrt(hepevt_.phep[0][0]*hepevt_.phep[0][0]
          + hepevt_.phep[0][1]*hepevt_.phep[0][1] + hepevt_.phep[0][2]*hepevt_.phep[0][2]);
  float heltau=fHelicity[ip]*fIHLON;

// If helicity is 0, try to estimate from parent
  if( fHelicity[ip] == 0 ) {
    if( abs(*kforig) == 24 ) { // W+- 
      heltau=*kforig>0 ? fIHLON : -fIHLON ; 
    }
    else if( abs(*kforig) == 37 ) { // H+-
      heltau=*kforig>0 ? -fIHLON : fIHLON ; 
    }
    else if( *kforig == 25 || *kforig == 23 ) { // H0(25) or Z(23)
      int dummy=0;
      float a_tau=0.15;
      float prob_tau_left=*kforig==25 ? 0.5 : (a_tau+1.0/2.0) ; 
      heltau=(float)pyr_(&dummy) > prob_tau_left ? 1 : -1 ;
      float hsign=*kforig==25 ? -1.0 : 1.0 ; 
      fHelicity[ip]=heltau>0.0 ? 1 : -1 ;
      if ( *iorig > 0 ) { // Consider correlation between two daughters
        int jtau3=pyjets_.K[3][*iorig-1];
        int jtau4=pyjets_.K[4][*iorig-1];
        int jttau=jtau3==*itau ? jtau4 : jtau3 ;
        if( jtau4-jtau3 != 1 || abs(pyjets_.K[1][jttau-1]) != 15 ) {
           int two=2;
           pylist_(&two);
           std::cerr << "Error in JSFHadronizer::Pytaud  tau in Higgs/Z decay is not tau+, tau- " 
		<< " *itau=" << *itau << " *iorig=" << *iorig 
		<< " *kforig=" << *kforig << " jtau3=" << jtau3 
	        << " jtau4=" << jtau4 << std::endl;
           std::cerr << " jttau=" << jttau << " K[1][jttau-1] =" << pyjets_.K[1][jttau-1]
	        << std::endl;
           std::cerr << " Program will stop here " << std::endl;
           exit(-1);
        }
        fHelicity[jttau-1]=hsign*heltau;
      }
    }

    fHelicity[ip]=heltau>0.0 ? 1 : -1 ;
  }
  pol[0]=heltau*hepevt_.phep[0][0]/ap;
  pol[1]=heltau*hepevt_.phep[0][1]/ap;
  pol[2]=heltau*hepevt_.phep[0][2]/ap;

  taupos_.np1=1;
  taupos_.np2=1;
  for(int i=0;i<4;i++) { p4tau_.p4tau[i]=hepevt_.phep[kp][i]; }

  int kto=2;
  if( pid <= 0 ) { kto=1; }
  dexay_(&kto, pol);

  Pyjets_t *pyjets=fPythia->GetPyjets();
  int np=pyjets->N;
  int np0=np;
  pyjets->K[3][ip]=hepevt_.jdahep[0][0]+np0-1;
  pyjets->K[4][ip]=hepevt_.jdahep[0][1]+np0-1;
//
  for(int j=1;j<hepevt_.nhep;j++) {
    for(int i=0;i<4;i++) { pyjets->V[i][np]=hepevt_.vhep[j][i]; }
    for(int i=0;i<5;i++) { pyjets->P[i][np]=hepevt_.phep[j][i]; }
    if( hepevt_.isthep[j] == 1 ) { 
       pyjets->K[0][np]=1 ; // status code
       pyjets->K[3][np]=0 ; // first daughter
       pyjets->K[4][np]=0 ; // last daughter
    }
    else {
       pyjets->K[0][np]=11;
       pyjets->K[3][np]=hepevt_.jdahep[j][0]+np0-1;
       pyjets->K[4][np]=hepevt_.jdahep[j][1]+np0-1;
    }

    pyjets->K[1][np]=hepevt_.idhep[j];
    int ipmo=hepevt_.jmohep[j][0]-1;
    int idmo=ipmo>=0 ? abs(hepevt_.idhep[ipmo]) : -1 ;
    if( idmo != 15 && idmo >= 0 ) {
      pyjets->K[2][np]=hepevt_.jmohep[j][0]+np0-1;
    }
    else if ( idmo == 15 )  {
      pyjets->K[2][np]=*itau;
    }
    else {
      std::cerr << "Error in JSFHadronizer::Pytaud ...  illegal mother id. idmo=" 
                << idmo << std::endl;
      std::cerr << "Program will stop here." << std::endl;
      exit(-1);
    }
    np++;
  }

  *ndecay=hepevt_.nhep-1;

  return;
}


// ___________________________________________________________________________
void JSFHadronizer::SetPythiaDBDStandard(Double_t mh, Double_t wh)
{
// Set OPAL tuned hadronization parameter
// Set OPAL tune parameters
  fPythia->SetPMAS(25,1,mh);
  fPythia->SetPMAS(25,2,wh);
  fPythia->SetMSTJ(41,2);
  fPythia->SetMSTU(22,20);
  fPythia->SetMSTJ(28,2);  // Tau decay : (0,1,2) = (pydecy, pytaud when mother known, allways pytaud) 
  fPythia->SetPARJ(21,0.40000);
  fPythia->SetPARJ(41,0.11000);
  fPythia->SetPARJ(42,0.52000);
  fPythia->SetPARJ(81,0.25000);
  fPythia->SetPARJ(82,1.90000);
  fPythia->SetMSTJ(11,3);
  fPythia->SetPARJ(54,-0.03100);
  fPythia->SetPARJ(55,-0.00200);
  fPythia->SetPARJ(1,0.08500);
  fPythia->SetPARJ(3,0.45000);
  fPythia->SetPARJ(4,0.02500);
  fPythia->SetPARJ(2,0.31000);
  fPythia->SetPARJ(11,0.60000);
  fPythia->SetPARJ(12,0.40000);
  fPythia->SetPARJ(13,0.72000);
  fPythia->SetPARJ(14,0.43000);
  fPythia->SetPARJ(15,0.08000);
  fPythia->SetPARJ(16,0.08000);
  fPythia->SetPARJ(17,0.17000);
  fPythia->SetMSTP( 3,1);

  std::cout << " === Pythia hadronization parameters : DBD standard value ==================" << std::endl;
  std::cout << "   PMAS(25,1) : " << fPythia->GetPMAS(25,1) << "           : Higgs mass " << std::endl;
  std::cout << "   PMAS(25,2) : " << fPythia->GetPMAS(25,2) << "           : Higgs width " << std::endl;

  std::cout << "   MSTJ(11) : " << fPythia->GetMSTJ(11) << "  : Choice of fragmentation function " << std::endl;
  std::cout << "   MSTJ(28) : " << fPythia->GetMSTJ(28) 
            << "   :  Tau decay : (0,1,2) = (pydecy, pytaud when mother known, allways pytaud) " 
            << std::endl;
  std::cout << "   MSTJ(41) : " << fPythia->GetMSTJ(41) << "   : Type of branching in shower " << std::endl;
  std::cout << "   MSTP( 3) : " << fPythia->GetMSTP(3)  << "   : Selection of Lambda in Alpha_s " << std::endl;
  std::cout << "   MSTU(22) : " << fPythia->GetMSTU(22) << "  : Maximum number of errors that are printed." << std::endl;

  Int_t ipPARJ[18]={1,2,3,4,11,   12,13,14,15,16,  17,21,41,32,54,  55,81,82};
  for(Int_t i=0;i<18;i++) {
    std::cout << "   PARJ(" << setw(2) << i << ") : " << fPythia->GetPARJ(ipPARJ[i]) << std::endl;
  }
  std::cout << " ===========================================================================" << std::endl;

}

