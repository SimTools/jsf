#include "zvtopoption.inc"
C--------------------------------------------------------->>>>ZVRES3
      INTEGER FUNCTION ZVRES3 (NTRK,ID,PXJ,PYJ,PZJ,PTOTJ,JETNO)
      IMPLICIT NONE
#include "zxtrks.inc"
#include "phbm.inc"
#include "zvkon3.inc"
#include "zvtopl3.inc"

      INTEGER NVMAXIMUM
      PARAMETER (NVMAXIMUM=100)

      INTEGER NTRK
      INTEGER ID(50)
      real rac
      REAL PXJ
      REAL PYJ
      REAL PZJ
      REAL PTOTJ
      INTEGER JETNO
      INTEGER   SIGNAL$S_WARNING
      PARAMETER (SIGNAL$S_WARNING=3)
CC      INTEGER  SGLGBL, SGLCHK, SGLRTN, SGLHDL, SGLRMV, SGLGET, SGLLEV,
CC     *SGLIT, SGLISS, SIGSEV(0:7)/3,1,4,2,5,5,5,5/
      INTEGER  SGLLEV,
     * SGLISS, SIGSEV(0:7)/3,1,4,2,5,5,5,5/
      COMMON /SGLCOM/ SGLLEV
CDEC$ PSECT /SGLCOM/ NOSHR
      INTEGER   SLD$NORMAL
      PARAMETER (SLD$NORMAL=1)
C----------------------------------------------------------------------
C
C ZVRES  -- resolve track V(r) into inclusive vertices
C----------------------------------------------------------------------
C
C  Arguments
C  =========
C   trsp   = track-spatial point relation
C   nresp  = no. of res. spatial points on tracks + ip
C   sp*r   = input spatial maxima size and location
C
C----------------------------------------------------------------------
C KEYWORDS: ZVTOP
C*--------------------------------------------------------------------*
C
C     OWNER: JACKSON, Dave          CO-OWNER: USHER, Tracy
C   SECTION: ZXFIND                    FILE: ZVRES3 PREPMORT
C
C                  Date            Explanation of Changes
C               ----------- -------------------------------------------
C  9  Feb 1995     Program creation date
C  30 May 1998     Max of nvert=7 protection, near end
C   4 May 1999     ZVTOP3 upgrades, original ALGO=1 here
CCurrent Version 3.00
C*--------------------------------------------------------------------*
      COMMON /ZVTOPL3 /ZVTOPL3
      INTEGER ZVTOPL3(-20:5000)
      COMMON/SWIMINT/HLXINT(6),DHLXINT(15)
      REAL *4 HLXINT,DHLXINT
      LOGICAL NEWV,OLDT,TRINVE
      LOGICAL RESM,FLAG,AGAIN
      INTEGER ZVTRK
      INTEGER NVERT,NISOL,ISOM
      INTEGER I,J,II,JJ,STEP,IMAX,VISOL(30),IV,NT,NTR
      INTEGER NTSPP,NV
CC      INTEGER VISSP(100),NTRV(30),VETR(20,30)
      INTEGER VISSP(100),NTRV(NVMAXIMUM),VETR(NVMAXIMUM,30)
      INTEGER ZXFIT,TMAX,NTEM,N2VER,NCHRG,IER,VID(50)
      INTEGER VER,TRA,SPO
      INTEGER NSTEP,NSP
      INTEGER ZNO,NZT,CRGZ,DONE(10),VORD(10),CRG(10),NEXV,FARV
      INTEGER MODE,NVRT,JSFZVTOP3_GUFLD,INDEX(5),N,I1,J1,K1,NTODO
      INTEGER TRK(30,30),NTSP(30)
Cspatial relations (+ no.) to each track
      INTEGER TRSP(30,30),NRSP(30)
Cres. ditto ditto...
      INTEGER NRESP,ISP,IRP,IT
      REAL SPX(400),SPY(400),SPZ(400),SPV(400)
Clocation and v at 2 trk
      REAL SPXR(100),SPYR(100),SPZR(100),SPVR(100)
Cres... ditto
      REAL LPNX(2,3),XI,YI,ZI,LAM,IPOS(9),RIPE,ZIPE,IPER(5,5)
      REAL PWID,KANG,KIPW,SWIM
      REAL POT,DX,DY,ST(30),CT(30),X0P(30),Z0(30)
      REAL RI(5),PIVOT,ELM,XPR,YPR,PRFI,PRFJ
      REAL LPRFI,LPRFJ,GMAX,MAX
      REAL VNEW,VOLD,VMIN,VM3D,ISTEP
      REAL B0(3),KAPPA(30),TL(30),S1S(30),S2S(30),CHIP(30),IMP(30)
      REAL C1,C2,C3,C4,C5,C6,C7,C8,C9,CM(5,5),SING,XD,YD,ZD
      REAL PHT(30),MAS(10),PXT,PYT,PZT,ETOT
      REAL AXI(3),AXF(3),TKI(3),TKV(3),TRD,LOD,ANTA
      REAL COSL,SINL,PTOT,PT,PX,PY,PZ,TRDI(30),LODI(30)
      REAL P1(10),P2(10),P3(10),ALPHA(10),DMAG,DLONG,DTRAN
      REAL DP1(10),DP2(10),DP3(10),DP4(10),DP5(10),DP6(10)
      REAL XISQ(10),NEAR,DIS(NVMAXIMUM)
      REAL VSIG(7),VTSG,VTR(30)
CC      REAL CHIV(30),VMINSP(100,100),MAXCHI
      REAL CHIV(NVMAXIMUM),VMINSP(100,100),MAXCHI
      REAL VPOS(3),VPOSSG(3),CHISQTK(50),PXYZ(3,50),CHISQ,XVTX(3)
      REAL XVTXSG(6),MAXS
      REAL IPX,IPY,IPZ,XCUT
      REAL VSG,ZVSIG3,VMINI,DIST
      REAL XC,YC,ZC,BMAX,VTEM(30)
      REAL DXC,DYC,DZC,VMAXI,VIJMIN,VRAT,RCUT
      REAL OK
      REAL S1I,S2I,S1M,S2M,S1F,S2F
      REAL QCOEF(30,2,3)
C  visol(30) -- 10 = isolated
C                0 = not done yet
C                1...7 = first to seventh vertex
C
C----------------------------------------------------------------------
CJSF      IF ((PPHBMFAM .EQ. 0)) THEN
CJSF         SGLISS=JZBLOC('PHBM', PPHBMFAM)
CJSF      END IF
CJSF      IF ((PZXTRKSFAM .EQ. 0)) THEN
CJSF         SGLISS=JZBLOC('ZXTRKS', PZXTRKSFAM)
CJSF      END IF
CJSF      IF ((PZVKONFAM .EQ. 0)) THEN
CJSF         SGLISS=JZBLOC('ZVKON3', PZVKONFAM)
CJSF      END IF
C.....Jazelle processed PPHBMFAM(JB$FORPT)...(Bank PHBM)
CJSF      PPHBM = JZL$I(JZL$X(PPHBMFAM)-1)
C.....Jazelle processed PZXTRKSFAM(JB$FORPT)...(Bank ZXTRKS)
CJSF      PZXTRKS = JZL$I(JZL$X(PZXTRKSFAM)-1)
C.....Jazelle processed PZVKONFAM(JB$FORPT)...(Bank ZVKON3)
CJSF      PZVKON = JZL$I(JZL$X(PZVKONFAM)-1)
C.....Jazelle processed PZVKON(MODE)...(Bank ZVKON3)
CJSF      MODE = JZL$I(JZL$X(PZVKON)+26)
      MODE = ZVKON3_MODE
C.....Jazelle processed PZVKON(RCUT)...(Bank ZVKON3)
CJSF      RCUT = JZL$R(JZL$X(PZVKON)+22)
      RCUT = ZVKON3_RCUT
C.....Jazelle processed PZVKON(XCUT)...(Bank ZVKON3)
CJSF      XCUT = JZL$R(JZL$X(PZVKON)+23)
      XCUT = ZVKON3_XCUT
C.....Jazelle processed PZVKON(KANG)...(Bank ZVKON3)
CJSF      KANG = JZL$R(JZL$X(PZVKON)+24)
      KANG = ZVKON3_KANG
C.....Jazelle processed PZVKON(KIPW)...(Bank ZVKON3)
CJSF      KIPW = JZL$R(JZL$X(PZVKON)+25)
      KIPW = ZVKON3_KIPW
C.....Jazelle processed PZVKON(PWID)...(Bank ZVKON3)
CJSF      PWID = JZL$R(JZL$X(PZVKON)+27)
      PWID = ZVKON3_PWID
C.....Jazelle processed PZVKON(SWIM)...(Bank ZVKON3)
CJSF      SWIM = JZL$R(JZL$X(PZVKON)+28)
      SWIM = ZVKON3_SWIM
C.....Jazelle processed PZVKON(RIPE)...(Bank ZVKON3)
CJSF      RIPE = JZL$R(JZL$X(PZVKON)+15)
      RIPE = ZVKON3_RIPE
C.....Jazelle processed PZVKON(ZIPE)...(Bank ZVKON3)
CJSF      ZIPE = JZL$R(JZL$X(PZVKON)+16)
      ZIPE = ZVKON3_ZIPE
C.....Jazelle processed PPHBM(BEAMPOS(1))...(Bank PHBM)
CJSF      IPX = JZL$R(JZL$X(PPHBM)+1+(1)*1)
      IPX = PHBM_BEAMPOS(1)
C.....Jazelle processed PPHBM(BEAMPOS(2))...(Bank PHBM)
CJSF      IPY = JZL$R(JZL$X(PPHBM)+1+(2)*1)
      IPY = PHBM_BEAMPOS(2)
C.....Jazelle processed PPHBM(BEAMPOS(3))...(Bank PHBM)
CJSF      IPZ = JZL$R(JZL$X(PPHBM)+1+(3)*1)
      IPZ = PHBM_BEAMPOS(3)
      VPOS(1) = IPX
      VPOS(2) = IPY
      VPOS(3) = IPZ
CJSF      SGLISS=GUFLD(VPOS,B0)
      SGLISS=JSFZVTOP3_GUFLD(VPOS,B0)
