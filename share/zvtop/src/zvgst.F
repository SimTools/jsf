C--------------------------------------------------------->>>>ZVGST
      INTEGER FUNCTION ZVGST (NTRK,ID,VPOS,GAXI,JETNO,GRES,ST,CT,X0P,Z0,
     *TL,S1S,S2S,C)
      IMPLICIT NONE
#include "zxtrks.inc"
#include "zvkon3.inc"
      INTEGER NTRK
      INTEGER ID(30)
      REAL VPOS(3)
      REAL GAXI(3)
      INTEGER JETNO
      REAL GRES
      REAL*8 ST(30)
      REAL*8 CT(30)
      REAL*8 X0P(30)
      REAL*8 Z0(30)
      REAL*8 TL(30)
      REAL*8 S1S(30)
      REAL*8 S2S(30)
      REAL*8 C(30,9)
      INTEGER*4 SIGNAL$S_WARNING
      PARAMETER (SIGNAL$S_WARNING=3)
      INTEGER  SGLGBL, SGLCHK, SGLRTN, SGLHDL, SGLRMV, SGLGET, SGLLEV,
     *SGLIT, SGLISS, SIGSEV(0:7)/3,1,4,2,5,5,5,5/
      COMMON /SGLCOM/ SGLLEV
CDEC$ PSECT /SGLCOM/ NOSHR
      
      INTEGER*4 SLD$NORMAL
      PARAMETER (SLD$NORMAL=1)
C----------------------------------------------------------------------
C
C ZVGST -- find ghost track and put it in ZXTRKS(trk(NCHRG + 1)
C----------------------------------------------------------------------
C
C  Arguments
C  =========
C INPUT:
C   NTRK,ID(30)  - number of tracks and zxtrks indices ID(1-NTRK)
C   VPOS, GAXI, origin (IP) and initial guess of Ghost AXIs direction
C OUTPUT:
C   GAXI(3),GRES final ghost axis direction and width (um)
C   st(30)...c(30,9)  ZVTOP parameters swam to LODI along found ghost
C----------------------------------------------------------------------
C KEYWORDS: ZVTOP
C*--------------------------------------------------------------------*
C
C       OWNER: D. Jackson            CO-OWNER:
C     SECTION: ZXFIND
C
C                    Date            Explanation of Changes
C                ----------- ------------------------------------------
CVersion   0.00;  2 Feb 1998          Program creation date
CVersion   0.50;  8 Mar 1999          Copied to ZV3B area from ZV3
CVersion   0.51;  4 May 1999          sent to DUCS
C*--------------------------------------------------------------------*
      COMMON /ZXTRKS /ZXTRKS
      INTEGER ZXTRKS(-20:1)
CDEC$ PSECT /ZXTRKS/ NOSHR
      COMMON /MCPART /MCPART
      INTEGER MCPART(-20:5000)
CDEC$ PSECT /MCPART/ NOSHR
      COMMON /ZVKON3 /ZVKON3
      INTEGER ZVKON3(-20:1)
CDEC$ PSECT /ZVKON3/ NOSHR
      LOGICAL FIRST/.TRUE./,FLAG,AGAIN,MOVE
C POINTER   pPhBmFam   --> PHBM,
C
C           pPhBm      --> PHBM;
C POINTER  phchrg --> phchrg,
C
C          pZvTopL    --> ZVTOPL,
C*  POINTER   pZBoutL    --> ZBoutL;                          *
      INTEGER PRENT
C* POINTER  pZbTopL    --> ZBTOPL;                            *
      INTEGER JZBFND,JZPIDX,JZBLOC,JZBEXP,ZVTRK
      INTEGER MCID,MCPP(30),PTYP,PATYP,IPMC,IPFLAV,IPVTX
      INTEGER I,II,J,JJ,NZT,NCHRG,NGTRK,ERR,IER
      INTEGER NOFF,NOK,RETN,VID(50),NITR
      INTEGER INDEX(5),N,I1,J1,K1,NTODO,SOK
      INTEGER ZXSWIM,DUMMY
      INTEGER IFA,IF2,NDOF,NTRY
      REAL CHFA,CHF2,PROB,PRBG,CHIT,MU,OK
      REAL JANG,VANG,JANGD,VANGD,TRDISUM,TSUM,TMIN,PHIV,THETAV,ISTEP
      REAL TKX(30),TKY(30),PX(30),PY(30),PZ(30)
      REAL PTOTI,PXI,PYI,PZI
      REAL TKI(3),TKV(3),AXI(3),AXF(3),XIMP(3),VAX,VAY,VAZ,VMAG,GMAG
      REAL PXJ,PYJ,PZJ,PTOTJ,COSL,SINL,PTOT(30)
      REAL IPX,IPY,IPZ,TRDI,LODI,ANTA,B3NOR
      REAL VPOSE(3),VPOSSG(3),CHISQTK(50),PXYZ(3,50)
      REAL XVTXSG(6),XVTX(3),JETD(3),AJET,POT
      REAL DX,DY,CHIS(30),S1BS(30),S2BS(30)
      REAL RP,RB,X0
      REAL*8 S1,S2,CHI0(30),IMP2,CL,CH,DELZ,IMP(30),TKZ(30)
      REAL*8 GWID
      REAL*8 DTRK(30),DREF(30)
      REAL*8 XI(30),YI(30),ZI(30)
      REAL*8 CM(5,5),SING,PIVOT,ELM,RI(5),DET
      REAL*8 C1,C2,C3,C4,C5,C6,C7,C8,C9
      REAL*8 XD,YD,ZD,XPR,YPR,PRFI,PRFJ,DEE,DE,DEP(30)
      REAL*8 GWIDN,GWIDMAX,CHISQ
      REAL*8 GMIN,CMAX
      COMMON/SWIMINT/HLXINT(6),DHLXINT(15)
C DATA    pPhBmFam/0/;
C----------------------------------------------------------------------
      REAL *4 HLXINT,DHLXINT
      IF ((FIRST)) THEN
CJSF         SGLISS=JZPIDX('ZXTRKS',ZXTRKS)
CJSF         IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO   20
CJSF         SGLISS=JZPIDX('MCPART',MCPART)
CJSF         IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO 20
CJSF         SGLISS=JZBLOC('MCPART',PRENT)
CJSF         IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO 20
C   *  $CALL JZBFND( 'ZBOUTL',PZBOUTL) ERROR RETURN;             *
         FIRST = .FALSE.
C    open(unit=82,file='ZVgst',status='new');
      END IF
C.....Jazelle processed ZVKON3(1)(GINI)...(Bank ZVKON3)
CJSF      GWID = JZL$R(JZL$X(ZVKON3(1))+29) /10000.0
      GWID = ZVKON3_GINI/10000.0
C.....Jazelle processed ZVKON3(1)(GMIN)...(Bank ZVKON3)
CJSF      GMIN = JZL$R(JZL$X(ZVKON3(1))+30) /10000.0
      GMIN = ZVKON3_GMIN /10000.0
C.....Jazelle processed ZVKON3(1)(CMAX)...(Bank ZVKON3)
CJSF      CMAX = JZL$R(JZL$X(ZVKON3(1))+31)
      CMAX = ZVKON3_CMAX
C IP location
      IPX = VPOS(1)
      IPY = VPOS(2)
      IPZ = VPOS(3)
      AXI(1) = IPX
      AXI(2) = IPY
      AXI(3) = IPZ
      POT=3.14159265/2.0
      DX = IPX
      DY = IPY
C.....Jazelle processed ZXTRKS(1)(NCHRG)...(Bank ZXTRKS)
CJSF      NCHRG = JZL$I(JZL$X(ZXTRKS(1))+24)
      NCHRG = IBNK_ZXTRKS(9)
      DO 31 II=1,NTRK,1
         NZT = ID(II)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(4))...(Bank ZXTRKS)
