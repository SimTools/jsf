C!---------------------------------------------------------------------*\
C!==>    template filled with topological vertices from ZVTMC           |
C!----------------------------------------------------------------------|
C! KEYWORDS: ZVTOP                                                      |
C!*--------------------------------------------------------------------*|
C!                                                                      |
C!     OWNER: JACKSON, Dave          CO-OWNER: USHER, Tracy             |
C!   SECTION: ZXFIND                     FILE: ZVTOPL3 TEMPLATE         |
C!                                                                      |
C!                                                                      |
C! Version     Name        Date            Explanation of Changes       |
C! ------- ------------ ----------- ----------------------------------- |
C!  1.00       Dave      1-Feb-95     Creation Date                     |
C!  2.00       Dave     10-Oct-95     Version II in ZXFIND              |
C!  3.00       Dave      4-May-99     Major upgrade                     |
C!*--------------------------------------------------------------------*/

      INTEGER*4  MAXTRK_ZVTOPL3, MAXJET_ZVTOPL3
      PARAMETER (MAXTRK_ZVTOPL3=40)
      PARAMETER (MAXJET_ZVTOPL3=20)
	
      INTEGER*4  IBNK_ZVTOPL3(15,MAXJET_ZVTOPL3),
     >           IVRT_ZVTOPL3(2,7,MAXJET_ZVTOPL3),
     >           ITRK_ZVTOPL3(4,MAXTRK_ZVTOPL3,MAXJET_ZVTOPL3)
      REAL*4     RBNK_ZVTOPL3(22,MAXJET_ZVTOPL3),
     >           RVRT_ZVTOPL3(14,7,MAXJET_ZVTOPL3),
     >           RTRK_ZVTOPL3(17,MAXTRK_ZVTOPL3,MAXJET_ZVTOPL3)
      COMMON /ZVTOPL3/ IBNK_ZVTOPL3,IVRT_ZVTOPL3,ITRK_ZVTOPL3, 
     >           RBNK_ZVTOPL3,RVRT_ZVTOPL3,RTRK_ZVTOPL3

 
C Bank ZVTOPL3  Context=DST  Maxid=5000 DEMAND_ZERO   
 
C! Variables of type MC* come from MC Truth, all others are reconstructed 
C 
C Data for IBNK_ZVTOPL3(15)
C                          !12 means filled be both ALGO=1 and ALGO=2 
C 1    INTEGER NTRK            !12 no. of tracks input        
C 2    INTEGER NVRT            !12 no. of vertices found      
C 3    INTEGER NISO            !12 no. of tracks left isolated 
 
C 4:7  INTEGER MCB(4)          !12 (1) LUND 6.3 Part ID of B angularly closest to jet
C                          !   (2) MC track multiplicity at this vertex
C                          !   (3) charge at this vertex
C                          !   (4) MCPART index
C 8:11 INTEGER MCD(4)          !12    
C                          !    MCD,MCDX in ccbar events contain MC D
C                          !    vertex information equivalent to MCB
C 12:15 INTEGER MCD2(4)         !12  events MCD and MCDX correspond to the
C                          !    D produced by the B decay; MCD2 and 
C                          !    MCDX2 are filled if a second D meson
C
C Data for RBNK_ZVTOPL3(22)
C  1:4  REAL    PTKS(4)         !12 XYZ,magnitude of summed track momenta
C  5:7  REAL    IPX(3)          !12 XYZ location of IP from PHBM(BEAMPOS)
C                          !   (more accurate than VRTS(1),POS(3) below) 
C  8:10 REAL    GAXI(3)         ! 2 found Ghost axis direction 
C 11    REAL    GWID            ! 2 fitted Ghost track Gaussian width /um
C 12:14 REAL    MCBX(3)         !12 XYZ location of MC B vertex
C 15:17 REAL    MCDX(3)         !12  and MCBX for bbbar MC events. For bbbar 
C 18:20 REAL    MCDX2(3)        !12  is created from the decaying B.
C 21:22 REAL    MCANG(2)        !12  (1)cosine of angle of the primary B or D 
C                          !    reported above closest to jet axis 
C                          !    (2)cosine of next closest

C Data for IVRT_ZVTOPL3(2,7,NJET) 
C BLOCK VRTS(7)            ! see Note 1 
C                     
C 1 INTEGER NTRV            !12 no. of tracks in this vertex
C 2 INTEGER CHARGE          !12 vertex charge
C
C Data for RVRTS_ZVTOPL3(14,7,NJET)
C 1    REAL    VSIG            !1  Vertex SIGnificance func. value
C 2:4  REAL    POS(3)          !12 XYZ position of vertex   ...from ZXFIT 
C 5:10 REAL    DPOS(6)         !12 ...and error matrix      ...  "    "
C 11   REAL    XISQ            !12 chi-squared of vertex fit         
C 12   REAL    PRBV            ! 2 probability of vertex fit    
C 13   REAL    MASS            !12 assuming pion particle masses
C 14   REAL    ALPHA           !1  angular displacement of vertex
 