C B0 in Kgauss, 10-1 Tesla
Cfor ZVTOP co-ords keep ip at 0,0 in xy to be rot. invariant
Chence need to shift tracks by -dx,-dy
      DX = IPX
      DY = IPY
      NSTEP = 16
      NSP = 0
CJSF       CALL UFILL(SPV, 1, 400, -1.)
CJSF       CALL UFILL(NTSP, 1, 30, 0 )
CJSF       CALL UFILL(SPVR, 1, 30, 0.)
      DO 8920 I=1,400
         SPV(I)=-1.0
 8920 continue
      do 8921 i=1,30
         ntsp(i)=0
         spvr(i)=0.0
 8921  continue


C*******************************************************************
C The first part of code transferred in from ZVTOP.PREPMORT 1 May 99
C*******************************************************************
C begin algo 1
C if lepton mode find relevant parameters of first track
      POT=3.14159265/2.0
      IF (MODE.GT.0) THEN
C   .....Jazelle processed PZXTRKS(TRK(ID(1)),HLXPAR(3))...(Bank ZXTRKS)
CJSF         COSL = COS(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(ID(1))*62+(3)*1) ))
         COSL = COS(ATAN(RTRK_ZXTRKS(24+3,ID(1))))
C   .....Jazelle processed PZXTRKS(TRK(ID(1)),HLXPAR(3))...(Bank ZXTRKS)
CJSF         SINL = SIN(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(ID(1))*62+(3)*1) ))
         SINL = SIN(ATAN(RTRK_ZXTRKS(24+3,ID(1))))
C   .....Jazelle processed PZXTRKS(TRK(ID(1)),HLXPAR(2))...(Bank ZXTRKS)
CJSF         PTOT = 1/(JZL$R(JZL$X(PZXTRKS)-10+(ID(1))*62+(2)*1) *COSL)
         PTOT = 1/(RTRK_ZXTRKS(24+2,ID(1))*COSL)
C    3-momentum
C   .....Jazelle processed PZXTRKS(TRK(ID(1)),HLXPAR(1))...(Bank ZXTRKS)
CJSF         LPNX(1,1) = PTOT*COSL*COS(JZL$R(JZL$X(PZXTRKS)-10+(ID(1))*62+(
CJSF     *   1)*1) )
         LPNX(1,1) = PTOT*COSL*COS(RTRK_ZXTRKS(24+1,ID(1)))
C   .....Jazelle processed PZXTRKS(TRK(ID(1)),HLXPAR(1))...(Bank ZXTRKS)
CJSF         LPNX(1,2) = PTOT*COSL*SIN(JZL$R(JZL$X(PZXTRKS)-10+(ID(1))*62+(
CJSF     *   1)*1) )
         LPNX(1,2) = PTOT*COSL*SIN(RTRK_ZXTRKS(24+1,ID(1)))
         LPNX(1,3) = PTOT*SINL
C    xyz location at 2D POCA to IP
C   .....Jazelle processed PZXTRKS(TRK(ID(1)),HLXPAR(4))...(Bank ZXTRKS)
CJSF         XI = JZL$R(JZL$X(PZXTRKS)-10+(ID(1))*62+(4)*1)  -DX
         XI = RTRK_ZXTRKS(24+4,ID(1)) - DX
C   .....Jazelle processed PZXTRKS(TRK(ID(1)),HLXPAR(5))...(Bank ZXTRKS)
CJSF         YI = JZL$R(JZL$X(PZXTRKS)-10+(ID(1))*62+(5)*1)  -DY
         YI = RTRK_ZXTRKS(24+5,ID(1))
C   .....Jazelle processed PZXTRKS(TRK(ID(1)),HLXPAR(6))...(Bank ZXTRKS)
CJSF         ZI = JZL$R(JZL$X(PZXTRKS)-10+(ID(1))*62+(6)*1)
         ZI = RTRK_ZXTRKS(24+6,ID(1))
         LAM = (LPNX(1,1)*(-XI) + LPNX(1,2)*(-YI) + LPNX(1,3)*(IPZ-ZI))/
     *   PTOT**2
C    xyz location at Normal point (3D POCA) to IP
         LPNX(2,1) = XI + LAM*LPNX(1,1)
         LPNX(2,2) = YI + LAM*LPNX(1,2)
C    have defined plane now remove lepton if requested
         LPNX(2,3) = ZI + LAM*LPNX(1,3)
         IF (MODE.GT.1) THEN
            DO 21 I=1,29
               ID(I) = ID(I+1)
21          CONTINUE
22          CONTINUE
            NTRK = NTRK - 1
         END IF
C   mode 2 or 3
      END IF
Cmode 1 or 2 or 3
C check track momentum normal to normal point
C  write(*,*) 'check ', lpnx(1,1)*(lpnx(2,1)-ipx) +
C
C               lpnx(1,2)*(lpnx(2,2)-ipy) +
C
C               lpnx(1,3)*(lpnx(2,3)-ipz);
      IPOS(1) = 0.0
      IPOS(2) = 0.0
C.....Jazelle processed PPHBM(BEAMPOS(3))...(Bank PHBM)
CJSF      IPOS(3) = JZL$R(JZL$X(PPHBM)+1+(3)*1)
      IPOS(3) = PHBM_BEAMPOS(3)
      IPOS(4) = (RIPE/10000.0)**2
      IPOS(5) = 0.0
      IPOS(6) = (RIPE/10000.0)**2
      IPOS(7) = 0.0
      IPOS(8) = 0.0
      IPOS(9) = (ZIPE/10000.0)**2
Cz error event by event around 70 um
Csimple matrix inv
      IPER(1,1) = 1.0/IPOS(4)
      IPER(1,2) = IPOS(5)
      IPER(2,1) = IPOS(5)
      IPER(2,2) = 1.0/IPOS(6)
      IPER(1,3) = IPOS(7)
      IPER(3,1) = IPOS(7)
      IPER(2,3) = IPOS(8)
      IPER(3,2) = IPOS(8)
      IPER(3,3) = 1.0/IPOS(9)
C  $call bmxinv(iper,3,sing);
      IPOS(4) = IPER(1,1)
      IPOS(5) = IPER(1,2)
      IPOS(6) = IPER(2,2)
      IPOS(7) = IPER(1,3)
      IPOS(8) = IPER(2,3)
      IPOS(9) = IPER(3,3)
      NVRT = 0
C preset Vmax
      II = 0
      DO 31 I=1,30,1
         SPV(I) = -1.0
31    CONTINUE
32    CONTINUE
      DO 41 I=1,NTRK,1
C    the correct rotation to primed co-ords would be zxtrks%(hlxpar(1))
C   -
C    pi/2, however the pi/2 bit is just a global gauge trans...
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(3))...(Bank ZXTRKS)
CJSF         COSL = COS(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(3)*1) ))
         COSL = COS(ATAN(RTRK_ZXTRKS(24+3,ID(I)))) 
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(3))...(Bank ZXTRKS)
CJSF         SINL = SIN(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(3)*1) ))
         SINL = SIN(ATAN(RTRK_ZXTRKS(24+3,ID(I))))
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(2))...(Bank ZXTRKS)
CJSF         PTOT = 1/(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(2)*1) *COSL)
         PTOT = 1/(RTRK_ZXTRKS(24+2,ID(I))*COSL)
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(1))...(Bank ZXTRKS)
CJSF         PX = PTOT*COSL*COS(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(1)*1) )
         PX = PTOT*COSL*COS(RTRK_ZXTRKS(24+1,ID(I)))
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(1))...(Bank ZXTRKS)
CJSF         PY = PTOT*COSL*SIN(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(1)*1) )
         PY = PTOT*COSL*SIN(RTRK_ZXTRKS(24+1,ID(I)))
         PZ = PTOT*SINL
         II = II + 1
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(1))...(Bank ZXTRKS)
CJSF         CT(II) = COS(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(1)*1) -POT)
         CT(II) = COS(RTRK_ZXTRKS(24+1,ID(I))-POT)
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(1))...(Bank ZXTRKS)
CJSF         ST(II) = SIN(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(1)*1) -POT)
         ST(II) = SIN(RTRK_ZXTRKS(24+1,ID(I))-POT)
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(6))...(Bank ZXTRKS)
CJSF         Z0(II) = JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(6)*1)
         Z0(II) =RTRK_ZXTRKS(24+6,ID(I))
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(4))...(Bank ZXTRKS)
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(5))...(Bank ZXTRKS)
CJSF         X0P(II) = CT(II)*(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(4)*1) -
CJSF     *   DX) + ST(II)*(JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(5)*1) -DY)
         X0P(II) = CT(II)*(RTRK_ZXTRKS(24+4,ID(I))-DX) +
     >             ST(II)*(RTRK_ZXTRKS(24+5,ID(I))-DY)
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),CHARGE)...(Bank ZXTRKS)
CJSF         IF (JZL$R(JZL$X(PZXTRKS)-35+(ID(I))*62) .LT.0.0) THEN
C      .0009
         IF(RTRK_ZXTRKS(1,ID(I)).LT.0.0 ) THEN

C      .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(2))...(Bank ZXTR
C      KS)
CJSF            KAPPA(II) = -0.000149896*B0(3)*JZL$R(JZL$X(PZXTRKS)-10+(ID(
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
CJSF         TL(II) = JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(3)*1)
         TL(II) = RTRK_ZXTRKS(24+3,ID(I))
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),DHLXPAR(10))...(Bank ZXTRK
C   S)
CJSF         S1S(II) = JZL$R(JZL$X(PZXTRKS)-4+(ID(I))*62+(10)*1)
         S1S(II) = RTRK_ZXTRKS(30+10,ID(I))
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),DHLXPAR(15))...(Bank ZXTRK
C   S)
CJSF         S2S(II) = JZL$R(JZL$X(PZXTRKS)-4+(ID(I))*62+(15)*1)
         S2S(II) = RTRK_ZXTRKS(30+15,ID(I))
C    correct z error to be in z direction, not perp to track
         S2S(II) = S2S(II)/(COS(ATAN(TL(II))))**2
C    *** begin Aarons parabolic track error parameters ***
C    Initialize interpolation coeffs in case of swim fail.
         QCOEF(II,1,1) = SQRT(S1S(II))
C    A for s1
         QCOEF(II,1,2) = 0
C    B
         QCOEF(II,1,3) = 0
C    C
         QCOEF(II,2,1) = SQRT(S2S(II))
C    A for s2
         QCOEF(II,2,2) = 0
