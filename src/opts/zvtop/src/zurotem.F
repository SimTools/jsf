C------------------------------------------------------->>>>ZUROTEM
      INTEGER FUNCTION ZUROTEM (PVECT,ERRXYZ,ERRPV)
      IMPLICIT NONE
      REAL PVECT(3)
      REAL ERRXYZ(6)
      REAL ERRPV(6)
      INTEGER  SGLGBL, SGLCHK, SGLRTN, SGLHDL, SGLRMV, SGLGET, SGLLEV,
     *SGLIT, SGLISS, SIGSEV(0:7)/3,1,4,2,5,5,5,5/
      COMMON /SGLCOM/ SGLLEV
CDEC$ PSECT /SGLCOM/ NOSHR
      INTEGER SLD$NORMAL
      PARAMETER (SLD$NORMAL=1)
C----------------------------------------------------------------------
C==>  Rotate vertex position xyz error matrix into vertex P-vector dirn
C----------------------------------------------------------------------
C
C  Arguments
C  ---------
C  PVECT  --- Input  Vertex momentum P-vector defines new z' axis
C  ERRXYZ --- Input  xyz error matrix in order ( xx,xy,yy,xz,yz,zz )
C  ERRPV  --- Output x'y'z' error matrix in same element order as input
C                    where x' = R-phi impact direction to P-vector
C                    where y' = R-Z   impact direction to P-vector
C                    where z' = Along the vertex P-vector direction.
C
C----------------------------------------------------------------------
C
C     OWNER: SU, Dong               CO-OWNER:
C   SECTION: ZXFIND                     FILE:  ZUROTEM PREPMORT
C
C                 Date            Explanation of Changes
C               --------- ---------------------------------------------
C 16 Sep 92 Creation
C 16 Feb 99 Protection against null PVECT
      REAL UPDATE / 0.1 /
CCurrent Version 0.1
C----------------------------------------------------------------------
      INTEGER I,J
      REAL SPHI, CPHI, STHE, CTHE, PV2, PV3, PVN(3)
      REAL ERRIN(3,3), ERROUT(3,3), ROTM(3,3), ROTMT(3,3), TEMP(3,3)
C----------------------------------------------------------------------
C  Error matrix element M(i,j) defined for CERN matrix multiplication
C  i = row    number count vertical
C  j = column number count horizontal
C Construct the full input (3,3) error matrix
      ERRIN(1,1) = ERRXYZ(1)
      ERRIN(1,2) = ERRXYZ(2)
      ERRIN(1,3) = ERRXYZ(4)
      ERRIN(2,1) = ERRXYZ(2)
      ERRIN(2,2) = ERRXYZ(3)
      ERRIN(2,3) = ERRXYZ(5)
      ERRIN(3,1) = ERRXYZ(4)
      ERRIN(3,2) = ERRXYZ(5)
      ERRIN(3,3) = ERRXYZ(6)
C
C Calculate the rotational sine & cosine
C
      PV2 = SQRT( PVECT(1)*PVECT(1) + PVECT(2)*PVECT(2) )
C
C Protection against null PVECT
C
      PV3 = SQRT( PV2*PV2 + PVECT(3)*PVECT(3) )
      IF (( PV3 .LT. 1.0E-20 )) THEN
         DO 21 I=1,6
            ERRPV(I) = ERRXYZ(I)
21       CONTINUE
22       CONTINUE
         SGLISS=SLD$NORMAL
         GOTO   30
      END IF
      DO 41 I=1,3
         PVN(I) = PVECT(I) / PV3
41    CONTINUE
42    CONTINUE
      PV2 = PV2 / PV3
      IF (( PV2 .LT. 1.0E-20 )) THEN
         SPHI = 0.0
         CPHI = 1.0
         STHE = 0.0
         CTHE = PVN(3)
      ELSE
         SPHI = PVN(2) / PV2
         CPHI = PVN(1) / PV2
         STHE = PV2
         CTHE = PVN(3)
      END IF
C
C Construct the rotation matrix
C
      ROTM(1,1) = SPHI
      ROTM(1,2) = - CPHI
      ROTM(1,3) = 0.0
      ROTM(2,1) = - CPHI*CTHE
      ROTM(2,2) = - SPHI*CTHE
      ROTM(2,3) = STHE
      ROTM(3,1) = CPHI*STHE
      ROTM(3,2) = SPHI*STHE
C
C Copy into the rotation matrix transposed
C
      ROTM(3,3) = CTHE
      DO 51 I=1,3
         DO 61 J=1,3
            ROTMT(J,I) = ROTM(I,J)
61       CONTINUE
62       CONTINUE
51    CONTINUE
52    CONTINUE
C
C 2 multiplications:   ERROUT = ROTM * ERRIN * ROTMT
C
      CALL RMMLT( 3,3,3, ROTM(1,1), ROTM(1,2), ROTM(2,1) , ERRIN(1,1),
     *ERRIN(1,2), ERRIN(2,1) , TEMP(1,1), TEMP(1,2), TEMP(2,1) )
      CALL RMMLT( 3,3,3, TEMP(1,1), TEMP(1,2), TEMP(2,1) , ROTMT(1,1),
     *ROTMT(1,2), ROTMT(2,1) , ERROUT(1,1),ERROUT(1,2),ERROUT(2,1) )
C
C Re-extract the half matrix elements to output
C
      ERRPV(1) = ERROUT(1,1)
      ERRPV(2) = ERROUT(2,1)
      ERRPV(3) = ERROUT(2,2)
      ERRPV(4) = ERROUT(3,1)
      ERRPV(5) = ERROUT(3,2)
      ERRPV(6) = ERROUT(3,3)
      SGLISS=SLD$NORMAL
30    CONTINUE
CJSF      IF (SIGSEV(JIAND(SGLISS,7)).GT.SGLLEV) THEN
CJSF         SGLISS=SGLIT(SGLISS, 'ZUROTEM' ,%DESCR(' ')  )
CJSF      END IF
      ZUROTEM=SGLISS
      RETURN
      END
