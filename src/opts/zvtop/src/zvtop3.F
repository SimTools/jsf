#include "zvtopoption.inc"
C--------------------------------------------------------->>>>ZVTOP3
      INTEGER FUNCTION ZVTOP3 (NTRK,ID,JETNO,ERFLG)
      IMPLICIT NONE
#include "zvkon3.inc"
#include "zxtrks.inc"
#include "phbm.inc"
#include "zvtopl3.inc"
#include "mcpart.inc"

      real rac

      INTEGER NTRK
      INTEGER ID(30)
      INTEGER JETNO
      INTEGER ERFLG
      INTEGER   SIGNAL$S_WARNING
      PARAMETER (SIGNAL$S_WARNING=3)
      INTEGER  SGLGBL, SGLCHK, SGLRTN, SGLHDL, SGLRMV, SGLGET, SGLLEV,
     *SGLIT, SGLISS, SIGSEV(0:7)/3,1,4,2,5,5,5,5/
      COMMON /SGLCOM/ SGLLEV
CDEC$ PSECT /SGLCOM/ NOSHR
      INTEGER*4 SLD$NORMAL
      PARAMETER (SLD$NORMAL=1)
C----------------------------------------------------------------------
C
C ZVTOP   --  determine the TOPOlogical Vertex structure of a jet
C----------------------------------------------------------------------
C
C  Arguments
C  =========
C INPUT:
C    NTRK   --  no. of tracks in the jet = 2...30
C    ID(30) --  ZXTRKS id's of tracks in the jet = ID(1)...ID(NTRK)
C    JETNO  --  jet number = 1...5
C  Templates
C  =========
C    ZVKON3  --  input control parameters
C    ZVTOPL3 --  output topological vertex information
C----------------------------------------------------------------------
C KEYWORDS: ZVTOP
C*--------------------------------------------------------------------*
C
C     OWNER: JACKSON, Dave          CO-OWNER: USHER, Tracy
C   SECTION: ZXFIND                    FILE: ZVTOP3 PREPMORT
C
C                 Date            Explanation of Changes
C              ----------- --------------------------------------------
C 9 Feb 1995   Program creation date
C10 Oct 1995   Version II installed into ZXFIND
C16 May 1997   Version II.5 with:
C 1) fix efficiency at > 1cm by using ZXFIT mode 1 (not 2)
C 2) calls to ZX2IMP/ZXSWIM to fix hlxpar at 2D poca if required
C 3) MODE 0-3 options in ZVKON, with PWID for plane constraint
C 4) z width of Gaussian tubes s2s, corrected for coslambda
C 5) allow z IP size and jet axis to be set in ZVKON
C30 May 1998   Protection, truncate to 29 tracks max
C 4 May 1999   Major ZVTOP upgrade, new ghost track ALGO
      REAL UPDATE / 3.00 /
CCurrent Version 3.00
C*--------------------------------------------------------------------*
      COMMON /ZVTOPL3 /ZVTOPL3
      INTEGER ZVTOPL3(-20:5000)
CDEC$ PSECT /ZVTOPL3/ NOSHR
      COMMON /ZVKON3 /ZVKON3
      INTEGER ZVKON3(-20:1)
CDEC$ PSECT /ZVKON3/ NOSHR
      COMMON /MCPART /MCPART
      INTEGER MCPART(-20:5000)
CDEC$ PSECT /MCPART/ NOSHR
      INTEGER PPHBMFAM
      INTEGER PPHBM
      INTEGER PZXTRKSFAM
      INTEGER PZXTRKS
      INTEGER PRENT
C*      pZBoutL    --> ZBoutL;                         *
      INTEGER PMCHEADFAM
      INTEGER PMCHEAD
      LOGICAL FIRST/.TRUE./,POSHEM,FLAG,AGAIN,NEWV
      INTEGER JZBLOC,JZBADD,JZPIDX,JZBFND
      INTEGER PHHEAVY,PHTMTYP,PHUDMLT
      INTEGER ZVSIG3,ZVRES3,ZVKIN,NSID,NSID2
      INTEGER NIT,II,JJ,I,J,CV,N,NSAME,NV,VF(30),IDOUT
      INTEGER IDIN/1/
      INTEGER BMXINV
      INTEGER ZXFIT,ZX2IMP,ZXSWIM,ZVFIT,JSFZVTOP3_GUFLD
      INTEGER NCHRG,IER,VID(50)
      INTEGER MCID1,MCID2,PAIND1,PAIND2,NKSH,NLAM,NGAM,NVMC
      INTEGER PA1F,PA2F,PA1P,PA2P
      INTEGER DA1P,DA2P,DA1F,DA2F
      INTEGER IPMC,IPVTX,K,CVEE
      INTEGER NVPASS,NRID,NBDR,NBAR,NBAL,RID(30),MCPA(30),IV1,IV2
      INTEGER TIDY,ALGO
      INTEGER CTYP1,CTYP2,MCTRUE
      INTEGER BTYP,BTKS,BCRG,BCG,CCG1,CCG2,CTKS
      INTEGER MULTB,MULTD1,MULTD2,MULTA,MCID,IPFLAV
      INTEGER CID,BID,CID1,BID1,CID2,BID2
      INTEGER NBHAD,BHIND(8),NBCHAD,BCIND(8),BCPAR(8),NCHAD,CHIND(8)
      REAL IMP3,AXI(3),AXF(3),TKV(3),TKI(3),ANTA,LODI,TRDI
      REAL IXC1,IYC1,IZC1,IXB,IYB,IZB,IXC2,IYC2,IZC2
      REAL BANG1,BANG2,BANG12,CANG1,CANG2,CANG12
      REAL GAXI(3),PRBV,ICUTP,ICUTN
      REAL B3MIN,DISNE,MKMIN,MKMAX,MLMIN,MLMAX,MGMAX
      REAL DKMIN,DLMIN,RIMIN,I3MIN
      REAL RVEM,ETOTL1,ETOTL2,ETOTE,MLAM1,MLAM2,MVGE
      REAL DVE2MAX,PRFK,LPRFK,DPRFK,NSIN,DISN,B3N1,B3N2,I3N1,I3N2
      REAL MU,AA,BB,CC,DD,EE,FF
      REAL PX1,PY1,PZ1,PX2,PY2,PZ2,X1,Y1,Z1,X2,Y2,Z2,DX0,DY0,DZ0
      REAL PROB,DVEE,DVEEMAX,VFOUND,MVEE,MVEP,MAS1,MAS2,ETOT,ETOTP
      REAL RVEE
      REAL VPOS(3),VPOSSG(3),CHISQTK(50),PXYZ(3,50),CHISQ,XVTX(3)
      REAL XVTXSG(6)
      REAL IPX,IPY,IPZ
      REAL DIRY,VSG,YP1,VS1,VS2,VS3
      REAL XC,YC,ZC,RNDM,BTEST,ST(30),CT(30),X0P(30),Z0(30)
      REAL YPR,KAPPA(30),TL(30),XPR,S1S(30),S2S(30),CHIP(30),IMP(30)
      REAL XT(30),YT(30),ZT(30)
      REAL VMA0XI,VM3D,YPRM,YPR2,STEP,VRX(8),VRY(8),VRZ(8),VRESL(20)
      REAL VX(30),VY(30),VZ(30),VS(30),XGM,YGM,ZGM,PRF,FMGL,FM3D
      REAL POT,XD,YD,ZD
      REAL PRFIN,PRFJN,SPFI,SPFJ,DIST,DIVV,DISM,ERRI,ERRJ
      REAL F0,FXP,FYP,FZP,FXM,FYM,FZM,FXYP,FYZP,FXZP,XV,YV,ZV,STP,DEN
      REAL DISI,DISD,ANG,NOR,NITN,VM3DN,NIT1,VM3D1,VMINI
      REAL FRAC,DXC,DYC,DZC,VI,VJ,PXJ,PYJ,PZJ,PTOTJ
      REAL PXJA,PYJA,PZJA,PTOTJA
      REAL PX,PY,PZ,PTOT,KANG,KIPW
      REAL VRAT,VMIN,VOLD,MAX,COSL,SINL
      REAL YPRI,YPRJ,DAGI,DAGJ,CTI,CTJ,STI,STJ,X0PI,X0PJ
      REAL ISTEP,DX,DY
      REAL LAM
      REAL TRKX(3),TRKT(3),DOCA,DOC0,DOC1,DOC2,DOCI
      REAL DOC0Z,DOC1Z,DOC2Z,DOTP,DVER
      REAL OK,B0(3),XIMP(3),DXIMP(6),IMP2,DIMP2,WIMP2
      INTEGER IMAX,IT,ISP,IRP,VERS
      LOGICAL FINDZ/.FALSE./
C----------------------------------------------------------------------
      DATA PPHBMFAM/0/,PZXTRKSFAM/0/,PMCHEADFAM/0/
      IF (FIRST) THEN
CJSF         SGLISS=JZPIDX('ZVKON3', ZVKON3)
CJSF         SGLISS=JZPIDX('ZVTOPL3', ZVTOPL3)
CJSF         SGLISS=JZPIDX('MCPART',MCPART)
CJSF         IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO   20
CJSF         SGLISS=JZBLOC('MCPART',PRENT)
C   *  $CALL JZBFND( 'ZBOUTL',PZBOUTL) ERROR RETURN;   *
CJSF         IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO 20
CJSF         IF ((ZVKON3(1) .EQ. 0)) THEN
CJSF            SGLISS=JZBADD('ZVKON3')
CJSF         END IF
         FIRST=.FALSE.
      END IF