CJSF         TKX(II) = JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(4)*1)
         TKX(II) = RTRK_ZXTRKS(24+4,NZT)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(5))...(Bank ZXTRKS)
CJSF         TKY(II) = JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(5)*1)
         TKY(II) = RTRK_ZXTRKS(24+5,NZT)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(6))...(Bank ZXTRKS)
CJSF         TKZ(II) = JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(6)*1)
         TKY(II) = RTRK_ZXTRKS(24+6,NZT)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),IMPACT3)...(Bank ZXTRKS)
CJSF         IMP(II) = JZL$R(JZL$X(ZXTRKS(1))+21+(NZT)*62)
         IMP(II) = RTRK_ZXTRKS(49,NZT)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),IMPACT)...(Bank ZXTRKS)
CJSF         IMP2 = JZL$R(JZL$X(ZXTRKS(1))-12+(NZT)*62)
         IMP2 = RTRK_ZXTRKS(24,NZT)
         DELZ = IPZ-TKZ(II)
C   3d delz
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(3))...(Bank ZXTRKS)
CJSF         CL = COS(ATAN(JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(3)*1) ))
         CL = COS(ATAN(RTRK_ZXTRKS(24+3,NZT)))
         DELZ = ABS(DELZ*CL)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(10))...(Bank ZXTRK
C   S)
CJSF         XD = IMP2/(JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(10)*1) /GWID**2 +
CJSF     *    1.0)
         XD = IMP2/(RTRK_ZXTRKS(30+10,NZT)/GWID**2+1.0)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(15))...(Bank ZXTRK
C   S)
CJSF         ZD = DELZ/(JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(15)*1) /GWID**2 +
CJSF     *    1.0)
         ZD = DELZ/(RTRK_ZXTRKS(30+10,NZT)/GWID**2 + 1.0)
         CH = XD**2/GWID**2 + ZD**2/GWID**2
         DEP(II) = GWID*SQRT(CH)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(10))...(Bank ZXTRK
C   S)
CJSF         CH = CH + (IMP2-XD)**2/JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(10)*
CJSF     *   1)
         CH = CH + (IMP2-XD)**2/RTRK_ZXTRKS(30+10,NZT)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(15))...(Bank ZXTRK
C   S)
CJSF         CH = CH + (DELZ-ZD)**2/JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(15)*
CJSF     *   1)
         CH = CH + (DELZ-ZD)**2/RTRK_ZXTRKS(30+15,NZT)
         CHI0(II) = CH
