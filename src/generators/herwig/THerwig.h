// $Id$

#ifndef __THerwig__
#define __THerwig__

// ////////////////////////////////////////////////////////////////
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
// /////////////////////////////////////////////////////////////////

#if 1
#include "TObjArray.h"
#endif
#ifndef ROOT_TGenerator
#include "TGenerator.h"
#endif

// include "HEPEVT.h"
#ifndef __HEPEVT_NMXHEP__
#define __HEPEVT_NMXHEP__ 4000

typedef struct {
  int NEVHEP,NHEP,ISTHEP[__HEPEVT_NMXHEP__],IDHEP[__HEPEVT_NMXHEP__];
  int JMOHEP[__HEPEVT_NMXHEP__][2],JDAHEP[__HEPEVT_NMXHEP__][2];
  double PHEP[__HEPEVT_NMXHEP__][5],VHEP[__HEPEVT_NMXHEP__][4];
} COMMON_HEPEVT_t ;

#endif

#include "THerwigCommons.h"
#include "THerwigMacros.h"

class THerwigParticle : public TObject {
 friend class THerwig;
 protected:
  char fName[12];  // Particle name
  int  fPdgCode;   // PDG code
  int  fFlavourCode;  // Herwig Flavour code
  int  fICharge;       // Electric charge (|e-|)  (*3 for (di-)quarks)
  double fMass;       // Mass (GeV/c^2)
  double fLifeTime;   // Proper life time (s)
  double fSpin;       // Spin
  bool   fQORQQB;     //  .TRUE. if it is a quark or an antidiquark
  bool   fQBORQQ;     //  .TRUE. if it is an antiquark or a diquark

 public:
  THerwigParticle();
  THerwigParticle(const THerwigParticle &part);
  virtual ~THerwigParticle(){}
  
  inline char *GetName(){ return fName; }
  inline int   GetPdgCode(){ return fPdgCode; }
  inline int   GetFlaourCode(){ return fFlavourCode; }
  inline int   GetICharge(){ return fICharge;}
  inline double GetMass(){ return fMass;}
  inline double GetTrueCharge(){ 
    return ( fPdgCode > -10 && fPdgCode < 10 ? ((double)fICharge)/3.0 : fICharge ) ;
  }
  inline double GetLifeTime(){ return fLifeTime; }
  inline double GetCtau(){ return fLifeTime*2.99792458e10; }
  inline double GetSpin(){ return fSpin; }
  inline bool   GetQORQQB(){ return fQORQQB; }
  inline bool   GetQBORQQ(){ return fQBORQQ; }

  ClassDef(THerwigParticle, 0)   // Herwig's particle data
};


//--------------------------------------------------
class THerwig : public TGenerator {
 protected:
  
  COMMON_HEPEVT_t *fHEPEVT; //! Address of HWEVNT common

  COMMON_HWEVNT_t *fHWEVNT; //! Address of HWEVNT common
  COMMON_HWBEAM_t *fHWBEAM; //! Address of HWBEAM common
  COMMON_HWBMCH_t *fHWBMCH; //! Address of HWBMCH common
  COMMON_HWPROC_t *fHWPROC; //! Address of HWPROC common
  COMMON_HWPRAM_t *fHWPRAM; //! Address of HWPRAM common
  COMMON_HWPRCH_t *fHWPRCH; //! Address of HWPRCH common
  COMMON_HWPART_t *fHWPART; //! Address of HWPART common
  COMMON_HWPARP_t *fHWPARP; //! Address of HWPARP common
  COMMON_HWBOSC_t *fHWBOSC; //! Address of HWBOSC common
  COMMON_HWPARC_t *fHWPARC; //! Address of HWPARC common
  COMMON_HWBRCH_t *fHWBRCH; //! Address of HWBRCH common
  COMMON_HWPROP_t *fHWPROP; //! Address of HWPROP common
  COMMON_HWUNAM_t *fHWUNAM; //! Address of HWUNAM common
  COMMON_HWUSUD_t *fHWUSUD; //! Address of HWUSUD common
  COMMON_HWMINB_t *fHWMINB; //! Address of HWMINB common
  COMMON_HWDIST_t *fHWDIST; //! Address of HWDIST common
  COMMON_HWUCLU_t *fHWUCLU; //! Address of HWUCLU common
  COMMON_HWUWTS_t *fHWUWTS; //! Address of HWUWTS common
  COMMON_HW6202_t *fHW6202; //! Address of HW6202 common
  COMMON_HWHARD_t *fHWHARD; //! Address of HWHARD common

