//$Id$
//
////////////////////////////////////////////////////////////////
//
// THerwig
//
// THerwig is an interface class to Herwig event generator.
// See Herwig manual for the details.
// 
// Please see the official HERWIG information page:
//    http://hepwww.rl.ac.uk/theory/seymour/herwig/
//
// This interface is prepared by Akiya Miyamoto, KEK
//
//(Usage)
// A simple usage of this class is as follows
//{ 
//  gSystem->Load("libEG.so");
//  gSystem->Load("libTHerwig.so");
//  THerwig herwig;
//  herwig.SetBeams("PBAR","P",1000.0, 1000.0, 1500);
//  herwig.Initialize();
//  for(Int_t ev=1;ev<=100;ev++){
//     herwig.GenerateEvent();
//     cout << "Number of generated particles is " << herwig.GetNHEP() << endl;
//  }
//}
//
// Following lines are comments taken from the source file of 
// Herwig 6.5 
//C-----------------------------------------------------------------------
//C                           H E R W I G
//C
//C            a Monte Carlo event generator for simulating
//C        +---------------------------------------------------+
//C        | Hadron Emission Reactions With Interfering Gluons |
//C        +---------------------------------------------------+
//C I.G. Knowles(*), G. Marchesini(+), M.H. Seymour($) and B.R. Webber(#)
//C-----------------------------------------------------------------------
//C with Minimal Supersymmetric Standard Model Matrix Elements by
//C                S. Moretti(&,X) and K. Odagiri(^)
//C-----------------------------------------------------------------------
//C R parity violating Supersymmetric Decays and Matrix Elements by
//C                          P. Richardson(#)
//C-----------------------------------------------------------------------
//C matrix element corrections to top decay and Drell-Yan type processes
//C                         by G. Corcella(")
//C-----------------------------------------------------------------------
//C Deep Inelastic Scattering and Heavy Flavour Electroproduction by
//C                  G. Abbiendi(@) and L. Stanco(%)
//C-----------------------------------------------------------------------
//C and Jet Photoproduction in Lepton-Hadron Collisions by J. Chyla(~)
//C-----------------------------------------------------------------------
//C(*)  Department of Physics & Astronomy, University of Edinburgh
//C(+)  Dipartimento di Fisica, Universita di Milano-Bicocca
//C($)  Department of Physics & Astronomy, University of Manchester
//C(#)  Cavendish Laboratory, Cambridge
//C(&)  Theory Division, CERN
//C(X)  Institute of Particle Physics Phenomenology, University of Durham
//C(^)  Theory Group, KEK
//C(")  Max Planck Institute, Munich
//C(@)  Dipartimento di Fisica, Universita di Bologna
//C(%)  Dipartimento di Fisica, Universita di Padova
//C(~)  Institute of Physics, Prague
//C-----------------------------------------------------------------------
//C                  Version 6.500 - 16 October 2002
//C-----------------------------------------------------------------------
//C Main references:
//C
//C    G.Corcella, I.G.Knowles, G.Marchesini, S.Moretti, K.Odagiri,
//C    P.Richardson, M.H.Seymour and B.R.Webber, JHEP 0101 (2001) 010
//C
//C    G.Marchesini,  B.R.Webber,  G.Abbiendi,  I.G.Knowles,  M.H.Seymour,
//C    and L.Stanco, Computer Physics Communications 67 (1992) 465.
//C-----------------------------------------------------------------------
//C Please see the official HERWIG information page:
//C    http://hepwww.rl.ac.uk/theory/seymour/herwig/
//C-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////

#ifdef __USEISOCXX__
#include <iostream>
#else
#include <iostream.h>
#endif
#include <strings.h>

#include "TROOT.h"
#include "TGenerator.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
//  #include "Hepevt.h"

#include "TParticle.h"
#include "TMath.h"
#include "THerwig.h"

#include "THerwigCommons.h"
#include "THerwigMacros.h"
#include "JSFSteer.h"

using namespace std; 

extern "C" {
  void* herwig_common_block_address_(char* , int len);

  void hwuine_();
  void hwepro_();
  void hwbgen_();
  void hwdhob_();
  void hwcfor_();
  void hwcdec_();
  void hwdhad_();
  void hwdhvy_();
  void hwmevt_();
  void hwufne_();

  void hwigin_();
  void hwuinc_();
  //  void hwusta_(fStableParticle.Data()); 
  void hweini_();

  void hwefin_();
}

  ClassImp(THerwigParticle)
  ClassImp(THerwig)