C    B
         QCOEF(II,2,3) = 0
C    C
         PT = SQRT(PX**2 + PY**2)
         IF (SWIM.GT.0.0001) THEN
C      ...if requested in ZVKON
C       Calculate errors at several points for interpolation.
C       Errors at IP
            S1I = SQRT(S1S(II))
            S2I = SQRT(S2S(II))
C       Errors 1cm in yprime direction
C      .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(4))...(Bank ZXTR
C      KS)
CJSF            VPOS(1) = JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(4)*1)  + PX/
CJSF     *      PT
            VPOS(1) = RTRK_ZXTRKS(24+4,ID(I))+PX/PT
C      .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(5))...(Bank ZXTR
C      KS)
CJSF            VPOS(2) = JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(5)*1)  + PY/
CJSF     *      PT
            VPOS(2) = RTRK_ZXTRKS(24+5,ID(I))+PY/PT
C      .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(6))...(Bank ZXTR
C      KS)
CJSF            VPOS(3) = JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(6)*1)  + PZ/
CJSF     *      PT
            VPOS(3) = RTRK_ZXTRKS(24+6,ID(I)) + PZ/PT
            CALL ZXSWIM(1,OK,VPOS,ID(I))
            IF (OK.GT.0.5) THEN
               GOTO   50
            END IF
            S1M = SQRT(DHLXINT(10))
            S2M = SQRT(DHLXINT(15))/COS(ATAN(HLXINT(3)))
C       Errors 2cm in yprime direction
C      .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(4))...(Bank ZXTR
C      KS)
CJSF            VPOS(1) = JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(4)*1)  + 2.0*
CJSF     *      PX/PT
            VPOS(1) = RTRK_ZXTRKS(24+4,ID(I))+2.0*PX/PT
C      .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(5))...(Bank ZXTR
C      KS)
CJSF            VPOS(2) = JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(5)*1)  + 2.0*
CJSF     *      PY/PT
            VPOS(2) = RTRK_ZXTRKS(24+5,ID(I))+2*PY/PT
C      .....Jazelle processed PZXTRKS(TRK(ID(I)),HLXPAR(6))...(Bank ZXTR
C      KS)
CJSf            VPOS(3) = JZL$R(JZL$X(PZXTRKS)-10+(ID(I))*62+(6)*1)  + 2.0*
CJSF     *      PZ/PT
            VPOS(3) = RTRK_ZXTRKS(24+6,ID(I)) + 2.0*PZ/PT
            CALL ZXSWIM(1,OK,VPOS,ID(I))
            IF (OK.GT.0.5) THEN
               GOTO 50
            END IF
            S1F = SQRT(DHLXINT(10))
            S2F = SQRT(DHLXINT(15))/COS(ATAN(HLXINT(3)))
C       Approx error growth as quadratic.
C       Error = A + B*Y + C*Y^2 -- Solve for A, B, C using 3 points.
            QCOEF(II,1,1) = S1I
C       A for s1
            QCOEF(II,1,3) = (S1F+S1I)/2.0 - S1M
C       C
            QCOEF(II,1,2) = (S1M-S1I) - QCOEF(II,1,3)
C       B
            QCOEF(II,2,1) = S2I
C       A for s2
            QCOEF(II,2,3) = (S2F+S2I)/2.0 - S2M
C       C
C       B
            QCOEF(II,2,2) = (S2M-S2I) - QCOEF(II,2,3)
50          CONTINUE
            QCOEF(II,1,1) = SWIM*QCOEF(II,1,1)
C       artificially narrow tubes
            QCOEF(II,1,2) = SWIM*QCOEF(II,1,2)
            QCOEF(II,1,3) = SWIM*QCOEF(II,1,3)
            QCOEF(II,2,1) = SWIM*QCOEF(II,2,1)
            QCOEF(II,2,2) = SWIM*QCOEF(II,2,2)
            QCOEF(II,2,3) = SWIM*QCOEF(II,2,3)
         END IF
         IF (SWIM.LT.-0.0001) THEN
C      ...if requested in ZVKON
            QCOEF(II,1,1) = ABS(SWIM)*QCOEF(II,1,1)
C       artificially narrow tubes
            QCOEF(II,2,1) = ABS(SWIM)*QCOEF(II,2,1)
         END IF
C    *** end Aarons parabolic track error parameters ***
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),B3NORM)...(Bank ZXTRKS)
CJSF         CHIP(II) = JZL$R(JZL$X(PZXTRKS)+22+(ID(I))*62) **2
         CHIP(II)=RTRK_ZXTRKS(50,ID(I))
C   .....Jazelle processed PZXTRKS(TRK(ID(I)),IMPACT)...(Bank ZXTRKS)
CJSF         IMP(II) = JZL$R(JZL$X(PZXTRKS)-12+(ID(I))*62)
         IMP(II) = RTRK_ZXTRKS(24,ID(I))
41    CONTINUE
42    CONTINUE
Cloop over zxtrks
C now loop over track pairs and IP
C ntrk+1 to allow for iptrack
      DO 61 I=1,NTRK+1,1
         DO 71 J=I+1,NTRK+1,1
            IF (J.LE.NTRK) THEN
               C1 = (CT(I)*CT(I)/S1S(I) + TL(I)*TL(I)*ST(I)*ST(I)/S2S(I)
     *          + CT(J)*CT(J)/S1S(J) + TL(J)*TL(J)*ST(J)*ST(J)/S2S(J))
               C2 = (ST(I)*ST(I)/S1S(I) + TL(I)*TL(I)*CT(I)*CT(I)/S2S(I)
     *          + ST(J)*ST(J)/S1S(J) + TL(J)*TL(J)*CT(J)*CT(J)/S2S(J))
               C3 = (1/S2S(I) + 1/S2S(J))
               C4 = CT(I)*ST(I)/S1S(I) - TL(I)*TL(I)*ST(I)*CT(I)/S2S(I)
     *         + CT(J)*ST(J)/S1S(J) - TL(J)*TL(J)*ST(J)*CT(J)/S2S(J)
               C5 = TL(I)*ST(I)/S2S(I) + TL(J)*ST(J)/S2S(J)
               C6 = -(TL(I)*CT(I)/S2S(I) + TL(J)*CT(J)/S2S(J))
               C7 = -(CT(I)*X0P(I)/S1S(I) + Z0(I)*TL(I)*ST(I)/S2S(I) +
     *         CT(J)*X0P(J)/S1S(J) + Z0(J)*TL(J)*ST(J)/S2S(J))
               C8 = (-ST(I)*X0P(I)/S1S(I) + Z0(I)*TL(I)*CT(I)/S2S(I) -
     *         ST(J)*X0P(J)/S1S(J) + Z0(J)*TL(J)*CT(J)/S2S(J))
               C9 = -(Z0(I)/S2S(I) + Z0(J)/S2S(J))
            END IF
C       if iptrack, coefficients differ
            IF (J.EQ.NTRK+1) THEN
               C1 = (CT(I)*CT(I)/S1S(I) + TL(I)*TL(I)*ST(I)*ST(I)/S2S(I)
     *          + IPER(1,1))
               C2 = (ST(I)*ST(I)/S1S(I) + TL(I)*TL(I)*CT(I)*CT(I)/S2S(I)
     *          + IPER(2,2))
               C3 = (1/S2S(I) + IPER(3,3))
               C4 = CT(I)*ST(I)/S1S(I) - TL(I)*TL(I)*ST(I)*CT(I)/S2S(I)
     *         + IPER(1,2)
               C5 = TL(I)*ST(I)/S2S(I) + IPER(1,3)
               C6 = -(TL(I)*CT(I)/S2S(I) + IPER(2,3))
C          ipos signs may not be right, but ipos1 = ipos2 = 0.0
               C7 = -(CT(I)*X0P(I)/S1S(I) + Z0(I)*TL(I)*ST(I)/S2S(I) -
     *         IPOS(1)*IPER(1,1) - IPOS(2)*IPER(1,2) - IPOS(3)*IPER(1,3)
     *         )
               C8 = (-ST(I)*X0P(I)/S1S(I) + Z0(I)*TL(I)*CT(I)/S2S(I) -
     *         IPOS(1)*IPER(1,2) - IPOS(2)*IPER(2,2) - IPOS(3)*IPER(2,3)
     *         )
               C9 = (-Z0(I)/S2S(I) - IPOS(1)*IPER(1,3) - IPOS(2)*IPER(2,
     *         3) - IPOS(3)*IPER(3,3))
            END IF
C       do the differentiation to find max. of f(i)f(j) here
            CM(1,1) = C1
            CM(1,2) = C4
            CM(2,1) = C4
            CM(1,3) = C5
            CM(3,1) = C5
            CM(2,2) = C2
            CM(2,3) = C6
            CM(3,2) = C6
            CM(3,3) = C3
C           $call bmxinv(cm,3,sing);
C         copied this code from bxxinv
            SING=1.0
            N=3
            J1=1
               GO TO 83
81             J1=J1+1
83             IF(J1-(N).GT.0)GO TO 82
               INDEX(J1)=1
            GO TO 81
82          CONTINUE
            NTODO=N
90          PIVOT=0.0
            DO 101 J1=1,N
               IF((INDEX(J1).EQ.0))GOTO   110
               ELM = ABS(CM(J1,J1))
               IF((ELM.LE.PIVOT))GOTO 110
               PIVOT=ELM
               I1=J1
110            CONTINUE
101         CONTINUE
102         CONTINUE
            IF((PIVOT.EQ.0.0))GOTO   120
            INDEX(I1)=0
130         PIVOT=-CM(I1,I1)
            DO 141 J1=1,N
               IF((J1-I1))  150,  160,  170
160            CM(I1,J1)=1.0/PIVOT
               RI(J1)=0.0
               GOTO   180
150            ELM=-CM(I1,J1)
               GOTO   190
170            ELM=-CM(J1,I1)
190            RI(J1)=ELM/PIVOT
               IF((ELM.EQ.0.0))GOTO   200
               DO   210 K1=1,J1
210            CM(J1,K1)=CM(J1,K1)+ELM*RI(K1)
200            IF (J1.GT.I1) GOTO   220
230            CM(I1,J1)=RI(J1)
               GOTO 180
220            CM(J1,I1)=RI(J1)
180            CONTINUE
141         CONTINUE
142         CONTINUE
            NTODO = NTODO - 1
            IF((NTODO.NE.0))GOTO 90