C   *    mcid = zxtrks%(trk(nzt),mcid);                          *
C   *    ptyp = 0;                                               *
C   *    mcpa(ii) = -2;                                          *
C   *    mcpp(ii) = -2;                                          *
C   *    if mcid.ne.0 [                                          *
C   *     ptyp = abs(mcpart(mcid)%(ptype))*mcpart(mcid)%(charge);*
C   *     prent = mcpart(mcid)%(parent);                         *
C   *     patyp = prent%(ptype);                                 *
C   *     $call BBTMTYP(mcid,ipmc,ipflav,ipvtx);                 *
C   *     mcpa(ii) = ipflav;                                     *
C   *     mcpp(ii) = abs(patyp);                                 *
C   *    ]                                                       *
31    CONTINUE
32    CONTINUE
C* $call jzbfnd('ZBTOPL', pZbTopL, dummy, jetNo);             *
C try MC cheat starting gaxi
C*  if pZbTopL%(mcd(1)).ne.0 [                                *
C*      gaxi(1) = pzbtopl%(mcdx(1))-ipx;                      *
C*      gaxi(2) = pzbtopl%(mcdx(2))-ipy;                      *
C*      gaxi(3) = pzbtopl%(mcdx(3))-ipz;                      *
C* ] else [                                                   *
C*      gaxi(1) = pzbtopl%(mcbx(1))-ipx;                      *
C*      gaxi(2) = pzbtopl%(mcbx(2))-ipy;                      *
C*      gaxi(3) = pzbtopl%(mcbx(3))-ipz;                      *
C* ]                                                         *
      VMAG = SQRT(GAXI(1)**2 + GAXI(2)**2 + GAXI(3)**2)
      GMAG = SQRT(GAXI(1)**2 + GAXI(2)**2 + GAXI(3)**2)
      GAXI(1) = GAXI(1)/VMAG
      GAXI(2) = GAXI(2)/VMAG
      GAXI(3) = GAXI(3)/VMAG
      VAX = GAXI(1)
      VAY = GAXI(2)
      VAZ = GAXI(3)
      JETD(1) = VAX
      JETD(2) = VAY
      JETD(3) = VAZ
      THETAV = ACOS(VAZ/1.0)
      PHIV = ACOS(VAX/SQRT(VAX**2+VAY**2))
      IF (VAY.LT.0.0) THEN
         PHIV = 6.283185-PHIV
      END IF
C if jetno.eq.1 [   otherwise overwrite the ghost track
C expand now and contract later
      NCHRG = NCHRG+1
CJSF      SGLISS=JZBEXP('ZXTRKS',1,NCHRG,ZXTRKS)
      IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO 20
C.....Jazelle processed ZXTRKS(1)(NCHRG)...(Bank ZXTRKS)
CJSF      JZL$I(JZL$X(ZXTRKS(1))+24)  = NCHRG
      IBNK_ZXTRKS(9) = NCHRG
C 
      NZT = NCHRG
      NGTRK = NTRK+1
      ID(NGTRK) = NZT
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(1))...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(1)*1)  = PHIV
      RTRK_ZXTRKS(24+1,NZT) = PHIV
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(2))...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(2)*1)  = 0.001
      RTRK_ZXTRKS(24+2,NZT) = 0.001
Chi momentum
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(3))...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(3)*1)  = VAZ/SQRT(VAX**2+VAY**
CJSF     *2)
      RTRK_ZXTRKS(24+3,NZT) = VAZ/SQRT(VAX**2+VAY**2)
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(4))...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(4)*1)  = IPX
      RTRK_ZXTRKS(24+4,NZT) = IPX
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(5))...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(5)*1)  = IPY
      RTRK_ZXTRKS(24+5,NZT) = IPY
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(6))...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(6)*1)  = IPZ
      RTRK_ZXTRKS(24+6,NZT) = IPZ
      DO 41 I=1,15
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(I))...(Bank ZXTRKS
C   )
CJSF         JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(I)*1)  = 0.0
	RTRK_ZXTRKS(30+I,NZT) = 0.0
41    CONTINUE
42    CONTINUE
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(1))...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(1)*1)  = (0.00001)**2
      RTRK_ZXTRKS(30+1, NZT) = (0.00001)**2
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(3))...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(3)*1)  = (0.00001)**2
      RTRK_ZXTRKS(30+3, NZT) = (0.00001)**2
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(6))...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(6)*1)  = (0.00001)**2
      RTRK_ZXTRKS(30+6, NZT) = (0.00001)**2
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(10))...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(10)*1)  = GWID**2
      RTRK_ZXTRKS(30+10, NZT) = GWID**2
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(15))...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(15)*1)  = GWID**2
      RTRK_ZXTRKS(30+15, NZT) = GWID**2
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),CHARGE)...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-35+(NZT)*62)  = 1
      RTRK_ZXTRKS(1, NZT) = 1
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),STATUS(4))...(Bank ZXTRKS)
CJSF      JZL$I(JZL$X(ZXTRKS(1))+12+(NZT)*62+(4)*1)  = 0
      ITRK_ZXTRKS(5+4, NZT) = 0
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),BNORM)...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-13+(NZT)*62)  = 0.0
      RTRK_ZXTRKS(23, NZT) = 0.0 
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),B3NORM)...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))+22+(NZT)*62)  = 0.0
      RTRK_ZXTRKS(50, NZT) = 0.0 
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),IMPACT)...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-12+(NZT)*62)  = 0.0
      RTRK_ZXTRKS(24, NZT) = 0.0 
C.....Jazelle processed ZXTRKS(1)(TRK(NZT),IMPACT3)...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))+21+(NZT)*62)  = 0.0
      RTRK_ZXTRKS(49, NZT) = 0.0 
      CT(NGTRK) = COS(PHIV-POT)
      ST(NGTRK) = SIN(PHIV-POT)
      Z0(NGTRK) = IPZ
      X0P(NGTRK) = 0.0
      TL(NGTRK) = VAZ/SQRT(VAX**2+VAY**2)
      S1S(NGTRK)=GWID**2
      S2S(NGTRK)=GWID**2/(COS(ATAN(TL(NGTRK))))**2
      DO 51 II=1,NTRK,1
         NZT = ID(II)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(3))...(Bank ZXTRKS)