//-----------------------------------------------------------------
THerwig::THerwig() : TGenerator("THerwig","THerwig")
{

  delete fParticles; // was allocated as TObjArray in TGenerator

  fParticles = new TClonesArray("TParticle",500);

  fHEPEVT = (COMMON_HEPEVT_t*)herwig_common_block_address_((char*)"HEPEVT",6);

  fHWEVNT = (COMMON_HWEVNT_t*)herwig_common_block_address_((char*)"HWEVNT",6);
  fHWBEAM = (COMMON_HWBEAM_t*)herwig_common_block_address_((char*)"HWBEAM",6);
  fHWBMCH = (COMMON_HWBMCH_t*)herwig_common_block_address_((char*)"HWBMCH",6);
  fHWPROC = (COMMON_HWPROC_t*)herwig_common_block_address_((char*)"HWPROC",6);
  fHWPRAM = (COMMON_HWPRAM_t*)herwig_common_block_address_((char*)"HWPRAM",6);
  fHWPRCH = (COMMON_HWPRCH_t*)herwig_common_block_address_((char*)"HWPRCH",6);
  fHWPART = (COMMON_HWPART_t*)herwig_common_block_address_((char*)"HWPART",6);
  fHWPARP = (COMMON_HWPARP_t*)herwig_common_block_address_((char*)"HWPARP",6);
  fHWBOSC = (COMMON_HWBOSC_t*)herwig_common_block_address_((char*)"HWBOSC",6);
  fHWPARC = (COMMON_HWPARC_t*)herwig_common_block_address_((char*)"HWPARC",6);
  fHWBRCH = (COMMON_HWBRCH_t*)herwig_common_block_address_((char*)"HWBRCH",6);
  fHWPROP = (COMMON_HWPROP_t*)herwig_common_block_address_((char*)"HWPROP",6);
  fHWUNAM = (COMMON_HWUNAM_t*)herwig_common_block_address_((char*)"HWUNAM",6);
  fHWUSUD = (COMMON_HWUSUD_t*)herwig_common_block_address_((char*)"HWUSUD",6);
  fHWMINB = (COMMON_HWMINB_t*)herwig_common_block_address_((char*)"HWMINB",6);
  fHWDIST = (COMMON_HWDIST_t*)herwig_common_block_address_((char*)"HWDIST",6);
  fHWUCLU = (COMMON_HWUCLU_t*)herwig_common_block_address_((char*)"HWUCLU",6);
  fHWUWTS = (COMMON_HWUWTS_t*)herwig_common_block_address_((char*)"HWUWTS",6);
  fHW6202 = (COMMON_HW6202_t*)herwig_common_block_address_((char*)"HW6202",6);
  fHWHARD = (COMMON_HWHARD_t*)herwig_common_block_address_((char*)"HWHARD",6);

}

//---------------------------------------------------------------------------
THerwig::~THerwig()
{
// Destroys the object, deletes and disposes all TMCParticles currently on list.
  if (fParticles) {
    fParticles->Delete();
    delete fParticles;
    fParticles = 0;
  }
}

   
//______________________________________________________________
THerwigParticle::THerwigParticle(): fPdgCode(0), fFlavourCode(0), fICharge(0), 
    fMass(0.0), fLifeTime(0.0), fSpin(0.0), fQORQQB(kFALSE), fQBORQQ(kFALSE)
{
  for(Int_t i=0;i<12;i++){ fName[i]=0x00; }
}
//______________________________________________________________
THerwigParticle::THerwigParticle(const THerwigParticle &p): TObject(p)
{
  // Copy constructor 
  fPdgCode=p.fPdgCode;
  fFlavourCode=p.fFlavourCode;
  fICharge=p.fICharge;
  fMass=p.fMass;
  fLifeTime=p.fLifeTime;
  fSpin=p.fSpin;
  fQORQQB=p.fQORQQB;
  fQBORQQ=p.fQBORQQ;
  for(Int_t i=0;i<12;i++){  fName[i]=p.fName[i]; }

}