240         DO   250 I1=1,N
            DO 250 J1=1,I1
            CM(I1,J1)=-CM(I1,J1)
250         CM(J1,I1)=CM(I1,J1)
            GOTO   260
120         SING=0.0
260         IF (SING.GT.0.5) THEN
               XD = -(CM(1,1)*C7 + CM(2,1)*C8 + CM(3,1)*C9)
               YD = -(CM(1,2)*C7 + CM(2,2)*C8 + CM(3,2)*C9)
               ZD = -(CM(1,3)*C7 + CM(2,3)*C8 + CM(3,3)*C9)
C          use ZXFIT location
               DIST = SQRT((IPX-XD)**2 + (IPY-YD)**2 + (IPZ-ZD)**2)
C          avoid being too far off
               IF (J.LE.NTRK) THEN
                  IF ((XD*XD + YD*YD + (ZD-IPZ)**2) .GT. 16.0) THEN
                     XD = 0.0
                     YD = 0.0
                     ZD = IPZ
                  END IF
                  VPOS(1) = XD + DX
                  VPOS(2) = YD + DY
                  VPOS(3) = ZD
                  VPOSSG(1) = 10.0
                  VPOSSG(2) = 10.0
                  VPOSSG(3) = 10.0
                  VID(1) = ID(I)
                  VID(2) = ID(J)
                  JJ = 2
                  SGLISS=ZXFIT(1,JJ,VID,VPOS,VPOSSG,CHISQ,XVTX,XVTXSG,
     *            CHISQTK,PXYZ,IER)
C              $call zvfit(vpos,vpossg,jj,vid,0,chisq,xvtx,xvtxsg,chisqt
C            k,pxyz,ier);
                  XD = XVTX(1) - DX
C             relative to ipx = ipy = 0 ZVTOP co-ords
                  YD = XVTX(2) - DY
C             protect against -ve error matrix
                  ZD = XVTX(3)
                  IF (XVTXSG(1).LT.0 .OR. XVTXSG(3).LT.0 .OR. XVTXSG(6).
     *            LT.0) THEN
                     XD = 100 
                     YD = 100
                     ZD = 100
C                set it crazy so prfi>100
                  END IF
               END IF
               SGLISS=ZVSIG3(NTRK,IPOS,ST,CT,X0P,Z0,KAPPA,TL,QCOEF, PXJ,
     *         PYJ,PZJ,PTOTJ,MODE,LPNX,PWID,KANG,KIPW,XD,YD,ZD,VSG)
               VMAXI = VSG
               XPR = CT(I)*XD + ST(I)*YD
               YPR = -ST(I)*XD + CT(I)*YD
C          protect against floating overflow
#ifndef ORIGINAL
               RAC = ABS(1/(2*KAPPA(I)))
               PRFI = ((XPR-(X0P(I)+YPR*YPR*KAPPA(I)
     *  + YPR**4*KAPPA(I)**3 + 2*YPR**6*KAPPA(I)**5
     *  ))**2)/S1S(I) + ((
     *         ZD-(Z0(I)+RAC*TL(I)*ASIN(YPR/RAC) ))**2)/S2S(I)
#else
               PRFI = ((XPR-(X0P(I)+YPR*YPR*KAPPA(I)))**2)/S1S(I) + ((
     *         ZD-(Z0(I)+YPR*TL(I)))**2)/S2S(I)
#endif
               IF (PRFI.LT.100.0) THEN
                  PRFI = EXP(-0.5*PRFI)
               ELSE
                  PRFI = 0.0
               END IF
C         take test point, to check trajectory
C
C            yprt = 2.0;
C
C            zc = z0(i)+yprt*tl(i);
C
C            xprt = x0p(i)+
C
C          yprt*yprt*kappa(i);
C
C           (kappa(i)/abs(kappa(i)))*(abs(kappa(i))-sqrt(kappa(i)**2-ypr
C         t**2));
C               xc =  ct(i)*xprt - st(i)*yprt;
C
C               yc =  st(i)*xprt + ct(i)*yprt;
C
C             vpos(1) = xc;
C
C             vpos(2) = yc;
C
C             vpos(3) = zc;
C
C             jj = vid(1);
C
C             call blSwimTk(jj,vpos,trkx,trkt,doca);
C
C             doc2 = doca;
C
C             doc2z = vpos(3)-trkx(3);
C
C
               IF (J.LE.NTRK) THEN
                  XPR = CT(J)*XD + ST(J)*YD
                  YPR = -ST(J)*XD + CT(J)*YD
C             protect against floating overflow
               RAC = ABS(1/(2*KAPPA(J)))
#ifndef ORIGINAL
                  PRFJ = ((XPR-(X0P(J)+YPR*YPR*KAPPA(J)
     * + YPR**4*KAPPA(J)**3 + 2*YPR**6*KAPPA(J)**5
     *    ))**2)/S1S(J) + (
     *    (ZD-(Z0(J)+RAC*TL(J)*ASIN(YPR/RAC)))**2)/S2S(J)
#else
                PRFJ = ((XPR-(X0P(J)+YPR*YPR*KAPPA(J)))**2)/S1S(J) + (
     *            (ZD-(Z0(J)+YPR*TL(J)))**2)/S2S(J)
#endif
                  IF (PRFJ.LT.100.0) THEN
                     PRFJ = EXP(-0.5*PRFJ)
                  ELSE
                     PRFJ = 0.0
                  END IF
               ELSE
                  PRFJ = IPOS(4)*(XD-IPOS(1))**2 + 2.0*IPOS(5)*(XD-IPOS(
     *            1))*(YD-IPOS(2)) + IPOS(6)*(YD-IPOS(2))**2 + 2.0*IPOS(
     *            7)*(XD-IPOS(1))*(ZD-IPOS(3)) + 2.0*IPOS(8)*(YD-IPOS(2)
     *            )*(ZD-IPOS(3)) + IPOS(9)*(ZD-IPOS(3))**2
                  IF (PRFJ.LT.100.0) THEN
                     PRFJ = EXP(-0.5*PRFJ)
                  ELSE
                     PRFJ = 0.0
                  END IF
               END IF
               IF (PRFI*PRFJ.GT.0.0) THEN
                  LPRFI = -2.0*LOG(PRFI)
                  LPRFJ = -2.0*LOG(PRFJ)
               END IF
C          use swam ZXFIT chi**2 for Aaron
               IF (SWIM.GT.0.0001) THEN
                  IF (J.LE.NTRK) THEN
                     LPRFI = CHISQTK(1)
                     LPRFJ = CHISQTK(2)
                  END IF
               END IF
               IF (LPRFI.LT.XCUT .AND. LPRFJ.LT.XCUT .AND. PRFI*PRFJ.GT.
     *         0.0 .AND. VMAXI.GT.0.001) THEN
                  if( nsp .ge.400) then
        print *,' zvres3.F: nsp exceeds 400.  This points are neglected'
             else if( ntsp(i).ge.30 .or. ntsp(j).ge.30 ) then
        print *,' zvres3.F: ntsp(i) or ntsp(j) is greater than 30',
     >   'i=',i,' ntsp(i)=',ntsp(i),' j=',j,' ntsp(j)=',ntsp(j),
     >   ' Further points are discarded.'
             else
                  NSP = NSP + 1
                  SPX(NSP) = XD
                  SPY(NSP) = YD
                  SPZ(NSP) = ZD
                  SPV(NSP) = VMAXI
                  NTSP(I) = NTSP(I) + 1
                  NTSP(J) = NTSP(J) + 1
                  TRK(I,NTSP(I)) = NSP
                  TRK(J,NTSP(J)) = NSP
             endif
               END IF
            END IF
C       2 track max found, sing>0.5
71       CONTINUE
72       CONTINUE
C    end do loop over track pairs
61    CONTINUE
62    CONTINUE
C end do loop over track pairs
C now resolve max for each track

      DO 271 IT=1,NTRK+1,1
C   loop over tracks + ip
         GMAX = 0.0
C    global max on track
C    no. of res. spatial points on track
         NRSP(IT) = 0
280      MAX = -1.0
         NRSP(IT) = NRSP(IT) + 1
         if( nrsp(it) .gt. 30 ) then
            print *,' Fatal error in ZVRES3 .. NRSP(IT)>30'
            stop
         endif
C   loop over track's spatial points
	if( ntsp(it) .le. 0 ) goto 300
         DO 291 ISP=1,NTSP(IT),1
C      flag as 0 when done
            IF (TRK(IT,ISP).NE.0) THEN
               IF (SPV(TRK(IT,ISP)) .GT. MAX) THEN
                  IMAX = ISP
C            tracks max
                  MAX = SPV(TRK(IT,ISP))
                  TRSP(IT,NRSP(IT)) = TRK(IT,ISP)
C            actual spat. point
               END IF
            END IF
291      CONTINUE
292      CONTINUE
C   loop over spatial points on track
         IF (NRSP(IT).EQ.1) THEN
            GMAX = MAX
         END IF
	if( IMAX.LE.0) then
	  print *,' Fatal error in zvres3 .. IMAX<0 '
	  print *,' IT=',IT,' NTSP(IT)=',NTSP(IT)
	  stop
           endif
         TRK(IT,IMAX) = 0
         IF (MAX.LT.0.001 .OR. MAX.LT.(0.1*GMAX)) THEN
C      must be >frac of big max
            NRSP(IT) = NRSP(IT) - 1
            GOTO   300
         END IF
C     goto :nextr:;  temporarily put this in, should reproduce old resul
C   t
C    if it .eq. (ntrk+1) [goto :iptrk:;] quit after biggest max for ip
         VNEW = MAX
         RESM = .TRUE.
         DO 311 IRP=1,(NRSP(IT)-1),1
C      is new max res. from previous max
            VOLD = SPV(TRSP(IT,IRP))
            IF (VNEW.GT.VOLD) THEN
               VMIN = VOLD
            ELSE
               VMIN=VNEW
            END IF
            DXC = (SPX(TRSP(IT,IRP))-SPX(TRSP(IT,NRSP(IT))))/NSTEP
            DYC = (SPY(TRSP(IT,IRP))-SPY(TRSP(IT,NRSP(IT))))/NSTEP
            DZC = (SPZ(TRSP(IT,IRP))-SPZ(TRSP(IT,NRSP(IT))))/NSTEP