CJSF         COSL = COS(ATAN(JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(3)*1) ))
	 COSL = COS(ATAN(RTRK_ZXTRKS(24+3,NZT)))
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(3))...(Bank ZXTRKS)
CJSF         SINL = SIN(ATAN(JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(3)*1) ))
         SINL = SIN(ATAN(RTRK_ZXTRKS(24+3,NZT)))
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(2))...(Bank ZXTRKS)
CJSF         PTOT(II) = 1/(JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(2)*1) *COSL)
         PTOT(II) = 1/(RTRK_ZXTRKS(24+2,NZT)*COSL)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(1))...(Bank ZXTRKS)
CJSF         PX(II) = PTOT(II)*COSL*COS(JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(
CJSF     *   1)*1) )
         PX(II) = PTOT(II)*COSL*COS(RTRK_ZXTRKS(24+1,NZT))
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(1))...(Bank ZXTRKS)
CJSF         PY(II) = PTOT(II)*COSL*SIN(JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(
CJSF     *   1)*1) )
         PY(II) = PTOT(II)*COSL*SIN(RTRK_ZXTRKS(24+1,NZT))
         PZ(II) = PTOT(II)*SINL
C    following needed for my chi**2 calc later
C   .....Jazelle processed ZXTRKS(1)(TRK(ID(II)),HLXPAR(4))...(Bank ZXTR
C   KS)
CJSF         XI(II) = JZL$R(JZL$X(ZXTRKS(1))-10+(ID(II))*62+(4)*1)
         XI(II) = RTRK_ZXTRKS(24+4,ID(II))
C   .....Jazelle processed ZXTRKS(1)(TRK(ID(II)),HLXPAR(5))...(Bank ZXTR
C   KS)
CJSF          YI(II) = JZL$R(JZL$X(ZXTRKS(1))-10+(ID(II))*62+(5)*1)
         YI(II) = RTRK_ZXTRKS(24+5,ID(II))
C   .....Jazelle processed ZXTRKS(1)(TRK(ID(II)),HLXPAR(6))...(Bank ZXTR
C   KS)
CJSF         ZI(II) = JZL$R(JZL$X(ZXTRKS(1))-10+(ID(II))*62+(6)*1)
         ZI(II) = RTRK_ZXTRKS(24+6,ID(II))
C   .....Jazelle processed ZXTRKS(1)(TRK(ID(II)),HLXPAR(1))...(Bank ZXTR
C   KS)
CJSF         CT(II) = COS(JZL$R(JZL$X(ZXTRKS(1))-10+(ID(II))*62+(1)*1) -POT)
         CT(II) = COS(RTRK_ZXTRKS(24+1,ID(II)) - POT )
C   .....Jazelle processed ZXTRKS(1)(TRK(ID(II)),HLXPAR(1))...(Bank ZXTR
C   KS)
CJSF         ST(II) = SIN(JZL$R(JZL$X(ZXTRKS(1))-10+(ID(II))*62+(1)*1) -POT)
         ST(II) = SIN(RTRK_ZXTRKS(24+1,ID(II))-POT)
         Z0(II) = ZI(II)
         X0P(II) = CT(II)*(XI(II)-DX) + ST(II)*(YI(II)-DY)
C   .....Jazelle processed ZXTRKS(1)(TRK(ID(II)),HLXPAR(3))...(Bank ZXTR
C   KS)
CJSF         TL(II) = JZL$R(JZL$X(ZXTRKS(1))-10+(ID(II))*62+(3)*1)
         TL(II) = RTRK_ZXTRKS(24+3,ID(II)) 
C   .....Jazelle processed ZXTRKS(1)(TRK(ID(II)),DHLXPAR(10))...(Bank ZX
C   TRKS)
CJSF         S1S(II) = JZL$R(JZL$X(ZXTRKS(1))-4+(ID(II))*62+(10)*1)
         S1S(II) = RTRK_ZXTRKS(30+10,ID(II))
C   .....Jazelle processed ZXTRKS(1)(TRK(ID(II)),DHLXPAR(15))...(Bank ZX
C   TRKS)
CJSF         S2S(II) = JZL$R(JZL$X(ZXTRKS(1))-4+(ID(II))*62+(15)*1)
         S2S(II) = RTRK_ZXTRKS(30+15,ID(II))
         S2S(II) = S2S(II)/(COS(ATAN(TL(II))))**2
         DTRK(II) = 0.0
         DREF(II) = 0.0
51    CONTINUE
52    CONTINUE
Cstart with 0.04 radian step size
      ISTEP = 0.04
      MOVE = .TRUE.
      NITR = 0
      NTRY = 1
C swim tracks to its dtrk
60    IF (MOVE) THEN
         DO 71 II=1,NTRK,1
            NZT = ID(II)
            IF (DTRK(II).GT.2.0) THEN
               DTRK(II) = 2.0
            END IF
            IF (DTRK(II).GT.0.1 .AND. ABS(DTRK(II)-DREF(II)).GT.0.1)
     *      THEN
C          swim to dtrk cm from track POCA
               XIMP(1) = DTRK(II)*PX(II)/PTOT(II)+TKX(II)
               XIMP(2) = DTRK(II)*PY(II)/PTOT(II)+TKY(II)
               XIMP(3) = DTRK(II)*PZ(II)/PTOT(II)+TKZ(II)
CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCc               SGLISS=ZXSWIM(1,OK,XIMP,ID(II))
               IF (OK.LT.0.5) THEN
                  DREF(II) = DTRK(II)
                  COSL = COS(ATAN(HLXINT(3)))
                  SINL = SIN(ATAN(HLXINT(3)))
                  PTOTI = 1/(HLXINT(2)*COSL)
                  PXI = PTOTI*COSL*COS(HLXINT(1))
                  PYI = PTOTI*COSL*SIN(HLXINT(1))
                  PZI = PTOTI*SINL