CJSF      SGLISS=JZBADD('ZVTOPL3',ZVTOPL3,IDOUT,JETNO)
CJSF      IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO 20
CJSF      IF ((PPHBMFAM .EQ. 0)) THEN
CJSF         SGLISS=JZBLOC('PHBM', PPHBMFAM)
CJSF      END IF
CJSF      IF ((PZXTRKSFAM .EQ. 0)) THEN
CJSF         SGLISS=JZBLOC('ZXTRKS', PZXTRKSFAM)
CJSF      END IF
CJSF      IF ((PMCHEADFAM .EQ. 0)) THEN
CJSF         SGLISS=JZBLOC('MCHEAD', PMCHEADFAM)
CJSF      END IF
C.....Jazelle processed PPHBMFAM(JB$FORPT)...(Bank PHBM)
CJSF      PPHBM = JZL$I(JZL$X(PPHBMFAM)-1)
C.....Jazelle processed PZXTRKSFAM(JB$FORPT)...(Bank ZXTRKS)
CJSF       PZXTRKS = JZL$I(JZL$X(PZXTRKSFAM)-1)
C.....Jazelle processed PMCHEADFAM(JB$FORPT)...(Bank MCHEAD)
CJSF      PMCHEAD = JZL$I(JZL$X(PMCHEADFAM)-1)
      POT=3.14159265/2.0
C.....Jazelle processed ZVKON3(1)(MC)...(Bank ZVKON3)
CJSF       MCTRUE = JZL$I(JZL$X(ZVKON3(1))+20)
      MCTRUE = ZVKON3_MC
C.....Jazelle processed ZVKON3(1)(TIDY)...(Bank ZVKON3)
CJSF      TIDY = JZL$I(JZL$X(ZVKON3(1)))
      TIDY = ZVKON3_TIDY
C.....Jazelle processed ZVKON3(1)(PRBV)...(Bank ZVKON3)
CJSF       PRBV = JZL$R(JZL$X(ZVKON3(1))+1)
      PRBV = ZVKON3_PRBV
C.....Jazelle processed ZVKON3(1)(B3MIN)...(Bank ZVKON3)
CJSF      B3MIN = JZL$R(JZL$X(ZVKON3(1))+2)
      B3MIN = ZVKON3_B3MIN
C.....Jazelle processed ZVKON3(1)(DISNE)...(Bank ZVKON3)
CJSF       DISNE = JZL$R(JZL$X(ZVKON3(1))+3)
      DISNE = ZVKON3_DISNE
C.....Jazelle processed ZVKON3(1)(MKMIN)...(Bank ZVKON3)
CJSF      MKMIN = JZL$R(JZL$X(ZVKON3(1))+4)
      MKMIN = ZVKON3_MKMIN
C.....Jazelle processed ZVKON3(1)(MKMAX)...(Bank ZVKON3)
CJSF       MKMAX = JZL$R(JZL$X(ZVKON3(1))+5)
      MKMAX = ZVKON3_MKMAX
C.....Jazelle processed ZVKON3(1)(MLMIN)...(Bank ZVKON3)
CJSF      MLMIN = JZL$R(JZL$X(ZVKON3(1))+6)
      MLMIN = ZVKON3_MLMIN  
C.....Jazelle processed ZVKON3(1)(MLMAX)...(Bank ZVKON3)
CJSF      MLMAX = JZL$R(JZL$X(ZVKON3(1))+7)
      MLMAX = ZVKON3_MLMAX
C.....Jazelle processed ZVKON3(1)(MGMAX)...(Bank ZVKON3)
CJSF       MGMAX = JZL$R(JZL$X(ZVKON3(1))+8)
      MGMAX = ZVKON3_MGMAX
C.....Jazelle processed ZVKON3(1)(DKMIN)...(Bank ZVKON3)
CJSF      DKMIN = JZL$R(JZL$X(ZVKON3(1))+9)
      DKMIN = ZVKON3_DKMIN
C.....Jazelle processed ZVKON3(1)(DLMIN)...(Bank ZVKON3)
CJSF      DLMIN = JZL$R(JZL$X(ZVKON3(1))+10)
      DLMIN = ZVKON3_DLMIN
C.....Jazelle processed ZVKON3(1)(RIMIN)...(Bank ZVKON3)
CJSF      RIMIN = JZL$R(JZL$X(ZVKON3(1))+11)
      RIMIN = ZVKON3_RIMIN
C.....Jazelle processed ZVKON3(1)(I3MIN)...(Bank ZVKON3)
CJSF      I3MIN = JZL$R(JZL$X(ZVKON3(1))+12)
      I3MIN = ZVKON3_I3MIN
C.....Jazelle processed ZVKON3(1)(ICUTP)...(Bank ZVKON3)
CJSF      ICUTP = JZL$R(JZL$X(ZVKON3(1))+13)
      ICUTP = ZVKON3_ICUTP
C.....Jazelle processed ZVKON3(1)(ICUTN)...(Bank ZVKON3)
CJSF      ICUTN = JZL$R(JZL$X(ZVKON3(1))+14)
      ICUTN = ZVKON3_ICUTN
C.....Jazelle processed ZVKON3(1)(ALGO)...(Bank ZVKON3)
CJSF       ALGO = JZL$I(JZL$X(ZVKON3(1))+21)
      ALGO = ZVKON3_ALGO
C.....Jazelle processed PPHBM(BEAMPOS(1))...(Bank PHBM)
CJSF       IPX = JZL$R(JZL$X(PPHBM)+1+(1)*1)
      IPX = PHBM_BEAMPOS(1)
C.....Jazelle processed PPHBM(BEAMPOS(2))...(Bank PHBM)
CJSF       IPY = JZL$R(JZL$X(PPHBM)+1+(2)*1)
      IPY = PHBM_BEAMPOS(2)
C.....Jazelle processed PPHBM(BEAMPOS(3))...(Bank PHBM)
CJSF       IPZ = JZL$R(JZL$X(PPHBM)+1+(3)*1)
      IPZ = PHBM_BEAMPOS(3)
C.....Jazelle processed ZVTOPL3(JETNO)(IPX(1))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+6+(1)*1)  = IPX
      RBNK_ZVTOPL3(4+1,JETNO)=IPX 
C.....Jazelle processed ZVTOPL3(JETNO)(IPX(2))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+6+(2)*1)  = IPY
      RBNK_ZVTOPL3(4+2,JETNO)=IPY 
C.....Jazelle processed ZVTOPL3(JETNO)(IPX(3))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+6+(3)*1)  = IPZ
      RBNK_ZVTOPL3(4+3,JETNO)=IPZ 
Cfor ZVTOP co-ords keep ip at 0,0 in xy to be rot. invariant
Chence need to shift tracks by -dx,-dy
      DX = IPX
      DY = IPY
      VPOS(1) = IPX
      VPOS(2) = IPY
      VPOS(3) = IPZ
C B0 in Kgauss, 10-1 Tesla
C protect arrays of ntrk+IP = 30
CJSF      SGLISS=GUFLD(VPOS,B0)
      SGLISS=JSFZVTOP3_GUFLD(VPOS,B0)
      IF (NTRK.GT.29) THEN
         print *,' More than 29 track is trancated.'
         NTRK = 29
      END IF
      DO 31 I=1,30,1
         MCPA(I) = -1
31    CONTINUE
32    CONTINUE
C.....Jazelle processed PZXTRKS(NCHRG)...(Bank ZXTRKS)
C make sure hlxpar xyz at 2d poca
CJSF      NCHRG = JZL$I(JZL$X(PZXTRKS)+24)
      NCHRG = IBNK_ZXTRKS(9)
      DO 41 I=1,NTRK,1
         N = ID(I)
C    corrected with -ipxy Jan 99, else swim ALL data tracks !!
C   .....Jazelle processed PZXTRKS(TRK(N),HLXPAR(4))...(Bank ZXTRKS)
C   .....Jazelle processed PZXTRKS(TRK(N),HLXPAR(1))...(Bank ZXTRKS)
C   .....Jazelle processed PZXTRKS(TRK(N),HLXPAR(5))...(Bank ZXTRKS)
C   .....Jazelle processed PZXTRKS(TRK(N),HLXPAR(1))...(Bank ZXTRKS)
CJSF          DOTP = (JZL$R(JZL$X(PZXTRKS)-10+(N)*62+(4)*1) -IPX)*COS(JZL$R(
CJSF      *   JZL$X(PZXTRKS)-10+(N)*62+(1)*1) ) +(JZL$R(JZL$X(PZXTRKS)-10+(N)
CJSF      *   *62+(5)*1) -IPY)*SIN(JZL$R(JZL$X(PZXTRKS)-10+(N)*62+(1)*1) )
         DOTP = (RTRK_ZXTRKS(24+4,N) -IPX)*COS(RTRK_ZXTRKS(24+1,N))
     *   + (RTRK_ZXTRKS(24+5,N)-IPY)*SIN(RTRK_ZXTRKS(24+1,N))
C   .....Jazelle processed PZXTRKS(TRK(N),IMPACT)...(Bank ZXTRKS)
CJSF          IF (JZL$R(JZL$X(PZXTRKS)-12+(N)*62) .NE.0.0) THEN
         IF (RTRK_ZXTRKS(24,N) .NE.0.0) THEN
C      .....Jazelle processed PZXTRKS(TRK(N),IMPACT)...(Bank ZXTRKS)
CJSF             DOTP = DOTP/JZL$R(JZL$X(PZXTRKS)-12+(N)*62)
            DOTP = DOTP/RTRK_ZXTRKS(24,N)
         END IF
         IF (ABS(DOTP).GT.0.00001) THEN