C       prevent having to find vmin if vertices are within 10 um
            DIST = NSTEP*SQRT(DXC**2 + DYC**2 +DZC**2)
            IF (DIST.LT.0.001) THEN
               VRAT = 1.0
            ELSE
               VMINI = VMIN
               DO 321 STEP=1,NSTEP,1
                  XC = SPX(TRSP(IT,NRSP(IT))) + STEP*DXC
                  YC = SPY(TRSP(IT,NRSP(IT))) + STEP*DYC
                  ZC = SPZ(TRSP(IT,NRSP(IT))) + STEP*DZC
                  SGLISS=ZVSIG3(NTRK,IPOS,ST,CT,X0P,Z0,KAPPA,TL,QCOEF,
     *            PXJ,PYJ,PZJ,PTOTJ,MODE,LPNX,PWID,KANG,KIPW,XC,YC,ZC,
     *            VSG)
                  IF (VSG .LT. VMINI) THEN
                     VMINI = VSG
                  END IF
321            CONTINUE
322            CONTINUE
C         enddo over steps
               VRAT = VMINI/VMIN
            END IF
C      calc. vrat
            IF (VRAT.GT.RCUT) THEN
               RESM = .FALSE.
            END IF
311      CONTINUE
312      CONTINUE
C   enddo over resol max.
         IF (.NOT.RESM) THEN
            NRSP(IT) = NRSP(IT) - 1
         END IF
         GOTO 280
300      CONTINUE
271   CONTINUE
272   CONTINUE
C endloop over tracks + ip
C :iptrk:
Cfill resolved spatial max. arrays
      NRESP = 1
      DO 331 ISP=1,NSP,1
C   use logical resm for convenience
         RESM = .FALSE.
         DO 341 IT=1,NTRK+1,1
            DO 351 IRP=1,NRSP(IT)
               IF (TRSP(IT,IRP).EQ.ISP) THEN
                  RESM = .TRUE.
                  TRSP(IT,IRP) = NRESP
               END IF
351         CONTINUE
352         CONTINUE
341      CONTINUE
342      CONTINUE
360      CONTINUE
         IF (RESM .AND. SPV(ISP).GT.0.001) THEN
C      ************ 3D iterative bit ****************
            XC=SPX(ISP)
            YC=SPY(ISP)
            ZC=SPZ(ISP)
            VM3D = VMAXI
C      start with 4 um step size
            ISTEP = 0.0004
370         FLAG = .FALSE.
380         XC = XC + ISTEP
            SGLISS=ZVSIG3(NTRK,IPOS,ST,CT,X0P,Z0,KAPPA,TL,QCOEF, PXJ,
     *      PYJ,PZJ,PTOTJ,MODE,LPNX,PWID,KANG,KIPW,XC,YC,ZC,VSG)
            IF (VSG.GT.VM3D) THEN
               VM3D = VSG
               FLAG = .TRUE.
               GOTO 380
            ELSE
               XC = XC - ISTEP
            END IF
            IF (.NOT.FLAG) THEN
390            XC = XC - ISTEP
               SGLISS=ZVSIG3(NTRK,IPOS,ST,CT,X0P,Z0,KAPPA,TL,QCOEF, PXJ,
     *         PYJ,PZJ,PTOTJ,MODE,LPNX,PWID,KANG,KIPW,XC,YC,ZC,VSG)
               IF (VSG.GT.VM3D) THEN
                  VM3D = VSG
                  GOTO 390
               ELSE
                  XC = XC + ISTEP
               END IF
            END IF
            AGAIN = .FALSE.
C      now y co-ordinate
            FLAG = .FALSE.
400         YC = YC + ISTEP
            SGLISS=ZVSIG3(NTRK,IPOS,ST,CT,X0P,Z0,KAPPA,TL,QCOEF, PXJ,
     *      PYJ,PZJ,PTOTJ,MODE,LPNX,PWID,KANG,KIPW,XC,YC,ZC,VSG)
            IF (VSG.GT.VM3D) THEN
               AGAIN = .TRUE.
               VM3D = VSG
               FLAG = .TRUE.
               GOTO 400
            ELSE
               YC = YC - ISTEP
            END IF
            IF (.NOT.FLAG) THEN
410            YC = YC - ISTEP
               SGLISS=ZVSIG3(NTRK,IPOS,ST,CT,X0P,Z0,KAPPA,TL,QCOEF, PXJ,
     *         PYJ,PZJ,PTOTJ,MODE,LPNX,PWID,KANG,KIPW,XC,YC,ZC,VSG)
               IF (VSG.GT.VM3D) THEN
                  AGAIN = .TRUE.
                  VM3D = VSG
                  GOTO 410
               ELSE
                  YC = YC + ISTEP
               END IF
            END IF
C      now z co-ordinate
            FLAG = .FALSE.
420         ZC = ZC + ISTEP
            SGLISS=ZVSIG3(NTRK,IPOS,ST,CT,X0P,Z0,KAPPA,TL,QCOEF, PXJ,
     *      PYJ,PZJ,PTOTJ,MODE,LPNX,PWID,KANG,KIPW,XC,YC,ZC,VSG)
            IF (VSG.GT.VM3D) THEN
               AGAIN = .TRUE.
               VM3D = VSG
               FLAG = .TRUE.
               GOTO 420
            ELSE
               ZC = ZC - ISTEP
            END IF
            IF (.NOT.FLAG) THEN
430            ZC = ZC - ISTEP
               SGLISS=ZVSIG3(NTRK,IPOS,ST,CT,X0P,Z0,KAPPA,TL,QCOEF, PXJ,
     *         PYJ,PZJ,PTOTJ,MODE,LPNX,PWID,KANG,KIPW,XC,YC,ZC,VSG)
               IF (VSG.GT.VM3D) THEN
                  AGAIN = .TRUE.
                  VM3D = VSG
                  GOTO 430
               ELSE
                  ZC = ZC + ISTEP
               END IF
            END IF
            IF (AGAIN) THEN
               GOTO 370
            END IF
            IF (ISTEP.GT.0.00015) THEN
               ISTEP = ISTEP/2.0
               GOTO 370
            END IF
            XD=XC
            YD=YC
C      find absolute max of V
            ZD=ZC
C      has this spatial max already been found
440         SGLISS=ZVSIG3(NTRK,IPOS,ST,CT,X0P,Z0,KAPPA,TL,QCOEF, PXJ,
     *      PYJ,PZJ,PTOTJ,MODE,LPNX,PWID,KANG,KIPW,XD,YD,ZD,VSG)
            DO 451 I=1,NRESP-1,1
               IF (SQRT((SPXR(I)-XD)**2 + (SPYR(I)-YD)**2 + (SPZR(I)-ZD)
     *         **2).LT.0.0005) THEN
C            if old spatial point
                  DO 461 IT=1,NTRK+1,1
C               assign tracks to it
                     DO 471 IRP=1,NRSP(IT)
                        IF (TRSP(IT,IRP).EQ.NRESP) THEN
                           TRSP(IT,IRP) = I
                        END IF
471                  CONTINUE
472                  CONTINUE
461               CONTINUE
462               CONTINUE
                  GOTO   480
               END IF
451         CONTINUE
452         CONTINUE
            SPXR(NRESP) = XD
            SPYR(NRESP) = YD
            SPZR(NRESP) = ZD
            SPVR(NRESP) = VSG
            NRESP = NRESP + 1
480         CONTINUE
         END IF
331   CONTINUE
332   CONTINUE
C*******************************************************************
C  the original ZVRES code started here, above from old ZVTOP
C*******************************************************************
C set resolution here
      NRESP = NRESP - 1
      DO 491 I=1,100,1
         VISSP(I) = 0
491   CONTINUE
492   CONTINUE
      DO 8501 I=1, NVMAXIMUM, 1
         NTRV(I)=0
         CHIV(I)=0
 8501 CONTINUE

      DO 501 I=1,30,1
         VISOL(I) = 0
c         NTRV(I) = 0
c         CHIV(I) = 0.0
c         DO 511 J=1,20,1
         DO 511 J=1,NVMAXIMUM,1
            VETR(J,I) = 0
511      CONTINUE
512      CONTINUE
501   CONTINUE
502   CONTINUE
C.....Jazelle processed PPHBM(BEAMPOS(1))...(Bank PHBM)
CJSF      IPX = JZL$R(JZL$X(PPHBM)+1+(1)*1)
      IPX = PHBM_BEAMPOS(1)
C.....Jazelle processed PPHBM(BEAMPOS(2))...(Bank PHBM)
CJSF      IPY = JZL$R(JZL$X(PPHBM)+1+(2)*1)
      IPY = PHBM_BEAMPOS(2)
C.....Jazelle processed PPHBM(BEAMPOS(3))...(Bank PHBM)
CJSF      IPZ = JZL$R(JZL$X(PPHBM)+1+(3)*1)
      IPZ = PHBM_BEAMPOS(3)
      NVERT = 0
      NISOL = 0
C no. of isolated max > cut
C**********************************************************************
C loop over spatial points, fill resolution matrix Vminsp
C**********************************************************************
      ISOM = 0
      DO 521 I=1,NRESP,1
         IF (VISSP(I).NE.10) THEN
            DO 531 J=1,I-1,1
               IF (VISSP(J).NE.10) THEN
                  VMINI = SPVR(I)
                  DXC = (SPXR(J)-SPXR(I))/NSTEP
                  DYC = (SPYR(J)-SPYR(I))/NSTEP
                  DZC = (SPZR(J)-SPZR(I))/NSTEP
C             prevent having to find vminsp if vertices are within 10 um
C
                  DIST = NSTEP*SQRT(DXC**2 + DYC**2 +DZC**2)
                  IF (DIST.LT.0.001) THEN
                     VMINSP(I,J) = 1.0
                  ELSE
                     DO 541 STEP=1,NSTEP,1
                        XC = SPXR(I) + STEP*DXC
                        YC = SPYR(I) + STEP*DYC
                        ZC = SPZR(I) + STEP*DZC
                        SGLISS=ZVSIG3(NTRK,IPOS,ST,CT,X0P,Z0,KAPPA,TL,
     *                  QCOEF, PXJ,PYJ,PZJ,PTOTJ,MODE,LPNX,PWID,KANG,
     *                  KIPW,XC,YC,ZC,VSG)
                        IF (VSG .LT. VMINI) THEN
                           VMINI = VSG
                        END IF
