
C--------------------------------------------------------->>>>ZVKIN
      INTEGER FUNCTION ZVKIN (NTRK,ID,VPOS,GAXI,JETNO)
      IMPLICIT NONE
#include "zvkon3.inc"
#include "zxtrks.inc"
#include "zvtopl3.inc"
      INTEGER NTRK
      INTEGER ID(30)
      REAL VPOS(3)
      REAL GAXI(3)
      INTEGER JETNO
      INTEGER   SIGNAL$S_WARNING
      PARAMETER (SIGNAL$S_WARNING=3)
      INTEGER  SGLGBL, SGLCHK, SGLRTN, SGLHDL, SGLRMV, SGLGET, SGLLEV,
     *SGLIT, SGLISS, SIGSEV(0:7)/3,1,4,2,5,5,5,5/
      COMMON /SGLCOM/ SGLLEV
CDEC$ PSECT /SGLCOM/ NOSHR
      INTEGER*4 SLD$NORMAL
      PARAMETER (SLD$NORMAL=1)
C----------------------------------------------------------------------
C
C ZVKIN -- ZVTOP KINematic algorithm option, with ghost track
C----------------------------------------------------------------------
C
C  Arguments
C  =========
C   NTRK, id(30) = the input ZXTRKS numbers
C   vpos(3)      = IP position, base of ghost axis
C   gaxi(3)      = initial ghost axis direction
C----------------------------------------------------------------------
C KEYWORDS: ZVTOP
C*--------------------------------------------------------------------*
C
C       OWNER: D. Jackson            CO-OWNER:
C     SECTION: ZXFIND
C
C                    Date            Explanation of Changes
C                ----------- ------------------------------------------
CVersion   0.00;  8 Mar 1999          Program creation date
CVersion   0.01; 24 FEB 2000 ASCHOU:use FITMODE=-1 to initialize ZXFITA
C*--------------------------------------------------------------------*
      COMMON /MCHEAD /MCHEAD
      INTEGER MCHEAD(-20:1)
CDEC$ PSECT /MCHEAD/ NOSHR
      COMMON /ZXTRKS /ZXTRKS
      INTEGER ZXTRKS(-20:1)
CDEC$ PSECT /ZXTRKS/ NOSHR
      COMMON /MCPART /MCPART
      INTEGER MCPART(-20:5000)
CDEC$ PSECT /MCPART/ NOSHR
      COMMON /ZVKON3 /ZVKON3
      INTEGER ZVKON3(-20:1)
CDEC$ PSECT /ZVKON3/ NOSHR
      COMMON /ZVTOPL3 /ZVTOPL3
      INTEGER ZVTOPL3(-20:5000)
CDEC$ PSECT /ZVTOPL3/ NOSHR
      LOGICAL FIRST/.TRUE./,POSHEM,FLAG,AGAIN,TID3,DOWN,GAP
      INTEGER PPHBMFAM
      INTEGER PPHBM
      INTEGER PHCHRG
      INTEGER PRENT
C*         pZBoutL    --> ZBoutL;                                    *
CJLC      INTEGER JZBFND,JZPIDX,JZBLOC,JZBEXP,ZYTOP,ZVPTM,ZXFIT,BTMIP
      INTEGER JZBFND,JZPIDX,JZBLOC,JZBEXP,ZYTOP,ZVPTM,BTMIP
      INTEGER ZVBST,BLKLBST,BLLNKLPD
      INTEGER BZBOOST,ZVGST,ZVDIP,ZVOUT
      INTEGER MULTV,MULTA
      INTEGER MCID,MCPA(30),PTYP,PATYP,IPMC,IPFLAV,IPVTX
      INTEGER I,J,K,II,JJ,NZT,NCHRG,ERR,IER,IV
      INTEGER IDL,IDI,IDI2,IDS,IDSEC(30),NSEC,CSEC,DUMMY
      INTEGER BID,BMIX,BTYP
      INTEGER NLOS,NLOQ,NGAI,NSBD,BMJT,DMJT
      INTEGER HILP,HITR,HITR2,HIK,VNO,VNOL,VNO2,VNOB,VNOD
      INTEGER NTV1,NTP1,NTB1,NTD1,NTVB,NTPB,NTBB,NTDB,NTVD,NTPD,NTBD,
     *NTDD
      INTEGER TOPL,NOTH
      INTEGER IDB(30),IDD(30),CRGD,CRGB,FRST,CHAR,KIND,MCPH1,MCPH2
      INTEGER MPIS,ZVID,NGTRK,NPRG
      INTEGER NISO,CLEP,VNOS1,VNOS2
      INTEGER INDEX(5),N,I1,J1,K1,NTODO,VISO(30),NOK,NOFF
      INTEGER DSPA,DSPA1,DSPA2
      INTEGER ID3(30),M1PR,C3PR,N3TD
      INTEGER IDII,IDJ,IDK,ID1
      INTEGER BSTAT,NVT,IMAX,JMAX,NVRT,NPASS,TIDY,NSEC1
      INTEGER STATUS,BTRKMSK(2),DTRKMSK(2),VID(50),VISOL(30)
      INTEGER NV,DONE(10),NVERT,VORD(10),NEXV,CRGZ,NVREQ,NK,NGAP
      INTEGER FITMODE
      REAL NEAR,DLON1,GSAF(3),IMP3,ICUTP,B3NO,ANTG(30)
      REAL IPERX,IPERY,IPERZ,XPOS(10,3),XPOS1(30,3),DLONG(30)
      REAL XPOSE(10,6),XPOS1E(30,6)
      REAL PBGV,PBMV,PMAT(30,30),PMAX,PIIP,PIP,PBIP,PCUT
      REAL*8 XP,YP,ZP,XPR,YPR,PRFI,V(30,9),G(9)
      REAL*8 ST(30),CT(30),X0P(30),Z0(30),TL(30),S1S(30),S2S(30),C(30,9)
      REAL*8 PTLO(30),P1,P2
      REAL BVTX(3),DVTX(3),PROBV,BRES
      REAL BPOS(3),E(40),BOOST(4)
      REAL T1PR,L1PR,B1PR,P3PR,M3PR,M3PR2,VLCH,VTCH,PLONG,PTRAN
      REAL PXI,PYI,PZI,PXJJ,PYJJ,PZJJ,PXK,PYK,PZK,PTOTI,PTOTJJ,PTOTK
      REAL JANG,VANG,JANGD,VANGD,TRDISUM,TSUM,TMIN,PHIV,THETAV,ISTEP
      REAL PX,PY,PZ,DMO,MDPH,MDIF,MDIF1,MDIF2
      REAL CANG,CANG2,PANG,BANG,DANG,VAXS,VAYS,VAZS,DISI,MSPTB,MSPTD
      REAL EAXI(6),EAXB(6),EAXD(6),P3NORM,PTMIN,PTMAX
      REAL X1,Y1,Z1,X2,Y2,Z2
      REAL*8 CM(5,5),SING,PIVOT,ELM,RI(5),DET
      REAL*8 C1,C2,C3,C4,C5,C6,C7,C8,C9
      REAL ELEP,KMAS,LPMK
      REAL TRIS,LOIS,MOMF,Y
      REAL DISB,DISD,DISBD,MASB,MASD
      REAL XB(3),XD(3)
      REAL PSEC,DSEC,MSEC,MSPT,DISV,DISV1,COSL,SINL
      REAL PXT,PYT,PZT,PTOT,ETOT,PXJ,PYJ,PZJ,PTOTJ,PTOT2
      REAL PXB,PYB,PZB,PXD,PYD,PZD,PTOTB,PTOTD,ETOTB,ETOTD,PTOTA(2)
      REAL PX1,PY1,PZ1,ETOT1,MPH1,PX2,PY2,PZ2,ETOT2,MPH2
      REAL ETOTP1,ETOTP2,ETOTO1,ETOTO2
      REAL X0(3,2),T0(3,2),CHRG(2),XVTB(3),DCA,DINT,TINT
      REAL IPX,IPY,IPZ,IPXV,IPYV,IPZV,PROB,IPMX,IPMY,IPMZ
      REAL VPOSE(3),VPOSSG(3),CHISQTK(50),PXYZ(3,50),CHISQ,XVTX(3),
     *XVTXSG(6)
      REAL ERRT,CHMO
      REAL JETANG,BPRP,BOST,BDTR
      REAL VX,VY,VZ,VBMAG,VXD,VYD,VZD,VDMAG,ITX,ITY,ITZ
      REAL SINT,WEGT
      REAL PLSE,PT2D,PT3D,MTSE,MMSE,PMIP,PMIN
      REAL LPB3,TRB3
      REAL LLDI,LPX,LPY,LPZ,OLLO,OTX,OTY,OTZ
      REAL XX,YY,ZZ,XXL,YYL,ZZL,LNX,LNY,LNZ,LAM,MU,LAMM,MUU,SIG,RHO
      REAL XI,YI,ZI,LPNX(2,3),LPPT,LPMO
      REAL LO2P,TR2P,LPDI
      REAL VAX,VAY,VAZ,VMAG
      REAL AXI(3),AXF(3),TKI(3),TKV(3),TRDI,LODI,ANTA
      REAL CHDI,ECHD,PTDI,NMUL,NMULW,NDIS,NTRM,NERR
      REAL PMUL,PMULW,PDIS,PTRM,PERR
      REAL PLON,BREC,DREC
      REAL JETQ,PT,P(3),PTO,PDOTT,POL,FPI,FQI,PBI,RNDM
      REAL DMD,FS,FD,FU,FSM,FSU,FST,ETA,PMIX,PBF,VCHD
      REAL GWID,GXYZ(30,3),EGXYZ(30,6)
      REAL RIPE,ZIPE
      REAL RADCL
C----------------------------------------------------------------------
      DATA PPHBMFAM/0/
      IF ((FIRST)) THEN
        SGLISS=0
CJSF         SGLISS=JZPIDX('ZXTRKS',ZXTRKS)
CJSF         IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO   20
C    $CALL JZPIDX('ZBTOPL',ZBTOPL) ERROR RETURN;
CJSF         SGLISS=JZPIDX('MCPART',MCPART)
CJSF         IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO 20
CJSF         SGLISS=JZPIDX('MCHEAD',MCHEAD)
CJSF         IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO 20
CJSF         SGLISS=JZBLOC('PHCHRG',PHCHRG)
CJSF         IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO 20
CJSF         SGLISS=JZBLOC('MCPART',PRENT)
         IF((SIGSEV(IAND(SGLISS,7)).GT.SIGNAL$S_WARNING))GOTO 20
C   * $CALL JZBFND( 'ZBOUTL',PZBOUTL) ERROR RETURN;
C   *
         FIRST = .FALSE.
      END IF
C  $call jzbfnd('ZBTOPL', pZbTopL, dummy, jetNo);
C.....Jazelle processed ZVKON3(1)(PCUT)...(Bank ZVKON3)
CJSF       PCUT = JZL$R(JZL$X(ZVKON3(1))+32)
      PCUT=ZVKON3_PCUT