C      .....Jazelle processed PZXTRKS(TRK(N),TKPAR(5))...(Bank ZXTRKS)
CJSF            IF (JZL$R(JZL$X(PZXTRKS)-34+(N)*62+(5)*1) .NE.0.0 .AND. B0(
CJSF      *      3).NE.0.0) THEN
              IF( RTRK_ZXTRKS(2+5,N).NE.0.0 .AND.B0(3).NE.0.0) THEN
C         .....Jazelle processed PZXTRKS(TRK(N),TKPAR0)...(Bank ZXTRKS)
C         .....Jazelle processed PZXTRKS(TRK(N),TKPAR(1))...(Bank ZXTRKS
C         )
C         .....Jazelle processed PZXTRKS(TRK(N),DTKPAR(1))...(Bank ZXTRK
C         S)
CJSF               SGLISS=ZX2IMP(B0(3),JZL$R(JZL$X(PZXTRKS)-34+(N)*62) ,
CJSF     *         JZL$R(JZL$X(PZXTRKS)-34+(N)*62+(1)*1) , JZL$R(JZL$X(
CJSF     *         PZXTRKS)-29+(N)*62+(1)*1) ,VPOS,FINDZ,XIMP, DXIMP,IMP2,
CJSF     *         DIMP2,WIMP2)
                 
                 print *,' In zvtop3 ... ZX2IMP should be called',
     >     ' But it''s call is commented out.'

CJSF               SGLISS=ZX2IMP(B0(3),RTRK_ZXTRKS(2,N),
CJSF     *         RTRK_ZXTRKS(2+1,N), RTRK_ZXTRKS(7+1,N), 
CJSF     *         VPOS,FINDZ,XIMP, DXIMP,IMP2,
CJSF     *         DIMP2,WIMP2)
CJSF               SGLISS=ZXSWIM(0,OK,XIMP,N)
            END IF
         END IF
41    CONTINUE
42    CONTINUE
      PXJ = 0.0
C jet total momentum
      PYJ = 0.0
      PZJ = 0.0
C this loop to fill s1s etc for V finding
      PTOTJ = 0.0
      DO 51 I=1,NTRK,1
C   .....Jazelle processed ZVTOPL3(JETNO)(TRKS(I),ZID)...(Bank ZVTOPL3)
CJSF         JZL$I(JZL$X(ZVTOPL3(JETNO))+128+(I)*21)  = ID(I)
         ITRK_ZVTOPL3(1,I,JETNO)=ID(I)
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),STATUS(4))...(Bank ZXTRKS)
CJSF          JZL$I(JZL$X(PZXTRKS)+12+(ID(I))*62+(4)*1)  = 0
         ITRK_ZXTRKS(5+4,ID(I))=0
C    the correct rotation to primed co-ords would be zxtrks%(hlxpar(1))
C   -
C    pi/2, however the pi/2 bit is just a global gauge trans...
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(3))...(Bank ZXTRKS)
CJSF         COSL = COS(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(3)*1) )) 
         COSL = COS(ATAN(RTRK_ZXTRKS(24+3,ID(I))))
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(3))...(Bank ZXTRKS)
CJSF          SINL = SIN(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(3)*1) ))
         SINL = SIN(ATAN(RTRK_ZXTRKS(24+3,ID(I))))
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(2))...(Bank ZXTRKS)
CJSF          PTOT = 1/(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(2)*1) *COSL)
         PTOT = 1/(RTRK_ZXTRKS(24+2,ID(I))*COSL)
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(1))...(Bank ZXTRKS)
CJSF         PX = PTOT*COSL*COS(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(1)*1) )
         PX = PTOT*COSL*COS(RTRK_ZXTRKS(24+1,ID(I)))
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(1))...(Bank ZXTRKS)
CJSF         PY = PTOT*COSL*SIN(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(1)*1) )
         PY = PTOT*COSL*SIN(RTRK_ZXTRKS(24+1,ID(I)))
         PZ = PTOT*SINL
         PXJ = PXJ + PX
         PYJ = PYJ + PY
         PZJ = PZJ + PZ
C     ptotj = ptotj + ptot; this was wrong Jan 99
         II = II + 1
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(1))...(Bank ZXTRKS)
CJSF         CT(II) = COS(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(1)*1) -POT)
         CT(II) = COS(RTRK_ZXTRKS(24+1,ID(I))-POT)
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(1))...(Bank ZXTRKS)
CJSF          ST(II) = SIN(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(1)*1) -POT)
         ST(II) = SIN(RTRK_ZXTRKS(24+1,ID(I))-POT)
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(6))...(Bank ZXTRKS)
CJSF         Z0(II) = JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(6)*1)
         Z0(II) = RTRK_ZXTRKS(24+6,ID(I))
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(4))...(Bank ZXTRKS)
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(5))...(Bank ZXTRKS)
CJSF          X0P(II) = CT(II)*(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(4)*1) -
CJSF      *   DX) + ST(II)*(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(5)*1) -DY)
         X0P(II) = CT(II)*(RTRK_ZXTRKS(24+4,ID(I)) - DX )
     *           + ST(II)*(RTRK_ZXTRKS(24+5,ID(I)) - DY )
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),CHARGE)...(Bank ZXTRKS)
CJSF          IF (JZL$R(JZL$X(PZXTRKS)-35+(ID(I))*62) .LT.0.0) THEN
         IF (RTRK_ZXTRKS(1,ID(I)) .LT.0.0) THEN
C      .0009
C      .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(2))...(Bank ZXTR
C      KS)
CJSF             KAPPA(II) = -0.000149896*B0(3)*JZL$R(JZL$X(PZXTRKS)-10+(ID(
CJSF     *      I))*62+(2)*1)
            KAPPA(II) = -0.000149896*B0(3)*RTRK_ZXTRKS(24+2,ID(I))
         ELSE
C      .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(2))...(Bank ZXTR
C      KS)
CJSF            KAPPA(II) = 0.000149896*B0(3)*JZL$R(JZL$X(PZXTRKS)-10+(ID(I)
CJSF     *      )*62+(2)*1)
            KAPPA(II) = 0.000149896*B0(3)*RTRK_ZXTRKS(24+2,ID(I))
         END IF
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(3))...(Bank ZXTRKS)
CJSF          TL(II) = JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(3)*1)
         TL(II) = RTRK_ZXTRKS(24+3,ID(I))
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),DHLXPAR(10))...(Bank ZXTRK
C   S)
CJSF          S1S(II) = JZL$R(JZL$X(PZXTRKS)-4+(ID(I))*62+(10)*1)
         S1S(II) = RTRK_ZXTRKS(30+10,ID(I))
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),DHLXPAR(15))...(Bank ZXTRK
C   S)
CJSF          S2S(II) = JZL$R(JZL$X(PZXTRKS)-4+(ID(I))*62+(15)*1)
         S2S(II) = RTRK_ZXTRKS(30+15,ID(I)) 
C    correct z error to be in z direction, not perp to track
         S2S(II) = S2S(II)/(COS(ATAN(TL(II))))**2
51    CONTINUE
52    CONTINUE
Cloop over zxtrks
Cput jet momentum, all tracks, in output bank
Cthis is corrected ptotj !
      PTOTJ = SQRT(PXJ*PXJ + PYJ*PYJ + PZJ*PZJ)
C.....Jazelle processed ZVTOPL3(JETNO)(PTKS(1))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+2+(1)*1)  = PXJ
      RBNK_ZVTOPL3(1,JETNO)=PXJ
C.....Jazelle processed ZVTOPL3(JETNO)(PTKS(2))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+2+(2)*1)  = PYJ
      RBNK_ZVTOPL3(2,JETNO)=PYJ
C.....Jazelle processed ZVTOPL3(JETNO)(PTKS(3))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+2+(3)*1)  = PZJ
      RBNK_ZVTOPL3(3,JETNO)=PZJ
C.....Jazelle processed ZVTOPL3(JETNO)(PTKS(4))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+2+(4)*1)  = PTOTJ
      RBNK_ZVTOPL3(4,JETNO)=PTOTJ
C.....Jazelle processed ZVKON3(1)(PXJA)...(Bank ZVKON3)
CJSF       PXJA = JZL$R(JZL$X(ZVKON3(1))+17)
      PXJA = ZVKON3_PXJA
C.....Jazelle processed ZVKON3(1)(PYJA)...(Bank ZVKON3)
CJSF       PYJA = JZL$R(JZL$X(ZVKON3(1))+18)
      PYJA = ZVKON3_PYJA
C.....Jazelle processed ZVKON3(1)(PZJA)...(Bank ZVKON3)
CJSF       PZJA = JZL$R(JZL$X(ZVKON3(1))+19)
      PZJA = ZVKON3_PZJA 
      PTOTJA = SQRT(PXJA**2 + PYJA**2 + PZJA**2)
      IF (PTOTJA .GT. 0.00001) THEN
C    use user input direction
         PXJ = PXJA
         PYJ = PYJA
         PZJ = PZJA
         PTOTJ = PTOTJA
      END IF
Cmoved MC truth part from ZBTMC to ZBTOP
      BTYP = 0
      CTYP1 = 0
      CTYP2 = 0
      BCRG = 0
      CCG1 = 0
      CCG2 = 0
      BTKS = 0
      CTKS = 0
      MULTB = 0
      MULTD1 = 0
      MULTD2 = 0
C find B,D nearest jet dirn.
      BID1 = 0
      BID2 = 0
      BID = 0
      CID = 0
      CID1 = 0
      CID2 = 0
      BANG1 = -2.0
      BANG2 = -2.0
      CANG1 = -2.0
      CANG2 = -2.0
      IF (MCTRUE .NE. 1) THEN
         GOTO   60
      END IF
