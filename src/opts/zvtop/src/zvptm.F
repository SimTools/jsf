C--------------------------------------------------------->>>>ZVPTM
      INTEGER FUNCTION ZVPTM (MODE,AXI,EAXI,AXF,EAXF,P,SIGMAX,P3NORM,PT,
     *PTMIN,PTMAX)
      IMPLICIT NONE
      INTEGER MODE
      REAL AXI(3)
      REAL EAXI(6)
      REAL AXF(3)
      REAL EAXF(6)
      REAL P(3)
      REAL SIGMAX
      REAL P3NORM
      REAL PT
      REAL PTMIN
      REAL PTMAX
CJSF      PARAMETER SIGNAL$S_SUCCESS=1
CJSF      PARAMETER SIGNAL$S_INFORMATION=2
CJSF      PARAMETER SIGNAL$S_WARNING=3
CJSF      PARAMETER SIGNAL$S_ERROR=4
CJSF      PARAMETER SIGNAL$S_FATAL=5
      INTEGER  SGLGBL, SGLCHK, SGLRTN, SGLHDL, SGLRMV, SGLGET, SGLLEV,
     *SGLIT, SGLISS, SIGSEV(0:7)/3,1,4,2,5,5,5,5/
      COMMON /SGLCOM/ SGLLEV
CDEC$ PSECT /SGLCOM/ NOSHR
      INTEGER SLD$NORMAL
      PARAMETER (SLD$NORMAL=1)
C----------------------------------------------------------------------
C
C ZVPTM  - find the min/max Pt of P(3) to vertex axis within error
C----------------------------------------------------------------------
C
C  Arguments
C  =========
C
C  INPUT
C   mode    - 1,2,3  - see explanation below in OUTPUT description
C   axi(3)  - initial x,y,z of reference (vertex) axis (eg IP position)
C   Eaxi(6) - error matrix for axi (xx,xy,yy,xz,yz,zz) in cm**2
C   axf(3)  - final x,y,z of reference (vertex) axis (eg seed vertex)
C   Eaxf(6) - error matrix for axf (xx,xy,yy,xz,yz,zz) in cm**2
C   p(3)    - x,y,z momentum of track or tracks being considered
C   sigmax  - max no. of sigmas based on error matrices within which
C             vertex axis can move in min/maximising the Pt of P(3)
C
C  OUTPUT
C   p3norm  - min no. sigma of p(3) from vertex axis dirn. (cf b3norm)
C   Pt      - trans mom. of p(3) to vertex axis
C   Ptmin   - min. trans. mom allowing for vertex axis within sigmax
C             -1.0 if calc. error (probably bad error matrix in INPUT)
C   Ptmax   - max. trans. mom allowing for vertex axis within sigmax
C
C   MODE = 1  only for use with axi = IP and axf = secondary vertex
C             where Eaxi(1) = Eaxi(3) and P(3) is the total momentum
C             of sec. vert. trks: code taken from mass tagging routine
C   MODES 2,3 for the general case, mode 3 being the most accurate
C   Ptmin/max found by adding Eaxi and Eaxf into one error matrix and:
C   MODE = 2  Pt min/maximised to the 2d sigmax contour of the error
C             ellipse in the plane containing the vertex axis and P(3)
C   MODE = 3  Pt min/maximised to the full 3d sigmax contour of the
C             error ellipsoid in the full 3d xyz space
C  (MODE = -3 is MODE 3 w/o PTMAX calc., saves a little CPU time)
C
C----------------------------------------------------------------------
C KEYWORDS: ZVTOP
C*--------------------------------------------------------------------*
C
C     OWNER: JACKSON, Dave          CO-OWNER: SU, Dong
C   SECTION: ZXFIND                    FILE: ZVPTM PREPMORT
C
C                  Date            Explanation of Changes
C               ----------- -------------------------------------------
C  9 Jan 1996 Program creation date
      REAL UPDATE / 0.00 /