C.....Jazelle processed ZVKON3(1)(RIPE)...(Bank ZVKON3)
CJSF       RIPE = JZL$R(JZL$X(ZVKON3(1))+15)
      RIPE=ZVKON3_RIPE
C.....Jazelle processed ZVKON3(1)(ZIPE)...(Bank ZVKON3)
CJSF      ZIPE = JZL$R(JZL$X(ZVKON3(1))+16)
      ZIPE=ZVKON3_ZIPE
C.....Jazelle processed ZVKON3(1)(NVREQ)...(Bank ZVKON3)
CJSF       NVREQ = JZL$I(JZL$X(ZVKON3(1))+33)
      NVREQ=ZVKON3_NVREQ 
C.....Jazelle processed ZVKON3(1)(MOMF)...(Bank ZVKON3)
CJSF       MOMF = JZL$R(JZL$X(ZVKON3(1))+34)
      MOMF=ZVKON3_MOMF
C begin June 99, second pass if TIDY = 2
C.....Jazelle processed ZVKON3(1)(TIDY)...(Bank ZVKON3)
CJSF       TIDY = JZL$I(JZL$X(ZVKON3(1)))
      TIDY=ZVKON3_TIDY
C.....Jazelle processed ZVKON3(1)(ICUTP)...(Bank ZVKON3)
CJSF       ICUTP = JZL$R(JZL$X(ZVKON3(1))+13)
      ICUTP=ZVKON3_ICUTP
      AGAIN = .FALSE.
      NPASS = 0
30    CONTINUE
      IF (NPASS.EQ.1) THEN
         DO 41 I=1,3
            GAXI(I) = GSAF(I)
41       CONTINUE
42       CONTINUE
      ELSE
         DO 51 I=1,3
            GSAF(I) = GAXI(I)
51       CONTINUE
52       CONTINUE
      END IF
C end June 99, second pass if TIDY = 2
      TID3 = .FALSE.
C for TIDY=3, 25 June 99
      IPX = VPOS(1)
      IPY = VPOS(2)
      IPZ = VPOS(3)
Cinitialize extimate of IP location with IP Vertex of tracks
      IPXV = IPX
      IPYV = IPY
      IPZV = IPZ
      XPOS1E(1,1) = (RIPE/10000.0)**2
      XPOS1E(1,2) = 0.0
      XPOS1E(1,3) = (RIPE/10000.0)**2
      XPOS1E(1,4) = 0.0
      XPOS1E(1,5) = 0.0
      XPOS1E(1,6) = (ZIPE/10000.0)**2
C* do ii = 1,20,1 [                                                  *
C*   pZBoutl%(mcpa(ii)) = -2;                                        *
C* ]                                                                 *
C* do ii = 1,ntrk,1 [                                                *
C*   mcid = zxtrks%(trk(id(ii)),mcid);                               *
C*   mcpa(ii) = -1;                                                  *
C*   if mcid.ne.0 [                                                  *
C*     $call BBTMTYP(mcid,ipmc,ipflav,ipvtx);                        *
C*     mcpa(ii) = ipflav;                                            *
C*   ]                                                               *
C*   if ii.le.20 [                                                   *
C*     pZBoutl%(mcpa(ii)) = mcpa(ii);                                *
C*   ]                                                               *
C* ]                                                                 *
      SGLISS=ZVGST(NTRK,ID,VPOS,GAXI,JETNO,GWID, ST,CT,X0P,Z0,TL,S1S,
     *S2S,C)
CASCHOU: Use fitmode=-1 to refill the ZXFITA inverse error matrices.
      FITMODE = -1
C ghost axis
      VAX = GAXI(1)
      VAY = GAXI(2)
      VAZ = GAXI(3)
      VMAG = 1.0
C IP errors
      IPERX = 1.0/(RIPE/10000.0)**2
      IPERY = 1.0/(RIPE/10000.0)**2
      IPERZ = 1.0/(ZIPE/10000.0)**2
C.....Jazelle processed ZVTOPL3(JETNO)(GAXI(1))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+9+(1)*1)  = GAXI(1)
      RBNK_ZVTOPL3(8,JETNO) = GAXI(1)
C.....Jazelle processed ZVTOPL3(JETNO)(GAXI(2))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+9+(2)*1)  = GAXI(2)
      RBNK_ZVTOPL3(9,JETNO) = GAXI(2)
C.....Jazelle processed ZVTOPL3(JETNO)(GAXI(3))...(Bank ZVTOPL3)
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+9+(3)*1)  = GAXI(3)
      RBNK_ZVTOPL3(10,JETNO) = GAXI(3)
C.....Jazelle processed ZVTOPL3(JETNO)(GWID)...(Bank ZVTOPL3)
C June 99, set GWID -ve to flag second pass with TIDY = 2
CJSF      JZL$R(JZL$X(ZVTOPL3(JETNO))+13)  = 10000.0*GWID
      RBNK_ZVTOPL3(11,JETNO) = 10000.0*GWID
      IF (AGAIN) THEN
C   .....Jazelle processed ZVTOPL3(JETNO)(GWID)...(Bank ZVTOPL3)
C   .....Jazelle processed ZVTOPL3(JETNO)(GWID)...(Bank ZVTOPL3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+13)  = -JZL$R(JZL$X(ZVTOPL3(JETNO))
CJSF     *   +13)
      RBNK_ZVTOPL3(11,JETNO) = - RBNK_ZVTOPL3(11,JETNO)
      END IF
C.....Jazelle processed ZXTRKS(1)(NCHRG)...(Bank ZXTRKS)
CJSF      NCHRG = JZL$I(JZL$X(ZXTRKS(1))+24)
      NCHRG = IBNK_ZXTRKS(9)
Cnchrg is ghost
      NGTRK = NTRK+1