Cis Monte C info requested ?
Cfirst find nearest D to jet for ccbar event
CJSF       SGLISS=PHHEAVY(NBHAD,BHIND,NBCHAD,BCIND,BCPAR,NCHAD,CHIND)
C.....Jazelle processed PMCHEAD(ORIGIN)...(Bank MCHEAD)
CJSF      IF (FLOAT(JZL$I(JZL$X(PMCHEAD)+1) ) .EQ. 8) THEN
      IF (1 .EQ. 8) THEN
C    MCPART find nearest D to jet:
         CANG1 = -9999.
         CANG12 = -9999.
         DO 71 I=1,NCHAD
            IF ((CHIND(I).EQ.0)) THEN
               GO TO 71
            END IF
C      .....Jazelle processed MCPART(CHIND(I))(P(1))...(Bank MCPART)
C      .....Jazelle processed MCPART(CHIND(I))(P(2))...(Bank MCPART)
C      .....Jazelle processed MCPART(CHIND(I))(P(3))...(Bank MCPART)
CJSF             CANG2 = JZL$R(JZL$X(MCPART(CHIND(I)))-1+(1)*1) *PXJ+JZL$R(
CJSF      *      JZL$X(MCPART(CHIND(I)))-1+(2)*1) *PYJ +JZL$R(JZL$X(MCPART(
CJSF      *      CHIND(I)))-1+(3)*1) *PZJ
             CANG2 = MCPART_P(1,CHIND(I))*PXJ
     >            + MCPART_P(2,CHIND(I))*PYJ
     >            + MCPART_P(3,CHIND(I))*PZJ
C      .....Jazelle processed MCPART(CHIND(I))(PTOT)...(Bank MCPART)
CJSF            CANG2 = CANG2/(PTOTJ*JZL$R(JZL$X(MCPART(CHIND(I)))+4) )
            CANG2 = CANG2/(PTOTJ*MCPART_PTOT(CHIND(I)))
            IF ((CANG2.GT.CANG1)) THEN
               CID = CHIND(I)
               CANG12 = CANG1
               CANG1 = CANG2
            ELSE IF((CANG2.GT.CANG12)) THEN
               CANG12 = CANG2
            END IF
71       CONTINUE
72       CONTINUE
C   .....Jazelle processed ZVTOPL3(JETNO)(MCANG(1))...(Bank ZVTOPL3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+34+(1)*1)  = CANG1
         RBNK_ZVTOPL3(21,JETNO)=CANG1
C   .....Jazelle processed ZVTOPL3(JETNO)(MCANG(2))...(Bank ZVTOPL3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+34+(2)*1)  = CANG12
         RBNK_ZVTOPL3(22,JETNO)=CANG12
         IF (CID.NE.0) THEN
C      .....Jazelle processed MCPART(CID)(PTYPE)...(Bank MCPART)
CJSF            CTYP1 = JZL$I(JZL$X(MCPART(CID))+5)
            CTYP1 = MCPART_PTYPE(CID)
C      .....Jazelle processed MCPART(CID)(XT(1))...(Bank MCPART)
CJSF            IXC1 = JZL$R(JZL$X(MCPART(CID))+7+(1)*1)
            IXC1 = MCPART_XT(1,CID)
C      .....Jazelle processed MCPART(CID)(XT(2))...(Bank MCPART)
CJSF            IYC1 = JZL$R(JZL$X(MCPART(CID))+7+(2)*1)
            IYC1 = MCPART_XT(2,CID)
C      .....Jazelle processed MCPART(CID)(XT(3))...(Bank MCPART)
CJSF            IZC1 = JZL$R(JZL$X(MCPART(CID))+7+(3)*1)
            IZC1 = MCPART_XT(3,CID)
C      .....Jazelle processed MCPART(CID)(CHARGE)...(Bank MCPART)
CJSF            CCG1 = JZL$R(JZL$X(MCPART(CID))+6)
            CCG1 = MCPART_CHARGE(CID)
         END IF
         SGLISS=PHUDMLT(CID,MULTA,MULTD1)
         CID1 = CID
      END IF
Cif ccbar event
Cnow BBbar event
C.....Jazelle processed PMCHEAD(ORIGIN)...(Bank MCHEAD)
CJSF      IF (FLOAT(JZL$I(JZL$X(PMCHEAD)+1) ) .EQ. 16) THEN
      IF (1 .EQ. 16) THEN
C    MCPART find nearest B to jet:
         BANG1 = -9999.
         BANG12 = -9999.
         DO 81 I=1,NBHAD
            IF ((BHIND(I).EQ.0)) THEN
               GO TO 81
            END IF
C      .....Jazelle processed MCPART(BHIND(I))(P(1))...(Bank MCPART)
C      .....Jazelle processed MCPART(BHIND(I))(P(2))...(Bank MCPART)
C      .....Jazelle processed MCPART(BHIND(I))(P(3))...(Bank MCPART)
CJSF            BANG2 = JZL$R(JZL$X(MCPART(BHIND(I)))-1+(1)*1) *PXJ+JZL$R(
CJSF     *      JZL$X(MCPART(BHIND(I)))-1+(2)*1) *PYJ +JZL$R(JZL$X(MCPART(
CJSF     *      BHIND(I)))-1+(3)*1) *PZJ
            BANG2 = MCPART_P(1,BHIND(I))*PXJ
     >            + MCPART_P(2,BHIND(I))*PYJ
     >            + MCPART_P(3,BHIND(I))*PZJ
C      .....Jazelle processed MCPART(BHIND(I))(PTOT)...(Bank MCPART)
CJSF            BANG2 = BANG2/(PTOTJ*JZL$R(JZL$X(MCPART(BHIND(I)))+4) )
            BANG2 = BANG2/(PTOTJ*MCPART_PTOT(BHIND(I)))
            IF ((BANG2.GT.BANG1)) THEN
               BID = BHIND(I)
               BANG12 = BANG1
               BANG1 = BANG2
            ELSE IF((BANG2.GT.BANG12)) THEN
               BANG12 = BANG2
            END IF
81       CONTINUE
82       CONTINUE
C   .....Jazelle processed ZVTOPL3(JETNO)(MCANG(1))...(Bank ZVTOPL3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+34+(1)*1)  = BANG1
         RBNK_ZVTOPL3(21,JETNO)=BANG1
C   .....Jazelle processed ZVTOPL3(JETNO)(MCANG(2))...(Bank ZVTOPL3)
C   now find D(s) from B
C       do i = 1,nbchad,1 [
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+34+(2)*1)  = BANG12
         RBNK_ZVTOPL3(22,JETNO)=BANG12
         DO 91 I=1,8,1
            IF (BHIND(BCPAR(I)).EQ.BID .AND. CID1.NE.0) THEN
               CID2 = BCIND(I)
            END IF
            IF (BHIND(BCPAR(I)).EQ.BID .AND. CID1.EQ.0) THEN
               CID1 = BCIND(I)
            END IF
91       CONTINUE
92       CONTINUE
         SGLISS=PHUDMLT(BID,MULTA,MULTB)
         SGLISS=PHUDMLT(CID1,MULTA,MULTD1)
         SGLISS=PHUDMLT(CID2,MULTA,MULTD2)
         IF (BID.NE.0) THEN
C      .....Jazelle processed MCPART(BID)(PTYPE)...(Bank MCPART)
CJSF            BTYP = JZL$I(JZL$X(MCPART(BID))+5)
            BTYP = MCPART_PTYPE(BID)
C      .....Jazelle processed MCPART(BID)(XT(1))...(Bank MCPART)
CJSF            IXB = JZL$R(JZL$X(MCPART(BID))+7+(1)*1)
            IXB = MCPART_XT(1,BID)
C      .....Jazelle processed MCPART(BID)(XT(2))...(Bank MCPART)
CJSF            IYB = JZL$R(JZL$X(MCPART(BID))+7+(2)*1)
            IYB = MCPART_XT(2,BID)
C      .....Jazelle processed MCPART(BID)(XT(3))...(Bank MCPART)
CJSF            IZB = JZL$R(JZL$X(MCPART(BID))+7+(3)*1)
            IZB = MCPART_XT(3,BID)
C      .....Jazelle processed MCPART(BID)(CHARGE)...(Bank MCPART)
CJSF            BCG = JZL$R(JZL$X(MCPART(BID))+6)
            BCG = MCPART_CHARGE(BID)
         END IF
         IF (CID1.NE.0) THEN
C      .....Jazelle processed MCPART(CID1)(PTYPE)...(Bank MCPART)
CJSF            CTYP1 = JZL$I(JZL$X(MCPART(CID1))+5)
            CTYP1 = MCPART_PTYPE(CID1)
C      .....Jazelle processed MCPART(CID1)(XT(1))...(Bank MCPART)
CJSF            IXC1 = JZL$R(JZL$X(MCPART(CID1))+7+(1)*1)
            IXC1 = MCPART_XT(1,CID1)
C      .....Jazelle processed MCPART(CID1)(XT(2))...(Bank MCPART)
CJSF            IYC1 = JZL$R(JZL$X(MCPART(CID1))+7+(2)*1)
            IYC1 = MCPART_XT(2,CID1)
C      .....Jazelle processed MCPART(CID1)(XT(3))...(Bank MCPART)
CJSF            IZC1 = JZL$R(JZL$X(MCPART(CID1))+7+(3)*1)
            IZC1 = MCPART_XT(3,CID1)
