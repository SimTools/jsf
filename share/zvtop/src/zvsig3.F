C--------------------------------------------------------->>>>ZVSIG3
#include "zvtopoption.inc"
      INTEGER FUNCTION ZVSIG3 (NTRK,IPOS,ST,CT,X0P,Z0,KAPPA,TL,QCOEF,PX,
     *PY,PZ,PTOT,MODE,LPNX,PWID,KANG,KIPW,XC,YC,ZC,VSG)
      IMPLICIT NONE
      INTEGER NTRK
      REAL RAC
      REAL IPOS(9)
      REAL ST(30)
      REAL CT(30)
      REAL X0P(30)
      REAL Z0(30)
      REAL KAPPA(30)
      REAL TL(30)
      REAL QCOEF(30,2,3)
      REAL PX
      REAL PY
      REAL PZ
      REAL PTOT
      INTEGER MODE
      REAL LPNX(2,3)
      REAL PWID
      REAL KANG
      REAL KIPW
      REAL XC
      REAL YC
      REAL ZC
      REAL VSG
	INTEGER SIGNAL$S_WARNING
      PARAMETER (SIGNAL$S_WARNING=3)
      INTEGER  SGLGBL, SGLCHK, SGLRTN, SGLHDL, SGLRMV, SGLGET, SGLLEV,
     *SGLIT, SGLISS, SIGSEV(0:7)/3,1,4,2,5,5,5,5/
      COMMON /SGLCOM/ SGLLEV
CDEC$ PSECT /SGLCOM/ NOSHR
      INTEGER SLD$NORMAL
      PARAMETER (SLD$NORMAL=1)
C----------------------------------------------------------------------
C
C ZVSIG  -- return vertex significance V(r) for NTRK tracks at x,y,z
C----------------------------------------------------------------------
C
C  Arguments
C  =========
C   NTRK....    = characteristics of NTRK tracks
C   ipos        = position and error on I.P.
C   px,py,pz    = jet momentum direction
C   pwid        = konstant for track-IP plane mode
C   kang        = konstant for jet core enhancement
C   kipw        = konstant ip weight
C   xc,yc,zc    = input coords at which to evaluate vsig
C   vsg         = output V(r)
C----------------------------------------------------------------------
C KEYWORDS: IVTX
C*--------------------------------------------------------------------*
C
C     OWNER: JACKSON, Dave          CO-OWNER: USHER, Tracy
C   SECTION: ZXFIND                    FILE: ZVSIG3 PREPMORT
C
C                    Date            Explanation of Changes
C                ----------- ------------------------------------------
C 15 Aprl 1994 Program creation date
C 10 Oct  1995 Version II moved to ZXFIND
C  6 Jul  1996 Protection added by Su Dong
C 16 May  1997 include MODE>0, plane option weight
C  4 May  1999 ZVTOP3 upgrades, Aarons qcoef tubes here
      REAL UPDATE / 3.00 /
CCurrent Version 3.00
C*--------------------------------------------------------------------*
      LOGICAL FIRST/.TRUE./,POSHEM
      INTEGER JZBLOC
      INTEGER I
      REAL PRSUMU,PRSUM2U,PRFU
      REAL XPR,YPR,DTRAN,DLONG,DMAG,VWGHT,ALPHA
      REAL LAM,MU,XX,YY,ZZ,DTRP
      REAL S1,S2
      REAL AYPR
      REAL RMAX
      PARAMETER(RMAX=2.0)
C----------------------------------------------------------------------
C cut out if behind ip or > 10.0 cm in front
Cwas out to 3cm until May 97
      DLONG = ((XC-IPOS(1))*PX+(YC-IPOS(2))*PY+(ZC-IPOS(3))*PZ)/PTOT
      IF (DLONG.GT.10.0 .OR. DLONG.LT.-0.01) THEN
         VSG = -1.0
         GOTO   20
      END IF
      DMAG = SQRT((XC-IPOS(1))**2 + (YC-IPOS(2))**2 + (ZC-IPOS(3))**2)
      IF ((ABS(DLONG).LT.ABS(DMAG))) THEN
         DTRAN = SQRT(DMAG**2 - DLONG**2)
      ELSE
         DTRAN = 0
      END IF
      PRSUMU = 0.0
      PRSUM2U = 0.0
      DO 31 I=1,NTRK,1
         XPR = CT(I)*XC + ST(I)*YC
         YPR = -ST(I)*XC + CT(I)*YC
C    ASCHOU: use quadratically interpolated errors.
         AYPR = YPR
         IF (AYPR.GT.RMAX) THEN
            AYPR = RMAX
         END IF
         IF (AYPR.LT.0.0) THEN
            AYPR = 0.0
         END IF
         S1 = QCOEF(I,1,1) + QCOEF(I,1,2)*AYPR + QCOEF(I,1,3)*AYPR**2
         S2 = QCOEF(I,2,1) + QCOEF(I,2,2)*AYPR + QCOEF(I,2,3)*AYPR**2