C*                                                                   *
C for ghost
C*                                                                   *
C*   c1 = c(j,1); c2 = c(j,2); c3 = c(j,3);                          *
C*   c4 = c(j,4); c5 = c(j,5); c6 = c(j,6);                          *
C*   c7 = c(j,7); c8 = c(j,8); c9 = c(j,9);                          *
C*                                                                   *
C for IP
C*                                                                   *
C*   c1 = iperx; c2 = ipery; c3 = iperz;                             *
C*   c4 = 0.0;   c5 = 0.0;   c6 = 0.0;                               *
C*   c7 = 0.0;   c8 = 0.0;   c9 = -vpos(3)*iperz;                    *
C*                                                                   *
C c1=0.;c2=0.;c3=0.;c4=0.;c5=0.;c6=0.;c7=0.;c8=0.;c9=0.;
C*                                                                   *
C* chisq = 0.0;                                                      *
C* nvt = 0;                                                          *
C*  do i = 1,ntrk,1 [                                                *
C*    if mcpa(i).eq.0 [                                              *
C*      nvt = nvt + 1;                                               *
C*      vid(nvt) = id(i);                                            *
C*      c1 = c1 + c(i,1); c2 = c2 + c(i,2); c3 = c3 + c(i,3);        *
C*      c4 = c4 + c(i,4); c5 = c5 + c(i,5); c6 = c6 + c(i,6);        *
C*      c7 = c7 + c(i,7); c8 = c8 + c(i,8); c9 = c9 + c(i,9);        *
C*    ]                                                              *
C*  ]                                                                *
C*                                                                   *
C* pZBoutl%(pbmvz) = -1.0;                                           *
C* pZBoutl%(pbmv) = -1.0;                                            *
C*                                                                   *
C* if nvt.ge.1 [                                                     *
C*     cm(1,1) = c1;      cm(1,2) = c4;      cm(1,3) = c5;           *
C*     cm(2,1) = c4;      cm(2,2) = c2;      cm(2,3) = c6;           *
C*     cm(3,1) = c5;      cm(3,2) = c6;      cm(3,3) = c3;           *
C*                                                                   *
C *****  copied this code from bmxinv, and densified *****
C*     sing=1.0; n=3;  for j1=1 to n [ index(j1)=1; ];  ntodo=n;     *
C*:100: pivot=0.0;  do j1 = 1,n [                                    *
C*      if (index(j1).eq.0) goto :120: ; elm = abs(cm(j1,j1));       *
C*      if (elm.le.pivot) goto :120: ; pivot=elm; i1=j1; :120:; ]    *
C*     if (pivot.eq.0.0) goto :900: ; index(i1)=0;                   *
C* :200: pivot=-cm(i1,i1);                                           *
C*     do j1 = 1,n [ if (j1-i1) :220:,:210:,:230: ;                  *
C*       :210: cm(i1,j1)=1.0/pivot ; ri(j1)=0.0 ; goto :290: ;       *
C*       :220: elm=-cm(i1,j1) ;  goto :240: ;                        *
C*       :230: elm=-cm(j1,i1) ; :240: ri(j1)=elm/pivot ;             *
C*         if (elm.eq.0.0) goto :260: ;  do :250: k1=1,j1 ;          *
C*       :250: cm(j1,k1)=cm(j1,k1)+elm*ri(k1) ;                      *
C*       :260: if (j1.gt.i1) goto :280: ;                            *
C*       :270: cm(i1,j1)=ri(j1) ; goto :290: ;                       *
C*       :280: cm(j1,i1)=ri(j1) ; :290:; ];                          *
C*     ntodo = ntodo - 1 ; if (ntodo.ne.0) goto :100: ;              *
C* :400: do :450: i1=1,n ; do :450: j1=1,i1 ; cm(i1,j1)=-cm(i1,j1) ; *
C* :450: cm(j1,i1)=cm(i1,j1) ; goto :1000:; :900: sing=0.0; :1000:   *
C *****  end of matrix inversion code *****
C*                                                                   *
C*    if sing.gt.0.5 [                                               *
C*     xp = -(cm(1,1)*c7 + cm(2,1)*c8 + cm(3,1)*c9);                 *
C*     yp = -(cm(1,2)*c7 + cm(2,2)*c8 + cm(3,2)*c9);                 *
C*     zp = -(cm(1,3)*c7 + cm(2,3)*c8 + cm(3,3)*c9);                 *
C*    ]                                                              *
C*                                                                   *
C*  do i = 1,ngtrk,1 [                                               *
C   do i = 1,ntrk,1 [
C*                                                                   *
C*  if mcpa(i).eq.0 .or. i.eq.ngtrk [                                *
C*                                                                   *
C*   if i.eq.ngtrk [  if primary fit                               *
C*    prfi = iperx*(xp-vpos(1))**2 +                                 *
C*           ipery*(yp-vpos(2))**2 +                                 *
C*           iperz*(zp-vpos(3))**2;                                  *
C*   ] else [                                                        *
C*     xpr =  ct(i)*xp + st(i)*yp;                                   *
C*     ypr = -st(i)*xp + ct(i)*yp;                                   *
C*    prfi = ((xpr-x0p(i))**2)/s1s(i) +                              *
C*       ((zp-(z0(i)+ypr*tl(i)))**2)/s2s(i);                         *
C*   ]                                                               *
C*                                                                   *
C*    chisq = chisq + prfi;                                          *
C*                                                                   *
C  write(*,*) 'mycal ',xp,yp,zp,prfi;
C*                                                                   *
C*   ]                                                               *
C*  ]                                                                *
C*                                                                   *
C*  pbgv = prob(chisq,(2*nvt-1));                                    *
C* if nvt.gt.1 [  pbgv = prob(chisq,(2*nvt-2)); ]                    *
C for primary vertex with IP
C*  pbgv = prob(chisq,(2*nvt));                                      *
C*                                                                   *
C*                                                                   *
C and take ghost out
C*   c1 = c1 - c(j,1); c2 = c2 - c(j,2); c3 = c3 - c(j,3);           *
C*   c4 = c4 - c(j,4); c5 = c5 - c(j,5); c6 = c6 - c(j,6);           *
C*   c7 = c7 - c(j,7); c8 = c8 - c(j,8); c9 = c9 - c(j,9);           *
C*                                                                   *
C or IP
C*   c1 = c1 - iperx; c2 = c2 - ipery; c3 = c3 - iperz;              *
C*   c9 = c9 + vpos(3)*iperz;                                        *
C*                                                                   *
C* if nvt.ge.2 [                                                     *
C*     cm(1,1) = c1;      cm(1,2) = c4;      cm(1,3) = c5;           *
C*     cm(2,1) = c4;      cm(2,2) = c2;      cm(2,3) = c6;           *
C*     cm(3,1) = c5;      cm(3,2) = c6;      cm(3,3) = c3;           *
C*                                                                   *
C *****  copied this code from bmxinv, and densified *****
C*     sing=1.0; n=3;  for j1=1 to n [ index(j1)=1; ];  ntodo=n;     *
C*:103: pivot=0.0;  do j1 = 1,n [                                    *
C*      if (index(j1).eq.0) goto :123: ; elm = abs(cm(j1,j1));       *
C*      if (elm.le.pivot) goto :123: ; pivot=elm; i1=j1; :123:; ]    *
C*     if (pivot.eq.0.0) goto :903: ; index(i1)=0;                   *
C* :203: pivot=-cm(i1,i1);                                           *
C*     do j1 = 1,n [ if (j1-i1) :223:,:213:,:233: ;                  *
C*       :213: cm(i1,j1)=1.0/pivot ; ri(j1)=0.0 ; goto :293: ;       *
C*       :223: elm=-cm(i1,j1) ;  goto :243: ;                        *
C*       :233: elm=-cm(j1,i1) ; :243: ri(j1)=elm/pivot ;             *
C*         if (elm.eq.0.0) goto :263: ;  do :253: k1=1,j1 ;          *
C*       :253: cm(j1,k1)=cm(j1,k1)+elm*ri(k1) ;                      *
C*       :263: if (j1.gt.i1) goto :283: ;                            *
C*       :273: cm(i1,j1)=ri(j1) ; goto :293: ;                       *
C*       :283: cm(j1,i1)=ri(j1) ; :293:; ];                          *
C*     ntodo = ntodo - 1 ; if (ntodo.ne.0) goto :103: ;              *
C* :403: do :453: i1=1,n ; do :453: j1=1,i1 ; cm(i1,j1)=-cm(i1,j1) ; *
C* :453: cm(j1,i1)=cm(i1,j1) ; goto :1003:; :903: sing=0.0; :1003:   *
C *****  end of matrix inversion code *****
C*                                                                   *
C*    if sing.gt.0.5 [                                               *
C*     xp = -(cm(1,1)*c7 + cm(2,1)*c8 + cm(3,1)*c9);                 *
C*     yp = -(cm(1,2)*c7 + cm(2,2)*c8 + cm(3,2)*c9);                 *
C*     zp = -(cm(1,3)*c7 + cm(2,3)*c8 + cm(3,3)*c9);                 *
C*    ]                                                              *
C*                                                                   *
C* chisq = 0.0;                                                      *
C*                                                                   *
C*  do i = 1,ntrk,1 [                                                *
C*                                                                   *
C*  if mcpa(i).eq.0 [                                                *
C*                                                                   *
C*     xpr =  ct(i)*xp + st(i)*yp;                                   *
C*     ypr = -st(i)*xp + ct(i)*yp;                                   *
C*    prfi = ((xpr-x0p(i))**2)/s1s(i) +                              *
C*       ((zp-(z0(i)+ypr*tl(i)))**2)/s2s(i);                         *
C*                                                                   *
C*    chisq = chisq + prfi;                                          *
C*                                                                   *
C*   ]                                                               *
C*  ]                                                                *
C*                                                                   *
C* if nvt.gt.1 [  pbmv = prob(chisq,(2*nvt-3)); ]                    *
C*                                                                   *
C  pbgv = prob(chisq,(2*nvt-3));
C* ]                                                                 *
C*                                                                   *
C*  pZBoutl%(pbgv) = pbgv;                                           *
C*  pZBoutl%(pbmv) = pbmv;  fit prob of MC B tracks alone          *
C*                                                                   *
C*      vpose(1) = 0.0;                                              *
C*      vpose(2) = 0.0;                                              *
C*      vpose(3) = 0.0;                                              *
C*      vpossg(1) = 10.0;                                            *
C*      vpossg(2) = 10.0;                                            *
C*      vpossg(3) = 10.0;                                            *
C first fit B tracks only
C*     call zxfit(1,nvt,vid,vpose,vpossg,chisq,xvtx,xvtxsg,          *
C*                                        chisqtk,pxyz,ier);         *
C*    if nvt.gt.1 [pZBoutl%(pbmvz) = prob(chisq,(2*nvt-3));]         *
C*                                                                   *
C then add in ghost
C*      vid(nvt+1)=id(ngtrk);                                        *
C*      nvt = nvt+1;                                                 *
C*     call zxfit(1,nvt,vid,vpose,vpossg,chisq,xvtx,xvtxsg,          *
C*                                        chisqtk,pxyz,ier);         *
C*                                                                   *
C  write(*,*) 'zxfit ',xvtx(1),xvtx(2),xvtx(3),chisqtk(1);
C
C    write(*,*) 'zxfit ',xvtx(1),xvtx(2),xvtx(3),chisqtk(nvt);
C*                                                                   *
C*  pZBoutl%(pbgvz) = prob(chisq,(2*nvt-3));                         *
C* if nvt.gt.2 [pZBoutl%(pbgvz) = prob(chisq,(2*nvt-4)); ]           *
C* ]  if secondary MC tracks                                       *
C*                                                                   *
C*  pZBoutl%(nbgv) = nvt-1;                                          *
C*  if nvt.eq.0 [pZBoutl%(nbgv) = 0;]                                *
C longitudinal momentum of each track
      J = NGTRK
      DO 61 II=1,NTRK,1
         IDS = ID(II)
C   .....Jazelle processed ZXTRKS(1)(TRK(IDS),HLXPAR(3))...(Bank ZXTRKS)
CJSF         COSL = COS(ATAN(JZL$R(JZL$X(ZXTRKS(1))-10+(IDS)*62+(3)*1) ))
         COSL = COS(ATAN(RTRK_ZXTRKS(24+3,IDS)))
C   .....Jazelle processed ZXTRKS(1)(TRK(IDS),HLXPAR(3))...(Bank ZXTRKS)
CJSF         SINL = SIN(ATAN(JZL$R(JZL$X(ZXTRKS(1))-10+(IDS)*62+(3)*1) ))
         SINL = SIN(ATAN(RTRK_ZXTRKS(24+3,IDS)))
C   .....Jazelle processed ZXTRKS(1)(TRK(IDS),HLXPAR(2))...(Bank ZXTRKS)
CJSF         PTOT = 1/(JZL$R(JZL$X(ZXTRKS(1))-10+(IDS)*62+(2)*1) *COSL)
         PTOT = 1/(RTRK_ZXTRKS(24+2,IDS)*COSL)
C   .....Jazelle processed ZXTRKS(1)(TRK(IDS),HLXPAR(1))...(Bank ZXTRKS)
CJSF         PX = PTOT*COSL*COS(JZL$R(JZL$X(ZXTRKS(1))-10+(IDS)*62+(1)*1) )
         PX = PTOT*COSL*COS(RTRK_ZXTRKS(24+1,IDS))
C   .....Jazelle processed ZXTRKS(1)(TRK(IDS),HLXPAR(1))...(Bank ZXTRKS)
CJSF         PY = PTOT*COSL*SIN(JZL$R(JZL$X(ZXTRKS(1))-10+(IDS)*62+(1)*1) )
         PY = PTOT*COSL*SIN(RTRK_ZXTRKS(24+1,IDS))
         PZ = PTOT*SINL
         PTLO(II) = PX*VAX + PY*VAY + PZ*VAZ
C    angle of each track to gaxi, 25 June 99
         ANTG(II) = PTLO(II)/PTOT
         IF (ANTG(II).GE.1.0) THEN
            ANTG(II) = 1.0
         END IF
         IF (ANTG(II).LE.-1.0) THEN
            ANTG(II) = -1.0
         END IF
         ANTG(II) = ACOS(ANTG(II))
         IF (PTLO(II).LT.0.00001) THEN
            PTLO(II) = 0.00001
         END IF
C   *         pZBoutl%(ptlo(ii)) = ptlo(ii);
C   *
61    CONTINUE
62    CONTINUE
C now try to cluster tracks + IP into a vertex
C  write(*,*) 'Next Jet';
C
C
      DO 71 I=1,9
C    ghost track parameters
         G(I) = C(NGTRK,I)
         DO 81 J=1,NTRK,1
            V(J,I) = C(J,I)
C       vertex parameters of the tracks
81       CONTINUE
82       CONTINUE
71    CONTINUE
72    CONTINUE
C and for IP
      V(NTRK+1,1) = IPERX
      V(NTRK+1,2) = IPERY
      V(NTRK+1,3) = IPERZ
      V(NTRK+1,4) = 0.0
      V(NTRK+1,5) = 0.0
      V(NTRK+1,6) = 0.0
      V(NTRK+1,7) = -VPOS(1)*IPERX
      V(NTRK+1,8) = -VPOS(2)*IPERY
      V(NTRK+1,9) = -VPOS(3)*IPERZ
      V(NTRK+1,7) = 0.0
C ZVTOP coords IP at (x,y) = (0,0)
      V(NTRK+1,8) = 0.0
      DO 91 I=1,9
         C(NTRK+1,I) = V(NTRK+1,I)
C    IP parameters stored here
91    CONTINUE
92    CONTINUE
C reset track-IP combination probability matrix
      DO 101 I=1,30
         DO 111 J=1,30
            PMAT(I,J) = 0.0
111      CONTINUE
112      CONTINUE
101   CONTINUE
102   CONTINUE
      DO 121 I=1,30
         VISOL(I) = 0
121   CONTINUE
122   CONTINUE
Cuse this to fill prip for each track
      FLAG = .FALSE.
130   CONTINUE
      DO 141 I=1,NTRK,1
         DO 151 J=I+1,NTRK+1,1
C       not swallowed in IP yet
            IF (VISOL(I).NE.1 .AND. VISOL(J).NE.1) THEN
C          not in same vertex yet
               IF (PMAT(J,I).LT.2.0) THEN
                  IF (J.LE.NTRK) THEN
                     C1 = G(1)
                     C2 = G(2)
                     C3 = G(3)
                     C4 = G(4)
                     C5 = G(5)
                     C6 = G(6)
                     C7 = G(7)
                     C8 = G(8)
                     C9 = G(9)
                  ELSE
C                do not use ghost if we have IP track
                     C1 = 0.0
                     C2 = 0.0
                     C3 = 0.0
                     C4 = 0.0
                     C5 = 0.0
                     C6 = 0.0
                     C7 = 0.0
                     C8 = 0.0
                     C9 = 0.0
                  END IF
                  C1 = C1 + V(I,1) + V(J,1)
                  C2 = C2 + V(I,2) + V(J,2)
                  C3 = C3 + V(I,3) + V(J,3)
                  C4 = C4 + V(I,4) + V(J,4)
                  C5 = C5 + V(I,5) + V(J,5)
                  C6 = C6 + V(I,6) + V(J,6)
                  C7 = C7 + V(I,7) + V(J,7)
                  C8 = C8 + V(I,8) + V(J,8)
                  C9 = C9 + V(I,9) + V(J,9)
                  CM(1,1) = C1
                  CM(1,2) = C4
                  CM(1,3) = C5
                  CM(2,1) = C4
                  CM(2,2) = C2
                  CM(2,3) = C6
                  CM(3,1) = C5
                  CM(3,2) = C6
                  CM(3,3) = C3
C             *****  copied this code from bmxinv, and densified *****
                  SING=1.0
                  N=3
                  J1=1
                     GO TO 163
161                  J1=J1+1
163                  IF(J1-(N).GT.0)GO TO 162
                     INDEX(J1)=1
                  GO TO 161
162               CONTINUE
                  NTODO=N
170               PIVOT=0.0
                  DO 181 J1=1,N
                     IF((INDEX(J1).EQ.0))GOTO   190
                     ELM = ABS(CM(J1,J1))
                     IF((ELM.LE.PIVOT))GOTO 190
                     PIVOT=ELM
                     I1=J1
190                  CONTINUE
181               CONTINUE
182               CONTINUE
                  IF((PIVOT.EQ.0.0))GOTO   200
                  INDEX(I1)=0
210               PIVOT=-CM(I1,I1)
                  DO 221 J1=1,N
                     IF((J1-I1))  230,  240,  250
240                  CM(I1,J1)=1.0/PIVOT
                     RI(J1)=0.0
                     GOTO   260
230                  ELM=-CM(I1,J1)
                     GOTO   270
250                  ELM=-CM(J1,I1)
270                  RI(J1)=ELM/PIVOT
                     IF((ELM.EQ.0.0))GOTO   280
                     DO   290 K1=1,J1
290                  CM(J1,K1)=CM(J1,K1)+ELM*RI(K1)
280                  IF (J1.GT.I1) GOTO   300
310                  CM(I1,J1)=RI(J1)
                     GOTO 260
300                  CM(J1,I1)=RI(J1)
260                  CONTINUE
221               CONTINUE
222               CONTINUE
                  NTODO = NTODO - 1
                  IF((NTODO.NE.0))GOTO 170
320               DO   330 I1=1,N
                  DO 330 J1=1,I1
                  CM(I1,J1)=-CM(I1,J1)
330               CM(J1,I1)=CM(I1,J1)
                  GOTO   340
C             *****  end of matrix inversion code *****
200               SING=0.0
340               IF (SING.GT.0.5) THEN
                     XP = -(CM(1,1)*C7 + CM(2,1)*C8 + CM(3,1)*C9)
                     YP = -(CM(1,2)*C7 + CM(2,2)*C8 + CM(3,2)*C9)
                     ZP = -(CM(1,3)*C7 + CM(2,3)*C8 + CM(3,3)*C9)
                  END IF
                  NPRG = 0
C             chisq contribution from each track + ghost:
                  CHISQ = 0.0
                  DO 351 K=1,NGTRK,1
                     IF (K.EQ.I .OR. K.EQ.J .OR. K.EQ.NGTRK .OR. PMAT(K,
     *               I).GT.2.0 .OR. PMAT(I,K).GT.2.0 .OR. PMAT(K,J).GT.
     *               2.0 .OR. PMAT(J,K).GT.2.0) THEN
                        IF (J.EQ.NTRK+1 .AND. K.EQ.NGTRK) THEN
C                     add IP, not ghost
                           PRFI = IPERX*(XP)**2 + IPERY*(YP)**2 + IPERZ*
     *                     (ZP-VPOS(3))**2
                        ELSE
                           XPR = CT(K)*XP + ST(K)*YP
                           YPR = -ST(K)*XP + CT(K)*YP
                           PRFI = ((XPR-X0P(K))**2)/S1S(K) + ((ZP-(Z0(K)
     *                     +YPR*TL(K)))**2)/S2S(K)
                           IF (J.EQ.NTRK+1) THEN
C                         add mometum factor to track
C                         try momentum factor
                              PRFI = PRFI + MOMF*LOG(PTLO(K))
                              IF (PRFI.LT.0.0) THEN
                                 PRFI = 0.0
                              END IF
                           END IF
                        END IF
                        NPRG = NPRG + 1
                        CHISQ = CHISQ + PRFI
                     END IF
351               CONTINUE
352               CONTINUE
C             loop over tracks
                  IF (J.LE.NTRK) THEN
                     PMAT(I,J) = PROB(CHISQ,(2*NPRG-4))
C               * ignore track-ghost vertices behind IP  -  little diffe
C               rence  *
C               *          if (xp)*gaxi(1)+(yp)*gaxi(2)+(zp-ipz)*gaxi(3)
C                      *
C               *            .lt. -0.05 [ PMAT(i,j) = 0.0; ]
C                      *
                  END IF
                  IF (J.EQ.NTRK+1) THEN
                     PMAT(I,J) = PROB(CHISQ,2*NPRG-2)
C               *        if .not. flag [   first loop
C                         *
C               *            pZBoutl%(prip(i)) = chisq;
C                           *
C               *        ]
C                           *
                  END IF
               END IF
C          if not combined yet
C          leave IP for NTRK+1
            ELSE IF(J.LE.NTRK) THEN
               IF (VISOL(I).EQ.1 .OR. VISOL(J).EQ.1) THEN
C            if condition redundant?
                  PMAT(I,J) = 0.0
               END IF
            END IF
151      CONTINUE
152      CONTINUE
C    loop over...
141   CONTINUE
142   CONTINUE
C ...track pairs
C find max probability
      PMAX=-1.0
      IMAX = 1
      JMAX = 1
      DO 361 I=1,NTRK,1
         DO 371 J=I+1,NTRK+1,1
            IF ((PMAT(I,J).GT.PCUT .OR. NVREQ.GT.0) .AND. PMAT(I,J).GT.
     *      PMAX .AND. PMAT(J,I).LT.2.0) THEN
C          not in same vertex yet
               PMAX = PMAT(I,J)
               IMAX = I
               JMAX = J
            END IF
371      CONTINUE
372      CONTINUE
361   CONTINUE
362   CONTINUE
C* if .not. flag [   first loop                                      *
C*  pZBoutl%(pmax) = pmax;                                           *
C*  pZBoutl%(par1) = mcpa(imax);                                     *
C*  pZBoutl%(par2) = mcpa(jmax);                                     *
C*  piip = -1.0;                                                     *
C*  pbip = -1.0;                                                     *
C*  do i = 1,ntrk [                                                  *
C*    if mcpa(i).eq.0 [ piip = PMAT(i,ntrk+1); ]                     *
C*    if mcpa(i).eq.1 [ pbip = PMAT(i,ntrk+1); ]                     *
C*  ]                                                                *
C*  pZBoutl%(piip) = piip;                                           *
C*  pZBoutl%(pbip) = pbip;                                           *
C* ]                                                                 *
C if pmax.lt.pcut .and. nvreq.le.0  [ goto :done:;]
      IF (PMAX.LT.PCUT .AND. NVREQ.LE.0) THEN
         GOTO   380
      END IF
C make sure
C visol filled !
      IF (NTRK.LT.NVREQ .AND. NVREQ.GT.1) THEN
         GOTO 380
      END IF
C else combine the relevant tracks
      PMAT(JMAX,IMAX) = 5.0
      IF (JMAX.LE.NTRK) THEN
         DO 391 II=1,9
            V(JMAX,II) = V(JMAX,II) + V(IMAX,II)
            V(IMAX,II) = V(JMAX,II)
391      CONTINUE
392      CONTINUE
C   if jmax not IP
C   if track imax is absorbed into IP
      ELSE
         DO 401 II=1,9
C       next line, try combining tracks in IP, not absorb
            V(JMAX,II) = V(JMAX,II) + V(IMAX,II)
            V(IMAX,II) = V(JMAX,II)
            VISOL(IMAX) = 1
C       track is in IP
401      CONTINUE
402      CONTINUE
C    IP + tracks vertex location
         IPXV = XP + IPX
         IPYV = YP + IPY
         IPZV = ZP
C    think this might be error matrix ?
         XPOS1E(1,1) = CM(1,1)
         XPOS1E(1,2) = CM(1,2)
         XPOS1E(1,3) = CM(2,2)
         XPOS1E(1,4) = CM(1,3)
         XPOS1E(1,5) = CM(2,3)
         XPOS1E(1,6) = CM(3,3)
      END IF
C fill in cross terms, if k in imax vertex, then k in jmax vert...
      DO 411 K=1,NTRK+1,1
         IF (K.NE.IMAX .AND. K.NE.JMAX) THEN
            IF (PMAT(K,IMAX).GT.2.0 .OR. PMAT(IMAX,K).GT.2.0) THEN
               IF (K.GT.JMAX) THEN
                  PMAT(K,JMAX) = 5.0
               ELSE
                  PMAT(JMAX,K) = 5.0
               END IF
               DO 421 II=1,9
                  V(K,II) = V(JMAX,II)
421            CONTINUE
422            CONTINUE
            END IF
            IF (PMAT(K,JMAX).GT.2.0 .OR. PMAT(JMAX,K).GT.2.0) THEN
               IF (K.GT.IMAX) THEN
                  PMAT(K,IMAX) = 5.0
               ELSE
                  PMAT(IMAX,K) = 5.0
               END IF
               DO 431 II=1,9
                  V(K,II) = V(JMAX,II)
431            CONTINUE
432            CONTINUE
            END IF
         END IF
411   CONTINUE
412   CONTINUE
C more cross terms...
C if i,j  in imax,jmax, then i,j in same vertex
      DO 441 I=1,NTRK,1
         DO 451 J=I+1,NTRK+1,1
            IF (I.NE.IMAX .AND. I.NE.JMAX .AND. J.NE.IMAX .AND. J.NE.
     *      JMAX .AND. PMAT(J,I).LT.2.0) THEN
               IF (((PMAT(I,IMAX).GT.2.0 .OR. PMAT(IMAX,I).GT.2.0) .AND.
     *          (PMAT(J,JMAX).GT.2.0 .OR. PMAT(JMAX,J).GT.2.0)) .OR. ((
     *         PMAT(I,JMAX).GT.2.0 .OR. PMAT(JMAX,I).GT.2.0) .AND. (
     *         PMAT(J,IMAX).GT.2.0 .OR. PMAT(IMAX,J).GT.2.0))) THEN
                  PMAT(J,I) = 5.0
                  DO 461 II=1,9
                     V(I,II) = V(JMAX,II)
                     V(J,II) = V(JMAX,II)
461               CONTINUE
462               CONTINUE
               END IF
            END IF
451      CONTINUE
452      CONTINUE
441   CONTINUE
442   CONTINUE
C is track in IP ?
      DO 471 K=1,NTRK,1
         IF (PMAT(NTRK+1,K).GT.2.0) THEN
            VISOL(K) = 1
         END IF
471   CONTINUE
472   CONTINUE
C   write(*,*) ' ';
C
C      write (*,'(a,i6,20i6)') ' ',ntrk,(mcpa(i),i=1,ntrk);
C
C  do i = 1,ntrk [
C
C      write (*,'(a,i6,20f6.3)') ' ',mcpa(i),(PMAT(i,j),j=1,ntrk+1);
C
C  ]
C
C     i = ntrk+1;
C
C      write (*,'(a,i6,20f6.3)') ' ',-1,(PMAT(i,j),j=1,ntrk+1);
C
C
C jump here if fewer tracks than NVREQ
C fill viso, current associations, for track-vertices
380   CONTINUE
         DO 381 I=1,NTRK,1
         VISO(I) = VISOL(I)
381   CONTINUE
382   CONTINUE
C only set for tracks in IP
      NVRT = 1
480   CONTINUE
         DO 481 I=1,NTRK,1
         IF (VISO(I).EQ.0) THEN
            NVRT = NVRT + 1
            VISO(I) = NVRT
            GOTO   490
         END IF
481   CONTINUE
482   CONTINUE
      GOTO   500
490   CONTINUE
         DO 491 J=1,NTRK,1
         IF (PMAT(I,J).GT.2.0 .OR. PMAT(J,I).GT.2.0) THEN
            VISO(J) = NVRT
         END IF
491   CONTINUE
492   CONTINUE
C  write (*,'(a,i6,20i6)') 'v',nvrt,(visol(i),i=1,ntrk);
C
C
      GOTO 480
500   CONTINUE
510   II = 0
      DO 521 I=2,NVRT,1
         NVT = 0
         VPOSE(1) = 0.0
         VPOSE(2) = 0.0
         VPOSE(3) = 0.4
         VPOSSG(1) = 10.0
         VPOSSG(2) = 10.0
         VPOSSG(3) = 10.0
         DO 531 J=1,NTRK,1
            IF (VISO(J).EQ.I) THEN
               NVT = NVT + 1
               VID(NVT) = ID(J)
            END IF
531      CONTINUE
532      CONTINUE
         IF (NVT.EQ.1) THEN
C       add in ghost if only one prong
            NVT = NVT+1
            VID(NVT)=ID(NGTRK)
         END IF
C    add ghost to all vertices ? no
C    use zxfit here to get error matrices
C    TEST zxfit by iterating.
         VPOSE(1) = 0.0
         VPOSE(2) = 0.0
         VPOSE(3) = 0.4
         CALL ZXFIT(FITMODE,NVT,VID,VPOSE,VPOSSG,CHISQ,XVTX,XVTXSG,
     *   CHISQTK,PXYZ,IER)
         IF (FITMODE .EQ. -1) THEN
            FITMODE = 1
         END IF
C        vpose(1) = xvtx(1); vpose(2) = xvtx(2); vpose(3) = xvtx(3);
C
C   |     call zxfit(fitmode,nvt,vid,vpose,vpossg,chisq,xvtx,xvtxsg,
C
C   |                                               chisqtk,pxyz,ier);
C
C   |
C
C   |     vpose(1) = xvtx(1); vpose(2) = xvtx(2); vpose(3) = xvtx(3);
C
C   |     call zxfit(fitmode,nvt,vid,vpose,vpossg,chisq,xvtx,xvtxsg,
C
C   |                                               chisqtk,pxyz,ier);
C
         XPOS1(I,1) = XVTX(1)
         XPOS1(I,2) = XVTX(2)
         XPOS1(I,3) = XVTX(3)
         XPOS1E(I,1) = XVTXSG(1)
         XPOS1E(I,2) = XVTXSG(2)
         XPOS1E(I,3) = XVTXSG(3)
         XPOS1E(I,4) = XVTXSG(4)
         XPOS1E(I,5) = XVTXSG(5)
         XPOS1E(I,6) = XVTXSG(6)
         DLONG(I) = (XVTX(1)-IPX)*GAXI(1) + (XVTX(2)-IPY)*GAXI(2) + (
     *   XVTX(3)-IPZ)*GAXI(3)
         IF (DLONG(I).LT.0.0) THEN
C      dont count -ve dlong for nvreq
            II = II + 1
         END IF
521   CONTINUE
522   CONTINUE
      IF (TID3) THEN
         GOTO   540
      END IF
      IF (PMAX.LT.PCUT .AND. NVREQ.LE.0) THEN
         GOTO   550
      END IF
Chave we reached requested vertex number ?
      IF (NVREQ.GT.0) THEN
         IF ((NVRT-II).LE.NVREQ) THEN
            GOTO 550
         END IF
      END IF
      FLAG = .TRUE.
      GOTO 130
550   CONTINUE
      IF (TID3) THEN
         GOTO 540
      END IF
C check for secondary tracks failing B3NOvANTA, TIDY=3
      IF (TIDY.GE.3) THEN
         TID3 = .TRUE.
C    only try this at end once
         DOWN = .FALSE.
         DO 561 I=1,NTRK,1
C      .....Jazelle processed ZXTRKS(1)(TRK(ID(I)),B3NORM)...(Bank ZXTRK
C      S)
CJSF            B3NO = JZL$R(JZL$X(ZXTRKS(1))+22+(ID(I))*62)
            B3NO = RTRK_ZXTRKS(50,ID(I))
            IF (VISO(I).GT.1 .AND. ( (ANTG(I).LT.0.5 .AND. B3NO.LT.6.0*
     *      ANTG(I)) .OR. (ANTG(I).GE.0.5 .AND. B3NO.LT.3.0) )) THEN
               VISO(I) = 1
C          reassign to primary
C         *     pZBoutl%(qqid) = -abs(pZBoutl%(qqid));
C              *
               DOWN = .TRUE.
            END IF
561      CONTINUE
562      CONTINUE
         IF (DOWN) THEN
C       at least one secondary track removed
            NGAP = 0
            DO 571 NV=2,NVRT,1
               IF (NV.LE.(NVRT-NGAP)) THEN
                  GAP = .TRUE.
                  DO 581 I=1,NTRK,1
                     IF (VISO(I).EQ.NV) THEN
                        GAP = .FALSE.
                     END IF
581               CONTINUE
582               CONTINUE
                  IF (GAP) THEN
                     NGAP = NGAP + 1
                     DO 591 I=1,NTRK,1
                        IF (VISO(I).GT.NV) THEN
                           VISO(I) = VISO(I)-1
                        END IF
591                  CONTINUE
592                  CONTINUE
                  END IF
               END IF
C          if not over end of chain
571         CONTINUE
572         CONTINUE
C       loop over secondaries...
            NVRT = NVRT-NGAP
            GOTO 510
         END IF
C    if a gap has appeared
      END IF
C endif TIDY=3, 25 June 99
540   CONTINUE
         DO 541 I=1,NTRK,1
         VISOL(I) = VISO(I)
541   CONTINUE
542   CONTINUE
C  write(*,*) 'final probabilities';
C
C      write(*,*) ' ';
C
C      write (*,'(a,i6,20i6)') ' ',ntrk,(mcpa(i),i=1,ntrk);
C
C  do i = 1,ntrk [
C
C      write (*,'(a,i6,20f6.3)') ' ',mcpa(i),(PMAT(i,j),j=1,ntrk+1);
C
C  ]
C
C     i = ntrk+1;
C
C      write (*,'(a,i6,20f6.3)') ' ',-1,(PMAT(i,j),j=1,ntrk+1);
C
C
C
C      write (*,'(a,i6,20i6)') 'v',ntrk,(visol(i),i=1,ntrk);
C
C
C fill visol for track-vertices
C first protect against too many vertices for ZVTOPL3, 29 May 99
      IF (NVRT.GT.7) THEN
         NVRT = 7
      END IF
      DO 601 I=1,NTRK,1
         IF (VISOL(I).GT.7) THEN
            VISOL(I) = -1
         END IF
601   CONTINUE
602   CONTINUE
C now get right vertex order
      DO 611 NV=1,NVRT,1
         DONE(NV) = 0
         VORD(NV) = 0
611   CONTINUE
612   CONTINUE
      NVERT = NVRT
      DO 621 I=2,NVRT,1
         IF (DLONG(I).LT.0.0) THEN
            NVERT = NVERT - 1
            DONE(I) = 1
            DO 631 J=1,NTRK,1
               IF (VISOL(J).EQ.I) THEN
                  VISOL(J) = -1
               END IF
631         CONTINUE
632         CONTINUE
         END IF
621   CONTINUE
622   CONTINUE
      VORD(1) = 1
      DO 641 II=2,NVERT,1
         NEAR = 100.0
         DO 651 NV=2,NVRT,1
            IF (DLONG(NV).LT.NEAR .AND. DONE(NV).EQ.0) THEN
               NEAR = DLONG(NV)
               NEXV = NV
            END IF
651      CONTINUE
652      CONTINUE
         DONE(NEXV) = 1
         VORD(NEXV) = II
641   CONTINUE
642   CONTINUE
      DO 661 I=1,NTRK,1
         IF (VISOL(I).GT.1) THEN
            VISOL(I) = VORD(VISOL(I))
         END IF
661   CONTINUE
662   CONTINUE
      DO 671 I=2,NVERT,1
         DO 681 NV=2,NVRT,1
            IF (VORD(NV).EQ.I) THEN
               XPOS(I,1) = XPOS1(NV,1)
               XPOS(I,2) = XPOS1(NV,2)
               XPOS(I,3) = XPOS1(NV,3)
               XPOSE(I,1) = XPOS1E(NV,1)
               XPOSE(I,2) = XPOS1E(NV,2)
               XPOSE(I,3) = XPOS1E(NV,3)
               XPOSE(I,4) = XPOS1E(NV,4)
               XPOSE(I,5) = XPOS1E(NV,5)
               XPOSE(I,6) = XPOS1E(NV,6)
            END IF
681      CONTINUE
682      CONTINUE
671   CONTINUE
672   CONTINUE
      NVRT = NVERT
      XPOS(1,1) = IPXV
      XPOS(1,2) = IPYV
      XPOS(1,3) = IPZV
      XPOSE(1,1) = XPOS1E(1,1)
      XPOSE(1,2) = XPOS1E(1,2)
      XPOSE(1,3) = XPOS1E(1,3)
      XPOSE(1,4) = XPOS1E(1,4)
      XPOSE(1,5) = XPOS1E(1,5)
      XPOSE(1,6) = XPOS1E(1,6)
C  write (*,'(a,i6,20i6)') ' ',nvrt,(visol(i),i=1,ntrk);
C
C
C put ZXTRKS back the way we found it by contracting !
C  nchrg = nchrg-1;
C
C   $CALL JZBEXP('ZXTRKS',1,NCHRG,ZXTRKS) ERROR RETURN;
C.....Jazelle processed ZXTRKS(1)(NCHRG)...(Bank ZXTRKS)
C begin new section 7 June
C 2nd iteration with higher impact tracks if long decay length
CJSF      JZL$I(JZL$X(ZXTRKS(1))+24)  = NCHRG-1
      IBNK_ZXTRKS(9) = NCHRG-1
      IF (TIDY.GE.2 .AND. NPASS.EQ.0) THEN
         NPASS = 1
         NVT = 0
         VPOSE(1) = 0.0
         VPOSE(2) = 0.0
         VPOSE(3) = 0.4
         VPOSSG(1) = 10.0
         VPOSSG(2) = 10.0
         VPOSSG(3) = 10.0
         DO 691 J=1,NTRK,1
            IF (VISOL(J).GT.1) THEN
C          combine all secondary tracks
               NVT = NVT + 1
               VID(NVT) = ID(J)
            END IF
691      CONTINUE
692      CONTINUE
         NSEC1 = NVT
C   *    pZBoutl%(nsec1) = nsec1;                                   *
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(7),NTRV)...(Bank ZVTOPL3)
CJSF         JZL$I(JZL$X(ZVTOPL3(JETNO))+21+(7)*16)  = NSEC1
         IVRT_ZVTOPL3(1,7,JETNO) = NSEC1