C      .....Jazelle processed MCPART(CID1)(CHARGE)...(Bank MCPART)
CJSF            CCG1 = JZL$R(JZL$X(MCPART(CID1))+6)
            CCG1 = MCPART_CHARGE(CID1)
         END IF
         IF (CID2.NE.0) THEN
C      .....Jazelle processed MCPART(CID2)(PTYPE)...(Bank MCPART)
CJSF            CTYP2 = JZL$I(JZL$X(MCPART(CID2))+5)
            CTYP2 = MCPART_PTYPE(CID2)
C      .....Jazelle processed MCPART(CID2)(XT(1))...(Bank MCPART)
CJSF            IXC2 = JZL$R(JZL$X(MCPART(CID2))+7+(1)*1)
            IXC2 = MCPART_XT(1,CID2)
C      .....Jazelle processed MCPART(CID2)(XT(2))...(Bank MCPART)
CJSF            IYC2 = JZL$R(JZL$X(MCPART(CID2))+7+(2)*1)
            IYC2 = MCPART_XT(2,CID2)
C      .....Jazelle processed MCPART(CID2)(XT(3))...(Bank MCPART)
CJSF            IZC2 = JZL$R(JZL$X(MCPART(CID2))+7+(3)*1)
            IZC2 = MCPART_XT(3,CID2)
C      .....Jazelle processed MCPART(CID2)(CHARGE)...(Bank MCPART)
CJSF            CCG2 = JZL$R(JZL$X(MCPART(CID2))+6)
            CCG2 = MCPART_CHARGE(CID2)
         END IF
      END IF
Cif bbbar event
C check input tracks
      DO 101 JJ=1,NCHRG,1
C   .....Jazelle processed PZXTRKS(TRK(JJ),MCID)...(Bank ZXTRKS)
CJSF          MCID = JZL$I2(ISHFT(JZL$X(PZXTRKS), 1)-22+(JJ)*124)
         MCID = ITRK_ZXTRKS(3,JJ)
         IF (MCID.NE.0) THEN
            DO 111 II=1,NTRK,1
C         .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),ZID)...(Bank ZV
C         TOPL3)
CJSF               IF (JZL$I(JZL$X(ZVTOPL3(JETNO))+128+(II)*21) .EQ.JJ)
               IF( ITRK_ZVTOPL3(1,II,JETNO).EQ.JJ )
     *         THEN
C            check right hemi
CJSF                   SGLISS=PHTMTYP(MCID,IPMC,IPFLAV,IPVTX)
                  IF ((IPFLAV.EQ.1 .AND. IPMC.NE.BID).OR.(IPFLAV .EQ. 2
     *            .AND. IPMC.NE.CID .AND. IPMC.NE.CID1 .AND. IPMC.NE.
     *            CID2)) THEN
                     IPFLAV = -IPFLAV
                     IPVTX = -IPVTX
                  END IF
                  MCPA(II) = IPFLAV
C            .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),MCFL)...(Ban
C            k ZVTOPL3)
CJSF                  JZL$I(JZL$X(ZVTOPL3(JETNO))+147+(II)*21)  = IPFLAV
                  ITRK_ZVTOPL3(3,II,JETNO)=IPFLAV
C            .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),MCVT)...(Ban
C            k ZVTOPL3)
CJSF                  JZL$I(JZL$X(ZVTOPL3(JETNO))+148+(II)*21)  = IPVTX
                  ITRK_ZVTOPL3(4,II,JETNO)=IPVTX
               END IF
111         CONTINUE
112         CONTINUE
C      loop over zvtopl3 tracks
         END IF
101   CONTINUE
102   CONTINUE
C loop over zxtrks
      BCG=BCG-CCG1-CCG2
C.....Jazelle processed ZVTOPL3(JETNO)(MCBX(1))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+17+(1)*1)  = IXB
      RBNK_ZVTOPL3(11+1,JETNO) = IXB
C.....Jazelle processed ZVTOPL3(JETNO)(MCBX(2))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+17+(2)*1)  = IYB
      RBNK_ZVTOPL3(11+2,JETNO) = IYB
C.....Jazelle processed ZVTOPL3(JETNO)(MCBX(3))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+17+(3)*1)  = IZB
      RBNK_ZVTOPL3(11+3,JETNO) = IZB
C.....Jazelle processed ZVTOPL3(JETNO)(MCDX(1))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+24+(1)*1)  = IXC1
      RBNK_ZVTOPL3(14+1,JETNO) = IXC1
C.....Jazelle processed ZVTOPL3(JETNO)(MCDX(2))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+24+(2)*1)  = IYC1
      RBNK_ZVTOPL3(14+2,JETNO) = IYC1
C.....Jazelle processed ZVTOPL3(JETNO)(MCDX(3))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+24+(3)*1)  = IZC1
      RBNK_ZVTOPL3(14+3,JETNO) = IZC1
C.....Jazelle processed ZVTOPL3(JETNO)(MCDX2(1))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+31+(1)*1)  = IXC2
      RBNK_ZVTOPL3(17+1,JETNO) = IXC2
C.....Jazelle processed ZVTOPL3(JETNO)(MCDX2(2))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+31+(2)*1)  = IYC2
      RBNK_ZVTOPL3(17+2,JETNO) = IYC2
C.....Jazelle processed ZVTOPL3(JETNO)(MCDX2(3))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+31+(3)*1)  = IZC2
      RBNK_ZVTOPL3(17+3,JETNO) = IZC2
C.....Jazelle processed ZVTOPL3(JETNO)(MCB(1))...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+13+(1)*1)  = BTYP
      IBNK_ZVTOPL3(3+1,JETNO) = BTYP
C.....Jazelle processed ZVTOPL3(JETNO)(MCB(2))...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+13+(2)*1)  = MULTB
      IBNK_ZVTOPL3(3+2,JETNO) = MULTB
C.....Jazelle processed ZVTOPL3(JETNO)(MCB(3))...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+13+(3)*1)  = BCG
      IBNK_ZVTOPL3(3+3,JETNO) = BCG  
C.....Jazelle processed ZVTOPL3(JETNO)(MCB(4))...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+13+(4)*1)  = BID
      IBNK_ZVTOPL3(3+4,JETNO) = BID  
C.....Jazelle processed ZVTOPL3(JETNO)(MCD(1))...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+20+(1)*1)  = CTYP1
      IBNK_ZVTOPL3(7+1,JETNO) = CTYP1
C.....Jazelle processed ZVTOPL3(JETNO)(MCD(2))...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+20+(2)*1)  = MULTD1
      IBNK_ZVTOPL3(7+2,JETNO) = MULTD1
C.....Jazelle processed ZVTOPL3(JETNO)(MCD(3))...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+20+(3)*1)  = CCG1
      IBNK_ZVTOPL3(7+3,JETNO) = CCG1 
C.....Jazelle processed ZVTOPL3(JETNO)(MCD(4))...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+20+(4)*1)  = CID1
      IBNK_ZVTOPL3(7+4,JETNO) = CID1 
C.....Jazelle processed ZVTOPL3(JETNO)(MCD2(1))...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+27+(1)*1)  = CTYP2
      IBNK_ZVTOPL3(11+1,JETNO) = CTYP2
C.....Jazelle processed ZVTOPL3(JETNO)(MCD2(2))...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+27+(2)*1)  = MULTD2
      IBNK_ZVTOPL3(11+2,JETNO) = MULTD2
C.....Jazelle processed ZVTOPL3(JETNO)(MCD2(3))...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+27+(3)*1)  = CCG2
      IBNK_ZVTOPL3(11+3,JETNO) = CCG2 
C.....Jazelle processed ZVTOPL3(JETNO)(MCD2(4))...(Bank ZVTOPL3)
C first loop over track pairs to find Vs and interactions
C flag tracks which are removed as V or int
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+27+(4)*1)  = CID2
      IBNK_ZVTOPL3(11+4,JETNO) = CID2 
60    CONTINUE
         DO 61 I=1,30,1
         RID(I) = 0
61    CONTINUE
62    CONTINUE
      NVPASS = 0
      NRID = 0
      NBDR = 0
      NBAR = 0
      NBAL = 0
Ctidy jet with V, interaction removal
      IF (TIDY.GT.0) THEN
120      NVPASS = NVPASS + 1
         VFOUND = 0.0
         NKSH = 0
         NLAM = 0
         NGAM = 0
         DVEEMAX = -1.0
         DVE2MAX = -1.0
         DO 131 I=1,NTRK,1
C       check both not removed yet
            DO 141 J=I+1,NTRK,1
               IF ((RID(I).EQ.0 .AND. RID(J).EQ.0 ) .OR.  (RID(I).EQ.0 .
     *         AND. RID(J).EQ.40) .OR. (RID(I).EQ.40 .AND. RID(J).EQ.0 )
     *         ) THEN
C            ..unless one from int.
                  VID(1) = ID(I)
                  VID(2) = ID(J)
C             first estimate close point to both tracks
C            .....Jazelle processed PZXTRKS(TRK(VID(1)),HLXPAR(4))...(Ba
C            nk ZXTRKS)
CJSF                  X1 = JZL$R(JZL$X(PZXTRKS)-10+(VID(1))*62+(4)*1)
	          X1=RTRK_ZXTRKS(24+4,VID(1))
C            .....Jazelle processed PZXTRKS(TRK(VID(1)),HLXPAR(5))...(Ba
C            nk ZXTRKS)
CJSF                  Y1 = JZL$R(JZL$X(PZXTRKS)-10+(VID(1))*62+(5)*1)
                  Y1=RTRK_ZXTRKS(24+5,VID(1))
