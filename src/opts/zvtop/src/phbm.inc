C
C  Beam information
C
C!---------------------------------------------------------------------*\
C!==> Information on the Interaction Point and Beam Energy              |
C!----------------------------------------------------------------------|
C! KEYWORDS: DST BEAM MINI MINI_DST                                     |
C!*--------------------------------------------------------------------*|
C!                                                                      |
C!     OWNER: TonyJ,Strauss,Homer    CO-OWNER:                          |
C!   SECTION: DST                        FILE: PHBM  TEMPLATE           |
C!                                                                      |
C!                                                                      |
C! Version     Name        Date            Explanation of Changes       |
C! ------- ------------ ----------- ----------------------------------- |
C!  0.00   mDST group   06 Jan 1994 Created as replacement for phbeam   |
C!  0.01   mDST group   11 Apr 1994 context changed to DST              |
C!*--------------------------------------------------------------------*/
 
      REAL*4 PHBM_ECM, PHBM_DECM,
     >       PHBM_BEAMPOS(3),PHBM_DBEAMPS(6),
     >       PHBM_POL,PHBM_DPOL,PHBM_PDTIME,
     >       PHBM_XYZPRI(3),PHBM_DXYZPRI(6),PHBM_RMSPRI    
      INTEGER   PHBM_RTRNPRI   

      COMMON /PHBM_BANK/ PHBM_ECM, PHBM_DECM,
     >       PHBM_BEAMPOS,PHBM_DBEAMPS,
     >       PHBM_POL,PHBM_DPOL,PHBM_PDTIME,
     >       PHBM_XYZPRI,PHBM_DXYZPRI,PHBM_RMSPRI,
     >       PHBM_RTRNPRI   

CBank PHBM Context=DST Maxid=1             "Beam energy and position"
C 
C  Real      ECM        "Center of mass energy"
C  Real      DECM       "Error on CM energy"
C 
C  REAL      BEAMPOS(3)    " I.P. Position from database             "
C  REAL      DBEAMPS(6)    " I.P. Position error from database       "
C 
C  Real      POL        "Signed Polarization:  RIGHT>0  LEFT<0 "
C  Real      DPOL       "Statistical error on polarization of e- beam"
C  Real      PDTIME     "Time dif between pol measurement and event (Seconds) "
C 
CC! Variables from ZXPRI primary vertex finding for this event
C  REAL      XYZPRI(3)     " I.P. from single event                  "
C  REAL      DXYZPRI(6)    " Error on I.P. from single event         "
C  REAL      RMSPRI        " RMS OF PRIMARY VERTEX FIT               "
C  INTEGER   RTRNPRI       " RTN CODE FROM PRIMRY VTX FINDER: see note 1"
C 
CCEndbank
 
C! Note 1
C!
C!     RTRNPRI  =   1   CONVERGED FIT
C!                  2   UNCONVERGED
C!                 12   FAILURE ON ENTRY