//______________________________________________________________
THerwigParticle &THerwig::GetParticleProperties(int pdgcode)
{
  // Return particle information stored in Herwig common

  static THerwigParticle prop;

  for(int i=0;i<__HERWIG_NMXRES_1__;i++){
    if( pdgcode == fHWPROP->IDPDG[i] ) {
      prop.fPdgCode = fHWPROP->IDPDG[i];
      prop.fFlavourCode = fHWPROP->IFLAV[i];
      prop.fICharge  = fHWPROP->ICHRG[i];
      prop.fMass    = fHWPROP->RMASS[i];
      prop.fLifeTime = fHWPROP->RLTIM[i];
      prop.fSpin     = fHWPROP->RSPIN[i];
      prop.fQORQQB   = fHWPROP->QORQQB[i];
      prop.fQBORQQ   = fHWPROP->QBORQQ[i];
      for(int j=0;j<8;j++){
	prop.fName[j]= fHWUNAM->RNAME[i][j];
      }
      prop.fName[8]=0x00;       prop.fName[9]=0x00; 
      prop.fName[10]=0x00;      prop.fName[11]=0x00; 

      return prop;

     }
  }

  if( pdgcode == 94 ) {
      prop.fPdgCode = pdgcode;
      prop.fFlavourCode = 0;
      prop.fICharge  = 0;
      prop.fMass    = 0;
      prop.fLifeTime = 0;
      prop.fSpin     = 0;
      prop.fQORQQB   = kFALSE;
      prop.fQBORQQ   = kFALSE;
      for(int j=0;j<12;j++){
	prop.fName[j]= 0x00;
      }
      prop.fName[0]='J';       prop.fName[1]='E'; 
      prop.fName[2]='T';

      return prop;
  }

  cout << "Error in THerwig::GetParticleProperties()";
  cout << " PDG " << pdgcode << " is not found " << endl;
  
  return prop;
}


//______________________________________________________________
void THerwig::SetBeams(const Char_t *part1, const Char_t *part2, 
       const Double_t pbeam1, const Double_t pbeam2, const Int_t process)
{
  for(int i=0;i<8;i++){
    fHWBMCH->PART1[i]=' ';  fHWBMCH->PART2[i]=' ';
  }    
	
  for(int i=0;i<(int)TMath::Min(strlen(part1),sizeof(fHWBMCH->PART1));i++){
    fHWBMCH->PART1[i]=part1[i];
  }

  for(int i=0;i<(int)TMath::Min(strlen(part2),sizeof(fHWBMCH->PART2));i++){
    fHWBMCH->PART2[i]=part2[i];
  }

  fHWPROC->PBEAM1=pbeam1;
  fHWPROC->PBEAM2=pbeam2;
  fHWPROC->IPROC=process;

  //C---INITIALISE OTHER COMMON BLOCKS
  hwigin_();

}

//______________________________________________________________
void THerwig::Initialize()
{
  //C---USER CAN RESET PARAMETERS AT
  //C   THIS POINT, OTHERWISE DEFAULT
  //C   VALUES IN HWIGIN WILL BE USED.
  //      PTMIN=100.
  
  gROOT->LoadMacro(gJSF->Env()->GetValue("JSFGUI.InitHerwigMacro","InitHerwig.C"));
  gROOT->ProcessLine("InitHerwig();");

  //C---COMPUTE PARAMETER-DEPENDENT CONSTANTS
  hwuinc_();
  //      CALL HWUINC
  //C---CALL HWUSTA TO MAKE ANY PARTICLE STABLE
  //hwusta_(fStableParticle.Data()); 
  //CALL HWUSTA('PI0     ')
  //C---USER'S INITIAL CALCULATIONS
  //      CALL HWABEG
  //C---INITIALISE ELEMENTARY PROCESS
  hweini_();
}

