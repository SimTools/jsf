C***********************************************************************
C*
C*  SPRING module for e+e- --> W+ W-
C*
C*  Besides the 13 integration variable used in BASES,  additional
C*  two randum variables are used to
C*     (1) Decide the direction of beamstraulung photon.
C*     (2) Swich e-neubarW+ and e+neuW-
C*
C*  A. Miyamoto  22-Oct-1990
C*
C***********************************************************************
C
C
C***********************************************************************
C*===================                                                  *
C* Subroutine SPINIT                                                   *
C*===================                                                  *
C*                                                                     *
C*     This routine is called at Run initialization.                   *
C*     Store Run conditions into Begin_Run records.                    *
C*                                                                     *
C*   Coded by A. Miyamoto  28-Aug-1990                                 *
C*                                                                     *
C***********************************************************************
C
      SUBROUTINE SPINIT
C
c      IMPLICIT REAL*8 (A-H,O-Z,$)
      IMPLICIT REAL*8 (A-H,O-Z)
C
      EXTERNAL    FUNC
C
      include 'WWZPRM.inc'
C
C
      COMMON /SSWORK/ ELMNO2(20), ELMNO3(10), IDUMMY(50)
      CHARACTER*32    PRCNAM
C
C =====< Entry Point >=================================================
C
C
C ---------------------------------------------------------------------
C (1) Integrand.
C ---------------------------------------------------------------------
C
      MXTRY = 500
      CALL SPFUNC(   FUNC, MXTRY )
C
      NBSSPR = 1
C
C
C ---------------------------------------------------------------------
C (1) Store Run parapemter into Bank 'Spring' Elm# 2.
C ---------------------------------------------------------------------
C
C     ELMNO2(1) = E-beam (GeV)
C           (2) = 1. When Beamstrahlung is on
C               = 0. When Beamstrahlung is off
C           (3:12) = reserved to store beam parameter.
C
      ELMNO2(1) = EBEAM
      ELMNO2(2) = NBSON
      ELMNO2(3) = 0.
      ELMNO2(4) = 0.
      ELMNO2(5) = 0.
      ELMNO2(6) = 0.
      ELMNO2(7) = 0.
      ELMNO2(8) = 0
      ELMNO2(9) = 0
      ELMNO2(10)= 0
      ELMNO2(11)= 0
      ELMNO2(12)= 0
C
      PRCNAM    = 'WW'
      READ(PRCNAM,'(8A4)') (ELMNO2(K),K=13,20)
      NWD       = 20
C
      CALL TBPUT(2,'Spring',2,NWD,ELMNO2, IRET )
      IF ( IRET .LT. 0 ) THEN
        PRINT *,' USERIN failed to TBPUT SPRING Elm#2 ',IRET
        STOP
      ENDIF
C
C ---------------------------------------------------------------------
C (2) Store Run parapemter into Bank 'Spring' Elm# 3.
C     Store information specific to this process.
C ---------------------------------------------------------------------
C
C     ELMNO3(1) = AHMAS
C     ELMNO3(2) = AHWID
C     ELMNO3(3) = NFBMAS
C
C     NWD = 3
C     CALL TBPUT(2,'Spring',3,NWD,ELMNO3, IRET )
C     IF ( IRET .LT. 0 ) THEN
C       PRINT *,' USERIN failed to TBPUT SPRING Elm#3 ',IRET
C       STOP
C     ENDIF
C
C
      RETURN
      END
C***********************************************************************
C*                                                                     *
C*===================                                                  *
C* Subroutine SPEVNT                                                   *
C*===================                                                  *
C*                                                                     *
C*(Purpose)                                                            *
C*   To enerate One event.                                             *
C*(Author)                                                             *
C*   Coded by A. Miyamoto 28-Aug-1990                                  *
C*                                                                     *
C***********************************************************************
C
      SUBROUTINE SPEVNT(NRET)
C
C
      include 'WWZPRM.inc'
C
C... Decay mode to Particle ID
C
      INTEGER*4  IDMODE(2,12,3)
      DATA  IDMODE/ -12, 11, -14, 13, -16, 15,
     >               -2,  1,  -2,  3,  -2,  5,
     >               -4,  1,  -4,  3,  -4,  5,
     >               -6,  1,  -6,  3,  -6,  5,
     >               12,-11,  14,-13,  16,-15,
     >                2, -1,   2, -3,   2, -5,
     >                4, -1,   4, -3,   4, -5,
     >                6, -1,   6, -3,   6, -5,
     >               12,-12,  14,-14,  16,-16,
     >               11,-11,  13,-13,  15,-15,
     >                2, -2,   4, -4,   6, -6,
     >                1, -1,   3, -3,   5, -5 /
      INTEGER*4  N3CHG(4) /0, 3, 2, 1/
      DATA       AMZ/91.17/,  AMW/80.0/
      INTEGER*4  NCHG(2,12)/ 0.,-3., 0.,-3., 0.,-3.,
     >                       2.,-1., 2.,-1., 2.,-1.,
     >                       2.,-1., 2.,-1., 2.,-1.,
     >                       2.,-1., 2.,-1., 2.,-1./