 public:
  THerwig();
  virtual ~THerwig();

  void SetBeams(const Char_t *part1, const Char_t *part2,
		const Double_t pbeam1, const Double_t pbeam2, 
		const Int_t process);

  THerwigParticle &GetParticleProperties(int pdgcode);
  void Initialize();
  void GenerateEvent();
  void Terminate();

  inline TClonesArray *GetPrimaries(){ return (TClonesArray*)fParticles; }
  inline Int_t GetNumberOfPrimaries(){ return fParticles->GetEntries(); }


  COMMON_HEPEVT_t *GetHEPEVT(){ return fHEPEVT; }
  COMMON_HWEVNT_t *GetHWEVNT(){ return fHWEVNT; } 
  COMMON_HWBEAM_t *GetHWBEAM(){ return fHWBEAM; } 
  COMMON_HWBMCH_t *GetHWBMCH(){ return fHWBMCH; } 
  COMMON_HWPROC_t *GetHWPROC(){ return fHWPROC; } 
  COMMON_HWPRAM_t *GetHWPRAM(){ return fHWPRAM; } 
  COMMON_HWPRCH_t *GetHWPRCH(){ return fHWPRCH; } 
  COMMON_HWPART_t *GetHWPART(){ return fHWPART; } 
  COMMON_HWPARP_t *GetHWPARP(){ return fHWPARP; } 
  COMMON_HWBOSC_t *GetHWBOSC(){ return fHWBOSC; } 
  COMMON_HWPARC_t *GetHWPARC(){ return fHWPARC; } 
  COMMON_HWBRCH_t *GetHWBRCH(){ return fHWBRCH; } 
  COMMON_HWPROP_t *GetHWPROP(){ return fHWPROP; } 
  COMMON_HWUNAM_t *GetHWUNAM(){ return fHWUNAM; } 
  COMMON_HWUSUD_t *GetHWUSUD(){ return fHWUSUD; } 
  COMMON_HWMINB_t *GetHWMINB(){ return fHWMINB; } 
  COMMON_HWDIST_t *GetHWDIST(){ return fHWDIST; } 
  COMMON_HWUCLU_t *GetHWUCLU(){ return fHWUCLU; } 
  COMMON_HWUWTS_t *GetHWUWTS(){ return fHWUWTS; } 
  COMMON_HW6202_t *GetHW6202(){ return fHW6202; } 
  COMMON_HWHARD_t *GetHWHARD(){ return fHWHARD; } 

  
  void SetBDECAY(char *bdecay);
  char *GetBDECAY();
  void SetAUTPDF(int i, char *bdecay);
  char *GetAUTPDF(int i);

  HERWIG_SetGet_Def(int, NEVHEP) 
  HERWIG_SetGet_Def(int, NHEP) 
  HERWIG_SetGetN_Def(int, ISTHEP) 
  HERWIG_SetGetN_Def(int, IDHEP) 
  HERWIG_SetGetNN_Def(int, JMOHEP)
  HERWIG_SetGetNN_Def(int, JDAHEP)
  HERWIG_SetGetNN_Def(double, PHEP)
  HERWIG_SetGetNN_Def(double, VHEP)
 
  HERWIG_SetGetN_Def(double, RMASS) 

  HERWIG_SetGet_Def(double, QCDLAM) 
  HERWIG_SetGet_Def(double, VQCUT) 
  HERWIG_SetGet_Def(double, VGCUT) 
  HERWIG_SetGet_Def(double, VPCUT) 
  HERWIG_SetGet_Def(double, CLMAX) 
  HERWIG_SetGet_Def(double, CLPOW) 
  HERWIG_SetGetN_Def(double, PSPLT) 
  HERWIG_SetGet_Def(double, QDIQK) 
  HERWIG_SetGet_Def(double, PDIQK) 
  HERWIG_SetGet_Def(double, QSPAC) 
  HERWIG_SetGet_Def(double, PTRMS) 

  HERWIG_SetGet_Def(int , IPRINT) 
  HERWIG_SetGet_Def(int , MAXPR) 
  HERWIG_SetGet_Def(bool ,PRVTX) 
  HERWIG_SetGet_Def(int , NPRFMT) 
  HERWIG_SetGet_Def(bool , PRNDEC) 
  HERWIG_SetGet_Def(bool , PRNDEF) 
  HERWIG_SetGet_Def(bool , PRNTEX) 
  HERWIG_SetGet_Def(bool , PRNWEB) 
  
  
  HERWIG_SetGet_Def(int , MAXER) 
  HERWIG_SetGet_Def(int , LWEVT) 
  HERWIG_SetGet_Def(int , LRSUD) 
  HERWIG_SetGet_Def(int , LWSUD) 
  