//______________________________________________________________
void THerwig::GenerateEvent()
{
  // C---INITIALISE EVENT
  hwuine_();
  // C---GENERATE HARD SUBPROCESS
  hwepro_();
  //C---GENERATE PARTON CASCADES
  hwbgen_();
  //C---DO HEAVY OBJECT DECAYS
  hwdhob_();
  //C---DO CLUSTER FORMATION
  hwcfor_();
  //C---DO CLUSTER DECAYS
  hwcdec_();
  //C---DO UNSTABLE PARTICLE DECAYS
  hwdhad_();
  //C---DO HEAVY FLAVOUR HADRON DECAYS
  hwdhvy_();
  //C---ADD SOFT UNDERLYING EVENT IF NEEDED
  hwmevt_();
  //C---FINISH EVENT
  hwufne_();
  //C---USER'S EVENT ANALYSIS
  //  CALL HWANAL
}

//______________________________________________________________
void THerwig::Terminate()
{
  //C---TERMINATE ELEMENTARY PROCESS
  hwefin_();
  //C---USER'S TERMINAL CALCULATIONS
  //      CALL HWAEND

}


//________________________________________________________________
void THerwig::SetRMASS(double RMASS, int i)
{
    fHWPROP->RMASS[i]=RMASS;
}

//________________________________________________________________
double THerwig::GetRMASS(int i)
{
  return fHWPROP->RMASS[i];
}

//________________________________________________________________
void THerwig::SetBDECAY(char *bdecay)
{
  if( strcmp(bdecay,"HERW") == 0 ) {
    fHWPRCH->BDECAY[0]='H';     fHWPRCH->BDECAY[1]='E'; 
    fHWPRCH->BDECAY[2]='R';     fHWPRCH->BDECAY[3]='W';
  }
  else if( strcmp(bdecay,"EURO") == 0 ) {
    fHWPRCH->BDECAY[0]='E';     fHWPRCH->BDECAY[1]='U'; 
    fHWPRCH->BDECAY[2]='R';     fHWPRCH->BDECAY[3]='O';
  }
  else if( strcmp(bdecay,"CLEO") == 0 ) {
    fHWPRCH->BDECAY[0]='C';     fHWPRCH->BDECAY[1]='L'; 
    fHWPRCH->BDECAY[2]='E';     fHWPRCH->BDECAY[3]='O';
  }
  else {
    printf("Error in THerwig::SetBDECAY...");
    printf("Undefined BDECAY name .. %s\n",bdecay);
  }

}

//________________________________________________________________
void THerwig::SetAUTPDF(int i, char *autpdf)
{
  for(int k=0;k<20;k++){
    fHWPRCH->AUTPDF[i][k]=autpdf[k];
  }

}

//________________________________________________________________
char *THerwig::GetBDECAY()
{
  static char bdecay[8];
  bdecay[0]=fHWPRCH->BDECAY[0];
  bdecay[1]=fHWPRCH->BDECAY[1];
  bdecay[2]=fHWPRCH->BDECAY[2];
  bdecay[3]=fHWPRCH->BDECAY[3];
  bdecay[4]=0x00;

  return bdecay;
}

//________________________________________________________________
char *THerwig::GetAUTPDF(int i)
{
  static char autpdf[24];
  for(int j=0;j<20;j++){
    autpdf[j]=fHWPRCH->AUTPDF[i-1][j];
  }
  for(int j=20;j<24;j++){ autpdf[j]=0x00; }
  return autpdf;

}


HERWIG_SetGet_PImp(fHEPEVT, int, NEVHEP) 
HERWIG_SetGet_PImp(fHEPEVT, int, NHEP) 
HERWIG_SetGetN_PImp(fHEPEVT, int, ISTHEP) 
HERWIG_SetGetN_PImp(fHEPEVT, int, IDHEP) 
HERWIG_SetGetNN_PImp(fHEPEVT, int, JMOHEP)
HERWIG_SetGetNN_PImp(fHEPEVT, int, JDAHEP)
HERWIG_SetGetNN_PImp(fHEPEVT, double, PHEP)
HERWIG_SetGetNN_PImp(fHEPEVT, double, VHEP)

HERWIG_SetGet_Imp(fHWPRAM, double, QCDLAM) 
HERWIG_SetGet_Imp(fHWPRAM, double, VQCUT) 
HERWIG_SetGet_Imp(fHWPRAM, double, VGCUT) 
HERWIG_SetGet_Imp(fHWPRAM, double, VPCUT) 
HERWIG_SetGet_Imp(fHWPRAM, double, CLMAX) 
HERWIG_SetGet_Imp(fHWPRAM, double, CLPOW) 
HERWIG_SetGetN_Imp(fHWPRAM, double, PSPLT) 
HERWIG_SetGet_Imp(fHWPRAM, double, QDIQK) 
HERWIG_SetGet_Imp(fHWPRAM, double, PDIQK) 
HERWIG_SetGet_Imp(fHWPRAM, double, QSPAC) 
HERWIG_SetGet_Imp(fHWPRAM, double, PTRMS) 