C             following needed for my chi**2 calc later
                  XI(II) = HLXINT(4)
                  YI(II) = HLXINT(5)
                  ZI(II) = HLXINT(6)
C             extraplote xyz to 2d poca
                  MU = PXI*(IPX-XI(II))+PYI*(IPY-YI(II))
                  MU = MU/(PXI**2 + PYI**2)
                  XI(II) = XI(II) + MU*PXI
                  YI(II) = YI(II) + MU*PYI
                  ZI(II) = ZI(II) + MU*PZI
                  CT(II) = COS(HLXINT(1)-POT)
                  ST(II) = SIN(HLXINT(1)-POT)
                  Z0(II) = ZI(II)
                  X0P(II) = CT(II)*(XI(II)-DX) + ST(II)*(YI(II)-DY)
                  TL(II) = HLXINT(3)
                  S1S(II) = DHLXINT(10)
                  S2S(II) = DHLXINT(15)
                  S2S(II) = S2S(II)/(COS(ATAN(TL(II))))**2
               END IF
            END IF
71       CONTINUE
72       CONTINUE
         RETN = 0
         GOTO   80
90       TMIN = TRDISUM
      END IF
Cif we moved theta, phiv with chi**2 calc
Citerate to find best vertex axis
      MOVE = .FALSE.
100   FLAG = .FALSE.
110   PHIV = PHIV + ISTEP
      RETN = 1
      GOTO 80
120   IF (TRDISUM.LT.TMIN) THEN
         TMIN = TRDISUM
         FLAG = .TRUE.
         MOVE = .TRUE.
         GOTO 110
      ELSE
         PHIV = PHIV - ISTEP
      END IF
      IF (.NOT.FLAG) THEN
130      PHIV = PHIV - ISTEP
         RETN = 2
         GOTO 80
140      IF (TRDISUM.LT.TMIN) THEN
            TMIN = TRDISUM
            MOVE = .TRUE.
            GOTO 130
         ELSE
            PHIV = PHIV + ISTEP
         END IF
      END IF
      AGAIN = .FALSE.
Cnow theta co-ordinate
      FLAG = .FALSE.
150   IF (THETAV.LT.2.705) THEN
C   keep theta in range for ZXFIT
         THETAV = THETAV + ISTEP
         RETN = 3
         GOTO 80
160      IF (TRDISUM.LT.TMIN) THEN
            AGAIN = .TRUE.
            TMIN = TRDISUM
            FLAG = .TRUE.
            MOVE = .TRUE.
            GOTO 150
         ELSE
            THETAV = THETAV - ISTEP
         END IF
      END IF
      IF (.NOT.FLAG) THEN
170      IF (THETAV.GT.0.437) THEN
C      keep theta in range for ZXFIT
            THETAV = THETAV - ISTEP
            RETN = 4
            GOTO 80
180         IF (TRDISUM.LT.TMIN) THEN
               AGAIN = .TRUE.
               TMIN = TRDISUM
               MOVE = .TRUE.
               GOTO 170
            ELSE
               THETAV = THETAV + ISTEP
            END IF
         END IF
      END IF
      IF (AGAIN) THEN
         GOTO 100
      END IF
C call here to make sure gwidmax,dtrk at best thetav,phiv
      RETN = 5
      GOTO 80
C iterate chi**2
190   TMIN = TRDISUM
      IF (ISTEP.GT.0.00015) THEN
         ISTEP = ISTEP/2.0
         GOTO 60
      END IF
C calculate gwid do all tracks within chi**2 < cmax
      IF (GWIDMAX.GT.0.0) THEN
         GWIDMAX = SQRT(GWIDMAX)
      END IF
      IF (GWIDMAX.GT.GMIN) THEN
         GWID = GWIDMAX
      ELSE
         GWID = GMIN
      END IF
C hence 3d norm to ghost at IP changes
      DO 201 II=1,NTRK,1
         NZT = ID(II)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),IMPACT)...(Bank ZXTRKS)
CJSF         IMP2 = JZL$R(JZL$X(ZXTRKS(1))-12+(NZT)*62)
         IMP2 = RTRK_ZXTRKS(24,NZT)
         DELZ = IPZ-TKZ(II)
C   3d delz
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),HLXPAR(3))...(Bank ZXTRKS)
CJSF         CL = COS(ATAN(JZL$R(JZL$X(ZXTRKS(1))-10+(NZT)*62+(3)*1) ))
         CL = COS(ATAN(RTRK_ZXTRKS(24+3,NZT)))
         DELZ = DELZ*CL
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(10))...(Bank ZXTRK
C   S)
CJSF         XD = IMP2/(JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(10)*1) /GWID**2 +
CJSF     *    1.0)
         XD = IMP2/(RTRK_ZXTRKS(30+10,NZT)/GWID**2 + 1.0)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(15))...(Bank ZXTRK
C   S)
CJSF         ZD = DELZ/(JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(15)*1) /GWID**2 +
CJSF     *    1.0)
         ZD = DELZ/(RTRK_ZXTRKS(30+15,NZT)/GWID**2 + 1.0)
         CH = XD**2/GWID**2 + ZD**2/GWID**2
         DEP(II) = GWID*SQRT(CH)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(10))...(Bank ZXTRK
C   S)
CJSF         CH = CH + (IMP2-XD)**2/JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(10)*
CJSF     *   1)
	 CH = CH + (IMP2-XD)**2/RTRK_ZXTRKS(30+10,NZT)