C            .....Jazelle processed PZXTRKS(TRK(VID(1)),HLXPAR(6))...(Ba
C            nk ZXTRKS)
CJSF                   Z1 = JZL$R(JZL$X(PZXTRKS)-10+(VID(1))*62+(6)*1)
                  Z1=RTRK_ZXTRKS(24+6,VID(1))
C            .....Jazelle processed PZXTRKS(TRK(VID(1)),HLXPAR(3))...(Ba
C            nk ZXTRKS)
CJSF                  COSL = COS(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(VID(1))*62+(
CJSF     *            3)*1) ))
                  COSL=COS(ATAN(RTRK_ZXTRKS(24+3,VID(1))))
C            .....Jazelle processed PZXTRKS(TRK(VID(1)),HLXPAR(3))...(Ba
C            nk ZXTRKS)
CJSF                  SINL = SIN(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(VID(1))*62+(
CJSF     *            3)*1) ))
                  SINL=SIN(ATAN(RTRK_ZXTRKS(24+3,VID(1))))
C            .....Jazelle processed PZXTRKS(TRK(VID(1)),HLXPAR(2))...(Ba
C            nk ZXTRKS)
CJSF                  PTOT = 1/(JZL$R(JZL$X(PZXTRKS)-10+(VID(1))*62+(2)*1) *
CJSF     *            COSL)
                  PTOT=1/(RTRK_ZXTRKS(24+2,VID(1))*COSL)
C            .....Jazelle processed PZXTRKS(TRK(VID(1)),HLXPAR(1))...(Ba
C            nk ZXTRKS)
CJSF                  PX1 = PTOT*COSL*COS(JZL$R(JZL$X(PZXTRKS)-10+(VID(1))*
CJSF     *            62+(1)*1) )
                  PX1 = PTOT*COSL*COS(RTRK_ZXTRKS(24+1,VID(1)))
C            .....Jazelle processed PZXTRKS(TRK(VID(1)),HLXPAR(1))...(Ba
C            nk ZXTRKS)
CJSF                  PY1 = PTOT*COSL*SIN(JZL$R(JZL$X(PZXTRKS)-10+(VID(1))*
CJSF     *            62+(1)*1) )
                  PY1 = PTOT*COSL*SIN(RTRK_ZXTRKS(24+1,VID(1)))
                  PZ1 = PTOT*SINL
C            .....Jazelle processed PZXTRKS(TRK(VID(2)),HLXPAR(4))...(Ba
C            nk ZXTRKS)
CJSF                  X2 = JZL$R(JZL$X(PZXTRKS)-10+(VID(2))*62+(4)*1)
                  X2 =RTRK_ZXTRKS(24+4,VID(2))
C            .....Jazelle processed PZXTRKS(TRK(VID(2)),HLXPAR(5))...(Ba
C            nk ZXTRKS)
CJSF                  Y2 = JZL$R(JZL$X(PZXTRKS)-10+(VID(2))*62+(5)*1)
                  Y2 = RTRK_ZXTRKS(24+5,VID(2))
C            .....Jazelle processed PZXTRKS(TRK(VID(2)),HLXPAR(6))...(Ba
C            nk ZXTRKS)
CJSF                  Z2 = JZL$R(JZL$X(PZXTRKS)-10+(VID(2))*62+(6)*1)
                  Z2 = RTRK_ZXTRKS(24+6, VID(2))
C            .....Jazelle processed PZXTRKS(TRK(VID(2)),HLXPAR(3))...(Ba
C            nk ZXTRKS)
CJSF                  COSL = COS(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(VID(2))*62+(
CJSF     *            3)*1) ))
                  COSL = COS(ATAN(RTRK_ZXTRKS(24+3,VID(2))))
C            .....Jazelle processed PZXTRKS(TRK(VID(2)),HLXPAR(3))...(Ba
C            nk ZXTRKS)
CJSF                  SINL = SIN(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(VID(2))*62+(
CJSF     *            3)*1) ))
                  SINL = SIN(ATAN(RTRK_ZXTRKS(24+3,VID(2))))
C            .....Jazelle processed PZXTRKS(TRK(VID(2)),HLXPAR(2))...(Ba
C            nk ZXTRKS)
CJSF                  PTOT = 1/(JZL$R(JZL$X(PZXTRKS)-10+(VID(2))*62+(2)*1) *
CJSF     *            COSL)
                  PTOT = 1/(RTRK_ZXTRKS(24+2,VID(2))*COSL)
C            .....Jazelle processed PZXTRKS(TRK(VID(2)),HLXPAR(1))...(Ba
C            nk ZXTRKS)
CJSF                  PX2 = PTOT*COSL*COS(JZL$R(JZL$X(PZXTRKS)-10+(VID(2))*
CJSF     *            62+(1)*1) )
                  PX2=PTOT*COSL*COS(RTRK_ZXTRKS(24+1,VID(2)))
C            .....Jazelle processed PZXTRKS(TRK(VID(2)),HLXPAR(1))...(Ba
C            nk ZXTRKS)
CJSF                  PY2 = PTOT*COSL*SIN(JZL$R(JZL$X(PZXTRKS)-10+(VID(2))*
CJSF     *            62+(1)*1) )
                  PY2 = PTOT*COSL*SIN(RTRK_ZXTRKS(24+1,VID(2)))
                  PZ2 = PTOT*SINL
                  DX0 = X1 - X2
                  DY0 = Y1 - Y2
                  DZ0 = Z1 - Z2
                  AA = (DX0*PX1 + DY0*PY1 + DZ0*PZ1)
                  BB = -(PX1*PX2 + PY1*PY2 + PZ1*PZ2)
                  CC = (PX1*PX1 + PY1*PY1 + PZ1*PZ1)
                  DD = -(DX0*PX2 + DY0*PY2 + DZ0*PZ2)
                  EE = (PX2*PX2 + PY2*PY2 + PZ2*PZ2)
                  FF = -(PX1*PX2 + PY1*PY2 + PZ1*PZ2)
                  LAM = (CC*EE - BB*FF)
                  IF ((LAM .GT. 0.0 .AND. SQRT(LAM).GT.0.000001)) THEN
C                root lam = p1mag*p2mag*sintheta
                     LAM = (BB*DD - AA*EE)/LAM
                     MU = -(FF*LAM + DD)/EE
                  ELSE
                     LAM = 0.0
                     MU = 0.0
                  END IF
C             best vertex guess is midpoint of closest points on each tr
C            ack
                  XD = (X1 + LAM*PX1 + X2 + MU*PX2)/2.0
                  YD = (Y1 + LAM*PY1 + Y2 + MU*PY2)/2.0
                  ZD = (Z1 + LAM*PZ1 + Z2 + MU*PZ2)/2.0
                  IF (SQRT((XD-IPX)**2 + (YD-IPY)**2 + (ZD-IPZ)**2).LT.
     *            5.0) THEN
                     VPOS(1) = XD
                     VPOS(2) = YD
                     VPOS(3) = ZD
                  ELSE
                     VPOS(1) = 0.0
                     VPOS(2) = 0.0
                     VPOS(3) = 0.0
                  END IF
                  VPOSSG(1) = 10.0
                  VPOSSG(2) = 10.0
                  VPOSSG(3) = 10.0
                  JJ = 2
C             protect against -ve error matrix
c                  PRINT *,' Zvtop3 will call  zxfit ... jj=',jj
                  SGLISS=ZXFIT(1,JJ,VID,VPOS,VPOSSG,CHISQ,XVTX,XVTXSG,
     *            CHISQTK,PXYZ,IER)
c                  print *,' calls zxfit in zvtop3 ...ierr=',ier

                  IF (XVTXSG(1).GE.0 .AND. XVTXSG(3).GE.0 .AND. XVTXSG(
     *            6).GE.0) THEN
                     IF (PROB(CHISQ,1).GT.PRBV) THEN
