//$Id$ 
// --------------------------------------------------
// Commons used by Herwig generator
// --------------------------------------------------
#ifndef __THERWIG_COMMONS__
#define __THERWIG_COMMONS__

//C
//C Beams, process and number of events
typedef struct {
  int IPART1, IPART2;
} COMMON_HWBEAM_t ;
//extern COMMON_HWBEAM_t hwbeam_;

typedef struct {
  char PART1[8], PART2[8];
} COMMON_HWBMCH_t ;
//extern COMMON_HWBMCH_t hwbmch_;

typedef struct {
  double EBEAM1, EBEAM2, PBEAM1, PBEAM2;
  int    IPROC, MAXEV;
} COMMON_HWPROC_t ;
//extern COMMON_HWPROC_t hwproc_;


//C
//C Basic parameters (and quantities derived from them)
typedef struct {
  double AFCH[2][16];
  double ALPHEM, B1LIM, BETAF, BTCLM, CAFAC, CFFAC;
  double CLMAX,CLPOW,CLSMR[2],CSPEED,ENSOF,ETAMIX,F0MIX,F1MIX,F2MIX,GAMH;
  double GAMW,GAMZ,GAMZP,GEV2NB,H1MIX,PDIQK,PGSMX,PGSPL[4],PHIMIX,PIFAC;
  double PRSOF,PSPLT[2],PTRMS,PXRMS,QCDL3,QCDL5,QCDLAM,QDIQK,QFCH[16],QG;
  double QSPAC,QV,SCABI,SWEIN,TMTOP,VFCH[2][16],VCKM[3][3],VGCUT,VQCUT;
  double VPCUT,ZBINM,EFFMIN,OMHMIX,ET2MIX,PH3MIX,GCUTME;
  int    IOPREM,IPRINT,ISPAC,LRSUD,LWSUD,MODPDF[2],NBTRY,NCOLO,NCTRY;
  int    NDTRY,NETRY,NFLAV,NGSPL,NSTRU,NSTRY,NZBIN,IOP4JT[2],NPRFMT;
  bool   AZSOFT,AZSPIN;
  int    CLDIR[2];
  bool   HARDME,NOSPAC,PRNDEC,PRVTX,SOFTME,ZPRIME;
  bool   PRNDEF,PRNTEX,PRNWEB;
} COMMON_HWPRAM_t ;
//extern COMMON_HWPRAM_t hwpram_;

//C
typedef struct {
  char AUTPDF[2][20];
  char BDECAY[4];
} COMMON_HWPRCH_t;
//extern COMMON_HWPRCH_t hwprch_;

//C
//C New standard event common
//      PARAMETER (NMXHEP=4000)
//      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
//     & JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
// extern COMMON_HEPEVT_t hepevt_;

//C
//C Parton shower common (same format as /HEPEVT/)
//      PARAMETER (NMXPAR=500)
//      COMMON/HWPART/NEVPAR,NPAR,ISTPAR(NMXPAR),IDPAR(NMXPAR),
//     & JMOPAR(2,NMXPAR),JDAPAR(2,NMXPAR),PPAR(5,NMXPAR),VPAR(4,NMXPAR)
#define __HERWIG_NMXPAR__ 500
typedef struct {
  int    NEVPAR,NPAR,ISTPAR[__HERWIG_NMXPAR__],IDPAR[__HERWIG_NMXPAR__];
  int    JMOPAR[__HERWIG_NMXPAR__][2],JDAPAR[__HERWIG_NMXPAR__][2];
  double PPAR[__HERWIG_NMXPAR__][5], VPAR[__HERWIG_NMXPAR__][4];
} COMMON_HWPART_t;
//extern COMMON_HWPART_t hwpart_;

//C
//C Parton polarization common
//      COMMON/HWPARP/DECPAR(2,NMXPAR),PHIPAR(2,NMXPAR),RHOPAR(2,NMXPAR),
//     & TMPAR(NMXPAR)
typedef struct {
  double DECPAR[__HERWIG_NMXPAR__][2], PHIPAR[__HERWIG_NMXPAR__][2];
  double RHOPAR[__HERWIG_NMXPAR__][2];
  bool   TMPAR[__HERWIG_NMXPAR__];
} COMMON_HWPARP_t;
//extern COMMON_HWPARP_t hwparp_;

//C
//C Electroweak boson common
//      PARAMETER (MODMAX=5)
//      COMMON/HWBOSC/ALPFAC,BRHIG(12),ENHANC(12),GAMMAX,RHOHEP(3,NMXHEP),
//     & IOPHIG,MODBOS(MODMAX)
//      PARAMETER (NMXHEP=4000)
#define __HERWIG_NMXHEP__ 4000
#define __HERWIG_MODMAX__ 5
typedef struct {
  double ALPFAC,BRHIG[12],ENHANC[12],GAMMAX,RHOHEP[__HERWIG_NMXHEP__][3];
  int    IOPHIG,MODBOS[__HERWIG_MODMAX__];
} COMMON_HWBOSC_t;
//extern COMMON_HWBOSC_t hwbosc_;

//C
//C Parton colour common
//      COMMON/HWPARC/JCOPAR(4,NMXPAR)
typedef struct {
  int JCOPAR[__HERWIG_NMXPAR__][4];
} COMMON_HWPARC_t;
//extern COMMON_HWPARC_t hwparc_;


