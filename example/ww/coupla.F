C
C **********************************************************************
C
      SUBROUTINE COUPLA (SW2 , GAL,GAU,GAD,GWF,GZN,GZL,GZU,GZD,G1,
     &                        GW,GWWZ,GWWA)
C
C This subroutine sets up the coupling constants in the STANDARD MODEL.
C The array of couplings specifies the chirality of the INCOMING
C fermion. G?(1) denotes a left-handed coupling, and G?(2) a right-
C handed coupling.
C
C INPUT:
C       real    SW2            : square of sine of Weinberg angle
C
C OUTPUT:
C       real    GAL(2)         : coupling with A of charged leptons
C       real    GAU(2)         : coupling with A of up-type quarks
C       real    GAD(2)         : coupling with A of down-type quarks
C       real    GWF(2)         : coupling with W-,W+ of fermions
C       real    GZN(2)         : coupling with Z of neutrinos
C       real    GZL(2)         : coupling with Z of charged leptons
C       real    GZU(2)         : coupling with Z of up-type quarks
C       real    GZD(2)         : coupling with Z of down-type quarks
C       real    G1(2)          : unit coupling of fermions
C       real    GW             : weak coupling constant
C       real    GWWZ           : coupling of W-,W+,Z
C       real    GWWA           : coupling of W-,W+,A
C
      IMPLICIT REAL (A-H,O-Z)
      REAL GAL(2),GAU(2),GAD(2),GWF(2),
     >       GZN(2),GZL(2),GZU(2),GZD(2),G1(2)
C
      CALL COUP1X(SW2, GW, GWWA, GWWZ)
      CALL COUP2X(SW2, GAL, GAU, GAD, GWF, GZN,GZL,GZU,GZD,G1)
 
      RETURN
      END
C
      SUBROUTINE PSUMXX( PEMF, PNU, PW)
      REAL PEMF(0:3), PNU(0:3), PW(0:3)
      DO 100 I = 0, 3
         PW(I) = PEMF(I) + PNU(I)
 100  CONTINUE
      RETURN
      END
 
      SUBROUTINE PMIRRX(PA, PB)
      REAL PA(0:3),PB(0:3)
      PB(0) = PA(0)
      PB(1) = -PA(1)
      PB(2) = -PA(2)
      PB(3) = -PA(3)
      RETURN
      END