HERWIG_SetGet_Imp(fHWPRAM, int , IPRINT) 
HERWIG_SetGet_Imp(fHWEVNT, int , MAXPR) 
HERWIG_SetGet_Imp(fHWPRAM, bool ,PRVTX) 
HERWIG_SetGet_Imp(fHWPRAM, int , NPRFMT) 
HERWIG_SetGet_Imp(fHWPRAM, bool , PRNDEC) 
HERWIG_SetGet_Imp(fHWPRAM, bool , PRNDEF) 
HERWIG_SetGet_Imp(fHWPRAM, bool , PRNTEX) 
HERWIG_SetGet_Imp(fHWPRAM, bool , PRNWEB) 

HERWIG_SetGet_Imp(fHWEVNT, int , MAXER) 
HERWIG_SetGet_Imp(fHWEVNT, int , LWEVT) 
HERWIG_SetGet_Imp(fHWPRAM, int , LRSUD) 
HERWIG_SetGet_Imp(fHWPRAM, int , LWSUD) 

HERWIG_SetGet_Imp(fHWUSUD,  int ,   SUDORD) 
HERWIG_SetGet_Imp(fHWUSUD,  int ,   INTER) 
HERWIG_SetGetN_Imp(fHWEVNT, int ,   NRN) 
HERWIG_SetGet_Imp(fHWEVNT,  double, WGTMAX) 
HERWIG_SetGet_Imp(fHWEVNT,  bool,   NOWGT) 
HERWIG_SetGet_Imp(fHWEVNT,  double, AVWGT) 
HERWIG_SetGet_Imp(fHWPRAM,  double, EFFMIN)
 
HERWIG_SetGet_Imp(fHWPRAM,  bool , AZSOFT)
HERWIG_SetGet_Imp(fHWPRAM,  bool , AZSPIN) 
HERWIG_SetGet_Imp(fHWPRAM,  bool , HARDME) 
HERWIG_SetGet_Imp(fHWPRAM,  bool , SOFTME) 
HERWIG_SetGet_Imp(fHWPRAM,  double, GCUTME) 
HERWIG_SetGet_Imp(fHWPRAM,  int,    NCOLO) 
HERWIG_SetGet_Imp(fHWPRAM,  int,    NFLAV) 
HERWIG_SetGetN_Imp(fHWPRAM, int,    MODPDF) 
HERWIG_SetGet_Imp(fHWPRAM,  int,    NSTRU)
 
HERWIG_SetGet_Imp(fHWPRAM,  double,    PRSOF) 
HERWIG_SetGet_Imp(fHWPRAM,  double,    ENSOF) 

HERWIG_SetGet_Imp(fHWMINB,  double,    PMBN1) 
HERWIG_SetGet_Imp(fHWMINB,  double,    PMBN2) 
HERWIG_SetGet_Imp(fHWMINB,  double,    PMBN3) 
HERWIG_SetGet_Imp(fHWMINB,  double,    PMBK1) 
HERWIG_SetGet_Imp(fHWMINB,  double,    PMBK2) 
HERWIG_SetGet_Imp(fHWMINB,  double,    PMBM1) 
HERWIG_SetGet_Imp(fHWMINB,  double,    PMBM2) 
HERWIG_SetGet_Imp(fHWMINB,  double,    PMBP1) 
HERWIG_SetGet_Imp(fHWMINB,  double,    PMBP2) 
HERWIG_SetGet_Imp(fHWMINB,  double,    PMBP3) 

HERWIG_SetGet_Imp(fHWPRAM,  int,       IOPREM) 
HERWIG_SetGet_Imp(fHWPRAM,  double,    BTCLM) 
HERWIG_SetGet_Imp(fHWDIST,  double,    VMIN2) 
HERWIG_SetGet_Imp(fHWUCLU,  bool,      CLRECO) 
HERWIG_SetGet_Imp(fHWUCLU,  double,    PRECO) 
HERWIG_SetGet_Imp(fHWDIST,  double,    EXAG) 