C
      INTEGER*4  IBUF(10)
      REAL*4     RBUF(100)
      COMMON /SSWORK/ IBUF, RBUF
      REAL*4  PINI(0:3), PTEMP(0:3)
      PARAMETER ( ALPHA0 = 1./137.0359895)
C
      DATA   NEVENT/0/
      DATA   PI/3.14159265/
      DATA   AMEL/0.51099906E-3/
      DATA   ISEED/1414213561/
CC    DATA   NDMODE/1/
C
C NDMODE = 0 ; Phase space decay of W using LUND6.3
C        = 1 ; Include polarization effect.
C
C
C =====< Entry Point >=================================================
C
C
C ---------------------------------------------------------------------
C (1) Output event information into bank.
C ---------------------------------------------------------------------
C
C ... Spring:Header
C       Elm#1  IBUF(1) = Event #.
C                  (2) = Date
C                  (3) = Time
C                  (4) = not used
C                  (5) = E (e-)    MeV unit.
C                  (6) = E (e+)    MeV unit.
C                  (7) = Ebeam     MeV unit.
C                  (8) = Pol (e-)
C                  (9) = Pol (e+)
C                 (10) = not used.
 
 
      NEVENT = NEVENT + 1
      IBUF(1) = NEVENT
CDBG       PRINT *,'Generating Nevent=',NEVENT
CDBG       PRINT *,'           NBSSPR=',NBSSPR
CDBG       PRINT *,' PGEN(,1)=',(PGEN(K,1),K=0,3)
CDBG       PRINT *,' PGEN(,2)=',(PGEN(K,2),K=0,3)
CDBG       PRINT *,' PGEN(,3)=',(PGEN(K,3),K=0,3)
CDBG       PRINT *,' PGEN(,4)=',(PGEN(K,4),K=0,3)
CDBG       PRINT *,' PGEN(,5)=',(PGEN(K,5),K=0,3)
      CALL UIDATE( IBUF(2) )
      CALL UITIME( IBUF(3) )
      IBUF(5) = EMINS*1.E3
      IBUF(6) = EPLUS*1.E3
      IBUF(7) = EBEAM*1.E3
      IBUF(8) = 0.
      IBUF(9) = 0
      CALL TBPUT(1, 'Spring:Header', 1, 10, IBUF, IRET )
C
C ... Store event  parameter into element # 2.
C
      CALL TBPUT(1,'Spring:Header',2,36,RS,IBUF)
C
C
C
C --------------------------------------------------------------------
C (2) Recalculate Beam energy after beamstrulung and Boost momentum
C --------------------------------------------------------------------
C
C    e+e- --> W- W+
C             +  +---> f(1) + fbar(2)
C             +------> f(3) + fbar(4)
C
C
C             1=e-, 2=neubar, 3=W+, 4=f, 5=fbar
C
C
      IF( NBSON .EQ. 2 .OR. NBSON .EQ.3 ) THEN
        XDIR   = DRN(ISEED)
        S      = 4.*EMINS*EPLUS
        BEEE   = 2.*ALPHA0/PI*( ALOG(S/(AMEL*AMEL)) - 1. )
        BEEINV = 1./BEEE
        IF( XDIR .LT. 0.5 ) THEN
          EMINS = (1. - XG(7)**BEEINV)*EMINS
        ELSE
          EPLUS = (1. - XG(7)**BEEINV)*EPLUS
        ENDIF
      ENDIF
      PINI(0) = (EMINS+EPLUS)
      PINI(1) = 0
      PINI(2) = 0
      PINI(3) =  EMINS - EPLUS
C      PRINT *,' '
C      PRINT *,' EMINS=',EMINS,' EPLUS=',EPLUS
C      PRINT *,' PINI =',PINI
      DO 210 I = 1, 6
C        PRINT *,' I=',I,'  PGEN=',(PGEN(K,I),K=0,3)
        DO 212 J = 0, 3
          PTEMP(J) = PGEN(J,I)
212     CONTINUE
        CALL BOOST0( PTEMP, PINI, PTEMP)
        DO 213 J = 0, 3
          PGEN(J,I) = PTEMP(J)
213     CONTINUE
 
C        PRINT *,'   boosted to  =',(PGEN(K,I),K=0,3)
210   CONTINUE
C
C Put minimum Pt of 1MeV to avoid error in simulator.
C
      DO 220 I = 1, 6
        PTS= PGEN(1,I)**2 + PGEN(2,I)**2
        IF( PTS .GT. 1.E-6 ) GO TO 220
        PGEN(1,I) = 1.E-3
