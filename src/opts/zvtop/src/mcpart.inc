C!---------------------------------------------------------------------*\
C!==> Monte Carlo particle bank                                         |
C!----------------------------------------------------------------------|
C! KEYWORDS: MC PARTICLE                                                |
C!*--------------------------------------------------------------------*|
C!                                                                      |
C!     OWNER: Johnson, Tony          CO-OWNER: Word, Gary B.            |
C!   SECTION: MC                         FILE: MCPART  TEMPLATE         |
C!                                                                      |
C! Version     Name        Date            Explanation of Changes       |
C! ------- ------------ ----------- ----------------------------------- |
C!  0.52   RXD           1 Apr 1991 Add SWUM bit parameter              |
C!  0.51   GaryW         5 Jun 1990 Make all ORIGIN bits PARAMETERS     |
C!  0.50   RXD          16 May 1990 change definition of ORIGIN bits    |
C!  0.00   Tony         20 Nov 1988 Program creation date               |
C!*--------------------------------------------------------------------*/
 
      INTEGER*4 MAX_MCPART
      PARAMETER (MAX_MCPART=500)
      REAL*4   MCPART_P(3,MAX_MCPART), MCPART_E(MAX_MCPART),
     >	       MCPART_PTOT(MAX_MCPART), MCPART_CHARGE(MAX_MCPART),
     >         MCPART_XT(3,MAX_MCPART)
      INTEGER*4 MCPART_PTYPE(MAX_MCPART), MCPART_ORIGIN(MAX_MCPART),
     >          MCPART_PARENT(MAX_MCPART)

      COMMON /MCPART_BANK/ MCPART_P, MCPART_E, MCPART_PTOT, 
     >        MCPART_CHARGE, MCPART_XT, MCPART_PTYPE, 
     >        MCPART_ORIGIN, MCPART_PARENT


C Bank MCPART Context=MCEVENT Maxid=5000 "MC Particle parameters"
C 
C  Real    P(3)       "X,Y,Z momentum of particle at track origin"
C  Real    E          "Energy of particle"
C  Real    PTOT       "Total momentum at track origin"
C 
C  Partid  PTYPE      "Particle type"
C  Real    CHARGE     "Charge of particle"
C 
C  Bits    ORIGIN     "Where did this particle come from/go to" ! Note 1
C  Real    XT(3)      "X,Y,Z of termination"
C 
C  Key     PARENT-->MCPART "Key giving parent particle"
C!
C! Notes:
C!
C!  1) The following bits may be set in ORIGIN (to be extended).
C!
C!                          Meaning of bit if set
C  Value ORIGIN
C   Parameter DECAYED =0  ! Decayed by generator
C   Parameter DECAYFLT=1  ! Decayed in flight by swimmer
C   Parameter BEAMPIPE=2  ! Does not hit detector ( down beam pipe
C   Parameter NOINTER =3  ! Traverses detector w/o interacting
C   Parameter STOPPED =4  ! Energy below cut; other bits may say why
C   Parameter INTERACT=5  ! Interacted, no further decision to be made
C   Parameter INTSHDEP=6  ! Interacted, further decision depends on
C                         ! selection of shower deposition
C   Parameter PRIMARY =8  ! e+e- primary interaction vertex
C   Parameter ISTOP1  =9  ! GEANT ISTOP=1, KCASE ne DCAY or
C                         !   HADR or (PAIR&Gamma)
C   Parameter ISTOP2  =10 ! GEANT ISTOP=2, KCASE ne STOP
C   Parameter KALTOCDC=11 ! Shower particle albedo from KAL back into CD
C   Parameter SWERROR =12 ! Error occured in swimming the track
C   Parameter SW2MNYST=13 ! Swim aborted: too many steps (ISTOP=99)
C   Parameter SWOUTOFT=14 ! Swim aborted: out of sensitive time of
C                         !   detector (ISTOP=4)
C   Parameter EMAXTERM=15 ! EM shower AXis TERMinated for sufficient depth
C   Parameter NOTTRACK=16 ! Not tracked by user request (neutrino, etc)
C   Parameter ISR     =17 ! Initial state radiation
C   Parameter BEAM    =18 ! Initial state beam particle (*)
C   Parameter PREFRAG =19 ! Pre-fragmentation particles (*)
C                         !   (gluons, partons etc.)
C   Parameter Swum    =20 ! this particle was produced by the swimmer
C  EndValue
C!
C!     (*) Particles in these categories will not normally be stored in the
C!         bank, but may be put here at the descretion of the generator, maybe
C!         under control of some flag.
C!
CEndBank
