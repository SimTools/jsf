C***********************************************************************
C*
C* Common /WWZPRM/
C*
C***********************************************************************
C*
      REAL*8 RS, EMINS, EPLUS, EBEAM, XG(25), AHMAS, AHWID
      INTEGER*4  NBSON, NFBMAS, NBSSPR, NBTYPE
      REAL*8     PGEN(0:3,10)
      INTEGER*4  NGDCY(4)
      COMMON /WWZPRM/ RS, EMINS, EPLUS, EBEAM, XG,
     >                AHMAS, AHWID, NBSON, NFBMAS,
     >                PGEN, NGDCY, NBSSPR, NBTYPE
C*
C*   RS  ; Collision energy
C*   EMINS ; Electron energy at collision.
C*   EPLUS ; Positron energy at collision.
C*   EBEAM ; Beam energy.
C*   AHMAS ; Higgs mass  (GeV)
C*   AHWID ; Higgs width (GeV)
C*
C*   XG(25) ; Function values of this event.
C*   PGEN(0:3,i) ; 4 momentum of generated particle.
C*   NGDCY(4)    ; Decay mode
C*   NBSSPR   = 0 for bases, 1=Spring.
C*   NBTYPE   = Beamstrahlung spectrum type.
C*
C*   Meanings of decay mode. (Determined by sub. WIDW,WIDZ
C*      W: 1=(n(e),e),2=(n(mu),mu), 3=(n(tau),tau),
C*         4=(u,d),   5=(u,s),      6=(u,b),
C*         7=(c,d),   8=(c,s),      9=(c,b),
C*        10=(t,d),  11=(t,s),     12=(t,b)
C*      Z: 1=(nu(e),nu(e)), 2=(nu(mu),nu(mu)), 3=(nu(tau),nu(tau))
C*         4=(e-,e+),  5=(mu-,mu+),  6=(tau-,tau+)
C*         7=(u,ubar), 8=(c,dbar),   9=(t,tbar),
C*        10=(d,dbar),11=(s,sbar),  12=(b,bar)
C*