220   CONTINUE
C
C
C ... Spring:Parton_List
C       Elm#i  RBUF(1) = Particle serial number.
C                  (2) = Particle ID
C                  (3) = Mass (GeV)
C                  (4) = Charge
C                  (5) = Px(GeV)
C                  (6) = Py(GeV)
C                  (7) = Pz(GeV)
C                  (8) = E(GeV)
C                  (9) =    not used
C                 (10) =    not used
C                 (11) =    not used
C                 (12) = # daughter parton
C                 (13) = particle serial # of the 1st daughter
C                 (14) = particle serial # of the parent daughter
C                 (15) =    not used
C                 (16) =    not used
C                 (17) = helicity
C                 (18) = colour single group ID
C                 (19) =    not used
C                 (20) =    not used
C
C --------------------------------------------------------------------
C (2) Store 4 Vector of produced particles.
C --------------------------------------------------------------------
C
C  W-
C
      CALL UVZERO( 20, RBUF )
      RBUF( 1) =   1
      RBUF( 2) =  -24
      RBUF( 3) =   AMW
      RBUF( 4) =   -1
      RBUF( 5) =   PGEN(1,1)
      RBUF( 6) =   PGEN(2,1)
      RBUF( 7) =   PGEN(3,1)
      RBUF( 8) =   PGEN(0,1)
      RBUF(12) =   2.
      RBUF(13) =   3.
      CALL TBPUT(1,'Spring:Parton_List', 1, 20, RBUF, IRET )
C
C  W+
C
      CALL UVZERO( 20, RBUF )
      RBUF( 1) =   2
      RBUF( 2) =   24
      RBUF( 3) =   AMW
      RBUF( 4) =   +1
      RBUF( 5) =   PGEN(1,2)
      RBUF( 6) =   PGEN(2,2)
      RBUF( 7) =   PGEN(3,2)
      RBUF( 8) =   PGEN(0,2)
      RBUF(12) =   2.
      RBUF(13) =   5.
      CALL TBPUT(1,'Spring:Parton_List', 2, 20, RBUF, IRET )
C
C  f from W-
C
      CALL UVZERO( 20, RBUF )
      RBUF( 1) =   3
      RBUF( 2) =   IDMODE(2,NGDCY(1),1)
      RBUF( 3) =   0.
      RBUF( 4) =   FLOAT(NCHG(2,NGDCY(1)))/3.
      RBUF( 5) =   PGEN(1,3)
      RBUF( 6) =   PGEN(2,3)
      RBUF( 7) =   PGEN(3,3)
      RBUF( 8) =   PGEN(0,3)
      RBUF(14) =   1.
      RBUF(18) =   1.
      RBUF(19) =   101.
      IF( IABS(IFIX(RBUF(2))).GE.11 ) RBUF(19)=0.
      CALL TBPUT(1,'Spring:Parton_List', 3, 20, RBUF, IRET )
C
C  fbar from W-
C
      CALL UVZERO( 20, RBUF )
      RBUF( 1) =   4
      RBUF( 2) =   IDMODE(1,NGDCY(1),1)
      RBUF( 3) =   0.
      RBUF( 4) =   FLOAT(NCHG(1,NGDCY(1)))/3.
      RBUF( 5) =   PGEN(1,4)
      RBUF( 6) =   PGEN(2,4)
      RBUF( 7) =   PGEN(3,4)
      RBUF( 8) =   PGEN(0,4)
      RBUF(14) =   1.
      RBUF(18) =   1.
      RBUF(19) =   101.
      IF( IABS(IFIX(RBUF(2))).GE.11 ) RBUF(19)=0.
      CALL TBPUT(1,'Spring:Parton_List', 4, 20, RBUF, IRET )
 
C
C  f from W+
C
      CALL UVZERO( 20, RBUF )
      RBUF( 1) =   5
      RBUF( 2) =   IDMODE(1,NGDCY(2),2)
      RBUF( 3) =   0.
      RBUF( 4) =  -FLOAT(NCHG(1,NGDCY(2)))/3.
      RBUF( 5) =   PGEN(1,5)
      RBUF( 6) =   PGEN(2,5)
      RBUF( 7) =   PGEN(3,5)
      RBUF( 8) =   PGEN(0,5)
      RBUF(14) =   2.
      RBUF(18) =   2.
      RBUF(19) =   202.
      IF( IABS(IFIX(RBUF(2))).GE.11 ) RBUF(19)=0.
      CALL TBPUT(1,'Spring:Parton_List', 5, 20, RBUF, IRET )
C
C  fbar from W+
C
      CALL UVZERO( 20, RBUF )
      RBUF( 1) =   6
      RBUF( 2) =   IDMODE(2,NGDCY(2),2)
      RBUF( 3) =   0.
      RBUF( 4) =  -FLOAT(NCHG(2,NGDCY(2)))/3.
      RBUF( 5) =   PGEN(1,6)
      RBUF( 6) =   PGEN(2,6)
      RBUF( 7) =   PGEN(3,6)
      RBUF( 8) =   PGEN(0,6)
      RBUF(14) =   2.
      RBUF(18) =   2.
      RBUF(19) =   202.
      IF( IABS(IFIX(RBUF(2))).GE.11 ) RBUF(19)=0.
      CALL TBPUT(1,'Spring:Parton_List', 6, 20, RBUF, IRET )

C      DO 100 I = 1, 6
C         print *,' PGEN =',(PGEN(K,I),K=0,3)
C 100     CONTINUE
 
C
C      CALL SPBLST(6)
C
      NRET = 0
 
      RETURN
      END