C   .....Jazelle processed ZXTRKS(1)(TRK(NZT),DHLXPAR(15))...(Bank ZXTRK
C   S)
CJSF         CH = CH + (DELZ-ZD)**2/JZL$R(JZL$X(ZXTRKS(1))-4+(NZT)*62+(15)*
CJSF     *   1)
         CH = CH + (DELZ-ZD)**2/RTRK_ZXTRKS(30+15,NZT)
         CHI0(II) = CH
201   CONTINUE
202   CONTINUE
      VAX = SIN(THETAV)*COS(PHIV)
      VAY = SIN(THETAV)*SIN(PHIV)
      VAZ = COS(THETAV)
C tried MC cheat ending gaxi
C  vmag = sqrt(gaxi(1)**2 + gaxi(2)**2 + gaxi(3)**2);
C
C  vax = gaxi(1)/vmag;
C
C  vay = gaxi(2)/vmag;
C
C  vaz = gaxi(3)/vmag;
C fill the final chis array
      RETN = 10
      GOTO 80
210   IF (NTRY.LT.2) THEN
         NTRY = NTRY + 1
         ISTEP = 0.04
         MOVE = .TRUE.
         GOTO 60
      END IF
      GRES = GWID
C*     pZBoutl%(gwid) = 10000.0*gwid;                         *
C fill ntuple track stuff
C*     axf(1) = axi(1) + vax;                                 *
C*     axf(2) = axi(2) + vay;                                 *
C*     axf(3) = axi(3) + vaz;                                 *
C*     trdisum = 0.0;                                         *
C*     nok = 0.0;                                             *
C*     chit = 0.0;                                            *
C*                                                            *
C*     chfa = -10.0;                                          *
C*     chf2 = -10.0;                                          *
C*                                                            *
C*     DO I = 1,20,1 [                                        *
C*        if i.le.ntrk  [                                     *
C*            tki(1) = tkx(i);                                *
C*            tki(2) = tky(i);                                *
C*            tki(3) = tkz(i);                                *
C*            tkv(1) = px(i);                                 *
C*            tkv(2) = py(i);                                 *
C*            tkv(3) = pz(i);                                 *
C*            call zvtrk(axi,axf,0,tki,tkv,trdi,lodi,anta);   *
C identify furthest and second furthest lodi
C*            if chis(i).gt.chfa [                            *
C*               chf2 = chfa; if2 = ifa;                      *
C*               chfa = chis(i); ifa = i;                     *
C*            ]                                               *
C*            elseif chis(i).gt.chf2 [                        *
C*               chf2 = chis(i); if2 = i;                     *
C*            ]                                               *
C*                                                            *
C*            nok = nok + 1.0;                                *
C   pZBoutl%(trdi(i)) = trdi;
C*            pZBoutl%(chis(i)) = chis(i);                    *
C*            chit = chit + chis(i);                          *
C*            trdisum = trdisum + trdi;                       *
C   pZBoutl%(lodi(i)) = lodi;
C*            pZBoutl%(mcpa(i)) = mcpa(i);                    *
C*            pZBoutl%(mcpp(i)) = mcpp(i);                    *
C*        ] else [                                            *
C   pZBoutl%(trdi(i)) = -1.0;
C   pZBoutl%(lodi(i)) = -10.0;
C*            pZBoutl%(mcpa(i)) = -1;                         *
C*            pZBoutl%(mcpp(i)) = -1;                         *
C*        ]                                                   *
C*      ]                                                     *
C*            pZBoutl%(tave) = trdisum/nok;                   *
C*            pZBoutl%(chit) = chit;                          *
C*          ndof = ntrk-2;                                    *
C*          if ndof.lt.1 [ndof = 1;]                          *
C*          prbg = prob(chit,ndof);                           *
C*            pZBoutl%(prbg) = prbg;                          *
C*                                                            *
C*                                                            *
C*            pZBoutl%(chfa) = chfa;                          *
C*            pZBoutl%(chf2) = chf2;                          *
C*            pZBoutl%(lofa) = pZBoutl%(lodi(ifa));           *
C*            pZBoutl%(trfa) = pZBoutl%(trdi(ifa));           *
C*            pZBoutl%(mcfa) = pZBoutl%(mcpa(ifa));           *
C fill zxtrks bank
      VMAG = SQRT(VAX**2 + VAY**2 + VAZ**2)
      VAX = VAX/VMAG
      VAY = VAY/VMAG
C make sure output gaxi not too crazy
C must be within 60deg of thrust and at least 25 deg from z
      VAZ = VAZ/VMAG
      IF ((GAXI(1)*VAX + GAXI(2)*VAY + GAXI(3)*VAZ).GT.0.5 .AND. ABS(
     *VAZ).LT.0.906) THEN
         GAXI(1) = VAX
         GAXI(2) = VAY
         GAXI(3) = VAZ
      END IF
      GOTO   220
C zxtrks algo
80    GWIDMAX = -100.0
      NITR = NITR + 1
      TRDISUM = 0.0
      VAX = SIN(THETAV)*COS(PHIV)
      VAY = SIN(THETAV)*SIN(PHIV)
      VAZ = COS(THETAV)
      CT(NGTRK) = COS(PHIV-POT)
      ST(NGTRK) = SIN(PHIV-POT)
      TL(NGTRK) = VAZ/SQRT(VAX**2+VAY**2)
      S1S(NGTRK)=GWID**2
      S2S(NGTRK)=GWID**2/(COS(ATAN(TL(NGTRK))))**2
      J = NGTRK
      DO 231 I=1,NTRK,1