C   *    pZBoutl%(dlon1) = -10.0;                                   *
         DLON1 = -10.0
         IF (NVT.EQ.1) THEN
C       add in ghost if only one prong
            NVT = NVT+1
            VID(NVT)=ID(NGTRK)
         END IF
C    add ghost to all vertices ? no
         IF (NVT.GT.1) THEN
C       use zxfit here to get error matrices
            CALL ZXFIT(1,NVT,VID,VPOSE,VPOSSG,CHISQ,XVTX,XVTXSG,
     *      CHISQTK,PXYZ,IER)
            DLON1 = (XVTX(1)-IPX)*GAXI(1) + (XVTX(2)-IPY)*GAXI(2) + (
     *      XVTX(3)-IPZ)*GAXI(3)
C      *     pZBoutl%(dlon1) = dlon1;                                  *
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(7),POS(1))...(Bank ZVT
C      OPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+22+(7)*16+(1)*1)  = XVTX(1)
            RVRT_ZVTOPL3(2,7,JETNO) = XVTX(1)
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(7),POS(2))...(Bank ZVT
C      OPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+22+(7)*16+(2)*1)  = XVTX(2)
            RVRT_ZVTOPL3(3,7,JETNO)=XVTX(2)
C      .....Jazelle processed ZVTOPL3(JETNO)(VRTS(7),POS(3))...(Bank ZVT
C      OPL3)
CJSF            JZL$R(JZL$X(ZVTOPL3(JETNO))+22+(7)*16+(3)*1)  = XVTX(3)
            RVRT_ZVTOPL3(4,7,JETNO)=XVTX(3)
         END IF