C ENDBLOCK
 
 
C BLOCK TRKS(NTRK<31)      ! see Note 2
C
C Data for ITRK_ZVTOPL3(4,ntrk,njet) 
C 1 INTEGER ZID             !12 id of track in ZXTRKS
C 2 INTEGER VNO             !12 no. of the vertex the track is associated
C                           !   with in the VRTS BLOCK above.
C 3 INTEGER MCFL            !12 Monte Carlo flavour of origin from BBTMTYP
C                           !   see note 3
C 4 INTEGER MCVT            !12 Monte Carlo vertex type from BBTMTYP
C                           !   see note 3
C
C Data for RTRK_ZVTOPL3(17,ntrk,njet)
C 1    REAL    XIST            !12 chi-squared contribution of the track to
C                              !   the vertex.
C 2:8  REAL    XISG(7)         ! 2 chi**2 cont. of track to VRTS(1-NVRT)
C 9:15 REAL    PRBG(7)         ! 2 vertex prob. if this track added to VRTS
C 16   REAL    TRDI            !12 distance in cm from the track to the IP
C                          !   if no secondary vertex is found, otherwise
C                          !   the closest approach of the track to the
C                          !   line 2.5 cm long from the IP passing through
C                          !   the 'secondary' vertex furthest from the 
C                          !   IP, - intended to check if isolated tracks
C                          !   are consistent with the decay chain
C 17   REAL    LODI            !12 distance from IP along 2.5 cm line to TRDI
 

C ENDBLOCK
 
CEndbank
 
C!  Note 1 :
C!     VRTS(N)
C!     The first vertex, N=1, is the primary, contains 0 to NTRK tracks.
C!     For ALGO=1 all other vertices must contain at least 2 tracks and
C!   are ordered according to their distance from the IP.
C!     For ALGO=2 all other vertices must contain at least 1 track and
C!   are ordered by their distance from the IP in the direction GAXI(3).
C!     For ALGO=2, POS and DPOS - for N=1, the primary tracks are
C!   fit with the IP ellipsiod - for N>1 only the vertex tracks are fit
C!   unless it is a 1-prong, in which case it is fit with the ghost track.
C!     For ALGO=2, XISQ and PRBV and determined by fitting the vertex 
C!   tracks together with the IP ellipsoid (for N=1) or with the ghost
C!   track (for N>1). 
C!
C!  Note 2 :
C!     TRKS(M)
C!     VNO = 1, means track in primary vertex etc...
C!     For ALGO=1, VNO = 0 track is classified as isolated
C!     For ALGO=2, VNO = -1 if the track is in a non-primary vertex which
C!   is 'behind' the IP w.r.t. the ghost axis direction. The total number
C!   of tracks with VNO=-1 is output in NISO  
C!     For ALGO=2, if track M is added (or already belongs to) vertex N
C!   it will (does) contribute the chi**2 XISG(N) to vertex N which in
C!   turn will (does) have total fit probablity PRBG(N). 
C!     Note, XISG(VNO) = XIST for each track in ALGO=2.  
C!     For ALGO=2, TRDI and LODI are measured w.r.t the ghost axis.
C!     For both algorithms, if TIDY=1 was used, then VNO also flags the
C!   which tracks were suppressed on the input list and why:
C!     VNO = -10    Kshort
C!     VNO = -20    Lambda
C!     VNO = -30    Gamma conversion
C!     VNO = -40    Other detector interaction vertex
C!     VNO = -50    High impact parameter
C!
C!  Note 3 : copied from DUCSROOT:[HEAVY]BBTMTYP.PREPMORT               
C!                                                                      
C!   MCFL (IPFLAV)--- Output The parent decay particle flavour type     
C!                   -1 = Unkown                                        
C!                    0 = Unstable primary                              
C!                    1 = Bottom  weak decay                           
C!                    2 = Charm   weak decay                            
C!                    3 = Tau     weak decay                            
C!                    4 = Strange weak decay                            
C!                    5 = Decay of long lived particle                  
C!   MCVT (IPVTX) --- Output The parent decay vertex type               
C!                   -1 = Unknown                                       
C!                    0 = Primary                                       
C!                    1 = Secondary decay                               
C!                    2 = Tertiary  decay                               
C!                    3 = Strange & longer lived decays                 
C!
 