HERWIG_SetGet_Imp(fHWPRAM,  double,    ETAMIX) 
HERWIG_SetGet_Imp(fHWPRAM,  double,    PHIMIX) 
HERWIG_SetGet_Imp(fHWPRAM,  double,    H1MIX) 
HERWIG_SetGet_Imp(fHWPRAM,  double,    F0MIX) 
HERWIG_SetGet_Imp(fHWPRAM,  double,    F1MIX) 
HERWIG_SetGet_Imp(fHWPRAM,  double,    F2MIX) 
HERWIG_SetGet_Imp(fHWPRAM,  double,    ET2MIX) 
HERWIG_SetGet_Imp(fHWPRAM,  double,    OMHMIX) 
HERWIG_SetGet_Imp(fHWPRAM,  double,    PH3MIX) 

HERWIG_SetGet_Imp(fHWPRAM,  double,    B1LIM) 
HERWIG_SetGetN_Imp(fHWPRAM, int,       CLDIR) 
HERWIG_SetGetN_Imp(fHWPRAM, double,    CLSMR) 

HERWIG_SetGetN_Imp(fHWUWTS, double,    PWT) 
HERWIG_SetGetNNN_Imp(fHWUWTS, double,  REPWT) 
HERWIG_SetGet_Imp(fHWUWTS, double,    SNGWT) 
HERWIG_SetGet_Imp(fHWUWTS, double,    DECWT) 

HERWIG_SetGet_Imp(fHWDIST, double,    PLTCUT) 
HERWIG_SetGetN_Imp(fHWPROP, bool,     VTOCDK) 
HERWIG_SetGet_Imp(fHWDIST,  bool,     PIPSMR) 

HERWIG_SetGetN_Imp(fHW6202, double,   VIPWID) 
HERWIG_SetGet_Imp(fHW6202,  double,   DXRCYL) 
HERWIG_SetGet_Imp(fHW6202,  double,   DXZMAX) 
HERWIG_SetGet_Imp(fHW6202,  double,   DXRSPH) 
HERWIG_SetGet_Imp(fHWDIST,  bool,     MAXDKL) 
HERWIG_SetGet_Imp(fHWDIST,  int,      IOPDKL) 
HERWIG_SetGet_Imp(fHWDIST,  bool,     MIXING) 
HERWIG_SetGetN_Imp(fHWDIST, double,     XMIX) 
HERWIG_SetGetN_Imp(fHWDIST, double,     YMIX) 

HERWIG_SetGet_Imp(fHWPRAM,  double,     GAMW) 
HERWIG_SetGet_Imp(fHWPRAM,  double,     GAMZ) 
HERWIG_SetGet_Imp(fHW6202,  bool,       WZRFR) 

HERWIG_SetGetN_Imp(fHWBOSC, int,        MODBOS) 
HERWIG_SetGet_Imp(fHWBOSC,  int,        IOPHIG) 
HERWIG_SetGet_Imp(fHWBOSC,  double,     GAMMAX) 
HERWIG_SetGetN_Imp(fHWBOSC, double,     ENHANC) 

HERWIG_SetGet_Imp(fHWPRAM,  double,     ALPHEM) 
HERWIG_SetGet_Imp(fHWPRAM,  double,     SWEIN) 
HERWIG_SetGetN_Imp(fHWPRAM, double,     QFCH) 
HERWIG_SetGetNN_Imp(fHWPRAM, double,    AFCH) 
HERWIG_SetGetNN_Imp(fHWPRAM, double,    VFCH) 
HERWIG_SetGet_Imp(fHWPRAM,   bool,      ZPRIME) 
HERWIG_SetGet_Imp(fHWPRAM,   bool,      GAMZP) 
HERWIG_SetGetNN_Imp(fHWPRAM, double,    VCKM) 
HERWIG_SetGet_Imp(fHWPRAM, double,    SCABI) 