C    try my own chi**2 calculation
         C1 = (CT(I)*CT(I)/S1S(I) + TL(I)*TL(I)*ST(I)*ST(I)/S2S(I) + CT(
     *   J)*CT(J)/S1S(J) + TL(J)*TL(J)*ST(J)*ST(J)/S2S(J))
         C2 = (ST(I)*ST(I)/S1S(I) + TL(I)*TL(I)*CT(I)*CT(I)/S2S(I) + ST(
     *   J)*ST(J)/S1S(J) + TL(J)*TL(J)*CT(J)*CT(J)/S2S(J))
         C3 = (1/S2S(I) + 1/S2S(J))
         C4 = CT(I)*ST(I)/S1S(I) - TL(I)*TL(I)*ST(I)*CT(I)/S2S(I) + CT(
     *   J)*ST(J)/S1S(J) - TL(J)*TL(J)*ST(J)*CT(J)/S2S(J)
         C5 = TL(I)*ST(I)/S2S(I) + TL(J)*ST(J)/S2S(J)
         C6 = -(TL(I)*CT(I)/S2S(I) + TL(J)*CT(J)/S2S(J))
         C7 = -(CT(I)*X0P(I)/S1S(I) + Z0(I)*TL(I)*ST(I)/S2S(I) + CT(J)*
     *   X0P(J)/S1S(J) + Z0(J)*TL(J)*ST(J)/S2S(J))
         C8 = (-ST(I)*X0P(I)/S1S(I) + Z0(I)*TL(I)*CT(I)/S2S(I) - ST(J)*
     *   X0P(J)/S1S(J) + Z0(J)*TL(J)*CT(J)/S2S(J))
         C9 = -(Z0(I)/S2S(I) + Z0(J)/S2S(J))
         CM(1,1) = C1
         CM(1,2) = C4
         CM(1,3) = C5
         CM(2,1) = C4
         CM(2,2) = C2
         CM(2,3) = C6
         CM(3,1) = C5
         CM(3,2) = C6
         CM(3,3) = C3
C    *****  copied this code from bmxinv, and densified *****
         SING=1.0
         N=3
         J1=1
            GO TO 243
241         J1=J1+1
243         IF(J1-(N).GT.0)GO TO 242
            INDEX(J1)=1
         GO TO 241
242      CONTINUE
         NTODO=N
250      PIVOT=0.0
         DO 261 J1=1,N
            IF((INDEX(J1).EQ.0))GOTO   270
            ELM = ABS(CM(J1,J1))
            IF((ELM.LE.PIVOT))GOTO 270
            PIVOT=ELM
            I1=J1
270         CONTINUE
261      CONTINUE
262      CONTINUE
         IF((PIVOT.EQ.0.0))GOTO   280
         INDEX(I1)=0
290      PIVOT=-CM(I1,I1)
         DO 301 J1=1,N
            IF((J1-I1))  310,  320,  330
320         CM(I1,J1)=1.0/PIVOT
            RI(J1)=0.0
            GOTO   340
310         ELM=-CM(I1,J1)
            GOTO   350
330         ELM=-CM(J1,I1)
350         RI(J1)=ELM/PIVOT
            IF((ELM.EQ.0.0))GOTO   360
            DO   370 K1=1,J1
370         CM(J1,K1)=CM(J1,K1)+ELM*RI(K1)
360         IF (J1.GT.I1) GOTO   380
390         CM(I1,J1)=RI(J1)
            GOTO 340
380         CM(J1,I1)=RI(J1)
340         CONTINUE
301      CONTINUE
302      CONTINUE
         NTODO = NTODO - 1
         IF((NTODO.NE.0))GOTO 250
400      DO   410 I1=1,N
         DO 410 J1=1,I1
         CM(I1,J1)=-CM(I1,J1)
410      CM(J1,I1)=CM(I1,J1)
         GOTO   420
C    *****  end of matrix inversion code *****
280      SING=0.0
420      IF (SING.GT.0.5) THEN
            XD = -(CM(1,1)*C7 + CM(2,1)*C8 + CM(3,1)*C9)
            YD = -(CM(1,2)*C7 + CM(2,2)*C8 + CM(3,2)*C9)
            ZD = -(CM(1,3)*C7 + CM(2,3)*C8 + CM(3,3)*C9)
         END IF
         XPR = CT(I)*XD + ST(I)*YD
         YPR = -ST(I)*XD + CT(I)*YD
C    distance along track form 2d doca to poca xd vertex
         DTRK(I) = (XD+DX-TKX(I))*PX(I)+(YD+DY-TKY(I))*PY(I) +(ZD-TKZ(I)
     *   )*PZ(I)
         DTRK(I) = DTRK(I)/PTOT(I)
         PRFI = ((XPR-X0P(I))**2)/S1S(I) + ((ZD-(Z0(I)+YPR*TL(I)))**2)/
     *   S2S(I)
         XPR = CT(J)*XD + ST(J)*YD
         YPR = -ST(J)*XD + CT(J)*YD
         PRFJ = ((XPR-X0P(J))**2)/S1S(J) + ((ZD-(Z0(J)+YPR*TL(J)))**2)/
     *   S2S(J)
         CHISQ = PRFI + PRFJ
C    behind IP
         DE = GWID*SQRT(PRFJ)
C    latter should not happen
C      my calc at POCA
         IF (((VAX*XD+VAY*YD+VAZ*(ZD-AXI(3))).LT.0.0 .OR.  CHISQ.GT.
     *   CHI0(I))) THEN
            IF (NTRY.EQ.2) THEN
               CHISQ = CHI0(I)
            ELSE
               CHISQ = 2.0*CHI0(I) - CHISQ
            END IF