C                   begin to study Vs and interactions here 16 jan 99
C                  *  mcid1 = pZxTrks%(trk(vid(1)),mcid);
C                          *
C                  *  mcid2 = pZxTrks%(trk(vid(2)),mcid);
C                          *
C                  *  if mcid1.ne.0 .and. mcid2.ne.0 [
C                          *
C                  *    prent = mcpart(mcid1)%(parent);
C                          *
C                  *    pa1p = prent%(ptype);
C                          *
C                  *    paind1 = prent%(JB$ID);
C                          *
C                  *    prent = mcpart(mcid2)%(parent);
C                          *
C                  *    pa2p = prent%(ptype);
C                          *
C                  *    paind2 = prent%(JB$ID);
C                          *
                        XD = XVTX(1) - DX
                        YD = XVTX(2) - DY
                        ZD = XVTX(3)
C                  *   if paind1.eq.paind2 .and. dvee.gt.0.1 [
C                          *
C                  *    if abs(pa1p).eq.1  [ngam = ngam + 1;]
C                          *
C                  *    if abs(pa1p).eq.37 [nksh = nksh + 1;]
C                          *
C                  *    if abs(pa1p).eq.57 [nlam = nlam + 1;]
C                          *
C                  *   ]
C                          *
C                   keep track of second distance vee with dve2max
                        DVEE = SQRT(XD**2+YD**2+(ZD-IPZ)**2)
                        IF (DVEE.GT.DVE2MAX .AND.DVEE.LT.DVEEMAX) THEN
                           DVE2MAX = DVEE
                        ELSE IF(DVEE.GT.DVEEMAX) THEN
                           DVE2MAX = DVEEMAX
                        END IF
                        IF (DVEE.GT.DVEEMAX .AND. DVEE.GT.0.1) THEN
                           VFOUND = 1.0
                           IV1 = I
                           IV2 = J
                           DVEEMAX = DVEE
C                     *     call BBTMTYP(paind1,ipmc,pa1f,ipvtx);
C                                *
C                     *     call BBTMTYP(paind2,ipmc,pa2f,ipvtx);
C                                *
C                     *     call BBTMTYP(mcid1,ipmc,da1f,ipvtx);
C                                *
C                     *     call BBTMTYP(mcid2,ipmc,da2f,ipvtx);
C                                *
C                      set parent information -ve if different parent
C                     *    if paind1.eq.paind2 [pZBoutl%(pa1p) = abs(pa1
C                     p);        *
C                     *                          pZBoutl%(pa2p) = abs(pa
C                     2p);       *
C                     *                          pZBoutl%(pa1f) = abs(pa
C                     1f);       *
C                     *                          pZBoutl%(pa2f) = abs(pa
C                     2f);]      *
C                     *                    else [pZBoutl%(pa1p) = -abs(p
C                     a1p);      *
C                     *                          pZBoutl%(pa2p) = -abs(p
C                     a2p);      *
C                     *                          pZBoutl%(pa1f) = -abs(p
C                     a1f);      *
C                     *                          pZBoutl%(pa2f) = -abs(p
C                     a2f);]     *
C                     *     da1p = mcpart(mcid1)%(ptype);
C                                *
C                     *     da2p = mcpart(mcid2)%(ptype);
C                                *
                           RVEE = SQRT(XD**2+YD**2)
C                     *   if nvpass.eq.1 [
C                                *
C                     *     pZBoutl%(da1f) = da1f;
C                                *
C                     *     pZBoutl%(da2f) = da2f;
C                                *
C                     *     pZBoutl%(da1p) = da1p;
C                                *
C                     *     pZBoutl%(da2p) = da2p;
C                                *
C                     *     pZBoutl%(dvee) = dvee;
C                                *
C                     *     pZBoutl%(rvee) = rvee;
C                                *
C                     *     pZBoutl%(prbv) = prob(chisq,1);
C                                *
C                     *   ]
C                                *
C                      mass of the vertex, using MC mass
C                     *     if abs(da1p) = 7 [mas1 = 0.00051; ]
C                                *
C                     *     elseif abs(da1p) = 9 [mas1 = 0.1057; ]
C                                *
C                     *     elseif abs(da1p) = 17 [mas1 = 0.1396; ]
C                                *
C                     *     elseif abs(da1p) = 18 [mas1 = 0.4937; ]
C                                *
C                     *     elseif abs(da1p) = 41 [mas1 = 0.9383; ]
C                                *
C                     *     else [mas1 = 0.1396; ]
C                                *
C                     *     if abs(da2p) = 7 [mas2 = 0.00051; ]
C                                *
C                     *     elseif abs(da2p) = 9 [mas2 = 0.1057; ]
C                                *
C                     *     elseif abs(da2p) = 17 [mas2 = 0.1396; ]
C                                *
C                     *     elseif abs(da2p) = 18 [mas2 = 0.4937; ]
C                                *
C                     *     elseif abs(da2p) = 41 [mas2 = 0.9383; ]
C                                *
C                     *     else [mas2 = 0.1396; ]
C                                *
                           PX = PXYZ(1,1)
                           PY = PXYZ(2,1)
                           PZ = PXYZ(3,1)
                           PTOT = SQRT(PX**2 + PY**2 + PZ**2)
C                     *    if nvpass.eq.1 [
C                                *
C                     *     pZBoutl%(pve1) = ptot;
C                                *
C                     *    ]
C                                *
C                     *     etot = sqrt(ptot*ptot + mas1*mas1);
C                                *
                           ETOTP = SQRT(PTOT*PTOT + 0.01949)
C                     assume pion mass
                           ETOTL1 = SQRT(PTOT*PTOT + 0.01949)
C                     assume pion mass
                           ETOTL2 = SQRT(PTOT*PTOT + 0.8804)
C                     assume proton mass
                           ETOTE = SQRT(PTOT*PTOT + 0.000)
C                     assume electron mass
                           PX = PX + PXYZ(1,2)
                           PY = PY + PXYZ(2,2)
                           PZ = PZ + PXYZ(3,2)
                           PTOT = SQRT(PXYZ(1,2)**2 + PXYZ(2,2)**2 +
     *                     PXYZ(3,2)**2)
C                     *   if nvpass.eq.1 [
C                               *
C                     *    pZBoutl%(pve2) = ptot;
C                               *
C                     *   ]
C                               *
C                     *    etot = etot + sqrt(ptot*ptot + mas2*mas2);
C                               *
                           ETOTP = ETOTP + SQRT(PTOT*PTOT + 0.01949)
                           ETOTL1 = ETOTL1 + SQRT(PTOT*PTOT + 0.8804)
                           ETOTL2 = ETOTL2 + SQRT(PTOT*PTOT + 0.01949)
                           ETOTE = ETOTE + SQRT(PTOT*PTOT + 0.000)
                           PTOT = SQRT(PX**2 + PY**2 + PZ**2)
C                     *    mvee = sqrt(etot**2 - ptot**2);
C                               *
                           MVEP = 0.0
                           MLAM1 = 0.0
                           MLAM2 = 0.0
                           MVGE = 0.0
                           MVEP = SQRT(ETOTP**2 - PTOT**2)
                           MLAM1 = SQRT(ETOTL1**2 - PTOT**2)
                           MLAM2 = SQRT(ETOTL2**2 - PTOT**2)
                           IF ((ETOTE**2-PTOT**2 .GT. 0.0)) THEN
                              MVGE = SQRT(ETOTE**2-PTOT**2)
                           ELSE
                              MVGE = 0.0
                           END IF
C                     *   if nvpass.eq.1 [
C                               *
C                     *    pZBoutl%(mvee) = mvee;
C                               *
C                     *    pZBoutl%(mvep) = mvep;
C                               *
C                     *    pZBoutl%(mvl1) = mlam1;
C                               *
C                     *    pZBoutl%(mvl2) = mlam2;
C                               *
C                     *    pZBoutl%(mvge) = mvge;
C                               *
C                     *    pZBoutl%(pvee) = ptot;
C                               *
C                     *   ]
C                               *
                           DISN = 10.0
                           NSIN = 1000.0
C                      number of sigmas to nearest track
                           DO 151 K=1,NTRK,1
                              IF (K.NE.I .AND. K.NE.J) THEN
                                 XPR = CT(K)*XD + ST(K)*YD
                                 YPR = -ST(K)*XD + CT(K)*YD
#ifndef ORIGINAL
         RAC = ABS(1/(2*KAPPA(K)))
         LPRFK = ((XPR-
     *   (X0P(K)+YPR*YPR*KAPPA(K)+YPR**4*KAPPA(K)**3 
     *    + 2*YPR**6*KAPPA(K)**5 )
     *    )**2)/S1S(K) + ((ZD-(Z0(K)
     *    + RAC*TL(K)*ASIN(YPR/RAC)))**2)/S2S(K)
                                 DPRFK = SQRT( (XPR-
     *    (X0P(K)+YPR*YPR*KAPPA(K)+YPR**4*KAPPA(K)**3
     *     + 2*YPR**6*KAPPA(K)**5  )
     *    )**2 + ((COS(ATAN(TL(K))))**2)
     *    *(ZD-(Z0(K)+RAC*TL(K)*ASIN(YPR/RAC)   ))**2 )
#else
                    LPRFK = ((XPR-(X0P(K)+YPR*YPR*KAPPA(K))
     *               )**2)/S1S(K) + ((ZD-(Z0(K)+YPR*TL(K)))*
     *               *2)/S2S(K)
                    DPRFK = SQRT( (XPR-(X0P(K)+YPR*YPR*
     *               KAPPA(K)))**2 + ((COS(ATAN(TL(K))))**2)
     *               *(ZD-(Z0(K)+YPR*TL(K)))**2 )
#endif

                                 IF (LPRFK.LT.NSIN) THEN
                                    NSIN = LPRFK
                                 END IF
                                 IF (DPRFK.LT.DISN) THEN
                                    DISN = DPRFK
                                 END IF
                              END IF
151                        CONTINUE
152                        CONTINUE
                           DISN = 10000.0*DISN
C                     convert to microns
C                     .....Jazelle processed PZXTRKS(TRK(VID(1)),CHARGE)
C                     ...(Bank ZXTRKS)
C                     .....Jazelle processed PZXTRKS(TRK(VID(2)),CHARGE)
C                     ...(Bank ZXTRKS)
CJSF                           CVEE = JZL$R(JZL$X(PZXTRKS)-35+(VID(1))*62)
CJSF     *                     + JZL$R(JZL$X(PZXTRKS)-35+(VID(2))*62)
                      CVEE=RTRK_ZXTRKS(1,VID(1))+RTRK_ZXTRKS(1,VID(2))
C                     .....Jazelle processed PZXTRKS(TRK(VID(1)),B3NORM)
C                     ...(Bank ZXTRKS)
CJSF                           B3N1 = JZL$R(JZL$X(PZXTRKS)+22+(VID(1))*62)
                           B3N1 = RTRK_ZXTRKS(50,VID(1))
C                     .....Jazelle processed PZXTRKS(TRK(VID(2)),B3NORM)
C                     ...(Bank ZXTRKS)
CJSF                           B3N2 = JZL$R(JZL$X(PZXTRKS)+22+(VID(2))*62)
                           B3N2 = RTRK_ZXTRKS(50,VID(2))
C                     .....Jazelle processed PZXTRKS(TRK(VID(1)),IMPACT3
C                     )...(Bank ZXTRKS)
CJSF                           I3N1 = JZL$R(JZL$X(PZXTRKS)+21+(VID(1))*62)
                           I3N1 = RTRK_ZXTRKS(49,VID(1))
C                     .....Jazelle processed PZXTRKS(TRK(VID(2)),IMPACT3
C                     )...(Bank ZXTRKS)
CJSF                           I3N2 = JZL$R(JZL$X(PZXTRKS)+21+(VID(2))*62)
                           I3N2 = RTRK_ZXTRKS(49,VID(2))
C                     *   if nvpass.eq.1 [
C                               *
C                     *    pZBoutl%(nsin) = nsin;
C                               *
C                     *    pZBoutl%(disn) = disn;
C                               *
C                     *    pZBoutl%(cvee) = cvee;
C                               *
C                     *    pZBoutl%(b3n1) = b3n1;
C                               *
C                     *    pZBoutl%(b3n2) = b3n2;
C                               *
C                     *    pZBoutl%(i3n1) = i3n1;
C                               *
C                     *    pZBoutl%(i3n2) = i3n2;
C                               *
C                     *   ]
C                               *
                        END IF
C                  most distant vee
C                  * ]   mcid check, only for MC study
C                         *
                     END IF
                  END IF
