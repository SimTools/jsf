
C--------------------------------------------------------->>>>ZVTRK
      INTEGER FUNCTION ZVTRK (AXI,AXF,ZNO,TKI,TKV,TRDI,LODI,ANTA)
      IMPLICIT NONE
#include "zxtrks.inc"
      REAL AXI(3)
      REAL AXF(3)
      INTEGER ZNO
      REAL TKI(3)
      REAL TKV(3)
      REAL TRDI
      REAL LODI
      REAL ANTA
      INTEGER  SGLGBL, SGLCHK, SGLRTN, SGLHDL, SGLRMV, SGLGET, SGLLEV,
     *SGLIT, SGLISS, SIGSEV(0:7)/3,1,4,2,5,5,5,5/
      COMMON /SGLCOM/ SGLLEV
CDEC$ PSECT /SGLCOM/ NOSHR
      INTEGER*4 SLD$NORMAL
      PARAMETER (SLD$NORMAL=1)
C----------------------------------------------------------------------
C
C ZVTRK  - find topological track parameters TRDI and LODI of ZVTOPL
C----------------------------------------------------------------------
C
C  Arguments
C  =========
C
C  INPUT
C   axi(3) -  initial x,y,z of reference (vertex) axis (eg IP position)
C   axf(3) -  final x,y,z of reference (vertex) axis (eg seed vertex)
C  / zno    -   ZXTRK number of track, -OR- if zno=0, then require:
C  | tki(3) -   x,y,z of track near IP
C  \ tkv(3) -   direction vector of track (eg momentum px,py,pz)
C
C  OUTPUT
C   trdi   - closet approach of track to reference (vertex) axis
C   lodi   - distance from axi along ref. axis to the point where trdi
C            is measured, -ve if opposite direction cf. axf
C        NB: lodi=-1000 if there is no solution: track parallel to axis
C   anta   - angle in radians between track and ref. (vert.) axis, the
C            value of lodi is less useful for low angles
C----------------------------------------------------------------------
C KEYWORDS: ZVTOP
C*--------------------------------------------------------------------*
C
C     OWNER: JACKSON, Dave          CO-OWNER: USHER, Tracy
C   SECTION: ZXFIND                    FILE: ZVTRK PREPMORT
C
C                  Date            Explanation of Changes
C               ----------- -------------------------------------------
C  9 Jan 1996 Program creation date
      REAL UPDATE / 0.00 /
CCurrent Version 0.00
C*--------------------------------------------------------------------*
      INTEGER PZXTRKSFAM
      INTEGER PZXTRKS
      LOGICAL FIRST/.TRUE./
      INTEGER JZBLOC
      REAL COSL,SINL,PTOT,ITX,ITY,ITZ,PX,PY,PZ,LAMBDA,MU
      REAL VX,VY,VZ,AA,BB,CC,DD,EE,IPX,IPY,IPZ,XU,YU,ZU,XT,YT,ZT
C----------------------------------------------------------------------
      DATA PZXTRKSFAM/0/
      IF (ZNO .EQ. 0) THEN
         ITX = TKI(1)
         ITY = TKI(2)
         ITZ = TKI(3)
         PX = TKV(1)
         PY = TKV(2)
         PZ = TKV(3)
      ELSE
         IF (FIRST) THEN
CJSF            SGLISS=JZBLOC('ZXTRKS', PZXTRKSFAM)
            FIRST=.FALSE.
         END IF
C   .....Jazelle processed PZXTRKSFAM(JB$FORPT)...(Bank ZXTRKS)
CJSF         PZXTRKS = JZL$I(JZL$XA(PZXTRKSFAM)-1)
C   .....Jazelle processed PZXTRKS(TRK(ZNO),HLXPAR(3))...(Bank ZXTRKS)
CJSF         COSL = COS(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(ZNO)*62+(3)*1) ))
         COSL = COS(ATAN(RTRK_ZXTRKS(27,ZNO)))
C   .....Jazelle processed PZXTRKS(TRK(ZNO),HLXPAR(3))...(Bank ZXTRKS)
CJSF         SINL = SIN(ATAN(JZL$R(JZL$X(PZXTRKS)-10+(ZNO)*62+(3)*1) ))
         SINL = SIN(ATAN(RTRK_ZXTRKS(27,ZNO)))
C   .....Jazelle processed PZXTRKS(TRK(ZNO),HLXPAR(2))...(Bank ZXTRKS)
CJSF         PTOT = 1/(JZL$R(JZL$X(PZXTRKS)-10+(ZNO)*62+(2)*1) *COSL)
         PTOT = 1/(RTRK_ZXTRKS(26,ZNO)*COSL)