CCurrent Version 0.00
C*--------------------------------------------------------------------*

      LOGICAL FIRST/.TRUE./
      INTEGER ZUROTEM
      INTEGER I,J,K,N,NIT,IMM
      REAL PX,PY,PZ,PTOT,ANTA,VX,VY,VZ
      REAL EM(6),XVECT(3),YVECT(3),VE(6),CM(3,3)
      REAL ROTM(3,3),ROTMT(3,3),TEMP(3,3),XVN(3),YVN(3),ZVN(3)
      REAL XL,YL,ZL,XM,YM,ZM,LAMBDA,MU,SIGMA
      REAL X0,Y0,PY2,PZ2,AA,BB,CC
      REAL APLUS,AMINU,AMIN,XMIN,YMIN,AMAX,XMAX,YMAX
      REAL XV3,YV3,ZV3
      REAL DIST,VERROT(6),DP,SEDXY,PXY,DPHI,DLAM,DPHIM
      REAL DLAMM,THETA,S,C,STEP,SMIN,DIR,PTMIN0,PTMAX0
      REAL A1,A2,A3,B1,B2,B3,X1,Y1,C1,C2
      REAL T1,T2,T3,T4,T5,T6,DET
C----------------------------------------------------------------------
      PX = P(1)
      PY = P(2)
      PZ = P(3)
      PTOT = SQRT(PX**2 + PY**2 + PZ**2)
C vertex location relative to (0,0,0)
      VX = AXF(1) - AXI(1)
      VY = AXF(2) - AXI(2)
      VZ = AXF(3) - AXI(3)
      DIST = SQRT(VX**2 + VY**2 + VZ**2)
      PTMIN = 1001.0
      PTMAX = -1.0
Ccostheta between track and axis
      ANTA = (VX*PX + VY*PY + VZ*PZ)/ (DIST*PTOT)
      IF (ANTA.GE.1.0) THEN
         ANTA = 1.0
      END IF
      IF (ANTA.LE.-1.0) THEN
         ANTA = -1.0
      END IF
      PT = PTOT*SQRT(1.0 - ANTA**2)
C add error matrices (already in quadrature)
      CM(1,1) = EAXI(1) + EAXF(1)
      CM(1,2) = EAXI(2) + EAXF(2)
      CM(2,1) = EAXI(2) + EAXF(2)
      CM(2,2) = EAXI(3) + EAXF(3)
      CM(1,3) = EAXI(4) + EAXF(4)
      CM(3,1) = EAXI(4) + EAXF(4)
      CM(2,3) = EAXI(5) + EAXF(5)
      CM(3,2) = EAXI(5) + EAXF(5)
      CM(3,3) = EAXI(6) + EAXF(6)
      IF (MODE.EQ.1) THEN
C    treat IP error differently
         CM(1,1) = EAXF(1)
         CM(1,2) = EAXF(2)
         CM(2,1) = EAXF(2)
         CM(2,2) = EAXF(3)
         CM(1,3) = EAXF(4)
         CM(3,1) = EAXF(4)
         CM(2,3) = EAXF(5)
         CM(3,2) = EAXF(5)
         CM(3,3) = EAXF(6)
      END IF
C error matrix
      EM(1) = CM(1,1)
      EM(2) = CM(1,2)
      EM(3) = CM(2,2)
      EM(4) = CM(1,3)
      EM(5) = CM(2,3)
      EM(6) = CM(3,3)
      IF (MODE.EQ.1) THEN
C    Su Dong's/Eric Weiss calc., from BJMASPT, and modified a little
         SGLISS=ZUROTEM(P,EM,VERROT)
C    Find Delta PHI and Delta lambda
         SEDXY = SQRT(VX**2 + VY**2)
         PXY = SQRT( P(1)**2 + P(2)**2 )
         DPHI = (P(1)*VX+P(2)*VY)/(SEDXY*PXY)
         IF (DPHI .GT. 1.0) THEN
            DPHI = 1.0
         END IF
         IF (DPHI .LT. -1.0) THEN
            DPHI = -1.0
         END IF
         DPHI = ACOS(DPHI)
         DLAM = ATAN((P(3)/PXY-VZ/SEDXY)/(1+P(3)*VZ/(PXY*SEDXY)))
C    1/2 factor makes mode 1 ptmin distribution similar to mode 3
C    assume Eaxi(1) = Eaxi(3), delX = delY, sym in phi for IP
         DPHIM = (SIGMAX/2)*SQRT(EAXI(1) + ABS(VERROT(1)) + ABS(VERROT(
     *   6))*SIN(DPHI)**2 )/SEDXY
         DLAMM = (SIGMAX/2)*SQRT(EAXI(6)*(SEDXY/DIST)**2 + ABS(VERROT(3)
     *   ) + ABS(VERROT(6))*SIN(DLAM)**2 )/DIST
         PTMIN = PT - SQRT((PTOT**2)*SIN(DLAMM)**2+(PXY*SIN(DPHIM))**2)
         IF (( PTMIN .LT. 0 )) THEN
            PTMIN = 0.0
         END IF
         PTMAX = PT + SQRT((PTOT**2)*SIN(DLAMM)**2+(PXY*SIN(DPHIM))**2)
         IF (( PTMAX .GT. PTOT )) THEN
            PTMAX = PTOT
         END IF
      END IF