C    put back any tracks near vertex with high impact param ?
C    21 June 1999, need to count non-returned TIDY tracks
         NK = 0
C   may add hi imp3 tracks
         IF (NSEC1.GT.1 .AND. DLON1.GT.(ICUTP/0.8)) THEN
C      .....Jazelle processed ZVTOPL3(JETNO)(NTRK)...(Bank ZVTOPL3)
CJSF            DO 701 II=1,JZL$I(JZL$X(ZVTOPL3(JETNO))) ,1
            DO 701 II=IBNK_ZVTOPL3(1,JETNO),1
C         .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),VNO)...(Bank ZV
C         TOPL3)
CJSF               IF (JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(II)*21) .EQ.-50)
CJSF     *         THEN
               IF (ITRK_ZVTOPL3(2,II,JETNO).EQ.-50) THEN

C            .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),ZID)...(Bank
C             ZVTOPL3)
CJSF                  IDS = JZL$I(JZL$X(ZVTOPL3(JETNO))+128+(II)*21)
                  IDS = ITRK_ZVTOPL3(1,II,JETNO)
                  AXI(1) = IPX
                  AXI(2) = IPY
                  AXI(3) = IPZ
                  AXF(1)=IPX+GAXI(1)
                  AXF(2)=IPY+GAXI(2)
                  AXF(3)=IPZ+GAXI(3)
                  CALL ZVTRK(AXI,AXF,IDS,TKI,TKV,TRDI,LODI,ANTA)
