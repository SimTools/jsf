CJSF      PROGRAM HWIGPR
      SUBROUTINE herwig64_test
C---COMMON BLOCKS ARE INCLUDED AS FILE HERWIG64.INC
      INCLUDE 'HERWIG64.INC'
      INTEGER N
      EXTERNAL HWUDAT
C---MAX NUMBER OF EVENTS THIS RUN
      MAXEV=100
C---BEAM PARTICLES
      PART1='PBAR'
      PART2='P'
C---BEAM MOMENTA
      PBEAM1=1000.
      PBEAM2=1000.
C---PROCESS
      IPROC=1500
C---INITIALISE OTHER COMMON BLOCKS
      CALL HWIGIN
C---USER CAN RESET PARAMETERS AT
C   THIS POINT, OTHERWISE DEFAULT
C   VALUES IN HWIGIN WILL BE USED.
      PTMIN=100.
C---COMPUTE PARAMETER-DEPENDENT CONSTANTS
      CALL HWUINC
C---CALL HWUSTA TO MAKE ANY PARTICLE STABLE
      CALL HWUSTA('PI0     ')
C---USER'S INITIAL CALCULATIONS
      CALL HWABEG
C---INITIALISE ELEMENTARY PROCESS
      CALL HWEINI
C---LOOP OVER EVENTS
      DO 100 N=1,MAXEV
C---INITIALISE EVENT
      CALL HWUINE
C---GENERATE HARD SUBPROCESS
      CALL HWEPRO
C---GENERATE PARTON CASCADES
      CALL HWBGEN
C---DO HEAVY OBJECT DECAYS
      CALL HWDHOB
C---DO CLUSTER FORMATION
      CALL HWCFOR
C---DO CLUSTER DECAYS
      CALL HWCDEC
C---DO UNSTABLE PARTICLE DECAYS
      CALL HWDHAD
C---DO HEAVY FLAVOUR HADRON DECAYS
      CALL HWDHVY
C---ADD SOFT UNDERLYING EVENT IF NEEDED
      CALL HWMEVT
C---FINISH EVENT
      CALL HWUFNE
C---USER'S EVENT ANALYSIS
      CALL HWANAL
  100 CONTINUE
C---TERMINATE ELEMENTARY PROCESS
      CALL HWEFIN
C---USER'S TERMINAL CALCULATIONS
      CALL HWAEND
CJSF      STOP
      RETURN
      END
C----------------------------------------------------------------------
      SUBROUTINE HWABEG
C     USER'S ROUTINE FOR INITIALIZATION
C----------------------------------------------------------------------
      INCLUDE 'HERWIG64.INC'
      END
C----------------------------------------------------------------------
      SUBROUTINE HWAEND
C     USER'S ROUTINE FOR TERMINAL CALCULATIONS, HISTOGRAM OUTPUT, ETC
C----------------------------------------------------------------------
      END
C----------------------------------------------------------------------
      SUBROUTINE HWANAL
C     USER'S ROUTINE TO ANALYSE DATA FROM EVENT
C----------------------------------------------------------------------
      END
C----------------------------------------------------------------------