//C other HERWIG branching, event and hard subprocess common blocks
//      COMMON/HWBRCH/ANOMSC(2,2),HARDST,PTINT(3,2),XFACT,INHAD,JNHAD,
//     & NSPAC(7),ISLENT,BREIT,FROST,USECMF
//C
typedef struct {
  double ANOMSC[2][2], HARDST,PTINT[2][3],XFACT;
  int    INHAD,JNHAD;
  int    NSPAC[7],ISLENT;
  bool   BREIT,FROST,USECMF;
} COMMON_HWBRCH_t;
//extern COMMON_HWBRCH_t hwbrch_;

//      COMMON/HWEVNT/AVWGT,EVWGT,GAMWT,TLOUT,WBIGST,WGTMAX,WGTSUM,WSQSUM,
//     & IDHW(NMXHEP),IERROR,ISTAT,LWEVT,MAXER,MAXPR,NOWGT,NRN(2),NUMER,
//     & NUMERU,NWGTS,GENSOF
//C
typedef struct {
  double AVWGT,EVWGT,GAMWT,TLOUT,WBIGST,WGTMAX,WGTSUM,WSQSUM;
  int    IDHW[__HERWIG_NMXHEP__],IERROR,ISTAT,LWEVT,MAXER,MAXPR,NOWGT,NRN[2],NUMER;
  int    NUMERU, NWGTS;
  bool   GENSOF;
} COMMON_HWEVNT_t;
// extern COMMON_HWEVNT_t hwevnt_;


//C
//C Arrays for particle properties (NMXRES = max no of particles defined)
//      PARAMETER(NMXRES=500)
//      COMMON/HWPROP/RLTIM(0:NMXRES),RMASS(0:NMXRES),RSPIN(0:NMXRES),
//     & ICHRG(0:NMXRES),IDPDG(0:NMXRES),IFLAV(0:NMXRES),NRES,
//     & VTOCDK(0:NMXRES),VTORDK(0:NMXRES),
//     & QORQQB(0:NMXRES),QBORQQ(0:NMXRES)
//C
//      COMMON/HWUNAM/RNAME(0:NMXRES),TXNAME(2,0:NMXRES)
#define __HERWIG_NMXRES__ 500
#define __HERWIG_NMXRES_1__ __HERWIG_NMXRES__+1
typedef struct {
  double RLTIM[__HERWIG_NMXRES_1__], RMASS[__HERWIG_NMXRES_1__];
  double RSPIN[__HERWIG_NMXRES_1__];
  int    ICHRG[__HERWIG_NMXRES_1__], IDPDG[__HERWIG_NMXRES_1__];
  int    IFLAV[__HERWIG_NMXRES_1__], NRES;
  bool   VTOCDK[__HERWIG_NMXRES_1__], VTORDK[__HERWIG_NMXRES_1__];
  bool   QORQQB[__HERWIG_NMXRES_1__], QBORQQ[__HERWIG_NMXRES_1__];
} COMMON_HWPROP_t;
//extern COMMON_HWPROP_t hwprop_;

typedef struct {
  char RNAME[__HERWIG_NMXRES_1__][8];
  char TXNAME[__HERWIG_NMXRES_1__][2][37];
} COMMON_HWUNAM_t;
//extern COMMON_HWUNAM_t hwunam_;

//C
//C Parameters for Sudakov form factors
//C (NMXSUD= max no of entries in lookup table)
//      PARAMETER (NMXSUD=1024)
//      COMMON/HWUSUD/ACCUR,QEV(NMXSUD,6),SUD(NMXSUD,6),INTER,NQEV,NSUD,
//     & SUDORD
#define __HERWIG_NMXSUD__ 1024
typedef struct {
  double ACCUR,QEV[6][__HERWIG_NMXSUD__];
  double SUD[6][__HERWIG_NMXSUD__];
  int    INTER,NQEV,NSUD,SUDORD;
} COMMON_HWUSUD_t;
//extern COMMON_HWUSUD_t hwusud_;


//C
//C Parameters for minimum bias/soft underlying event
//      COMMON/HWMINB/
//     & PMBN1,PMBN2,PMBN3,PMBK1,PMBK2,PMBM1,PMBM2,PMBP1,PMBP2,PMBP3
typedef struct {
  double PMBN1,PMBN2,PMBN3,PMBK1,PMBK2,PMBM1,PMBM2,PMBP1,PMBP2,PMBP3;
} COMMON_HWMINB_t;
//extern COMMON_HWMINB_t hwminb_;

//C--VTXPIP should have been a 5-vector, problems with NAG compiler
//      COMMON/HWDIST/EXAG,GEV2MM,HBAR,PLTCUT,VMIN2,VTXPIP(5),XMIX(2),
//     & XMRCT(2),YMIX(2),YMRCT(2),IOPDKL,MAXDKL,MIXING,PIPSMR
typedef struct {
  double EXAG,GEV2MM,HBAR,PLTCUT,VMIN2,VTXPIP[5],XMIX[2];
  double XMRCT[2],YMIX[2],YMRCT[2];
  int    IOPDKL;
  bool   MAXDKL,MIXING,PIPSMR;
} COMMON_HWDIST_t ;
//extern COMMON_HWDIST_t hwdist_;



//C
//C Parameters for cluster decays (NMXCDK = max total no of cluster
//C                                         decay channels)
//      PARAMETER(NMXCDK=4000)
//      COMMON/HWUCLU/CLDKWT(NMXCDK),CTHRPW(12,12),PRECO,RESN(12,12),
//     & RMIN(12,12),LOCN(12,12),NCLDK(NMXCDK),NRECO,CLRECO
#define __HERWIG_NMXCDK__ 4000
typedef struct {
   double CLDKWT[__HERWIG_NMXCDK__],CTHRPW[12][12];
   double PRECO,RESN[12][12];
   double RMIN[12][12];
   int    LOCN[12][12],NCLDK[__HERWIG_NMXCDK__],NRECO;
   bool   CLRECO;
} COMMON_HWUCLU_t;
//extern COMMON_HWUCLU_t hwuclu_;