C             sign imp3 w.r.t. vertex axis (gaxi), not jet axis (gsaf)
C            .....Jazelle processed ZXTRKS(1)(TRK(IDS),IMPACT3)...(Bank
C            ZXTRKS)
CJSF                  IMP3 = JZL$R(JZL$X(ZXTRKS(1))+21+(IDS)*62)
                  IMP3=RTRK_ZXTRKS(49,IDS)
                  IF (LODI.LT.0.0) THEN
                     IMP3 = -IMP3
                  END IF
C             do we want to put this track back ?
                  IF (TRDI.LT.0.1 .AND. ABS(LODI-DLON1).LT.1.0 .AND.
     *            IMP3.GT.0.0 .AND. IMP3.LT.(0.8*DLON1)) THEN
                     NTRK = NTRK + 1
                     DO 711 J=NTRK,II+1-NK,-1
                        ID(J) = ID(J-1)
711                  CONTINUE
712                  CONTINUE
C               .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),ZID)...(B
C               ank ZVTOPL3)
CJSF                     ID(II-NK) = JZL$I(JZL$X(ZVTOPL3(JETNO))+128+(II)*
CJSF      *               21)
                  ID(II-NK) = ITRK_ZVTOPL3(1,II,JETNO)
C               .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),VNO)...(B
C               ank ZVTOPL3)
CJSF                     JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(II)*21)  = 0
                     ITRK_ZVTOPL3(2,II,JETNO)=0
                     AGAIN = .TRUE.
                  ELSE
                     NK = NK + 1
                  END IF
C             leave out of id()
C             if high impact track
C            .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),VNO)...(Bank
C             ZVTOPL3)
CJSF               ELSE IF(JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(II)*21) .LT.-5)
CJSF     *         THEN
               ELSE IF(ITRK_ZVTOPL3(2,II,JETNO).LT.-5) THEN
                  NK = NK + 1
               END IF
701         CONTINUE
702         CONTINUE
C       loop of tracks
         END IF
C    if there is >= 2-prong vertex
C    try throwing all high impact tracks back in
         IF (NSEC1.LE.1) THEN
C      .....Jazelle processed ZVTOPL3(JETNO)(NTRK)...(Bank ZVTOPL3)
CJSF            DO 721 II=1,JZL$I(JZL$X(ZVTOPL3(JETNO))) ,1
            DO 721 II=1,IBNK_ZVTOPL3(1,JETNO) ,1
C         .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),VNO)...(Bank ZV
C         TOPL3)
CJSF               IF (JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(II)*21) .EQ.-50)
CJSF     *         THEN
               IF (ITRK_ZVTOPL3(2,II,JETNO).EQ.-50) THEN
C            .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),ZID)...(Bank
C             ZVTOPL3)
CJSF                  IDS = JZL$I(JZL$X(ZVTOPL3(JETNO))+128+(II)*21)
                  IDS = ITRK_ZVTOPL3(1,II,JETNO)
                  NTRK = NTRK + 1
                  DO 731 J=NTRK,II+1-NK,-1
                     ID(J) = ID(J-1)
731               CONTINUE
732               CONTINUE
C            .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),ZID)...(Bank
C             ZVTOPL3)
CJSF                  ID(II-NK) = JZL$I(JZL$X(ZVTOPL3(JETNO))+128+(II)*21)
                  ID(II-NK) = ITRK_ZVTOPL3(1,II,JETNO)
C            .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),VNO)...(Bank
C             ZVTOPL3)
CJSF                  JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(II)*21)  = 0
                  ITRK_ZVTOPL3(2,II,JETNO) = 0
                  AGAIN = .TRUE.
C             if high impact track
C            .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),VNO)...(Bank
C             ZVTOPL3)
CJSF               ELSE IF(JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(II)*21) .LT.-5)
CJSF     *         THEN
               ELSE IF(ITRK_ZVTOPL3(2,II,JETNO) .LT.-5) THEN
                  NK = NK + 1
               END IF
721         CONTINUE
722         CONTINUE
C       loop of tracks
         END IF
C    if there is <= 1-prong vertex
         IF (AGAIN) THEN
            GOTO 30
         END IF
      END IF
C if TIDY = 2
C end new section 7 June
C output dipole info to ZvTOPL3
C.....Jazelle processed ZVTOPL3(JETNO)(NVRT)...(Bank ZVTOPL3)
C check which tracks we swapped out early in ZVTOP, put them back
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+1)  = NVRT
      IBNK_ZVTOPL3(2,JETNO) = NVRT
C.....Jazelle processed ZVTOPL3(JETNO)(NTRK)...(Bank ZVTOPL3)
      DO 741 II=1,IBNK_ZVTOPL3(1,JETNO) ,1
C   .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),VNO)...(Bank ZVTOPL3)
CJSF         IF (JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(II)*21) .LT.0) THEN
         IF (ITRK_ZVTOPL3(2,II,JETNO) .LT.0) THEN
            NTRK = NTRK + 1
            DO 751 J=NTRK,II+1,-1
               VISOL(J) = VISOL(J-1)
               ID(J) = ID(J-1)
               PTLO(J) = PTLO(J-1)