541                  CONTINUE
542                  CONTINUE
C               enddo over steps
                     VIJMIN = SPVR(I)
                     IF (SPVR(J).LT.SPVR(I)) THEN
                        VIJMIN = SPVR(J)
                     END IF
                     IF (VIJMIN.LT.0.00001) THEN
                        VIJMIN = 0.00001
                     END IF
                     VMINSP(I,J) = VMINI/VIJMIN
C               temp
C
C                   pdv(1) = dist;
C
C                   pdv(2) = vmini/vijmin;
C
C                   if spvr(j)<spvr(i) [pdv(3) = spvr(j); pdv(4) = spvr(
C               i);]
C                   if spvr(j)>spvr(i) [pdv(3) = spvr(i); pdv(4) = spvr(
C               j);]
C                   $call NTUPme2(INT(4), pdv(1), int(3)) error return;
                  END IF
                  VMINSP(J,I) = DIST
               END IF
531         CONTINUE
532         CONTINUE
         END IF
521   CONTINUE
522   CONTINUE
C  now find resolved vertices within rcut
Chave already cut against spvr<0.01
Cdo i = 1,nresp,1 [ if spvr(i).lt.0.01 [vissp(i) = 10; nisol = nisol + 1
C;];]
      VMAXI = 0.0
      IMAX = 0
      DO 551 I=1,NRESP,1
         IF (VISSP(I) .EQ. 0) THEN
            IF (SPVR(I) .GT. VMAXI) THEN
               BMAX = SPVR(I)
               VMAXI = SPVR(I)
               IMAX = I
            END IF
         END IF
551   CONTINUE
552   CONTINUE
Cloop over spat. points

      IF (IMAX.EQ.0) THEN
         GOTO   560
      END IF
      VISSP(IMAX) = NVERT + 1
570   CONTINUE
      NVERT = NVERT + 1
580   NEWV = .FALSE.
      DO 591 J=1,NRESP,1
         IF (VISSP(J) .EQ. NVERT) THEN
            IV = J
            DO 601 I=1,NRESP,1
               IF (VISSP(I) .EQ. 0) THEN
                  IF (I.GT.IV) THEN
                     VRAT = VMINSP(I,IV)
                  ELSE
                     VRAT = VMINSP(IV,I)
                  END IF
C            is the sp. pt. part of nvertth vertex ?
                  IF (VRAT.GT.RCUT) THEN
                     VISSP(I) = NVERT
                     NEWV = .TRUE.
                  END IF
               END IF
601         CONTINUE
602         CONTINUE
         END IF
591   CONTINUE
592   CONTINUE
      IF (NEWV) THEN
         GOTO 580
      END IF
Cnow look for next vertex if any
      VMAXI = 0.0
      IMAX = 0
      DO 611 I=1,NRESP,1
         IF (VISSP(I) .EQ. 0 .AND. SPVR(I) .GT. VMAXI) THEN
            VMAXI = SPVR(I)
            IMAX = I
         END IF
611   CONTINUE
612   CONTINUE
      IF (IMAX.EQ.0) THEN
         GOTO 560
      ELSE
         VISSP(IMAX) = NVERT + 1
         GOTO 570
      END IF
560   CONTINUE
C**********************************************************************
Cnow loop over tracks/vertices and find which tracks belong to them
C**********************************************************************
      IF( NVERT .GT. NVMAXIMUM ) THEN
         print *,' Fatal error in ZVRES3 ..'
         print *,' Number of candidate vertices (',NVERT,
     >    ')  is larger than buffersize (',NVMAXIMUM,').'
         print *,' Please increase array size of NTRV, NVERT.'
         stop
      endif

      DO 621 NT=1,NTRK,1
         DO 631 NTSPP=1,NRSP(NT),1
            DO 641 NV=1,NVERT,1
         if( TRSP(NT,NTSPP).LE.0.OR.TRSP(NT,NTSPP).GT.100) THEN
                  print *,' nt,ntspp=',nt,ntspp,' trsp=',
     >       trsp(nt,ntspp),' this point is neglected.'
                  goto 641
               endif

               IF (VISSP(TRSP(NT,NTSPP)).EQ.NV) THEN
C            track is in vertex
                  OLDT = .FALSE.
C            check not already there
                  DO 651 I=1,NTRV(NV),1
                     IF (VETR(NV,I).EQ.NT) THEN
                        OLDT = .TRUE.
                     END IF
651               CONTINUE
652               CONTINUE
                  IF (.NOT.OLDT) THEN
                     NTRV(NV) = NTRV(NV) + 1
                     VETR(NV,NTRV(NV)) = NT
                  END IF
               END IF
C         if trk sp point is in vertex
641         CONTINUE
642         CONTINUE
C      loop over verts.
631      CONTINUE
632      CONTINUE
C   loop over trk sp points
621   CONTINUE
622   CONTINUE
Cloop over tracks
Cand where is ip track ?
      DO 661 I=1,NRSP(NTRK+1),1
         DO 671 NV=1,NVERT,1
            IF (VISSP(TRSP(NTRK+1,I)).EQ.NV) THEN
C         ip is in vertex
               VISOL(NTRK+1) = NV
               GOTO   680
            END IF
671      CONTINUE
672      CONTINUE
661   CONTINUE
662   CONTINUE
680   IF (VISOL(NTRK+1).EQ.0) THEN
         VISOL(NTRK+1) = NVERT + 1
         NVERT = NVERT + 1
         NTRV(NVERT) = 0
      END IF
C**********************************************************************
Cnow fit vertices with ZXFIT and do chi**2 trimming
Cfirst get zxtrk nos.
C**********************************************************************
C.....Jazelle processed PZXTRKS(NCHRG)...(Bank ZXTRKS)
CJSF      NCHRG = JZL$I(JZL$X(PZXTRKS)+24)
      NCHRG = IBNK_ZXTRKS(9)
      NTEM = NVERT

      DO 691 NV=1,NVERT,1
         IF (NTRV(NV) .LT. 2) THEN
            NTEM = NTEM - 1
         END IF
691   CONTINUE
692   CONTINUE
      N2VER = NVERT
      DO 701 NV=1,NVERT,1
710      IF (NTRV(NV) .LT. 2) THEN
            N2VER = N2VER - 1
         ELSE
            DO 721 NT=1,NTRV(NV),1
               VID(NT) = ID(VETR(NV,NT))
721         CONTINUE
722         CONTINUE
            VPOS(1) = 0.0
            VPOS(2) = 0.0
            VPOS(3) = 0.0
            VPOSSG(1) = 10.0
            VPOSSG(2) = 10.0
            VPOSSG(3) = 10.0
            SGLISS=ZXFIT(1,NTRV(NV),VID,VPOS,VPOSSG,CHISQ,XVTX,XVTXSG,
     *      CHISQTK,PXYZ,IER)
C       protect against -ve error matrix
            IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO   730
            IF (XVTXSG(1).LT.0 .OR. XVTXSG(3).LT.0 .OR. XVTXSG(6).LT.0)
     *      THEN
               CHISQTK(1) = XCUT + 1.0
C          force to fail chi**2 trimming
            END IF
            CHIV(NV) = CHISQ
            MAXCHI = 0.0
            DO 741 NT=1,NTRV(NV),1
               IF (CHISQTK(NT).GT.MAXCHI) THEN
                  MAXCHI = CHISQTK(NT)
                  TMAX = NT
               END IF
741         CONTINUE
742         CONTINUE
            IF (MAXCHI.GT.XCUT) THEN
C         drop this track
               VETR(NV,TMAX) = VETR(NV,NTRV(NV))
C         swap out replaced track
               NTRV(NV) = NTRV(NV) - 1
               GOTO 710
            END IF
         END IF
C   more than 1 track in vertex
701   CONTINUE
702   CONTINUE
Cloop over vertices
C**********************************************************************
Cnow decide on track ambiguities by starting with highest spat. max in v
Cert
C**********************************************************************
750   MAXS = -1.0
      DO 761 NT=1,NTRK,1
         IF (NRSP(NT) .NE. 100) THEN
            DO 771 NTSPP=1,NRSP(NT),1
C         make sure track is still in vertex after chi**2 trimming
               TRINVE = .FALSE.
               if( trsp(nt,ntspp).le.0 ) goto 771
               if( vissp(trsp(nt,ntspp)).le.0 ) goto 771
               DO 781 NTR=1,NTRV(VISSP(TRSP(NT,NTSPP))),1
                  IF (VETR(VISSP(TRSP(NT,NTSPP)),NTR) .EQ. NT) THEN
                     TRINVE = .TRUE.
                  END IF