C    protect against floating overflow
         RAC = ABS(1/(2*KAPPA(I)))
#ifndef ORIGINAL
         PRFU = ((XPR-(X0P(I)+YPR*YPR*KAPPA(I)
     *  + YPR**4*KAPPA(I)**3 + 2*YPR**6*KAPPA(I)**5
     *  ))/S1)**2 + ((ZC-(Z0(I)+
     *   RAC*TL(I)*ASIN(YPR/RAC)))/S2)**2
#else
         PRFU=((XPR-(X0P(I)+YPR*YPR*KAPPA(I)))/S1)**2 + ((ZC-(Z0(I)+
     *   YPR*TL(I)))/S2)**2
#endif

         IF (PRFU.LT.100.0) THEN
            PRFU = EXP(-0.5*PRFU)
         ELSE
            PRFU = 0.0
         ENDIF
         PRSUMU = PRSUMU + PRFU
         PRSUM2U = PRSUM2U + PRFU*PRFU
31    CONTINUE
32    CONTINUE
C add in IP information if required
      PRFU = IPOS(4)*(XC-IPOS(1))**2 + 2.0*IPOS(5)*(XC-IPOS(1))*(YC-
     *IPOS(2)) + IPOS(6)*(YC-IPOS(2))**2 + 2.0*IPOS(7)*(XC-IPOS(1))*(ZC-
     *IPOS(3)) + 2.0*IPOS(8)*(YC-IPOS(2))*(ZC-IPOS(3)) + IPOS(9)*(ZC-
     *IPOS(3))**2
      IF (PRFU.LT.100.0) THEN
         PRFU = EXP(-0.5*PRFU)
      ELSE
         PRFU = 0.0
      END IF
      PRSUMU = PRSUMU + KIPW*PRFU
      PRSUM2U = PRSUM2U + KIPW*PRFU*PRFU
      VSG = 0.0
      IF (PRSUMU .GT. 0.00001) THEN
         VSG = PRSUMU - PRSUM2U/PRSUMU
      END IF
Cinclude extra factors such as jet core weighting
      IF (DTRAN.GT.0.005) THEN
         ALPHA = ACOS((DLONG+0.01)/SQRT(((DLONG+0.01)**2)+(DTRAN-0.005)*
     *   *2))
         VSG = VSG*EXP(-KANG*ALPHA**2)
      END IF
C constrain to plane if lepton mode
C plane from ipos(1-2-3) to lpnx(2,1-2-3) and dirn lpnx(1,1-2-3)
C parameterised by        mu         and        lam
C only works if directions in plane are perpendicular !
C calculate distance from xc,yc,zc to plane
      IF (MODE.GT.0) THEN
         LAM = (LPNX(1,1)*(XC-IPOS(1))+LPNX(1,2)*(YC-IPOS(2)) +LPNX(1,3)
     *   *(ZC-IPOS(3))) /(LPNX(1,1)**2+LPNX(1,2)**2+LPNX(1,3)**2)
         MU = ((LPNX(2,1)-IPOS(1))*(XC-IPOS(1)) +(LPNX(2,2)-IPOS(2))*(
     *   YC-IPOS(2)) +(LPNX(2,3)-IPOS(3))*(ZC-IPOS(3))) / ((LPNX(2,1)-
     *   IPOS(1))**2 +(LPNX(2,2)-IPOS(2))**2 +(LPNX(2,3)-IPOS(3))**2)
         XX = IPOS(1) + MU*(LPNX(2,1)-IPOS(1)) + LAM*LPNX(1,1)
         YY = IPOS(2) + MU*(LPNX(2,2)-IPOS(2)) + LAM*LPNX(1,2)
         ZZ = IPOS(3) + MU*(LPNX(2,3)-IPOS(3)) + LAM*LPNX(1,3)
C    Distance TRansverse to Plane
         DTRP = SQRT((XX-XC)**2 + (YY-YC)**2 + (ZZ-ZC)**2)
C    Gaussian width of PWID microns
C   remove area on IP side of vertex
         VSG = VSG*EXP(-(DTRP/(0.0001*PWID))**2)
         IF (MODE.EQ.3) THEN
            IF (((XX-LPNX(2,1))*(LPNX(2,1)-IPOS(1)) + (YY-LPNX(2,2))*(
     *      LPNX(2,2)-IPOS(2)) + (ZZ-LPNX(2,3))*(LPNX(2,3)-IPOS(3))) .
     *      LT. 0.0) THEN
               VSG = 0.0
            END IF
         END IF
      END IF
20    SGLISS=SLD$NORMAL
40    CONTINUE
      IF (SIGSEV(IAND(SGLISS,7)).GT.SGLLEV) THEN
CC         SGLISS=SGLIT(SGLISS, 'ZVSIG3' ,%DESCR(' ')  )
      END IF
      ZVSIG3=SGLISS
      RETURN
      END