//C
//C Weights used in cluster decays
//      COMMON/HWUWTS/REPWT(0:3,0:4,0:4),SNGWT,DECWT,QWT(3),PWT(12),
//     & SWTEF(NMXRES)
typedef struct {
  double REPWT[5][5][4],SNGWT,DECWT,QWT[3],PWT[12];
  double SWTEF[__HERWIG_NMXRES__];
} COMMON_HWUWTS_t;
//extern COMMON_HWUWTS_t hwuwts_;

//C
//C  Other new parameters for version 6.2
//      DOUBLE PRECISION VIPWID,DXRCYL,DXZMAX,DXRSPH
//      LOGICAL WZRFR,FIX4JT
//      INTEGER IMSSM,IHIGGS,PARITY,LRSUSY
//      COMMON/HW6202/VIPWID(3),DXRCYL,DXZMAX,DXRSPH,WZRFR,FIX4JT,
//     & IMSSM,IHIGGS,PARITY,LRSUSY
typedef struct {
  double VIPWID[3],DXRCYL,DXZMAX,DXRSPH;
  bool   WZRFR,FIX4JT;
  int    IMSSM,IHIGGS,PARITY,LRSUSY;
} COMMON_HW6202_t;
//extern COMMON_HW6202_t hw6202_;

//C
//      COMMON/HWHARD/ASFIXD,CLQ(7,6),COSS,COSTH,CTMAX,DISF(13,2),EMLST,
//     & EMMAX,EMMIN,EMPOW,EMSCA,EPOLN(3),GCOEF(7),GPOLN,OMEGA0,PHOMAS,
//     & PPOLN(3),PTMAX,PTMIN,PTPOW,Q2MAX,Q2MIN,Q2POW,Q2WWMN,Q2WWMX,QLIM,
//     & SINS,THMAX,Y4JT,TMNISR,TQWT,XX(2),XLMIN,XXMIN,YBMAX,YBMIN,YJMAX,
//     & YJMIN,YWWMAX,YWWMIN,WHMIN,ZJMAX,ZMXISR,IAPHIG,IBRN(2),IBSH,
//     & ICO(10),IDCMF,IDN(10),IFLMAX,IFLMIN,IHPRO,IPRO,MAPQ(6),MAXFL,
//     & BGSHAT,COLISR,FSTEVT,FSTWGT,GENEV,HVFCEN,TPOL,DURHAM
typedef struct {
  double ASFIXD,CLQ[6][7],COSS,COSTH,CTMAX,DISF[2][13],EMLST;
  double EMMAX,EMMIN,EMPOW,EMSCA,EPOLN[3],GCOEF[7],GPOLN,OMEGA0,PHOMAS;
  double PPOLN[3],PTMAX,PTMIN,PTPOW,Q2MAX,Q2MIN,Q2POW,Q2WWMN,Q2WWMX,QLIM;
  double SINS,THMAX,Y4JT,TMNISR,TQWT,XX[2],XLMIN,XXMIN,YBMAX,YBMIN,YJMAX;
  double YJMIN,YWWMAX,YWWMIN,WHMIN,ZJMAX,ZMXISR;
  int    IAPHIG,IBRN[2],IBSH;
  int    ICO[10],IDCMF,IDN[10],IFLMAX,IFLMIN,IHPRO,IPRO,MAPQ[6],MAXFL;
  bool   BGSHAT,COLISR,FSTEVT,FSTWGT,GENEV,HVFCEN,TPOL,DURHAM;
} COMMON_HWHARD_t;
//extern COMMON_HWHARD_t hwhard_;