  HERWIG_SetGet_Def(int ,   SUDORD) 
  HERWIG_SetGet_Def(int ,   INTER) 
  HERWIG_SetGetN_Def(int ,   NRN) 
  HERWIG_SetGet_Def(double, WGTMAX) 
  HERWIG_SetGet_Def(bool,   NOWGT) 
  HERWIG_SetGet_Def(double, AVWGT) 
  HERWIG_SetGet_Def(double, EFFMIN)
   
  HERWIG_SetGet_Def(bool , AZSOFT)
  HERWIG_SetGet_Def(bool , AZSPIN) 
  HERWIG_SetGet_Def(bool , HARDME) 
  HERWIG_SetGet_Def(bool , SOFTME) 
  HERWIG_SetGet_Def(double, GCUTME) 
  HERWIG_SetGet_Def(int,    NCOLO) 
  HERWIG_SetGet_Def(int,    NFLAV) 
  HERWIG_SetGetN_Def(int,   MODPDF) 
  HERWIG_SetGet_Def(int,    NSTRU)
   
  HERWIG_SetGet_Def(double,    PRSOF) 
  HERWIG_SetGet_Def(double,    ENSOF) 
  
  HERWIG_SetGet_Def(double,    PMBN1) 
  HERWIG_SetGet_Def(double,    PMBN2) 
  HERWIG_SetGet_Def(double,    PMBN3) 
  HERWIG_SetGet_Def(double,    PMBK1) 
  HERWIG_SetGet_Def(double,    PMBK2) 
  HERWIG_SetGet_Def(double,    PMBM1) 
  HERWIG_SetGet_Def(double,    PMBM2) 
  HERWIG_SetGet_Def(double,    PMBP1) 
  HERWIG_SetGet_Def(double,    PMBP2) 
  HERWIG_SetGet_Def(double,    PMBP3) 
  
  HERWIG_SetGet_Def(int,       IOPREM) 
  HERWIG_SetGet_Def(double,    BTCLM) 
  HERWIG_SetGet_Def(double,    VMIN2) 
  HERWIG_SetGet_Def(bool,      CLRECO) 
  HERWIG_SetGet_Def(double,    PRECO) 
  HERWIG_SetGet_Def(double,    EXAG) 
  
  HERWIG_SetGet_Def(double,    ETAMIX) 
  HERWIG_SetGet_Def(double,    PHIMIX) 
  HERWIG_SetGet_Def(double,    H1MIX) 
  HERWIG_SetGet_Def(double,    F0MIX) 
  HERWIG_SetGet_Def(double,    F1MIX) 
  HERWIG_SetGet_Def(double,    F2MIX) 
  HERWIG_SetGet_Def(double,    ET2MIX) 
  HERWIG_SetGet_Def(double,    OMHMIX) 
  HERWIG_SetGet_Def(double,    PH3MIX) 
  
  HERWIG_SetGet_Def(double,    B1LIM) 
  HERWIG_SetGetN_Def(int,       CLDIR) 
  HERWIG_SetGetN_Def(double,    CLSMR) 
  
  HERWIG_SetGetN_Def(double,    PWT) 
  HERWIG_SetGetNNN_Def(double,  REPWT) 
  HERWIG_SetGet_Def(double,    SNGWT) 
  HERWIG_SetGet_Def(double,    DECWT) 
  
  HERWIG_SetGet_Def(double,    PLTCUT) 
  HERWIG_SetGetN_Def(bool,     VTOCDK) 
  HERWIG_SetGet_Def(bool,     PIPSMR) 
  
  HERWIG_SetGetN_Def(double,   VIPWID) 
  HERWIG_SetGet_Def(double,   DXRCYL) 
  HERWIG_SetGet_Def(double,   DXZMAX) 
  HERWIG_SetGet_Def(double,   DXRSPH) 
  HERWIG_SetGet_Def(bool,     MAXDKL) 
  HERWIG_SetGet_Def(int,      IOPDKL) 
  HERWIG_SetGet_Def(bool,     MIXING) 
  HERWIG_SetGetN_Def(double,     XMIX) 
  HERWIG_SetGetN_Def(double,     YMIX) 
  