751         CONTINUE
752         CONTINUE
C       need to reallocate all track, (IP and ghost) parameters
            DO 761 J=NTRK+1,II+1,-1
               DO 771 I=1,9,1
                  C(J,I) = C(J-1,I)
771            CONTINUE
772            CONTINUE
               ST(J) = ST(J-1)
               CT(J) = CT(J-1)
               Z0(J) = Z0(J-1)
               X0P(J) = X0P(J-1)
               TL(J) = TL(J-1)
               S1S(J) = S1S(J-1)
               S2S(J) = S2S(J-1)
761         CONTINUE
762         CONTINUE
C      .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),VNO)...(Bank ZVTOP
C      L3)
CJSF            VISOL(II) = JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(II)*21)
            VISOL(II) = ITRK_ZVTOPL3(2,II,JETNO)
C      .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),ZID)...(Bank ZVTOP
C      L3)
CJSF            ID(II) = JZL$I(JZL$X(ZVTOPL3(JETNO))+128+(II)*21)
            ID(II) = ITRK_ZVTOPL3(1,II,JETNO)
C       do not need st(ii), etc, since ii not in any vertex
         ELSE IF(VISOL(II).GT.NVERT) THEN
            VISOL(II)=0
         END IF
C   redundant ?
741   CONTINUE
742   CONTINUE
      NGTRK = NTRK+1
C make sure ghost track still at end !
      NISO = 0
      DO 781 II=1,NTRK,1
C   .....Jazelle processed ZVTOPL3(JETNO)(TRKS(II),VNO)...(Bank ZVTOPL3)
CJSF          JZL$I(JZL$X(ZVTOPL3(JETNO))+129+(II)*21)  = VISOL(II)
         ITRK_ZVTOPL3(2,II,JETNO)  = VISOL(II)
         IF (VISOL(II).EQ.-1) THEN
            NISO = NISO + 1
         END IF
C   track behind IP
781   CONTINUE
782   CONTINUE
C.....Jazelle processed ZVTOPL3(JETNO)(NISO)...(Bank ZVTOPL3)
CJSF      JZL$I(JZL$X(ZVTOPL3(JETNO))+2)  = NISO
      IBNK_ZVTOPL3(3,JETNO) = NISO
      DO 791 I=1,NTRK,1
C    fill trdi and lodi along ghost axis
         AXI(1) = IPX
         AXI(2) = IPY
         AXI(3) = IPZ
         AXF(1)=IPX+GAXI(1)
         AXF(2)=IPY+GAXI(2)
         AXF(3)=IPZ+GAXI(3)
         CALL ZVTRK(AXI,AXF,ID(I),TKI,TKV,TRDI,LODI,ANTA)
C   .....Jazelle processed ZVTOPL3(JETNO)(TRKS(I),TRDI)...(Bank ZVTOPL3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+145+(I)*21)  = TRDI
         RTRK_ZVTOPL3(16,I,JETNO)=TRDI
C   .....Jazelle processed ZVTOPL3(JETNO)(TRKS(I),LODI)...(Bank ZVTOPL3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+146+(I)*21)  = LODI
         RTRK_ZVTOPL3(17,I,JETNO)=LODI
791   CONTINUE
792   CONTINUE
      DO 801 I=1,NVRT,1
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(I),POS(1))...(Bank ZVTOPL
C   3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+22+(I)*16+(1)*1)  = XPOS(I,1)
         RVRT_ZVTOPL3(2,I,JETNO)=XPOS(I,1)
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(I),POS(2))...(Bank ZVTOPL
C   3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+22+(I)*16+(2)*1)  = XPOS(I,2)
         RVRT_ZVTOPL3(3,I,JETNO)=XPOS(I,2)
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(I),POS(3))...(Bank ZVTOPL
C   3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+22+(I)*16+(3)*1)  = XPOS(I,3)
         RVRT_ZVTOPL3(4,I,JETNO)=XPOS(I,3)
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(I),DPOS(1))...(Bank ZVTOP
C   L3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+25+(I)*16+(1)*1)  = XPOSE(I,1)
         RVRT_ZVTOPL3(5,I,JETNO)=XPOSE(I,1)
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(I),DPOS(2))...(Bank ZVTOP
C   L3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+25+(I)*16+(2)*1)  = XPOSE(I,2)
         RVRT_ZVTOPL3(6,I,JETNO)=XPOSE(I,2)
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(I),DPOS(3))...(Bank ZVTOP
C   L3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+25+(I)*16+(3)*1)  = XPOSE(I,3)
         RVRT_ZVTOPL3(7,I,JETNO)=XPOSE(I,3)
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(I),DPOS(4))...(Bank ZVTOP
C   L3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+25+(I)*16+(4)*1)  = XPOSE(I,4)
         RVRT_ZVTOPL3(8,I,JETNO)=XPOSE(I,4)
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(I),DPOS(5))...(Bank ZVTOP
C   L3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+25+(I)*16+(5)*1)  = XPOSE(I,5)
         RVRT_ZVTOPL3(9,I,JETNO)=XPOSE(I,5)
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(I),DPOS(6))...(Bank ZVTOP
C   L3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+25+(I)*16+(6)*1)  = XPOSE(I,6)
         RVRT_ZVTOPL3(10,I,JETNO)=XPOSE(I,6)
801   CONTINUE
802   CONTINUE
C loop over tracks in each vertex for no. tracks, mass, charge
      DO 811 NV=1,NVRT,1
         NZT = 0
         CRGZ = 0
         PXT = 0.0
         PYT = 0.0
         PZT = 0.0
         ETOT = 0.0
         DO 821 JJ=1,NTRK,1
            IF (VISOL(JJ).EQ.NV) THEN
               NZT = NZT + 1
C         .....Jazelle processed ZXTRKS(1)(TRK(ID(JJ)),CHARGE)...(Bank Z
C         XTRKS)
CJSF                CRGZ = CRGZ + JZL$R(JZL$X(ZXTRKS(1))-35+(ID(JJ))*62)
          CRGZ=CRGZ+RTRK_ZXTRKS(1,ID(JJ))
C         .....Jazelle processed ZXTRKS(1)(TRK(ID(JJ)),HLXPAR(3))...(Ban
C         k ZXTRKS)
CJSF               COSL = COS(ATAN(JZL$R(JZL$X(ZXTRKS(1))-10+(ID(JJ))*62+(3)
CJSF     *         *1) ))
          COSL = COS(ATAN(RTRK_ZXTRKS(24+3,ID(JJ))))
C         .....Jazelle processed ZXTRKS(1)(TRK(ID(JJ)),HLXPAR(3))...(Ban
C         k ZXTRKS)
CJSF               SINL = SIN(ATAN(JZL$R(JZL$X(ZXTRKS(1))-10+(ID(JJ))*62+(3)
CJSF     *         *1) ))
          SINL = SIN(ATAN(RTRK_ZXTRKS(24+3,ID(JJ))))
C         .....Jazelle processed ZXTRKS(1)(TRK(ID(JJ)),HLXPAR(2))...(Ban
C         k ZXTRKS)
CJSF               PTOT = 1/(JZL$R(JZL$X(ZXTRKS(1))-10+(ID(JJ))*62+(2)*1) *
CJSF     *         COSL)
          PTOT = 1/(RTRK_ZXTRKS(24+2,ID(JJ)))
C         .....Jazelle processed ZXTRKS(1)(TRK(ID(JJ)),HLXPAR(1))...(Ban
C         k ZXTRKS)
CJSF               PX = PTOT*COSL*COS(JZL$R(JZL$X(ZXTRKS(1))-10+(ID(JJ))*62+
CJSF     *         (1)*1) )
          PX = PTOT*COSL*COS(RTRK_ZXTRKS(24+1,ID(JJ)))
C         .....Jazelle processed ZXTRKS(1)(TRK(ID(JJ)),HLXPAR(1))...(Ban
C         k ZXTRKS)
CJSF               PY = PTOT*COSL*SIN(JZL$R(JZL$X(ZXTRKS(1))-10+(ID(JJ))*62+
CJSF     *         (1)*1) )
          PY = PTOT*COSL*SIN(RTRK_ZXTRKS(24+1,ID(JJ)))
               PZ = PTOT*SINL
               PXT = PXT + PX
               PYT = PYT + PY
               PZT = PZT + PZ
               ETOT = ETOT + SQRT(PTOT*PTOT + 0.01948)
            END IF
821      CONTINUE
822      CONTINUE
         PTOT = SQRT(PXT*PXT+PYT*PYT+PZT*PZT)
         RADCL = ETOT*ETOT - PTOT*PTOT
         IF((RADCL .LT. 0))RADCL = 0.
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(NV),NTRV)...(Bank ZVTOPL3
C   )
CJSF     JZL$I(JZL$X(ZVTOPL3(JETNO))+21+(NV)*16)  = NZT
         IVRT_ZVTOPL3(1,NV,JETNO)=NZT
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(NV),MASS)...(Bank ZVTOPL3
C   )
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+34+(NV)*16)  = SQRT(RADCL)
         RVRT_ZVTOPL3(13,NV,JETNO)=SQRT(RADCL)
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(NV),CHARGE)...(Bank ZVTOP
C   L3)
CJSF         JZL$I(JZL$X(ZVTOPL3(JETNO))+35+(NV)*16)  = CRGZ
         IVRT_ZVTOPL3(2,NV,JETNO)=CRGZ
811   CONTINUE
812   CONTINUE
C now find chi**2s and probs for track-vertex combination probabilities
C store vertex parameters in v()
      DO 831 J=1,NVRT
C   primary vertex
         IF (J.EQ.1) THEN
            DO 841 I=1,9
               V(J,I) = C(NTRK+1,I)
841         CONTINUE
842         CONTINUE
C      start with IP parameters
         ELSE
            DO 851 I=1,9
               V(J,I) = G(I)
851         CONTINUE
852         CONTINUE
C       ...start with Ghost parameters
         END IF
         DO 861 K=1,NTRK
C       track is in vertex
            IF (VISOL(K).EQ.J) THEN
               DO 871 I=1,9
                  V(J,I) = V(J,I) + C(K,I)
871            CONTINUE
872            CONTINUE
            END IF
861      CONTINUE
862      CONTINUE
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(J),XISQ)...(Bank ZVTOPL3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+32+(J)*16)  = 0.0
         RVRT_ZVTOPL3(11,J,JETNO) = 0.0
C    reset
C   .....Jazelle processed ZVTOPL3(JETNO)(VRTS(J),PRBV)...(Bank ZVTOPL3)
CJSF         JZL$R(JZL$X(ZVTOPL3(JETNO))+33+(J)*16)  = 0.0
         RVRT_ZVTOPL3(12,J,JETNO) = 0.0
831   CONTINUE
832   CONTINUE
C now try all tracks in all vertices
      DO 881 J=1,NVRT
         DO 891 I=1,NTRK,1