/*  

C
      LOGICAL
     & AZSOFT,AZSPIN,BGSHAT,BREIT,CLRECO,COLISR,DKPSET,FROST,FSTEVT,
     & FSTWGT,GENEV,GENSOF,HARDME,HVFCEN,MAXDKL,MIXING,NOSPAC,NOWGT,
     & PRNDEC,PIPSMR,PRVTX,RSTAB,SOFTME,TMPAR,TPOL,USECMF,VTOCDK,VTORDK,
     & ZPRIME,RPARTY,COLUPD,PRNDEF,PRNTEX,PRNWEB,DURHAM,SUSYIN,
     & QORQQB,QBORQQ
C
      INTEGER
     & CLDIR,IAPHIG,IBRN,IBSH,ICHRG,ICO,IDCMF,IDHEP,IDHW,IDK,IDKPRD,IDN,
     & IDPAR,IDPDG,IERROR,IFLAV,IFLMAX,IFLMIN,IHPRO,IMQDK,INHAD,INTER,
     & IOPDKL,IOPHIG,IOPREM,IPART1,IPART2,IPRINT,IPRO,IPROC,ISLENT,
     & ISPAC,ISTAT,ISTHEP,ISTPAR,JCOPAR,JDAHEP,JDAPAR,JMOHEP,JMOPAR,
     & JNHAD,LNEXT,LOCN,LOCQ,LRSUD,LSTRT,LWEVT,LWSUD,MAPQ,MAXER,MAXEV,
     & MAXFL,MAXPR,MODBOS,MODMAX,MODPDF,NBTRY,NCLDK,NCOLO,NCTRY,NDKYS,
     & NDTRY,NETRY,NEVHEP,NEVPAR,NFLAV,NGSPL,NHEP,NME,NMODES,NMXCDK,
     & NMXDKS,NMXHEP,NMXJET,NMXMOD,NMXPAR,NMXQDK,NMXRES,NMXSUD,NPAR,
     & NPRODS,NQDK,NQEV,NRES,NRN,NSPAC,NSTRU,NSTRY,NSUD,NUMER,NUMERU,
     & NWGTS,NZBIN,SUDORD,IOP4JT,HRDCOL,NMXCL,NCL,IDCL,NPRFMT,NRECO
C
      DOUBLE PRECISION
     & ACCUR,AFCH,ALPFAC,ALPHEM,ANOMSC,ASFIXD,AVWGT,B1LIM,BETAF,BRFRAC,
     & BRHIG,BTCLM,CAFAC,CFFAC,CLDKWT,CLMAX,CLPOW,CLQ,CLSMR,CMMOM,COSS,
     & COSTH,CSPEED,CTHRPW,CTMAX,DECPAR,DECWT,DISF,DKLTM,EBEAM1,EBEAM2,
     & EMLST,EMMAX,EMMIN,EMPOW,EMSCA,ENHANC,ENSOF,EPOLN,ETAMIX,EVWGT,
     & EXAG,F0MIX,F1MIX,F2MIX,GAMH,GAMMAX,GAMW,GAMWT,GAMZ,GAMZP,GCOEF,
     & GEV2NB,GEV2MM,GPOLN,H1MIX,HBAR,HARSDT,OMEGA0,PBEAM1,PBEAM2,PDIQK,
     & PGSMX,PGSPL,PHEP,PHIMIX,PHIPAR,PHOMAS,PIFAC,PLTCUT,PPAR,PPOLN,
     & PRECO,PRSOF,PSPLT,PTINT,PTMAX,PTMIN,PTPOW,PTRMS,PXRMS,PWT,Q2MAX,
     & Q2MIN,Q2POW,Q2WWMN,Q2WWMX,QCDL3,QCDL5,QCDLAM,QDIQK,QEV,QFCH,QG,
     & QLIM,QSPAC,QV,QWT,REPWT,RESN,RHOHEP,RHOPAR,RLTIM,RMASS,RMIN,
     & RSPIN,SCABI,SINS,SNGWT,SWEIN,SWTEF,SUD,THMAX,TLOUT,TMTOP,TMNISR,
     & TQWT,VCKM,VFCH,VGCUT,VHEP,VMIN2,VPAR,VPCUT,VQCUT,VTXPIP,VTXQDK,
     & WBIGST,WGTMAX,WGTSUM,WHMIN,WSQSUM,XFACT,XLMIN,XMIX,XMRCT,XX,
     & XXMIN,YBMAX,YBMIN,YJMAX,YJMIN,YMIX,YMRCT,YWWMAX,YWWMIN,ZBINM,
     & ZJMAX,ZMXISR,Y4JT,EFFMIN,PPCL,
     & TANB,ALPHAH,COSBPA,SINBPA,COSBMA,SINBMA,COSA,SINA,COSB,SINB,COTB,
     & ZMIXSS,ZMXNSS,ZSGNSS,LFCH,RFCH,SLFCH,SRFCH, WMXUSS,WMXVSS,WSGNSS,
     & QMIXSS,LMIXSS,THETAT,THETAB,THETAL,ATSS,ABSS,ALSS,MUSS,FACTSS,
     & GHWWSS,GHZZSS,GHDDSS,GHUUSS,GHWHSS,GHSQSS,
     & XLMNSS,RMMNSS,DMSSM,SENHNC,SSPARITY,LAMDA1,LAMDA2,LAMDA3,
     & PMBN1,PMBN2,PMBN3,PMBK1,PMBK2,PMBM1,PMBM2,PMBP1,PMBP2,PMBP3,
     & OMHMIX,ET2MIX,PH3MIX,GCUTME


C           ****COMMON BLOCK FILE FOR HERWIG VERSION 6.4****
C
C ALTERATIONS: Layout completely overhauled for 5.9
C
C
C New common blocks added for version 6.1:
C              HWCLUS,HWSUSY,HWRPAR,HWMINB
C
C New variables added for version 6.1:
C              OMHMIX,ET2MIX,PH3MIX,IOP4JT,NPRFMT,
C              PRNDEF,PRNTEX,PRNWEB,EFFMIN,GCUTME,
C              IOP4JT,NPRFMT                       see HWPRAM
C              Y4JT,DURHAM                         see HWHARD
C              QORQQB,QBORQQ                       see HWPROP
C              NRECO                               see HWUCLU
C              TXNAME                              see HWUNAM
C              PPCL,NCL,IDCL                       see HWCLUS
C              TANB,ALPHAH,COSBPA,SINBPA,COSBMA,
C              SINBMA,COSA,SINA,COSB,SINB,COTB,
C              ZMIXSS,ZMXNSS,ZSGNSS,LFCH,RFCH,
C              SLFCH,SRFCH,WMXUSS,WMXVSS,WSGNSS,
C              QMIXSS,LMIXSS,THETAT,THETAB,THETAL,
C              ATSS,ABSS,ALSS,MUSS,FACTSS,GHWWSS,
C              GHZZSS,GHDDSS,GHUUSS,GHWHSS,GHSQSS,
C              XLMNSS,RMMNSS,DMSSM,SENHNC,
C              SSPARITY,SUSYIN                     see HWSUSY
C              LAMDA1,LAMDA2,LAMDA3,HRDCOL,RPARTY,
C              COLUPD                              see HWRPAR
C              PMBN1,PMBN2,PMBN3,PMBK1,PMBK2,
C              PMBM1,PMBM2,PMBP1,PMBP2,PMBP3       see HWMINB
C
C New parameters added for version 6.1:
C              NMXCL
C
C Parameter NMXRES raised to 500
C
C Scalar variables changed to arrays of size 2:
C              CLSMR,PSPLT,CLDIR                   see HWPRAM
C
C   NEW for HERWIG6.200
C
C New common blocks added for version 6.2:
C	       HWGRAV 				   see HWHGRV
C
C   NEW for HERWIG6.202
C
C New common block added for version 6.202:
C              HW6202
C which contains all other new variables, parameters and
C control flags introduced since version 6.1, so that
C other common blocks become identical to version 6.1
C
C New parameters added for version 6.2:
C	       VIPWID,DXRCYL,DXZMAX,DXRSPH,LRSUSY  see HWIGIN	
C	       GRVLAM,EMGRV,GAMGRV		   see HWHGRV
C
C New control flags added for version 6.2:
C              WZRFR                               see HWBJCO
C              FIX4JT                              see HWIGIN
C              IMSSM,IHIGGS                        see HWUINC
C
C New variable added for version 6.2:
C              PARITY                              see HWUINC
C
C  Version 6.203:
C
C  NMXHEP raised to 4000 for LHC studies
C
C
      IMPLICIT NONE
      DOUBLE PRECISION ZERO,ONE,TWO,THREE,FOUR,HALF
      PARAMETER (ZERO =0.D0, ONE =1.D0, TWO =2.D0,
     &           THREE=3.D0, FOUR=4.D0, HALF=0.5D0)
C
      DOUBLE PRECISION
     & ACCUR,AFCH,ALPFAC,ALPHEM,ANOMSC,ASFIXD,AVWGT,B1LIM,BETAF,BRFRAC,
     & BRHIG,BTCLM,CAFAC,CFFAC,CLDKWT,CLMAX,CLPOW,CLQ,CLSMR,CMMOM,COSS,
     & COSTH,CSPEED,CTHRPW,CTMAX,DECPAR,DECWT,DISF,DKLTM,EBEAM1,EBEAM2,
     & EMLST,EMMAX,EMMIN,EMPOW,EMSCA,ENHANC,ENSOF,EPOLN,ETAMIX,EVWGT,
     & EXAG,F0MIX,F1MIX,F2MIX,GAMH,GAMMAX,GAMW,GAMWT,GAMZ,GAMZP,GCOEF,
     & GEV2NB,GEV2MM,GPOLN,H1MIX,HBAR,HARDST,OMEGA0,PBEAM1,PBEAM2,PDIQK,
     & PGSMX,PGSPL,PHEP,PHIMIX,PHIPAR,PHOMAS,PIFAC,PLTCUT,PPAR,PPOLN,
     & PRECO,PRSOF,PSPLT,PTINT,PTMAX,PTMIN,PTPOW,PTRMS,PXRMS,PWT,Q2MAX,
     & Q2MIN,Q2POW,Q2WWMN,Q2WWMX,QCDL3,QCDL5,QCDLAM,QDIQK,QEV,QFCH,QG,
     & QLIM,QSPAC,QV,QWT,REPWT,RESN,RHOHEP,RHOPAR,RLTIM,RMASS,RMIN,
     & RSPIN,SCABI,SINS,SNGWT,SWEIN,SWTEF,SUD,THMAX,TLOUT,TMTOP,TMNISR,
     & TQWT,VCKM,VFCH,VGCUT,VHEP,VMIN2,VPAR,VPCUT,VQCUT,VTXPIP,VTXQDK,
     & WBIGST,WGTMAX,WGTSUM,WHMIN,WSQSUM,XFACT,XLMIN,XMIX,XMRCT,XX,
     & XXMIN,YBMAX,YBMIN,YJMAX,YJMIN,YMIX,YMRCT,YWWMAX,YWWMIN,ZBINM,
     & ZJMAX,ZMXISR,Y4JT,EFFMIN,PPCL,
     & TANB,ALPHAH,COSBPA,SINBPA,COSBMA,SINBMA,COSA,SINA,COSB,SINB,COTB,
     & ZMIXSS,ZMXNSS,ZSGNSS,LFCH,RFCH,SLFCH,SRFCH, WMXUSS,WMXVSS,WSGNSS,
     & QMIXSS,LMIXSS,THETAT,THETAB,THETAL,ATSS,ABSS,ALSS,MUSS,FACTSS,
     & GHWWSS,GHZZSS,GHDDSS,GHUUSS,GHWHSS,GHSQSS,
     & XLMNSS,RMMNSS,DMSSM,SENHNC,SSPARITY,LAMDA1,LAMDA2,LAMDA3,
     & PMBN1,PMBN2,PMBN3,PMBK1,PMBK2,PMBM1,PMBM2,PMBP1,PMBP2,PMBP3,
     & OMHMIX,ET2MIX,PH3MIX,GCUTME
C
      INTEGER
     & CLDIR,IAPHIG,IBRN,IBSH,ICHRG,ICO,IDCMF,IDHEP,IDHW,IDK,IDKPRD,IDN,
     & IDPAR,IDPDG,IERROR,IFLAV,IFLMAX,IFLMIN,IHPRO,IMQDK,INHAD,INTER,
     & IOPDKL,IOPHIG,IOPREM,IPART1,IPART2,IPRINT,IPRO,IPROC,ISLENT,
     & ISPAC,ISTAT,ISTHEP,ISTPAR,JCOPAR,JDAHEP,JDAPAR,JMOHEP,JMOPAR,
     & JNHAD,LNEXT,LOCN,LOCQ,LRSUD,LSTRT,LWEVT,LWSUD,MAPQ,MAXER,MAXEV,
     & MAXFL,MAXPR,MODBOS,MODMAX,MODPDF,NBTRY,NCLDK,NCOLO,NCTRY,NDKYS,
     & NDTRY,NETRY,NEVHEP,NEVPAR,NFLAV,NGSPL,NHEP,NME,NMODES,NMXCDK,
     & NMXDKS,NMXHEP,NMXJET,NMXMOD,NMXPAR,NMXQDK,NMXRES,NMXSUD,NPAR,
     & NPRODS,NQDK,NQEV,NRES,NRN,NSPAC,NSTRU,NSTRY,NSUD,NUMER,NUMERU,
     & NWGTS,NZBIN,SUDORD,IOP4JT,HRDCOL,NMXCL,NCL,IDCL,NPRFMT,NRECO
C
      LOGICAL
     & AZSOFT,AZSPIN,BGSHAT,BREIT,CLRECO,COLISR,DKPSET,FROST,FSTEVT,
     & FSTWGT,GENEV,GENSOF,HARDME,HVFCEN,MAXDKL,MIXING,NOSPAC,NOWGT,
     & PRNDEC,PIPSMR,PRVTX,RSTAB,SOFTME,TMPAR,TPOL,USECMF,VTOCDK,VTORDK,
     & ZPRIME,RPARTY,COLUPD,PRNDEF,PRNTEX,PRNWEB,DURHAM,SUSYIN,
     & QORQQB,QBORQQ
C
      CHARACTER*4
     & BDECAY
      CHARACTER*8
     & PART1,PART2,RNAME
      CHARACTER*20
     & AUTPDF
      CHARACTER*37
     & TXNAME
C
C New standard event common
      PARAMETER (NMXHEP=4000)
      COMMON/HEPEVT/NEVHEP,NHEP,ISTHEP(NMXHEP),IDHEP(NMXHEP),
     & JMOHEP(2,NMXHEP),JDAHEP(2,NMXHEP),PHEP(5,NMXHEP),VHEP(4,NMXHEP)
C
C Beams, process and number of events
      COMMON/HWBEAM/IPART1,IPART2
      COMMON/HWBMCH/PART1,PART2
      COMMON/HWPROC/EBEAM1,EBEAM2,PBEAM1,PBEAM2,IPROC,MAXEV
C
C Basic parameters (and quantities derived from them)
      COMMON/HWPRAM/AFCH(16,2),ALPHEM,B1LIM,BETAF,BTCLM,CAFAC,CFFAC,
     & CLMAX,CLPOW,CLSMR(2),CSPEED,ENSOF,ETAMIX,F0MIX,F1MIX,F2MIX,GAMH,
     & GAMW,GAMZ,GAMZP,GEV2NB,H1MIX,PDIQK,PGSMX,PGSPL(4),PHIMIX,PIFAC,
     & PRSOF,PSPLT(2),PTRMS,PXRMS,QCDL3,QCDL5,QCDLAM,QDIQK,QFCH(16),QG,
     & QSPAC,QV,SCABI,SWEIN,TMTOP,VFCH(16,2),VCKM(3,3),VGCUT,VQCUT,
     & VPCUT,ZBINM,EFFMIN,OMHMIX,ET2MIX,PH3MIX,GCUTME,
     & IOPREM,IPRINT,ISPAC,LRSUD,LWSUD,MODPDF(2),NBTRY,NCOLO,NCTRY,
     & NDTRY,NETRY,NFLAV,NGSPL,NSTRU,NSTRY,NZBIN,IOP4JT(2),NPRFMT,
     & AZSOFT,AZSPIN,CLDIR(2),HARDME,NOSPAC,PRNDEC,PRVTX,SOFTME,ZPRIME,
     & PRNDEF,PRNTEX,PRNWEB
C
      COMMON/HWPRCH/AUTPDF(2),BDECAY
C
C Parton shower common (same format as /HEPEVT/)
      PARAMETER (NMXPAR=500)
      COMMON/HWPART/NEVPAR,NPAR,ISTPAR(NMXPAR),IDPAR(NMXPAR),
     & JMOPAR(2,NMXPAR),JDAPAR(2,NMXPAR),PPAR(5,NMXPAR),VPAR(4,NMXPAR)
C
C Parton polarization common
      COMMON/HWPARP/DECPAR(2,NMXPAR),PHIPAR(2,NMXPAR),RHOPAR(2,NMXPAR),
     & TMPAR(NMXPAR)
C
C Electroweak boson common
      PARAMETER (MODMAX=5)
      COMMON/HWBOSC/ALPFAC,BRHIG(12),ENHANC(12),GAMMAX,RHOHEP(3,NMXHEP),
     & IOPHIG,MODBOS(MODMAX)
C
C Parton colour common
      COMMON/HWPARC/JCOPAR(4,NMXPAR)
C
C other HERWIG branching, event and hard subprocess common blocks
      COMMON/HWBRCH/ANOMSC(2,2),HARDST,PTINT(3,2),XFACT,INHAD,JNHAD,
     & NSPAC(7),ISLENT,BREIT,FROST,USECMF
C
      COMMON/HWEVNT/AVWGT,EVWGT,GAMWT,TLOUT,WBIGST,WGTMAX,WGTSUM,WSQSUM,
     & IDHW(NMXHEP),IERROR,ISTAT,LWEVT,MAXER,MAXPR,NOWGT,NRN(2),NUMER,
     & NUMERU,NWGTS,GENSOF
C
      COMMON/HWHARD/ASFIXD,CLQ(7,6),COSS,COSTH,CTMAX,DISF(13,2),EMLST,
     & EMMAX,EMMIN,EMPOW,EMSCA,EPOLN(3),GCOEF(7),GPOLN,OMEGA0,PHOMAS,
     & PPOLN(3),PTMAX,PTMIN,PTPOW,Q2MAX,Q2MIN,Q2POW,Q2WWMN,Q2WWMX,QLIM,
     & SINS,THMAX,Y4JT,TMNISR,TQWT,XX(2),XLMIN,XXMIN,YBMAX,YBMIN,YJMAX,
     & YJMIN,YWWMAX,YWWMIN,WHMIN,ZJMAX,ZMXISR,IAPHIG,IBRN(2),IBSH,
     & ICO(10),IDCMF,IDN(10),IFLMAX,IFLMIN,IHPRO,IPRO,MAPQ(6),MAXFL,
     & BGSHAT,COLISR,FSTEVT,FSTWGT,GENEV,HVFCEN,TPOL,DURHAM
C
C Arrays for particle properties (NMXRES = max no of particles defined)
      PARAMETER(NMXRES=500)
      COMMON/HWPROP/RLTIM(0:NMXRES),RMASS(0:NMXRES),RSPIN(0:NMXRES),
     & ICHRG(0:NMXRES),IDPDG(0:NMXRES),IFLAV(0:NMXRES),NRES,
     & VTOCDK(0:NMXRES),VTORDK(0:NMXRES),
     & QORQQB(0:NMXRES),QBORQQ(0:NMXRES)
C
      COMMON/HWUNAM/RNAME(0:NMXRES),TXNAME(2,0:NMXRES)
C
C Arrays for particle decays (NMXDKS = max total no of decays,
C                             NMXMOD = max no of modes for a particle)
      PARAMETER(NMXDKS=4000,NMXMOD=200)
      COMMON/HWUPDT/BRFRAC(NMXDKS),CMMOM(NMXDKS),DKLTM(NMXRES),
     & IDK(NMXDKS),IDKPRD(5,NMXDKS),LNEXT(NMXDKS),LSTRT(NMXRES),NDKYS,
     & NME(NMXDKS),NMODES(NMXRES),NPRODS(NMXDKS),DKPSET,RSTAB(0:NMXRES)
C
C Weights used in cluster decays
      COMMON/HWUWTS/REPWT(0:3,0:4,0:4),SNGWT,DECWT,QWT(3),PWT(12),
     & SWTEF(NMXRES)
C
C Parameters for cluster decays (NMXCDK = max total no of cluster
C                                         decay channels)
      PARAMETER(NMXCDK=4000)
      COMMON/HWUCLU/CLDKWT(NMXCDK),CTHRPW(12,12),PRECO,RESN(12,12),
     & RMIN(12,12),LOCN(12,12),NCLDK(NMXCDK),NRECO,CLRECO
C
C Variables controling mixing and vertex information
      COMMON/HWDIST/EXAG,GEV2MM,HBAR,PLTCUT,VMIN2,VTXPIP(4),XMIX(2),
     & XMRCT(2),YMIX(2),YMRCT(2),IOPDKL,MAXDKL,MIXING,PIPSMR
C
C Arrays for temporarily storing heavy-b,c-hadrons decaying partonicaly
C (NMXQDK = max no such decays in an event)
      PARAMETER (NMXQDK=20)
      COMMON/HWQDKS/VTXQDK(4,NMXQDK),IMQDK(NMXQDK),LOCQ(NMXQDK),NQDK
C
C Parameters for Sudakov form factors
C (NMXSUD= max no of entries in lookup table)
      PARAMETER (NMXSUD=1024)
      COMMON/HWUSUD/ACCUR,QEV(NMXSUD,6),SUD(NMXSUD,6),INTER,NQEV,NSUD,
     & SUDORD
C
      PARAMETER (NMXJET=200)
C
C SUSY parameters
      COMMON/HWSUSY/
     & TANB,ALPHAH,COSBPA,SINBPA,COSBMA,SINBMA,COSA,SINA,COSB,SINB,COTB,
     & ZMIXSS(4,4),ZMXNSS(4,4),ZSGNSS(4), LFCH(16),RFCH(16),
     & SLFCH(16,4),SRFCH(16,4), WMXUSS(2,2),WMXVSS(2,2), WSGNSS(2),
     & QMIXSS(6,2,2),LMIXSS(6,2,2),
     & THETAT,THETAB,THETAL,ATSS,ABSS,ALSS,MUSS,FACTSS,
     & GHWWSS(3),GHZZSS(3),GHDDSS(4),GHUUSS(4),GHWHSS(3),
     & GHSQSS(4,6,2,2),XLMNSS,RMMNSS,DMSSM,SENHNC(24),SSPARITY,SUSYIN
C
C R-Parity violating parameters and colours
      COMMON /HWRPAR/ LAMDA1(3,3,3),LAMDA2(3,3,3),
     &                LAMDA3(3,3,3),HRDCOL(2,5),RPARTY,COLUPD
C
C Parameters for minimum bias/soft underlying event
      COMMON/HWMINB/
     & PMBN1,PMBN2,PMBN3,PMBK1,PMBK2,PMBM1,PMBM2,PMBP1,PMBP2,PMBP3
C
C Cluster common used by soft event routines
      PARAMETER (NMXCL=500)
      COMMON/HWCLUS/PPCL(5,NMXCL),IDCL(NMXCL),NCL
C
C  Parameters for resonant graviton production
      DOUBLE PRECISION GRVLAM,EMGRV,GAMGRV
      COMMON/HWGRAV/GRVLAM,EMGRV,GAMGRV
C
C  Other new parameters for version 6.2
      DOUBLE PRECISION VIPWID,DXRCYL,DXZMAX,DXRSPH
      LOGICAL WZRFR,FIX4JT
      INTEGER IMSSM,IHIGGS,PARITY,LRSUSY
      COMMON/HW6202/VIPWID(3),DXRCYL,DXZMAX,DXRSPH,WZRFR,FIX4JT,
     & IMSSM,IHIGGS,PARITY,LRSUSY
C
C  New parameters for version 6.203
      DOUBLE PRECISION ABWGT,ABWSUM,AVABW
      INTEGER NNEGWT,NNEGEV
      LOGICAL NEGWTS
      COMMON/HW6203/ABWGT,ABWSUM,AVABW,NNEGWT,NNEGEV,NEGWTS
C
C  New parameters for version 6.3
      INTEGER IMAXCH,IMAXOP
      PARAMETER (IMAXCH=20,IMAXOP=40)
      DOUBLE PRECISION MJJMIN,CHNPRB(IMAXCH)
      INTEGER IOPSTP,IOPSH
      LOGICAL OPTM,CHON(IMAXCH)
      COMMON/HW6300/MJJMIN,CHNPRB,IOPSTP,IOPSH,OPTM,CHON
C  New PDF's for version 6.3
      INTEGER NXMRS,NQMRS,NPMRS
      PARAMETER(NXMRS=49,NQMRS=37,NPMRS=8)
      DOUBLE PRECISION FMRS(3,NPMRS,NXMRS,NQMRS+1)
      COMMON /HWPMRS/FMRS
C  Circe interface for version 6.3
      INTEGER CIRCOP,CIRCAC,CIRCVR,CIRCRV,CIRCCH
      COMMON /HWCIRC/CIRCOP,CIRCAC,CIRCVR,CIRCRV,CIRCCH
C  New parameters and commons for spin correlations
C--constants for the arrays
      INTEGER NMXSPN,NCFMAX
      PARAMETER(NMXSPN=50,NCFMAX=3)
      INTEGER NMODE2,NMODE3,NDIAGR,NMODEB,NMODE4
      PARAMETER(NMODE2=500,NMODE3=500,NDIAGR=8,NMODEB=50,NMODE4=4)
C--common block for X --> X gauge boson
      DOUBLE PRECISION ABMODE(2,NMODEB),BBMODE(2,12,NMODEB),
     & PBMODE(12,NMODEB),WTBMAX(12,NMODEB)
      INTEGER IDBPRT(NMODEB),IBMODE(NMODEB),IBDRTP(NMODEB),NBMODE
      COMMON /HWDSPB/ABMODE,BBMODE,PBMODE,WTBMAX,IDBPRT,IBDRTP,IBMODE,
     & NBMODE
C--common block for two body decays	
      DOUBLE PRECISION A2MODE(2,NMODE2),P2MODE(NMODE2),WT2MAX(NMODE2)
      INTEGER ID2PRT(NMODE2),I2DRTP(NMODE2),N2MODE
      COMMON /HWDSP2/A2MODE,P2MODE,WT2MAX,ID2PRT,I2DRTP,N2MODE
C--common block for three body decays
      DOUBLE PRECISION A3MODE(2,NDIAGR,NMODE3),B3MODE(2,NDIAGR,NMODE3),
     & P3MODE(NMODE3),WT3MAX(NMODE3),SPN3CF(NCFMAX,NCFMAX,NMODE3)
      INTEGER ID3PRT(NMODE3),I3MODE(NDIAGR,NMODE3),
     & I3DRTP(NDIAGR,NMODE3),N3MODE,NDI3BY(NMODE3),N3NCFL(NMODE3),
     &	I3DRCF(NDIAGR,NMODE3)
      COMMON /HWDSP3/A3MODE,B3MODE,P3MODE,WT3MAX,SPN3CF,ID3PRT,I3MODE,
     &	I3DRTP,N3MODE,NDI3BY,N3NCFL,I3DRCF
C--common block for four body decays
      DOUBLE PRECISION A4MODE(2,12,NMODE4),B4MODE(2,12,NMODE4),
     & P4MODE(12,12,NMODE4),WT4MAX(12,12,NMODE4)
      INTEGER ID4PRT(NMODE4),I4MODE(2,NMODE4),N4MODE
      COMMON /HWDSP4/A4MODE,B4MODE,P4MODE,WT4MAX,ID4PRT,I4MODE,N4MODE
C--common block for spin correlations in event
      INTEGER NDECSY,NSEARCH,LRDEC,LWDEC
      LOGICAL SYSPIN,THREEB,FOURB
      CHARACTER *6 TAUDEC
      COMMON /HWDSPN/NDECSY,NSEARCH,LRDEC,LWDEC,SYSPIN,THREEB,
     &	FOURB,TAUDEC

      INTEGER IDSPN(NMXSPN),JMOSPN(NMXSPN),JDASPN(2,NMXSPN),NSPN,
     &	ISNHEP(NMXHEP),NSNTRY,NCFL(NMXSPN),SPCOPT
      DOUBLE COMPLEX MESPN(2,2,2,2,NCFMAX,NMXSPN),RHOSPN(2,2,NMXSPN)
      DOUBLE PRECISION SPNCFC(NCFMAX,NCFMAX,NMXSPN)
      LOGICAL DECSPN(NMXSPN)
      COMMON /HWSPIN/MESPN,RHOSPN,SPNCFC,IDSPN,JMOSPN,JDASPN,
     &	NSPN,ISNHEP,NSNTRY,DECSPN,NCFL,SPCOPT
      INTEGER JAK1,JAK2,ITDKRC,IFPHOT
      COMMON /HWSTAU/ JAK1,JAK2,ITDKRC,IFPHOT


*/


#endif