C closest point of vertex on mom vector
      LAMBDA = (PX*VX + PY*VY + PZ*VZ)/PTOT**2
      XL = LAMBDA * PX
      YL = LAMBDA * PY
      ZL = LAMBDA * PZ
      X0 = SQRT(XL**2 + YL**2 + ZL**2)
      Y0 = SQRT((VX-XL)**2 + (VY-YL)**2 + (VZ-ZL)**2)
C rotate error matrix s.t. x-axis along P(3) and y points to vertex
      XVECT(1) = PX
      XVECT(2) = PY
      XVECT(3) = PZ
      YVECT(1) = VX-XL
      YVECT(2) = VY-YL
      YVECT(3) = VZ-ZL
      XV3 = SQRT( XVECT(1)**2 + XVECT(2)**2 + XVECT(3)**2 )
      YV3 = SQRT( YVECT(1)**2 + YVECT(2)**2 + YVECT(3)**2 )
      DO 21 I=1,3
         XVN(I) = XVECT(I)/XV3
         YVN(I) = YVECT(I)/YV3
21    CONTINUE
22    CONTINUE
      ZVN(1) = XVN(2)*YVN(3) - XVN(3)*YVN(2)
      ZVN(2) = XVN(3)*YVN(1) - XVN(1)*YVN(3)
      ZVN(3) = XVN(1)*YVN(2) - XVN(2)*YVN(1)
C
C Construct the rotation matrix
C
      ROTM(1,1) = XVN(1)
      ROTM(1,2) = XVN(2)
      ROTM(1,3) = XVN(3)
      ROTM(2,1) = YVN(1)
      ROTM(2,2) = YVN(2)
      ROTM(2,3) = YVN(3)
      ROTM(3,1) = ZVN(1)
      ROTM(3,2) = ZVN(2)
C
C Copy into the rotation matrix transposed
C
      ROTM(3,3) = ZVN(3)
      DO 31 I=1,3
         DO 41 J=1,3
            ROTMT(J,I) = ROTM(I,J)
41       CONTINUE
42       CONTINUE
31    CONTINUE
32    CONTINUE
C
C 2 multiplications:   ERROUT = ROTM * ERRIN * ROTMT
      DO 51 I=1,3
         DO 61 J=1,3
            TEMP(I,J) = 0.0
            DO 71 K=1,3
               TEMP(I,J) = TEMP(I,J) + CM(I,K)*ROTMT(K,J)
71          CONTINUE
72          CONTINUE
61       CONTINUE
62       CONTINUE
51    CONTINUE
52    CONTINUE
      DO 81 I=1,3
         DO 91 J=1,3
            CM(I,J) = 0.0
            DO 101 K=1,3
               CM(I,J) = CM(I,J) + ROTM(I,K)*TEMP(K,J)
101         CONTINUE
102         CONTINUE
91       CONTINUE
92       CONTINUE
81    CONTINUE
82    CONTINUE
C matrix inversion code
      T1 = CM(1,1)
      T2 = CM(1,2)
      T3 = CM(2,2)
      T4 = CM(1,3)
      T5 = CM(2,3)
      T6 = CM(3,3)
      DET = T1*T3*T6 + 2*T2*T4*T5 - T3*T4*T4 - T1*T5*T5 - T2*T2*T6
      IF (DET.EQ.0.0) THEN
         P3NORM = -1.0
         IF (MODE.NE.1) THEN
            GOTO   110
         END IF
      END IF
Cfailed matrix inv!?
C inverse of the combined error matrix in new coord system
      VE(1) = (T3*T6 - T5*T5)/DET
      VE(2) = (T4*T5 - T2*T6)/DET
      VE(3) = (T1*T6 - T4*T4)/DET
      VE(4) = (T2*T5 - T3*T4)/DET
      VE(5) = (T2*T4 - T1*T5)/DET
      VE(6) = (T1*T3 - T2*T2)/DET
C if (x,y,z) = lambda.(1,0,0), most proble location in error matrix:
      LAMBDA = ( 1*(VE(1)*X0 + VE(2)*Y0 ) ) / ( VE(1) )
      XM = LAMBDA