C   .....Jazelle processed PZXTRKS(TRK(ZNO),HLXPAR(1))...(Bank ZXTRKS)
CJSF         PX = PTOT*COSL*COS(JZL$R(JZL$X(PZXTRKS)-10+(ZNO)*62+(1)*1) )
         PX = PTOT*COSL*COS(RTRK_ZXTRKS(25,ZNO) )
C   .....Jazelle processed PZXTRKS(TRK(ZNO),HLXPAR(1))...(Bank ZXTRKS)
CJSF         PY = PTOT*COSL*SIN(JZL$R(JZL$X(PZXTRKS)-10+(ZNO)*62+(1)*1) )
         PY = PTOT*COSL*SIN(RTRK_ZXTRKS(25,ZNO) )
         PZ = PTOT*SINL
C   .....Jazelle processed PZXTRKS(TRK(ZNO),HLXPAR(4))...(Bank ZXTRKS)
CJSF         ITX = JZL$R(JZL$X(PZXTRKS)-10+(ZNO)*62+(4)*1)
         ITX = RTRK_ZXTRKS(28,ZNO)
C   .....Jazelle processed PZXTRKS(TRK(ZNO),HLXPAR(5))...(Bank ZXTRKS)
CJSF         ITY = JZL$R(JZL$X(PZXTRKS)-10+(ZNO)*62+(5)*1)
         ITY = RTRK_ZXTRKS(29,ZNO)
C   .....Jazelle processed PZXTRKS(TRK(ZNO),HLXPAR(6))...(Bank ZXTRKS)
CJSF         ITZ = JZL$R(JZL$X(PZXTRKS)-10+(ZNO)*62+(6)*1)
         ITZ = RTRK_ZXTRKS(29,ZNO)
      END IF
      IPX = AXI(1)
      IPY = AXI(2)
      IPZ = AXI(3)
      VX = AXF(1) - AXI(1)
      VY = AXF(2) - AXI(2)
      VZ = AXF(3) - AXI(3)
Cangle between track and axis
      ANTA = (VX*PX + VY*PY + VZ*PZ)/ (SQRT(VX**2 + VY**2 + VZ**2)*SQRT(
     *PX**2 + PY**2 + PZ**2))
      IF (ANTA.GE.1.0) THEN
         ANTA = 1.0
      END IF
      IF (ANTA.LE.-1.0) THEN
         ANTA = -1.0
      END IF
      ANTA = ACOS(ANTA)
Cnow solve for closest approach
      AA = (ITX-IPX)*VX + (ITY-IPY)*VY + (ITZ-IPZ)*VZ
      DD = (ITX-IPX)*PX + (ITY-IPY)*PY + (ITZ-IPZ)*PZ
      BB = PX*VX + PY*VY + PZ*VZ
      CC = -(VX*VX + VY*VY + VZ*VZ)
      EE = PX*PX + PY*PY + PZ*PZ
      IF ((CC+BB*BB/EE).EQ.0.0) THEN
C    no solution, track,axis parallel
         LODI = -1000.0
         LAMBDA = 0.0
         MU = -DD/EE
      ELSE
         LAMBDA = (DD*BB/EE-AA)/(CC+BB*BB/EE)
         MU = (BB*LAMBDA-DD)/EE
      END IF
Cxu,yu,zu = point of normal hit on vertex axis
Cxt,yt,zt = point of normal hit on track vector
      XU = IPX + LAMBDA*VX
      YU = IPY + LAMBDA*VY
      ZU = IPZ + LAMBDA*VZ
      XT = ITX + MU*PX
      YT = ITY + MU*PY
      ZT = ITZ + MU*PZ
C-ve if we're behind IP
      LODI = SQRT((XU-IPX)**2 + (YU-IPY)**2 +(ZU- IPZ)**2)
      IF (((XU-IPX)*VX+(YU-IPY)*VY+(ZU-IPZ)*VZ) .LT. 0.0) THEN
         LODI = -LODI
      END IF
      TRDI = SQRT((XU-XT)**2 + (YU-YT)**2 +(ZU-ZT)**2)
      IF ((CC+BB*BB/EE).EQ.0.0) THEN
C   no solution, track,axis parallel: reset lodi
         LODI = -1000.0
      END IF
      SGLISS=SLD$NORMAL
20    CONTINUE
      IF (SIGSEV(IAND(SGLISS,7)).GT.SGLLEV) THEN
CJSF         SGLISS=SGLIT(SGLISS, 'ZVTRK' ,%DESCR(' ')  )
      END IF
      ZVTRK=SGLISS
      RETURN
      END