C      this added after DUCS crash 10 MAY
            IF (VISOL(I).GE.-1) THEN
               IF (VISOL(I).EQ.J) THEN
C             track is in vertex
                  C1 = V(J,1)
                  C2 = V(J,2)
                  C3 = V(J,3)
                  C4 = V(J,4)
                  C5 = V(J,5)
                  C6 = V(J,6)
                  C7 = V(J,7)
                  C8 = V(J,8)
                  C9 = V(J,9)
               ELSE
C             add track to vertex
                  C1 = V(J,1)+C(I,1)
                  C2 = V(J,2)+C(I,2)
                  C3 = V(J,3)+C(I,3)
                  C4 = V(J,4)+C(I,4)
                  C5 = V(J,5)+C(I,5)
                  C6 = V(J,6)+C(I,6)
                  C7 = V(J,7)+C(I,7)
                  C8 = V(J,8)+C(I,8)
                  C9 = V(J,9)+C(I,9)
               END IF
               CM(1,1) = C1
               CM(1,2) = C4
               CM(1,3) = C5
               CM(2,1) = C4
               CM(2,2) = C2
               CM(2,3) = C6
               CM(3,1) = C5
               CM(3,2) = C6
               CM(3,3) = C3
C          *****  copied this code from bmxinv, and densified *****
               SING=1.0
               N=3
               J1=1
                  GO TO 903
901               J1=J1+1
903               IF(J1-(N).GT.0)GO TO 902
                  INDEX(J1)=1
               GO TO 901
902            CONTINUE
               NTODO=N
910            PIVOT=0.0
               DO 921 J1=1,N
                  IF((INDEX(J1).EQ.0))GOTO   930
                  ELM = ABS(CM(J1,J1))
                  IF((ELM.LE.PIVOT))GOTO 930
                  PIVOT=ELM
                  I1=J1
930               CONTINUE
921            CONTINUE
922            CONTINUE
               IF((PIVOT.EQ.0.0))GOTO   940
               INDEX(I1)=0
950            PIVOT=-CM(I1,I1)
               DO 961 J1=1,N
                  IF((J1-I1))  970,  980,  990
980               CM(I1,J1)=1.0/PIVOT
                  RI(J1)=0.0
                  GOTO   1000
970               ELM=-CM(I1,J1)
                  GOTO   1010
990               ELM=-CM(J1,I1)
1010              RI(J1)=ELM/PIVOT
                  IF((ELM.EQ.0.0))GOTO   1020
                  DO   1030 K1=1,J1
1030              CM(J1,K1)=CM(J1,K1)+ELM*RI(K1)
1020              IF (J1.GT.I1) GOTO   1040
1050              CM(I1,J1)=RI(J1)
                  GOTO 1000
1040              CM(J1,I1)=RI(J1)
1000              CONTINUE
961            CONTINUE
962            CONTINUE
               NTODO = NTODO - 1
               IF((NTODO.NE.0))GOTO 910
1060           DO   1070 I1=1,N
               DO 1070 J1=1,I1
               CM(I1,J1)=-CM(I1,J1)
1070           CM(J1,I1)=CM(I1,J1)
               GOTO   1080
C          *****  end of matrix inversion code *****
940            SING=0.0
1080           IF (SING.GT.0.5) THEN
                  XP = -(CM(1,1)*C7 + CM(2,1)*C8 + CM(3,1)*C9)
                  YP = -(CM(1,2)*C7 + CM(2,2)*C8 + CM(3,2)*C9)
                  ZP = -(CM(1,3)*C7 + CM(2,3)*C8 + CM(3,3)*C9)
               END IF
               NPRG = 0
C          chisq contribution from each track + IP or ghost:
               CHISQ = 0.0
               DO 1091 K=1,NTRK,1
                  IF (VISOL(K).EQ.J .OR. K.EQ.I) THEN
                     XPR = CT(K)*XP + ST(K)*YP
                     YPR = -ST(K)*XP + CT(K)*YP
                     PRFI = ((XPR-X0P(K))**2)/S1S(K) + ((ZP-(Z0(K)+YPR*
     *               TL(K)))**2)/S2S(K)
                     IF (J.EQ.1) THEN
C                  primary vertex, add track momentum factor
                        PRFI = PRFI + MOMF*LOG(PTLO(K))
                        IF (PRFI.LT.0.0) THEN
                           PRFI = 0.0
                        END IF
                     END IF
                     IF (K.EQ.I) THEN
C                   this is track i, output chi**2 to this vertex
C                  .....Jazelle processed ZVTOPL3(JETNO)(TRKS(I),XISG(J)
C                  )...(Bank ZVTOPL3)
CJSF                        JZL$R(JZL$X(ZVTOPL3(JETNO))+130+(I)*21+(J)*1)  =
CJSF     *                   PRFI
                            RTRK_ZVTOPL3(8+J,I,JETNO) = PRFI
                        IF (VISOL(I).EQ.J) THEN
C                     .....Jazelle processed ZVTOPL3(JETNO)(TRKS(I),XIST
C                     )...(Bank ZVTOPL3)
CJSF                           JZL$R(JZL$X(ZVTOPL3(JETNO))+130+(I)*21)  =
CJSF     *                     PRFI
                           RTRK_ZVTOPL3(1,I,JETNO) = PRFI
                        END IF
                     END IF
                     NPRG = NPRG + 1
                     CHISQ = CHISQ + PRFI
                  END IF
1091           CONTINUE
1092           CONTINUE
               IF (J.EQ.1) THEN
C            primary vertex, add IP
                  PRFI = IPERX*(XP)**2 + IPERY*(YP)**2 + IPERZ*(ZP-VPOS(
     *            3))**2
                  NPRG = NPRG + 1
                  CHISQ = CHISQ + PRFI
C            .....Jazelle processed ZVTOPL3(JETNO)(TRKS(I),PRBG(1))...(B
C            ank ZVTOPL3)
CJSF                  JZL$R(JZL$X(ZVTOPL3(JETNO))+137+(I)*21+(1)*1)  = PROB(
CJSF     *            CHISQ,2*NPRG-2)
                  RTRK_ZVTOPL3(9,I,JETNO)=PROB(CHISQ,2*NPRG-2)
                  IF (VISOL(I).EQ.1) THEN
C               .....Jazelle processed ZVTOPL3(JETNO)(VRTS(1),XISQ)...(B
C               ank ZVTOPL3)
CJSF                     JZL$R(JZL$X(ZVTOPL3(JETNO))+32+(1)*16)  = CHISQ
                     RVRT_ZVTOPL3(11,1,JETNO)=CHISQ
C               .....Jazelle processed ZVTOPL3(JETNO)(VRTS(1),PRBV)...(B
C               ank ZVTOPL3)
CJSF                     JZL$R(JZL$X(ZVTOPL3(JETNO))+33+(1)*16)  = PROB(
CJSF     *               CHISQ,2*NPRG-2)
                     RVRT_ZVTOPL3(12,1,JETNO) =
     *                       PROB(CHISQ,2*NPRG-2)
                  END IF
               END IF
               IF (J.GT.1) THEN
C            not primary vertex, add ghost
                  XPR = CT(NGTRK)*XP + ST(NGTRK)*YP
                  YPR = -ST(NGTRK)*XP + CT(NGTRK)*YP
                  PRFI = ((XPR-X0P(NGTRK))**2)/S1S(NGTRK) + ((ZP-(Z0(
     *            NGTRK)+YPR*TL(NGTRK)))**2)/S2S(NGTRK)
                  NPRG = NPRG + 1
                  CHISQ = CHISQ + PRFI
                  IF (NPRG.GT.2) THEN
C               .....Jazelle processed ZVTOPL3(JETNO)(TRKS(I),PRBG(J))..
C               .(Bank ZVTOPL3)
CJSF                     JZL$R(JZL$X(ZVTOPL3(JETNO))+137+(I)*21+(J)*1)  =
                     RTRK_ZVTOPL3(8+J,I,JETNO) = 
     *               PROB(CHISQ,2*NPRG-4)
                  ELSE
C               .....Jazelle processed ZVTOPL3(JETNO)(TRKS(I),PRBG(J))..
C               .(Bank ZVTOPL3)
CJSF                     JZL$R(JZL$X(ZVTOPL3(JETNO))+137+(I)*21+(J)*1)  =
                     RTRK_ZVTOPL3(8+J,I,JETNO) =
     *               PROB(CHISQ,1)
                  END IF
                  IF (VISOL(I).EQ.J) THEN
C               .....Jazelle processed ZVTOPL3(JETNO)(VRTS(J),XISQ)...(B
C               ank ZVTOPL3)
CJSF                     JZL$R(JZL$X(ZVTOPL3(JETNO))+32+(J)*16)  = CHISQ
                     RVRT_ZVTOPL3(11,J,JETNO) = CHISQ
                     IF (NPRG.GT.2) THEN
C                  .....Jazelle processed ZVTOPL3(JETNO)(VRTS(J),PRBV)..
C                  .(Bank ZVTOPL3)
CJSF                        JZL$R(JZL$X(ZVTOPL3(JETNO))+33+(J)*16)  = PROB(
CJSF     *                  CHISQ,2*NPRG-4)
                        RVRT_ZVTOPL3(12, J, JETNO) = 
     *                  PROB(CHISQ, 2*NPRG-4)
                     ELSE
C                  .....Jazelle processed ZVTOPL3(JETNO)(VRTS(J),PRBV)..
C                  .(Bank ZVTOPL3)
CJSF                        JZL$R(JZL$X(ZVTOPL3(JETNO))+33+(J)*16)  = PROB(
CJSF     *                  CHISQ,1)
                        RVRT_ZVTOPL3(12,J,JETNO) =
     *                  PROB(CHISQ,1)
                     END IF
                  END IF
               END IF
            END IF
C       if, track must be in vertices to avoid rid tracks
891      CONTINUE
892      CONTINUE
C    loop over tracks
881   CONTINUE
882   CONTINUE
C loop over vertices
C  do i = 1,nvrt,1 [
C
C      write (*,'(a,f6.3,20f6.3)') ' ',zvtopl3(jetno)%(vrts(i),prbv),
C
C                      (zvtopl3(jetno)%(trks(j),prbg(i)),j=1,ntrk+1);
C
C    ]
C    pZBoutl%(prg2) = zvtopl3(jetno)%(vrts(1),prbv);
C   write(*,*) 'jetno = ', jetno;
C
C    write(*,*) 'nip, probip = ',zvtopl3(jetno)%(vrts(1),ntrv),
C
C                                zvtopl3(jetno)%(vrts(1),prbv);
C
C
      SGLISS=SLD$NORMAL
20    CONTINUE
      IF (SIGSEV(IAND(SGLISS,7)).GT.SGLLEV) THEN
CJSF         SGLISS=SGLIT(SGLISS, 'ZVKIN' ,%DESCR(' ')  )
      END IF
      ZVKIN=SGLISS
      RETURN
      END