Cprotect against sqrt(-ve) due to rounding error
      SIGMA = ( VE(1)*(XM-X0)**2 + 2*VE(2)*(XM-X0)*(-Y0) + VE(3)*(-Y0)**
     *2)
      IF (SIGMA.GT.0.0) THEN
         SIGMA = SQRT(SIGMA)
      ELSE
         SIGMA = 0.0
      END IF
      P3NORM = SIGMA
      IF (MODE.EQ.1) THEN
         GOTO 110
      END IF
      IF (SIGMA.LE.SIGMAX) THEN
C   inside error ellipse
         PTMIN = 0.0
      END IF
C if (x,y,z) = (0,lambda,mu), most proble location in error matrix:
      C1 = VE(2)*X0 + VE(3)*Y0
C+ ve(5)*z0;
      C2 = VE(4)*X0 + VE(5)*Y0
C+ ve(6)*z0;
      LAMBDA = (C1*VE(6) - C2*VE(5))/(VE(3)*VE(6) - VE(5)*VE(5))
      MU = (C2 - VE(5)*LAMBDA)/VE(6)
      YM = LAMBDA
      ZM = MU
Cprotect against sqrt(-ve) due to rounding error
      SIGMA = (VE(1)*(-X0)**2 + 2*VE(2)*(-X0)*(YM-Y0) + VE(3)*(YM-Y0)**
     *2 + 2*VE(4)*(-X0)*(ZM) + 2*VE(5)*(YM-Y0)*(ZM) + VE(6)*(ZM)**2)
      IF (SIGMA.GT.0.0) THEN
         SIGMA = SQRT(SIGMA)
      ELSE
         SIGMA = 0.0
      END IF
      IF (SIGMA.LE.SIGMAX) THEN
C   a normal to P(3) is inside error ellipse
         PTMAX = PTOT
      END IF
C do ptmin then ptmax if iterating
      DO 121 IMM=1,2
         IF((IMM.EQ.1 .AND. PTMIN.LT.0.00001))GOTO   130
         IF((IMM.EQ.2 .AND. ABS(PTMAX-PTOT).LT.0.00001))GOTO 110
         PTMIN0 = 1000.0
         PTMAX0 = 0.0
         N = -1
         THETA = 0.0
C   initial step size and minimum step, smin
         DIR = 1.0
         IF ((0.001*SIGMAX/Y0) .LT. 0.1) THEN
            STEP = 0.001*SIGMAX/Y0
            SMIN = 0.00005*SIGMAX/Y0
         ELSE
            STEP = 0.1
            SMIN = 0.005
         END IF
         STEP = STEP*5.0
         NIT = 0
140      NIT = NIT + 1
         C = COS(THETA)
         S = SIN(THETA)
C   solve for a in y = ax
         AA = VE(2)**2*X0**2 - VE(1)*VE(3)*X0**2 + VE(3)*SIGMAX**2
         BB = 2*VE(1)*VE(3)*X0*Y0 - 2*VE(2)**2*X0*Y0 + 2*VE(2)*SIGMAX**
     *   2
         CC = VE(2)**2*Y0**2 - VE(1)*VE(3)*Y0**2 + VE(1)*SIGMAX**2
C    correction for rotation of plane
         AA = AA*C*C + S*(VE(4)**2*X0**2*S + VE(5)**2*Y0**2*S + 2*VE(2)*
     *   VE(4)*X0**2*C + 2*VE(3)*VE(4)*Y0*X0*C + 2*VE(4)*VE(5)*X0*Y0*S -
     *    2*VE(1)*VE(5)*X0**2*C - VE(1)*VE(6)*X0**2*S - 2*VE(2)*VE(5)*
     *   X0*Y0*C - 2*VE(2)*VE(6)*X0*Y0*S - VE(3)*VE(6)*Y0**2*S + (2*VE(
     *   5)*C + VE(6)*S )*SIGMAX**2)