C        try this
         ELSE IF(DE.GT.0.00001) THEN
            GWIDN = CHISQ*S1S(J)**2*(CHISQ/CMAX-1.0)/DE**2+S1S(J)
            IF (GWIDN.GT.GWIDMAX) THEN
               GWIDMAX = GWIDN
            END IF
C      *    max gwid for all chi**2 < 2                              *
C      *      de = sqrt((xpr-x0p(j))**2                              *
C      *            +(zd-(z0(j)+ypr*tl(j)))**2*cos(atan(tl(j)))**2); *
C      *      Dee = chisq*s1s(j)/de;                                 *
C      *      s2 = Dee**2/chisq-s1s(j);                              *
C      *      gwidn = (Dee**2/cmax-s2);                              *
         END IF
         CHIS(I) = CHISQ
         TRDISUM = TRDISUM + CHISQ
231   CONTINUE
232   CONTINUE
C end loop over tracks
C  write(*,*) 'end 1 t ';
      AJET = VAX*JETD(1)+VAY*JETD(2)+VAZ*JETD(3)
      IF (AJET.GE.1.0) THEN
         AJET = 1.0
      END IF
      IF (AJET.LE.-1.0) THEN
         AJET = -1.0
      END IF
      AJET = ACOS(AJET)
      AJET = ABS(AJET-0.02)**0.8
C try soft jet core weight
      TRDISUM = TRDISUM + (AJET/0.3)**2
      IF (RETN.EQ.0) GOTO 90
      IF (RETN.EQ.1) GOTO 120
      IF (RETN.EQ.2) GOTO 140
      IF (RETN.EQ.3) GOTO 160
      IF (RETN.EQ.4) GOTO 180
      IF (RETN.EQ.5) GOTO 190
      IF (RETN.EQ.10) GOTO 210
C  write(*,*) 'end 1 e ';
C*     pZBoutl%(nitr) = nitr;                                 *
C now fill output info
220   VAX = SIN(THETAV)*COS(PHIV)
      VAY = SIN(THETAV)*SIN(PHIV)
      VAZ = COS(THETAV)
      CT(NGTRK) = COS(PHIV-POT)
      ST(NGTRK) = SIN(PHIV-POT)
      TL(NGTRK) = VAZ/SQRT(VAX**2+VAY**2)
      S1S(NGTRK)=GWID**2
      S2S(NGTRK)=GWID**2/(COS(ATAN(TL(NGTRK))))**2
      IF (PHIV.LT.0.0) THEN
         PHIV = PHIV + 6.283185
      END IF
      IF (PHIV.GT.6.283185) THEN
         PHIV = PHIV - 6.283185
      END IF
C.....Jazelle processed ZXTRKS(1)(TRK(NCHRG),HLXPAR(1))...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-10+(NCHRG)*62+(1)*1)  = PHIV
      RTRK_ZXTRKS(24+1,NCHRG) = PHIV
C*    pZBoutl%(phiv) = phiv;                                  *
C.....Jazelle processed ZXTRKS(1)(TRK(NCHRG),HLXPAR(3))...(Bank ZXTRKS)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-10+(NCHRG)*62+(3)*1)  = VAZ/SQRT(VAX**2+
CJSF     *VAY**2)
      RTRK_ZXTRKS(24+3,NCHRG) = VAZ/SQRT(VAX**2+VAY**2)
C.....Jazelle processed ZXTRKS(1)(TRK(NCHRG),DHLXPAR(10))...(Bank ZXTRKS
C)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-4+(NCHRG)*62+(10)*1)  = GWID**2
      RTRK_ZXTRKS(30+10,NCHRG) = GWID**2
C.....Jazelle processed ZXTRKS(1)(TRK(NCHRG),DHLXPAR(15))...(Bank ZXTRKS
C)
CJSF      JZL$R(JZL$X(ZXTRKS(1))-4+(NCHRG)*62+(15)*1)  = GWID**2
      RTRK_ZXTRKS(30+15,NCHRG) = GWID**2
      DO 431 I=1,NGTRK,1
C   parameters for max finding in ZVKIN
         C(I,1) = CT(I)*CT(I)/S1S(I) + TL(I)*TL(I)*ST(I)*ST(I)/S2S(I)
         C(I,2) = ST(I)*ST(I)/S1S(I) + TL(I)*TL(I)*CT(I)*CT(I)/S2S(I)
         C(I,3) = 1/S2S(I)
         C(I,4) = CT(I)*ST(I)/S1S(I) - TL(I)*TL(I)*ST(I)*CT(I)/S2S(I)
         C(I,5) = TL(I)*ST(I)/S2S(I)
         C(I,6) = -TL(I)*CT(I)/S2S(I)
         C(I,7) = -(CT(I)*X0P(I)/S1S(I) + Z0(I)*TL(I)*ST(I)/S2S(I))
         C(I,8) = -ST(I)*X0P(I)/S1S(I) + Z0(I)*TL(I)*CT(I)/S2S(I)
         C(I,9) = -Z0(I)/S2S(I)
431   CONTINUE
432   CONTINUE
C end loop over tracks
      SGLISS=SLD$NORMAL
20    CONTINUE
      IF (SIGSEV(IAND(SGLISS,7)).GT.SGLLEV) THEN
CJSF          SGLISS=SGLIT(SGLISS, 'ZVGST' ,%DESCR(' ')  )
      END IF
      ZVGST=SGLISS
      RETURN
      END
