C!---------------------------------------------------------------------*\
C!==>    template poked with konstants to control ZVTOP routine         |
C!----------------------------------------------------------------------|
C! KEYWORDS: ZVTOP                                                      |
C!*--------------------------------------------------------------------*|
C!                                                                      |
C!     OWNER: JACKSON, Dave          CO-OWNER: USHER, Tracy             |
C!   SECTION: ZXFIND                     FILE: ZVKON3 TEMPLATE          |
C!                                                                      |
C!                                                                      |
C! Version     Name        Date            Explanation of Changes       |
C! ------- ------------ ----------- ----------------------------------- |
C!  1.00       Dave      11-Jan-95     Creation Date                    |
C!  2.00       Dave      10-Oct-95     Version II in ZXFIND             |
C!  2.50       Dave      16-May-97     a few new options                |
C!  3.00       Dave       4-May-99     major upgrade                    |
C!*--------------------------------------------------------------------*/
 
C Bank ZVKON3  Context=PARAM Maxid=1 DEMAND_ZERO   
               
C! option to first tidy jet with removal of V and interaction tracks
C! see note 1

      INTEGER*4 ZVKON3_TIDY,ZVKON3_MC,ZVKON3_ALGO,
     >          ZVKON3_MODE,ZVKON3_NVREQ

      REAL*4    ZVKON3_PRBV, ZVKON3_B3MIN, ZVKON3_DISNE, 
     >	ZVKON3_MKMIN,ZVKON3_MKMAX,ZVKON3_MLMIN,ZVKON3_MLMAX,
     >  ZVKON3_MGMAX,ZVKON3_DKMIN,ZVKON3_DLMIN,ZVKON3_RIMIN,
     >  ZVKON3_I3MIN,ZVKON3_ICUTP,ZVKON3_ICUTN,ZVKON3_RIPE,
     >  ZVKON3_ZIPE,ZVKON3_PXJA,ZVKON3_PYJA,ZVKON3_PZJA,ZVKON3_RCUT,
     >  ZVKON3_XCUT,ZVKON3_KANG,ZVKON3_KIPW,ZVKON3_PWID,ZVKON3_SWIM,
     >  ZVKON3_GINI,ZVKON3_GMIN,ZVKON3_CMAX,ZVKON3_PCUT,ZVKON3_MOMF

      COMMON/ ZVKON3_BANK/ZVKON3_TIDY,ZVKON3_MC,ZVKON3_ALGO,
     >          ZVKON3_MODE,ZVKON3_NVREQ,
     >  ZVKON3_PRBV, ZVKON3_B3MIN, ZVKON3_DISNE, 
     >	ZVKON3_MKMIN,ZVKON3_MKMAX,ZVKON3_MLMIN,ZVKON3_MLMAX,
     >  ZVKON3_MGMAX,ZVKON3_DKMIN,ZVKON3_DLMIN,ZVKON3_RIMIN,
     >  ZVKON3_I3MIN,ZVKON3_ICUTP,ZVKON3_ICUTN,ZVKON3_RIPE,
     >  ZVKON3_ZIPE,ZVKON3_PXJA,ZVKON3_PYJA,ZVKON3_PZJA,ZVKON3_RCUT,
     >  ZVKON3_XCUT,ZVKON3_KANG,ZVKON3_KIPW,ZVKON3_PWID,ZVKON3_SWIM,
     >  ZVKON3_GINI,ZVKON3_GMIN,ZVKON3_CMAX,ZVKON3_PCUT,ZVKON3_MOMF

C
C INTEGER TIDY /0/          ! set TIDY=0 to use all tracks 
C 
C REAL PRBV  /0.01/         ! consider V if fit prob. > PRBV
C 
C REAL B3MIN /3.0/          ! both V tracks must be B3MIN sigma from IP
C                           ! (for Kshort and lambda only)
C REAL DISNE /250.0/        ! min DIStance to NEarest other track from V
C                           ! in microns (for Kshort, lambda and gamma)
C REAL MKMIN /0.485/        ! lower Mass range for Kshort identification
C REAL MKMAX /0.515/        ! upper Mass...
C 
C REAL MLMIN /1.112/        ! lower Mass range for Lambda identification
C REAL MLMAX /1.120/        ! upper Mass (..both pion-proton combinations)
C 
C REAL MGMAX /0.025/        ! MAX Mass for e+e- conversion
C 
C REAL DKMIN /0.5/          ! MIN distance/cm from IP to V for Kshort
C REAL DLMIN /1.0/          ! MIN distance/cm from IP to V for Lambda
C REAL RIMIN /2.2/          ! MIN Radius/cm for Interaction vertex
C                           ! (for gamma conversion, detector interaction)
C REAL I3MIN /0.15/         ! remove if both tracks 3D Imp. para. > I3MIN/cm 
C                           ! (detector interaction selection only)
C 
C REAL ICUTP /0.3/           ! remove ANY track 3D Imp. para > ICUTP/cm 
C REAL ICUTN /-0.1/          ! remove ANY track 3D Imp. para < ICUTN/cm 
C 
C! general ZVTOP input
C 
C REAL RIPE /7.0/           ! xy error on IP ellipsoid in ZVTOP, microns.
C REAL ZIPE /30.0/          ! z error on IP ellipsoid in ZVTOP, microns.
C 
C REAL PXJA /0.0/           ! Jet Axis direction used to calculate angle
C REAL PYJA /0.0/           ! A for KANG weight for ALGO=1 and for initial
C REAL PZJA /0.0/           ! ghost axis in ALGO=2. By default (all P*JA = 0) 
C                           ! the sum of the input track momenta is used.
C INTEGER MC /1/            ! MC=1 : include Monte Carlo information in
                           ! the output. 
 