HERWIG_SetGetN_Imp(fHWHARD, double,    EPOLN) 
HERWIG_SetGetN_Imp(fHWHARD, double,    PPOLN) 
HERWIG_SetGet_Imp(fHWHARD, double,     QLIM) 
HERWIG_SetGet_Imp(fHWHARD, double,     THMAX) 
HERWIG_SetGet_Imp(fHWHARD, double,     Y4JT) 
HERWIG_SetGet_Imp(fHWHARD, bool,     DURHAM) 
HERWIG_SetGetN_Imp(fHWPRAM, int,     IOP4JT) 
HERWIG_SetGet_Imp(fHWHARD, bool,    BGSHAT) 

HERWIG_SetGet_Imp(fHWBRCH, bool,    BREIT) 
HERWIG_SetGet_Imp(fHWBRCH, bool,    USECMF) 

HERWIG_SetGet_Imp(fHWPRAM, bool,    NOSPAC) 
HERWIG_SetGet_Imp(fHWPRAM, bool,    ISPAC) 

HERWIG_SetGet_Imp(fHWHARD, double,   TMNISR) 
HERWIG_SetGet_Imp(fHWHARD, double,   ZMXISR) 
HERWIG_SetGet_Imp(fHWHARD, double,   ASFIXD) 
HERWIG_SetGet_Imp(fHWHARD, double,   OMEGA0) 
HERWIG_SetGet_Imp(fHWHARD, int,      IAPHIG) 
HERWIG_SetGet_Imp(fHWHARD, double,   PHOMAS) 
HERWIG_SetGet_Imp(fHWHARD, double,   PTMIN) 
HERWIG_SetGet_Imp(fHWHARD, double,   PTMAX) 
HERWIG_SetGet_Imp(fHWHARD, double,   PTPOW) 
HERWIG_SetGet_Imp(fHWHARD, double,   YJMIN) 
HERWIG_SetGet_Imp(fHWHARD, double,   YJMAX) 
HERWIG_SetGet_Imp(fHWHARD, double,   EMMIN) 
HERWIG_SetGet_Imp(fHWHARD, double,   EMMAX) 
HERWIG_SetGet_Imp(fHWHARD, double,   EMPOW) 
HERWIG_SetGet_Imp(fHWHARD, double,   Q2MIN) 
HERWIG_SetGet_Imp(fHWHARD, double,   Q2MAX) 
HERWIG_SetGet_Imp(fHWHARD, double,   Q2POW) 
HERWIG_SetGet_Imp(fHWHARD, double,   YBMIN) 
HERWIG_SetGet_Imp(fHWHARD, double,   YBMAX) 
HERWIG_SetGet_Imp(fHWHARD, double,   WHMIN) 
HERWIG_SetGet_Imp(fHWHARD, double,   ZJMAX) 
HERWIG_SetGet_Imp(fHWHARD, double,   Q2WWMN) 
HERWIG_SetGet_Imp(fHWHARD, double,   Q2WWMX) 
HERWIG_SetGet_Imp(fHWHARD, double,   YWWMIN) 
HERWIG_SetGet_Imp(fHWHARD, double,   YWWMAX) 

HERWIG_SetGet_Imp(fHWPRAM, double,   CSPEED) 
HERWIG_SetGet_Imp(fHWPRAM, double,   GEV2NB) 

HERWIG_SetGet_Imp(fHWHARD, int,   IBSH) 
HERWIG_SetGetN_Imp(fHWHARD, int,   IBRN) 

HERWIG_SetGet_Imp(fHWUSUD, int,   NQEV) 
HERWIG_SetGet_Imp(fHWPRAM, double,  ZBINM) 
HERWIG_SetGet_Imp(fHWPRAM, int,     NZBIN) 

HERWIG_SetGet_Imp(fHWPRAM, int,     NBTRY) 
HERWIG_SetGet_Imp(fHWPRAM, int,     NCTRY) 
HERWIG_SetGet_Imp(fHWPRAM, int,     NETRY) 
HERWIG_SetGet_Imp(fHWPRAM, int,     NSTRY) 

HERWIG_SetGet_Imp(fHWUSUD, double,   ACCUR)


  // HERWIG_SetGet_Imp(hwusu_, bool,   RPARTY)
  // HERWIG_SetGet_Imp(hwusu_, bool,   SUSYIN)
  // HERWIG_SetGet_Imp(hwusu_, int,  LRSUSY)
 