781            CONTINUE
782            CONTINUE
C
C
C         | IBM does not like this form...
C
C         |   if (ntrv(vissp(trsp(nt,ntspp))).gt.1 .or.
C
C         |      vissp(trsp(nt,ntspp)).eq.visol(ntrk+1))
C
C         |      .and. spvr(trsp(nt,ntspp)). gt. maxs .and. trinve .eq.
C         .true. [
C          Require:
C            at least 2 trk vert...
C             ... or in ip
               IF ((NTRV(VISSP(TRSP(NT,NTSPP))) .GT. 1 .OR. VISSP(TRSP(
     *         NT,NTSPP)) .EQ. VISOL(NTRK+1)) .AND. SPVR(TRSP(NT,NTSPP))
     *          .GT. MAXS .AND. TRINVE) THEN
                  MAXS = SPVR(TRSP(NT,NTSPP))
                  VER = VISSP(TRSP(NT,NTSPP))
                  TRA = NT
                  SPO = NTSPP
                  VTSG = SPVR(TRSP(NT,NTSPP))
               END IF
771         CONTINUE
772         CONTINUE
         END IF
761   CONTINUE
762   CONTINUE
      IF (MAXS .LT. 0.0) THEN
         GOTO   790
C   found next biggest max, remove track from other verts
      ELSE
         IF (NRSP(TRA).GT.1) THEN
            DO 801 NV=1,NVERT,1
               IF (NV .NE. VER) THEN
                  DO 811 NT=1,NTRV(NV),1
                     IF (VETR(NV,NT) .EQ. TRA) THEN
                        VETR(NV,NT) = VETR(NV,NTRV(NV))
C                  swap out track
                        NTRV(NV) = NTRV(NV) - 1
                     END IF
811               CONTINUE
812               CONTINUE
               END IF
801         CONTINUE
802         CONTINUE
         END IF
         NRSP(TRA) = 100
C   flag track as being done
         VISOL(TRA) = VISSP(TRSP(TRA,SPO))
C   this track in vtx with sp pnt
         VTR(TRA) = VTSG
C   with this value of V
      END IF
      GOTO 750
C**********************************************************************
Cre-sort vertices if at least two tracks or ip
C**********************************************************************
790   NV = 1
820   CONTINUE
      IF (NTRV(NV).LT.2 .AND. NV.NE.VISOL(NTRK+1)) THEN
C   unless ip is in it
         NTRV(NV) = NTRV(NVERT)
         DO 831 NT=1,NTRK+1,1
            IF (VISOL(NT).EQ.NV) THEN
               VISOL(NT) = 10
            END IF
C      isolated
            IF (VISOL(NT).EQ.NVERT) THEN
               VISOL(NT) = NV
            END IF
C      and reassign trks too
831      CONTINUE
832      CONTINUE
         NVERT = NVERT - 1
      ELSE
         NV = NV + 1
      END IF
      IF (NV .LE. NVERT) THEN
         GOTO 820
      END IF
C do this later...
C make sure iptrack has visol(ntrk+1) = 1
      IF (VISOL(NTRK+1).GE.10) THEN
         VISOL(NTRK+1) = 1
         NVERT = NVERT + 1
         DO 841 I=1,NTRK,1
            IF (VISOL(I).EQ.1) THEN
               VISOL(I) = NVERT
            END IF
841      CONTINUE
842      CONTINUE
      END IF
      IF (VISOL(NTRK+1).NE.1) THEN
         DO 851 I=1,NTRK,1
            VTEM(I) = VISOL(I)
            IF (VISOL(I).EQ.1) THEN
               VTEM(I) = VISOL(NTRK+1)
            END IF
            IF (VISOL(I).EQ.VISOL(NTRK+1)) THEN
               VTEM(I) = 1
            END IF
851      CONTINUE
852      CONTINUE
         DO 861 I=1,NTRK,1
            VISOL(I) = VTEM(I)
861      CONTINUE
862      CONTINUE
         VISOL(NTRK+1) = 1
      END IF
      NISOL = 0
      DO 871 NT=1,NTRK,1
         IF (VISOL(NT).EQ.10 .OR. VISOL(NT).EQ.0) THEN
            NISOL = NISOL + 1
         ELSE
            IF (VISOL(NT).LE.7) THEN
C          avoid Nicolo's crash, max 7 vrts
               VSIG(VISOL(NT)) = VTR(NT)
            ELSE
               VISOL(NT) = 0
               NISOL = NISOL + 1
               IF (NVERT.GT.7) THEN
                  NVERT = 7
               END IF
            END IF
         END IF
871   CONTINUE
872   CONTINUE
C  $call zbtmc(ntrk,id,visol,nvert,vsig,int(nisol),jetno);
C just add the old ZBTMC code here to write out to zbtopl
C do ii = 1,ntrk,1 [if visol(ii).gt.nvert [visol(ii)=0;]]
C check which tracks we swapped out early in ZVTOP, put them back
C.....Jazelle processed ZVTOPL3(JETNO)(NTRK)...(Bank ZVTOPL3)
CJSF      DO 881 II=1,JZL$I(JZL$X(ZVTOPL3(JETNO))) ,1
      DO 881 II=1,IBNK_ZVTOPL3(1,JETNO) ,1
C   .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),VNO)...(Bank ZVTOPL3)
CJSF         IF (JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(II)*21) .LT.0) THEN
         IF (ITRK_ZVTOPL3(2,II,JETNO).LT.0) THEN
            NTRK = NTRK + 1
            DO 891 J=NTRK,II+1,-1
               VISOL(J) = VISOL(J-1)
               ID(J) = ID(J-1)
891         CONTINUE
892         CONTINUE
C      .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),VNO)...(Bank ZVTOP
C      L3)
CJSF            VISOL(II) = JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(II)*21)
            VISOL(II) = ITRK_ZVTOPL3(2,II,JETNO)
C      .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),ZID)...(Bank ZVTOP
C      L3)
CJSF            ID(II) = JZL$I(JZL$X(ZVTOPL3(JETNO))+128+(II)*21)
            ID(II) = ITRK_ZVTOPL3(1,II,JETNO)
         ELSE IF(VISOL(II).GT.NVERT) THEN
            VISOL(II)=0
         END IF
881   CONTINUE
882   CONTINUE
C.....Jazelle processed ZVTOPL3(JETNO)(NVRT)...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+1)  = NVERT
      IBNK_ZVTOPL3(2,JETNO) = NVERT
C.....Jazelle processed ZVTOPL3(JETNO)(NISO)...(Bank ZVTOPL3)
Cnow decide if track vertex is primary etc
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+2)  = NISOL
      IBNK_ZVTOPL3(3,JETNO) = NVERT
      DO 901 NV=0,NVERT,1
         NT = 0
         NZT = 0
         CRGZ = 0
         DO 911 JJ=1,NTRK,1
            NT = NT+1
            IF (VISOL(NT).EQ.NV) THEN
               NZT = NZT + 1
C         .....Jazelle processed PZXTRKS(TRK(ID(JJ)),CHARGE)...(Bank ZXT
C         RKS)
CJSF               CRGZ = CRGZ + JZL$R(JZL$X(PZXTRKS)-35+(ID(JJ))*62)
               CRGZ = CRGZ + RTRK_ZXTRKS(1,ID(JJ)) 
C         .....Jazelle processed ZVTOPL3(JETNO)(TRKS(NT),ZID)...(Bank ZV
C         TOPL3)
CJSF               JZL$I(JZL$X(ZVTOPL3(JETNO))+128+(NT)*21)  = ID(JJ)
               ITRK_ZVTOPL3(1,NT,JETNO) = ID(JJ)
C         .....Jazelle processed ZVTOPL3(JETNO)(TRKS(NT),VNO)...(Bank ZV
C         TOPL3)
CJSF               JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(NT)*21)  = NV
               ITRK_ZVTOPL3(2,NT,JETNO) = NV
C         change later
               VID(NZT) = ID(JJ)
C          zx trk no.
               PHT(NZT) = NT
C          ph trk no.
C         .....Jazelle processed PZXTRKS(TRK(ID(JJ)),STATUS(4))...(Bank
C         ZXTRKS)
CJSF               JZL$I(JZL$X(PZXTRKS)+12+(ID(JJ))*62+(4)*1)  = 0
               ITRK_ZXTRKS(5+4,ID(JJ)) = 0
            END IF
C       right vertex
911      CONTINUE
912      CONTINUE
C   loop over jet tracks
         IF (NV.GT.0) THEN
C       vertex characteristics
            NTRV(NV) = NZT
            IF (NZT.EQ.0) THEN
               MAS(NV) = 0.0
            END IF
            IF (NZT.EQ.1) THEN
               MAS(NV) = 0.14
            END IF
            IF (NZT.GE.2) THEN
               VPOS(1) = 0.0
               VPOS(2) = 0.0
               VPOS(3) = 0.0
               VPOSSG(1) = 10.0
               VPOSSG(2) = 10.0
               VPOSSG(3) = 10.0
               SGLISS=ZXFIT(1,NZT,VID,VPOS,VPOSSG,CHISQ,XVTX,XVTXSG,
     *         CHISQTK,PXYZ,IER)
C          protect against -ve error matrix
               IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO
     *         730
               IF (XVTXSG(1).LT.0 .OR. XVTXSG(3).LT.0 .OR. XVTXSG(6).LT.
     *         0) THEN
C            .....Jazelle processed ZVTOPL3(JETNO)(NVRT)...(Bank ZVTOPL3
C            )
CJSF                  JZL$I(JZL$X(ZVTOPL3(JETNO))+1)  = NVERT-1
                  IBNK_ZVTOPL3(2,JETNO)=NVERT-1
                  XVTX(1) = 1000
C             remove the vertex by forcing it to be furthest
                  XVTX(2) = 1000
                  XVTX(3) = 1000
               END IF
C          find invariant mass of tracks
               PXT= 0.0
               PYT= 0.0
               PZT= 0.0
               ETOT= 0.0
               DO 921 JJ=1,NZT
C             loop over pZxTrks
C            .....Jazelle processed PZXTRKS(TRK(VID(JJ)),HLXPAR(3))...(B
C            ank ZXTRKS)
CJSF                  COSL = COS(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(VID(JJ))*62+(
CJSF     *            3)*1) ))
                  COSL = COS(ATAN(RTRK_ZXTRKS(24+3,VID(JJ))))
C            .....Jazelle processed PZXTRKS(TRK(VID(JJ)),HLXPAR(3))...(B
C            ank ZXTRKS)
CJSF                  SINL = SIN(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(VID(JJ))*62+(
CJSF     *            3)*1) ))
                  SINL = SIN(ATAN(RTRK_ZXTRKS(24+3,VID(JJ))))
C            .....Jazelle processed PZXTRKS(TRK(VID(JJ)),HLXPAR(2))...(B
C            ank ZXTRKS)
CJSF                  PTOT = 1/(JZL$R(JZL$X(PZXTRKS)-10+(VID(JJ))*62+(2)*1)
CJSF     *            *COSL)
                  PTOT = 1/(RTRK_ZXTRKS(24+2,VID(JJ))*COSL)
C            .....Jazelle processed PZXTRKS(TRK(VID(JJ)),HLXPAR(1))...(B
C            ank ZXTRKS)
CJSF                  PX = PTOT*COSL*COS(JZL$R(JZL$X(PZXTRKS)-10+(VID(JJ))*
CJSF     *            62+(1)*1) )
                  PX = PTOT*COSL*COS(RTRK_ZXTRKS(24+1,VID(JJ)))
C            .....Jazelle processed PZXTRKS(TRK(VID(JJ)),HLXPAR(1))...(B
C            ank ZXTRKS)
CJSF                  PY = PTOT*COSL*SIN(JZL$R(JZL$X(PZXTRKS)-10+(VID(JJ))*
CJSF     *            62+(1)*1) )
                  PY = PTOT*COSL*SIN(RTRK_ZXTRKS(24+1,VID(JJ)))
                  PZ = PTOT*SINL
                  PXT = PXT + PX
                  PYT = PYT + PY
                  PZT = PZT + PZ
                  ETOT = ETOT + SQRT(PTOT*PTOT + 0.01948)