  HERWIG_SetGet_Def(double,     GAMW) 
  HERWIG_SetGet_Def(double,     GAMZ) 
  HERWIG_SetGet_Def(bool,       WZRFR) 
  
  HERWIG_SetGetN_Def(int,        MODBOS) 
  HERWIG_SetGet_Def(int,        IOPHIG) 
  HERWIG_SetGet_Def(double,     GAMMAX) 
  HERWIG_SetGetN_Def(double,     ENHANC) 
  
  HERWIG_SetGet_Def(double,     ALPHEM) 
  HERWIG_SetGet_Def(double,     SWEIN) 
  HERWIG_SetGetN_Def(double,     QFCH) 
  HERWIG_SetGetNN_Def(double,    AFCH) 
  HERWIG_SetGetNN_Def(double,    VFCH) 
  HERWIG_SetGet_Def(bool,      ZPRIME) 
  HERWIG_SetGet_Def(bool,      GAMZP) 
  HERWIG_SetGetNN_Def(double,    VCKM) 
  HERWIG_SetGet_Def(double,    SCABI) 
  
  HERWIG_SetGetN_Def(double,    EPOLN) 
  HERWIG_SetGetN_Def(double,    PPOLN) 
  HERWIG_SetGet_Def(double,     QLIM) 
  HERWIG_SetGet_Def(double,     THMAX) 
  HERWIG_SetGet_Def(double,     Y4JT) 
  HERWIG_SetGet_Def(bool,     DURHAM) 
  HERWIG_SetGetN_Def(int,     IOP4JT) 
  HERWIG_SetGet_Def(bool,    BGSHAT) 
  
  HERWIG_SetGet_Def(bool,    BREIT) 
  HERWIG_SetGet_Def(bool,    USECMF) 
  
  HERWIG_SetGet_Def(bool,    NOSPAC) 
  HERWIG_SetGet_Def(bool,    ISPAC) 
  
  HERWIG_SetGet_Def(double,   TMNISR) 
  HERWIG_SetGet_Def(double,   ZMXISR) 
  HERWIG_SetGet_Def(double,   ASFIXD) 
  HERWIG_SetGet_Def(double,   OMEGA0) 
  HERWIG_SetGet_Def(int,      IAPHIG) 
  HERWIG_SetGet_Def(double,   PHOMAS) 
  HERWIG_SetGet_Def(double,   PTMIN) 
  HERWIG_SetGet_Def(double,   PTMAX) 
  HERWIG_SetGet_Def(double,   PTPOW) 
  HERWIG_SetGet_Def(double,   YJMIN) 
  HERWIG_SetGet_Def(double,   YJMAX) 
  HERWIG_SetGet_Def(double,   EMMIN) 
  HERWIG_SetGet_Def(double,   EMMAX) 
  HERWIG_SetGet_Def(double,   EMPOW) 
  HERWIG_SetGet_Def(double,   Q2MIN) 
  HERWIG_SetGet_Def(double,   Q2MAX) 
  HERWIG_SetGet_Def(double,   Q2POW) 
  HERWIG_SetGet_Def(double,   YBMIN) 
  HERWIG_SetGet_Def(double,   YBMAX) 
  HERWIG_SetGet_Def(double,   WHMIN) 
  HERWIG_SetGet_Def(double,   ZJMAX) 
  HERWIG_SetGet_Def(double,   Q2WWMN) 
  HERWIG_SetGet_Def(double,   Q2WWMX) 
  HERWIG_SetGet_Def(double,   YWWMIN) 
  HERWIG_SetGet_Def(double,   YWWMAX) 
  
  HERWIG_SetGet_Def(double,   CSPEED) 
  HERWIG_SetGet_Def(double,   GEV2NB) 
  
  HERWIG_SetGet_Def(int,   IBSH) 
  HERWIG_SetGetN_Def(int,   IBRN) 
  
  HERWIG_SetGet_Def(int,   NQEV) 
  HERWIG_SetGet_Def(double,  ZBINM) 
  HERWIG_SetGet_Def(int,     NZBIN) 
  
  HERWIG_SetGet_Def(int,     NBTRY) 
  HERWIG_SetGet_Def(int,     NCTRY) 
  HERWIG_SetGet_Def(int,     NETRY) 
  HERWIG_SetGet_Def(int,     NSTRY) 
  
  HERWIG_SetGet_Def(double,   ACCUR)
  
  ClassDef(THerwig, 1)  // Interface to Herwig generator

};

#endif

  