C    +- parts
         BB = BB*C + 2*S*(VE(1)*VE(5)*X0*Y0 + VE(2)*VE(5)*Y0**2 - VE(2)*
     *   VE(4)*X0*Y0 - VE(3)*VE(4)*Y0**2 + VE(4)*SIGMAX**2)
         IF ((BB**2 - 4*AA*CC).GT.0.0) THEN
            APLUS = (-BB + SQRT(BB**2 - 4*AA*CC))/(2*AA)
            AMINU = (-BB - SQRT(BB**2 - 4*AA*CC))/(2*AA)
            IF (ABS(AMINU).GT.ABS(APLUS)) THEN
               AMIN = APLUS
               AMAX = AMINU
            ELSE
               AMIN = AMINU
               AMAX = APLUS
            END IF
            IF (IMM.EQ.1) THEN
               XMIN = (VE(1)*X0+VE(2)*Y0+VE(2)*AMIN*X0*C+VE(3)*AMIN*Y0*
     *         C + VE(4)*X0*AMIN*S + VE(5)*Y0*AMIN*S)/ (VE(1) + 2*VE(2)*
     *         AMIN*C + VE(3)*AMIN**2*C*C +2*VE(4)*AMIN*S + 2*VE(5)*
     *         AMIN**2*C*S + VE(6)*AMIN**2*S*S)
               YMIN = AMIN*XMIN
               PTMIN = PTOT*ABS(YMIN)/SQRT(XMIN**2+YMIN**2)
            ELSE
               XMAX = (VE(1)*X0+VE(2)*Y0+VE(2)*AMAX*X0*C+VE(3)*AMAX*Y0*
     *         C + VE(4)*X0*AMAX*S + VE(5)*Y0*AMAX*S)/ (VE(1) + 2*VE(2)*
     *         AMAX*C + VE(3)*AMAX**2*C*C +2*VE(4)*AMAX*S + 2*VE(5)*
     *         AMAX**2*C*S + VE(6)*AMAX**2*S*S)
               YMAX = AMAX*XMAX
               PTMAX = PTOT*ABS(YMAX)/SQRT(XMAX**2+YMAX**2)
            END IF
         ELSE
            IF (IMM.EQ.1) THEN
               PTMIN = 1001.0
            ELSE
               PTMAX = -1.0
            END IF
         END IF
C    went too far
         IF (MODE.EQ.2 .OR. (MODE.EQ.-3. AND. IMM.EQ.2)) THEN
            GOTO 130
         END IF
C   else mode = 3, iterate...
         IF ((IMM.EQ.1 .AND. PTMIN.LT.PTMIN0) .OR. (IMM.EQ.2 .AND.
     *   PTMAX.GT.PTMAX0)) THEN
            N = N + 1
            PTMIN0 = PTMIN
            PTMAX0 = PTMAX
            THETA = THETA + DIR*STEP
            GOTO 140
         ELSE IF(N.EQ.0) THEN
            N = N + 1
            THETA = THETA - DIR*STEP
            DIR = -DIR
            THETA = THETA + DIR*STEP
            GOTO 140
         ELSE
            THETA = THETA - DIR*STEP
            IF (STEP.LT.SMIN) THEN
               GOTO 130
            END IF
            STEP = STEP/5.0
            THETA = THETA + DIR*STEP
            N = 0
            GOTO 140
         END IF
C    this code used to calc min no. of sigma in 2d slice...
C
C   !  ...found (bb**2 - 4*aa*cc) < 0 if sigma > sigmax, cross check OK
C
C   !
C
C   !  a1 = ve(1);
C
C   !  a2 = ve(2)*c + ve(4)*s;
C
C   !  a3 = ve(1)*x0 + ve(2)*y0;
C
C   !  b1 = a2;
C
C   !  b2 = ve(3)*c*c + 2*ve(5)*c*s + ve(6)*s*s;
C
C   !  b3 = ve(2)*x0*c + ve(3)*y0*c + ve(4)*x0*s + ve(5)*y0*s;
C
C   !
C
C   !  y1 = (b3 - b1*a3/a1)/(b2 - b1*a2/a1);
C
C   !  x1 = (a3 - a2*y1)/a1;
C
C   !  sigma = ve(1)*(x1-x0)**2 + 2*ve(2)*(x1-x0)*(c*y1-y0) +
C
C   !          ve(3)*(c*y1-y0)**2 + 2*ve(4)*(x1-x0)*(s*y1) +
C
C   !         2*ve(5)*(c*y1-y0)*(s*y1) + ve(6)*(s*y1)**2;
C
C   !  sigma = sqrt(sigma);
C
         GOTO 140
130      CONTINUE
121   CONTINUE
122   CONTINUE
C do loop imm, over ptmin then ptmax
110   IF (PTMIN.GT.999) THEN
         PTMIN = -1.0
         PTMAX = -1.0
C    flag error
      END IF
      SGLISS=SLD$NORMAL
150   CONTINUE
CJSF      IF (SIGSEV(JIAND(SGLISS,7)).GT.SGLLEV) THEN
CJSF         SGLISS=SGLIT(SGLISS, 'ZVPTM' ,%DESCR(' ')  )
CJSF      END IF
      ZVPTM=SGLISS
      RETURN
      END