921            CONTINUE
922            CONTINUE
               PTOT = SQRT(PXT*PXT+PYT*PYT+PZT*PZT)
               MAS(NV) = SQRT(ETOT*ETOT-PTOT*PTOT)
               DO 931 I=1,NZT,1
C            .....Jazelle processed ZVTOPL3(JETNO)(TRKS(PHT(I)),XIST)...
C            (Bank ZVTOPL3)
CJSF                  JZL$R(JZL$X(ZVTOPL3(JETNO))+130+(PHT(I))*21)  =
CJSF     *            CHISQTK(I)
                  RTRK_ZVTOPL3(1,PHT(I),JETNO) = CHISQTK(I)
931            CONTINUE
932            CONTINUE
               P1(NV) = XVTX(1)
               P2(NV) = XVTX(2)
               P3(NV) = XVTX(3)
               DP1(NV) = XVTXSG(1)
               DP2(NV) = XVTXSG(2)
               DP3(NV) = XVTXSG(3)
               DP4(NV) = XVTXSG(4)
               DP5(NV) = XVTXSG(5)
               DP6(NV) = XVTXSG(6)
               XISQ(NV) = CHISQ
               DIS(NV) = SQRT((P1(NV)-IPX)**2 + (P2(NV)-IPY)**2 + (P3(
     *         NV)-IPZ)**2)
C         angular displacement
               ALPHA(NV) = 0.0
               DLONG = ((P1(NV)-IPX)*PXT+(P2(NV)-IPY)*PYT+(P3(NV)-IPZ)*
     *         PZT)/PTOT
               DMAG = SQRT((P1(NV)-IPX)**2 + (P2(NV)-IPY)**2 + (P3(NV)-
     *         IPZ)**2)
               DTRAN = DMAG**2 - DLONG**2
               IF (DTRAN.LE.0.0) THEN
                  DTRAN = 0.0
               END IF
               DTRAN = SQRT(DTRAN)
               IF (DTRAN.GT.0.005) THEN
                  ALPHA(NV) = ACOS((DLONG+0.01)/SQRT(((DLONG+0.01)**2)+(
     *            DTRAN-0.005)**2))
               END IF
            END IF
C       at least 2 tracks
            CRG(NV) = CRGZ
            NTRV(NV) = NZT
         END IF
C   do not vertex isolated tracks !
901   CONTINUE
902   CONTINUE
Cendloop over vertices
C get order from IP
      DO 941 NV=2,NVERT,1
         DONE(NV) = 0
941   CONTINUE
942   CONTINUE
      VORD(1) = 1
      DO 951 II=2,NVERT,1
         NEAR = 100.0
         DO 961 NV=2,NVERT,1
            IF (DIS(NV).LT.NEAR .AND. DONE(NV).EQ.0) THEN
               NEAR = DIS(NV)
               NEXV = NV
            END IF
961      CONTINUE
962      CONTINUE
         DONE(NEXV) = 1
         VORD(NEXV) = II
951   CONTINUE
952   CONTINUE
      DO 971 NV=1,NVERT
         IF (VORD(NV).EQ.NVERT) THEN
            FARV = NV
         END IF
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),NTRV)...(Bank Z
C   VTOPL3)
CJSF         JZL$I(JZL$X(ZVTOPL3(JETNO))+21+(VORD(NV))*16)  = NTRV(NV)
         IVRT_ZVTOPL3(1,VORD(NV),JETNO) = NTRV(NV)
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),VSIG)...(Bank Z
C   VTOPL3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+22+(VORD(NV))*16)  = VSIG(NV)
         RVRT_ZVTOPL3(1,VORD(NV),JETNO) = VSIG(NV)
         IF (NTRV(NV).GE.2) THEN
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),POS(1))...(B
C      ank ZVTOPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+22+(VORD(NV))*16+(1)*1)  = P1(
CJSF     *      NV)
            RVRT_ZVTOPL3(1+1,VORD(NV),JETNO)=P1(NV)
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),POS(2))...(B
C      ank ZVTOPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+22+(VORD(NV))*16+(2)*1)  = P2(
CJSF     *      NV)
            RVRT_ZVTOPL3(1+2,VORD(NV),JETNO)=P2(NV)
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),POS(3))...(B
C      ank ZVTOPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+22+(VORD(NV))*16+(3)*1)  = P3(
CJSF     *      NV)
            RVRT_ZVTOPL3(1+3,VORD(NV),JETNO)=P3(NV)
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),DPOS(1))...(
C      Bank ZVTOPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+25+(VORD(NV))*16+(1)*1)  = DP1(
CJSF     *      NV)
            RVRT_ZVTOPL3(4+1,VORD(NV),JETNO)=DP1(NV)
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),DPOS(2))...(
C      Bank ZVTOPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+25+(VORD(NV))*16+(2)*1)  = DP2(
CJSF     *      NV)
            RVRT_ZVTOPL3(4+2,VORD(NV),JETNO)=DP2(NV)
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),DPOS(3))...(
C      Bank ZVTOPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+25+(VORD(NV))*16+(3)*1)  = DP3(
CJSF     *      NV)
            RVRT_ZVTOPL3(4+3,VORD(NV),JETNO)=DP3(NV)
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),DPOS(4))...(
C      Bank ZVTOPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+25+(VORD(NV))*16+(4)*1)  = DP4(
CJSF     *      NV)
            RVRT_ZVTOPL3(4+4,VORD(NV),JETNO)=DP4(NV)
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),DPOS(5))...(
C      Bank ZVTOPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+25+(VORD(NV))*16+(5)*1)  = DP5(
CJSF     *      NV)
            RVRT_ZVTOPL3(4+5,VORD(NV),JETNO)=DP5(NV)
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),DPOS(6))...(
C      Bank ZVTOPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+25+(VORD(NV))*16+(6)*1)  = DP6(
CJSF     *      NV)
            RVRT_ZVTOPL3(4+6,VORD(NV),JETNO)=DP6(NV)
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),XISQ)...(Ban
C      k ZVTOPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+32+(VORD(NV))*16)  = XISQ(NV)
            RVRT_ZVTOPL3(11,VORD(NV),JETNO)=XISQ(NV)
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),ALPHA)...(Ba
C      nk ZVTOPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+36+(VORD(NV))*16)  = ALPHA(NV)
            RVRT_ZVTOPL3(14,VORD(NV),JETNO)=ALPHA(NV)
         END IF
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),MASS)...(Bank Z
C   VTOPL3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+34+(VORD(NV))*16)  = MAS(NV)
         RVRT_ZVTOPL3(13,VORD(NV),JETNO)=MAS(NV)
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(VORD(NV)),CHARGE)...(Bank
C    ZVTOPL3)
CJSF         JZL$I(JZL$X(ZVTOPL3(JETNO))+35+(VORD(NV))*16)  = CRG(NV)
         IVRT_ZVTOPL3(2,VORD(NV),JETNO)=CRG(NV)
971   CONTINUE
972   CONTINUE
Credo track vertex association
      DO 981 I=1,NTRK,1
C   .....Jazelle processed ZVTOPL3(JETNO)(TRKS(I),VNO)...(Bank ZVTOPL3)
CJSF         IF (JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(I)*21) .GT.0) THEN
         IF (ITRK_ZVTOPL3(2,I,JETNO) .GT.0) THEN
C      .....Jazelle processed ZVTOPL3(JETNO)(TRKS(I),VNO)...(Bank ZVTOPL
C      3)
CJSF            NV = VORD(JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(I)*21) )
            NV = VORD(ITRK_ZVTOPL3(2,I,JETNO))
C      .....Jazelle processed ZVTOPL3(JETNO)(TRKS(I),VNO)...(Bank ZVTOPL
C      3)
CJSF            JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(I)*21)  = NV
            ITRK_ZVTOPL3(2,I,JETNO)=NV
         END IF
981   CONTINUE
982   CONTINUE
C find how close track was to decay chain
      DO 991 JJ=1,NTRK,1
         IF (NVERT.LE.1) THEN
C      .....Jazelle processed PZXTRKS(TRK(ID(JJ)),IMPACT3)...(Bank ZXTRK
C      S)
CJSF            TRDI(JJ) = JZL$R(JZL$X(PZXTRKS)+21+(ID(JJ))*62)
            TRDI(JJ)=RTRK_ZXTRKS(49,ID(JJ))
            LODI(JJ) = 0.0
         ELSE
            AXI(1) = IPX
            AXI(2) = IPY
            AXI(3) = IPZ
            AXF(1) = P1(FARV)
            AXF(2) = P2(FARV)
            AXF(3) = P3(FARV)
            ZNO = ID(JJ)
            TKI(1) = 0.0
            TKI(2) = 0.0
            TKI(3) = 0.0
            TKV(1) = 0.0
            TKV(2) = 0.0
            TKV(3) = 0.0
            SGLISS=ZVTRK(AXI,AXF,ZNO,TKI,TKV,TRD,LOD,ANTA)
            LODI(JJ) = LOD
            TRDI(JJ) = TRD
         END IF
C    find close approach if >= 2 verts
C    now fill lodi and trdi in zvtopl3
C   .....Jazelle processed ZVTOPL3(JETNO)(TRKS(JJ),TRDI)...(Bank ZVTOPL3
C   )
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+145+(JJ)*21)  = TRDI(JJ)
         RTRK_ZVTOPL3(16,JJ,JETNO)=TRDI(JJ)
C   .....Jazelle processed ZVTOPL3(JETNO)(TRKS(JJ),LODI)...(Bank ZVTOPL3
C   )
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+146+(JJ)*21)  = LODI(JJ)
         RTRK_ZVTOPL3(17,JJ,JETNO)=LODI(JJ)

991   CONTINUE
992   CONTINUE
C loop over pZxTrks
      SGLISS=SLD$NORMAL
730   CONTINUE
      IF (SIGSEV(IAND(SGLISS,7)).GT.SGLLEV) THEN
CJSF         SGLISS=SGLIT(SGLISS, 'ZVRES3' ,%DESCR(' ')  )
      END IF
      ZVRES3=SGLISS
      RETURN
      END