C! select ZVTOP algorithm
 
C INTEGER ALGO /1/          ! 1 = ZVRES  original pure vertex RESolubility
C                           ! 2 = ZVKIN  KINematic info and ghost track
C! tunable inputs for ZVRES
 
C REAL RCUT /0.6/           ! resolubility cut, can vary as 0.0 < VCUT < 1.0,
C                           ! larger values produce more vertices.
 
C REAL XCUT /10.0/          ! cut on the chi**2 contribution of a track to 
C                           ! a vertex.  
C 
C REAL KANG /5.0/           ! Weights vertex finding probability by the
C                           ! factor exp(-KANG*A**2) where A is the angle
C                           ! in radians between the line joining the IP
C                           ! to the 3D spatial co-ordinate and the jet
C                           ! axis. (+ve values ~10 increase vertex 
C                           ! finding efficiency in core of jet).
C 
C REAL KIPW /1.0/           ! i.p track weight, weights the significance
C                           ! of the IP, large values will tend to absorb
C                           ! nearby vertices/fakes into the primary vertex.
C 
C 
C INTEGER MODE /0/          ! 0 -usual ZVTOP/ZVRES
C                           ! 1 -lepton or high impact track mode - Note 2
C                           ! 2 -like 1, but remove track for vertex finding 
C                           ! 3 -...and remove space on IP side of lepton 
C 
C REAL PWID /100.0/         ! Gaussian WIDth of Plane for MODE>=1 in microns
C 
C REAL SWIM /0.0/           ! 1.0 - use Aaron's track swim errors in ZVRES
C                           ! SWIM>0.0 multiply swam track by SWIM
C                           ! SWIM<0.0 multiply non-swam track by |SWIM| 
C 
C! tunable inputs for ZVKIN
C 
C REAL GINI /25.0/          ! Initial ghost track width/um
C 
C REAL GMIN /25.0/          ! Minimum ghost track width/um
C 
C REAL CMAX /1.0/           ! Max chi**2 of +ve LODI track to ghost
C 
C REAL PCUT /0.01/          ! Min prob. to cluster track vertices
C 
C INTEGER NVREQ /0/         ! Number of reconstructed Vertices REQuested
C                           ! NVREQ = 0, no request, PCUT is used
C                           ! NVREQ > 0, vertex clustering stops when
C                           ! vertex candidates reduced to NVREQ.
C 
C REAL MOMF /0.0/           ! MOMentum Factor IP/BD discrimination
C                           ! MOMF=1.0 is fairly large momentum weight
C 
CC Endbank
 
C!  Note 1
C!     TIDY > 0     initially loops over all track pairs and find the
C!                  furthest V vertex from IP with fit probability > PRBV.
C!                  If the 2 satisfies the above cuts for Kshort, lambda
C!                  or gamma (and is 0 charge vertex), or interaction
C!                  with detector material, the two tracks are flagged
C!                  and the next furthest 2-prong is then considered
C!                  iteratively until no more tracks are flagged. Flagged
C!                  tracks are not used further by ZVTOP, but they are
C!                  output with their flag in ZVTOPL.
C!
C!     TIDY = 2     if N >= 2, (where N is the number of secondary tracks 
C!     (ALGO=2)     found by ZVKIN) they are fit to a common vertex distance
C!                  D along vertex axis. Tracks that initially failed ICUTP
C!                  but pass close to the vertex (in TRDI and LODI - D) are
C!                  'unflagged' and the whole algorithm is called again.
C!                  Similarly, if N < 2, all tracks initially failing ICUTP
C!                  are put back into the jet and the algorithm rerun.
C!                  (ZVTOPL3.GWID is set -ve to flag such a second iteration).
C!
C!  Note 2
C!     MODE > 0     the first track passed to ZVTOP, id(1), together
C!     (ALGO=1)     with the IP location, is used to define a plane.
C!                  Locations not in the plane are suppressed as
C!                  a function of distance from the plane by weighing 
C!                  V(r) with Gaussian factor tunable via PWID, smaller
C!                  values to PWID confine vertex search closer to plane.
C!                - For track id(1) a reasonable 3D impact distance 
C!                  to the IP, > 2 or 3 sigma, is recommended.
