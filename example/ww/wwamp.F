C**********************************************************************
C*
C*  ----------------------------------------------------------======
C*   Subroutine WWAMP( AM,PEMOM,CSPE,PVMOM,CVCUR,NFLG,ABCPL, CAMPL )
C*  ----------------------------------------------------------======
C*
C*(Function)
C* Calculate Helicity amplitude of
C*   e- + e + ---> W- + W+
C*
C*(Input)
C*   AM(6)        ; Mass and width of bossons.
C*          AM(1) = Z mass, AM(2) = Z Width
C*          AM(3) = W mass, AM(4) = W Width
C*   PEMOM(0:3,2) ; Four momentum of e-(1) and e+(2)
C*   CSPE(2,2,2)  ; Weyl spinor.
C*                (i,1,1) ; incomming e- spinor of chirarity -1
C*                   2,1                                     +1
C*                   1,2  ; out going e+                     -1
C*                   2,2                                     +1
C*   PVMOM(0:3,2) ; Outgoing 4 momentum of W-(1),W+(2)
C*   CVCUR(0:3,2) ; Current of vector.
C*   NFLG         ; 0/1 (not include/include) anomalous coupling
C*   ABCPL(4,2)   ; Anomalous WWZ/WWA coupling
C*        1=Kappa - 1, 2=lambda, 3=kappa_tilde, 4=lambda-tilde
C*        1=Z, 2=A
C*
C*(Output)
C*   CAMPL(0:4) (Complex*8) ; Amplitude sum of the diagrams.
C*         CAMPL(0) is sum of all diagram.
C*         CAMPL(1) is WWZ/WWA diagram
C*         CAMPL(2) is neutrino exchange diagram
C*         CAMPL(3) is anomalous WWZ amplitude.
C*         CAMPL(4) is anomalous WWA amplitude.
C*
C*(Author)
C*   A. Miyamoto   20-Jun-1990  Original version.
C*
C**********************************************************************
C
      SUBROUTINE WWAMP(AM,PEMOM,CSPE,PVMOM,CVCUR,NFLG,ABCPL, CAMPL)
C
      IMPLICIT REAL*4 (A-B,D-H,O-Z)
      IMPLICIT COMPLEX*8 (C)
C
      REAL*4     AM(4), PEMOM(0:3,2), PVMOM(0:3,2)
      COMPLEX*8  CSPE(6,2), CVCUR(6,2), CAMPL(0:4)
      COMPLEX*8  CTEST(2)
      REAL*4     ABCPL(4,2)
C
      COMPLEX*8  CJ3(6),CSTMP(6)
      REAL*4     GAL(2),GAU(2),GAD(2),GWF(2),GZN(2),GZL(2),
     &           GZU(2), GZD(2), G1(2), GWW3, GWWZ, GWWA
C
      REAL*4    SW2/0.2300 /
 
      REAL*4    AMNU/0.0/
      REAL*4    AWNU/0./
C
      DATA      IFIRST/1/
      SAVE      IFIRST
C
      PARAMETER (NPEM=1, NPEP=2, NPWM=1, NPWP=2)
C
C =======< Entry Point >===============================================
C
C The coupling of electron to the photon is denoted G, coupling of the f
C state fermion to the photon is denoted GR, GL for right- and left-hand
C current respectively.
C
      IF( IFIRST .EQ. 1 ) THEN
        CALL COUPLA(SW2 , GAL,GAU,GAD,GWF,GZN,GZL,GZU,GZD,G1,
     &                    GWW3,GWWZ,GWWA)
        IFIRST = 0
      ENDIF
C
C ---------------------------------------------------------------------
C (1) Diagram 1.   S-channel gamma/Z eexchange.
C ---------------------------------------------------------------------
C
      CALL J3XXXX(CSPE(1,NPEM), CSPE(1,NPEP), GAL, GZL, AM(1), AM(2),
     >            CJ3)
      CALL VVVXXX(CVCUR(1,NPWM), CVCUR(1,NPWP), CJ3, GWW3, CAMPL(1))
 
C
C (1.1) Anomalous WWZ/WWA Couplings.
C
C
C      IF( NFLG.NE.0 ) THEN
C        CALL JM0000(CSPE(1,NPEM), GZL, QV, AM(1), AM(2),
C     >              CSPE(1,NPEP), CJ3 )
C        CALL DLWWV0(CVCUR(0,NPWM), PVMOM(0,NPWM),
C     >              CVCUR(0,NPWP), PVMOM(0,NPWP),
C     >              CJ3,           PWRK(0,1),  GWWZ,
C     >              ABCPL(1,1),ABCPL(2,1),ABCPL(3,1),ABCPL(4,1),
C     >              AM(3),         CAMPL(3)  )
C        CALL J00000(CSPE(1,NPEM), GAL, QV,
C     >              CSPE(1,NPEP), CJ3 )
C        CALL DLWWV0(CVCUR(0,NPWM), PVMOM(0,NPWM),
C     >              CVCUR(0,NPWP), PVMOM(0,NPWP),
C     >              CJ3,           PWRK(0,1),  GWWA,
C     >              ABCPL(1,2),ABCPL(2,2),ABCPL(3,2),ABCPL(4,2),
C     >              AM(3),         CAMPL(4)  )
C      ELSE
        CAMPL(3) = 0
        CAMPL(4) = 0
C      ENDIF
C
C ---------------------------------------------------------------------
C (2) Diagram 2.   t-channel neu exchange
C ---------------------------------------------------------------------
C
      CALL FVIXXX(CSPE(1,NPEM), CVCUR(1,NPWM), GWF,
     >            AMNU, AWNU, CSTMP)
      CALL IOVXXX(CSTMP, CSPE(1,NPEP), CVCUR(1,NPWP),
     >            GWF, CAMPL(2))
C
      CAMPL(0) = CAMPL(1) + CAMPL(2) + CAMPL(3) + CAMPL(4)
C
C      PRINT *,' CAMPL =',CAMPL
      RETURN
      END