C             good vertex fit
               END IF
C          endif both tracks not rid yet
141         CONTINUE
142         CONTINUE
C       do loops over tracks
131      CONTINUE
132      CONTINUE
C   * if vfound.lt.0.5 [pZBoutl%(dvee) = 0.0];                    *
C   * if nvpass.eq.1 [                                            *
C   *  pZBoutl%(dve2) = dve2max;                                  *
C   *  pZBoutl%(ngam) = ngam;                                     *
C   *  pZBoutl%(nksh) = nksh;                                     *
C   *  pZBoutl%(nlam) = nlam;                                     *
C   * ]                                                           *
C    is the furthest V to be removed ?
         AGAIN = .FALSE.
         DVEE = DVEEMAX
         IF (VFOUND.GT.0.5) THEN
C      candidate K or lambda

cc            print *,' iv1,iv2=',iv1,iv2,' mvep=',mvep
cc            print *,' b3n1,b3n2=',b3n1,b3n2,' b3min=',b3min,
cc     >            ' cvee=',cvee,' disn=',disn,' disne=',disne
            IF (B3N1.GT.B3MIN .AND. B3N2.GT.B3MIN .AND. CVEE.EQ.0 .AND.
     *      DISN.GT.DISNE) THEN
cc               print *,' mvep=',mvep,' mkmin,mkmax=',mkmin,mkmax,
cc     >     ' dvee,dkmin=',dvee,dkmin
               IF (MVEP.GT.MKMIN .AND. MVEP.LT.MKMAX .AND. DVEE.GT.
     *         DKMIN) THEN
                  IF (RID(IV1).EQ.0) THEN
                     NRID = NRID + 1
                  END IF
                  IF (RID(IV2).EQ.0) THEN
                     NRID = NRID + 1
                  END IF
                  RID(IV1) = 10
C            flag kaons
                  RID(IV2) = 10
cc                  print *,' rid is set for iv1,iv2=',iv1,iv2
                  AGAIN = .TRUE.
               ELSE IF(((MLAM1.GT.MLMIN .AND. MLAM1.LT.MLMAX) .OR. (
     *         MLAM2.GT.MLMIN .AND. MLAM2.LT.MLMAX)) .AND. DVEE.GT.
     *         DLMIN) THEN
                  IF (RID(IV1).EQ.0) THEN
                     NRID = NRID + 1
                  END IF
                  IF (RID(IV2).EQ.0) THEN
                     NRID = NRID + 1
                  END IF
                  RID(IV1) = 20
C            flag lambdas
                  RID(IV2) = 20
                  AGAIN = .TRUE.
               END IF
            ELSE IF(RVEE.GT.RIMIN) THEN
               IF (MVGE.LT.MGMAX .AND. DISN.GT.DISNE .AND. CVEE.EQ.0)
     *         THEN
                  IF (RID(IV1).EQ.0) THEN
                     NRID = NRID + 1
                  END IF
                  IF (RID(IV2).EQ.0) THEN
                     NRID = NRID + 1
                  END IF
                  RID(IV1) = 30
C            flag gammas
                  RID(IV2) = 30
                  AGAIN = .TRUE.
               ELSE IF(I3N1.GT.I3MIN .AND. I3N2.GT.I3MIN) THEN
                  IF (RID(IV1).EQ.0) THEN
                     NRID = NRID + 1
                  END IF
                  IF (RID(IV2).EQ.0) THEN
                     NRID = NRID + 1
                  END IF
                  RID(IV1) = 40
C            flag interactions
                  RID(IV2) = 40
                  AGAIN = .TRUE.
               END IF
            END IF
            IF (AGAIN) THEN
               GOTO 120
            END IF
         END IF
C    pZBoutl%(nvpa) = nvpass;
         AXI(1) = IPX
         AXI(2) = IPY
         AXI(3) = IPZ
         AXF(1) = AXI(1) + PXJ/PTOTJ
         AXF(2) = AXI(2) + PYJ/PTOTJ
         AXF(3) = AXI(3) + PZJ/PTOTJ
         DO 161 I=1,NTRK,1
C      .....Jazelle processed PZXTRKS(TRK(ID(I)),IMPACT3)...(Bank ZXTRKS
C      )
CJSF            IMP3 = JZL$R(JZL$X(PZXTRKS)+21+(ID(I))*62)
            IMP3 = RTRK_ZXTRKS(49,ID(I))
            CALL ZVTRK(AXI,AXF,ID(I),TKI,TKV,TRDI,LODI,ANTA)
            IF (LODI.LT.0.0) THEN
               IMP3 = -IMP3
            END IF
            IF ((IMP3.GT.ICUTP .OR. IMP3.LT.ICUTN) .AND. RID(I).EQ.0)
     *      THEN
               NRID = NRID + 1
               RID(I) = 50
C          flag fail impact param cut
            END IF
161      CONTINUE
162      CONTINUE
      END IF
Cendif tidy>0
C swap out tracks to be rid of
C.....Jazelle processed ZVTOPL3(JETNO)(NTRK)...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO)))  = NTRK
      IBNK_ZVTOPL3(1,JETNO) = NTRK
      II = 0
      DO 171 I=NTRK,1,-1
C   .....Jazelle processed ZVTOPL3(JETNO)(TRKS(I),VNO)...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(I)*21)  = 0
         ITRK_ZVTOPL3(2,I,JETNO) = 0
         IF (RID(I).NE.0) THEN
            II = II+1
C       vno records that a track was rid of
C      .....Jazelle processed ZVTOPL3(JETNO)(TRKS(I),VNO)...(Bank ZVTOPL
C      3)
CJSF            JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(I)*21)  = -RID(I)
            ITRK_ZVTOPL3(2,I,JETNO) = -RID(I)
            DO 181 J=I,NTRK-II,1
               ID(J) = ID(J+1)
               RID(J) = RID(J+1)
181         CONTINUE
182         CONTINUE
         END IF
171   CONTINUE
172   CONTINUE
C redo the zboutl imp3 and ptrk
C* do i = 1,ntrk,1 [                                             *
C*  if i.le.20 [                                                 *
C*   pZBoutl%(imp3(i)) = pZxTrks%(trk(id(i)),impact3);           *
C*      call zvtrk(axi,axf,id(i),tki,tkv,trdi,lodi,anta);        *
C*                                                               *
C*    if lodi.lt.0.0 [pZBoutl%(imp3(i)) = -pZBoutl%(imp3(i));]   *
C*                                                               *
C*     cosl = cos(atan(pZxTrks%(trk(id(i)),hlxpar(3))));         *
C*   pZBoutl%(ptrk(i)) = 1/(pZxTrks%(trk(id(i)),hlxpar(2))*cosl);*
C*  ]                                                            *
C* ]                                                             *
C* do i = 1,20,1 [ put V track info in ZBOUTL                    *
C*   pZBoutl%(rid(i)) = rid(i);                                  *
C* ]                                                             *

      NTRK = NTRK-II
      IF (NTRK.LT.2) THEN
C   .....Jazelle processed ZVTOPL3(JETNO)(NVRT)...(Bank ZVTOPL3)
CJSF         JZL$I(JZL$X(ZVTOPL3(JETNO))+1)  = -1
          IBNK_ZVTOPL3(2, JETNO)=-1
C    not enough tracks !
      ELSE
         IF (ALGO.EQ.1) THEN
C       begin algo 1 by calling zvres
            SGLISS=ZVRES3(NTRK,ID,PXJ,PYJ,PZJ,PTOTJ,JETNO)
         ELSE
C       begin algo 2 by calling zvkin
            GAXI(1) = PXJ
            GAXI(2) = PYJ
            GAXI(3) = PZJ
            VPOS(1) = IPX
            VPOS(2) = IPY
            VPOS(3) = IPZ
            SGLISS=ZVKIN(NTRK,ID,VPOS,GAXI,JETNO)
CCCCCCC            print *,'The use of ZVKIN is not supported yet.'
         END IF
      END IF
      SGLISS=SLD$NORMAL
20    CONTINUE
      IF (SIGSEV(IAND(SGLISS,7)).GT.SGLLEV) THEN
CJSF         SGLISS=SGLIT(SGLISS, 'ZVTOP3' ,%DESCR(' ')  )
      END IF
      ZVTOP3=SGLISS
      RETURN
      END
