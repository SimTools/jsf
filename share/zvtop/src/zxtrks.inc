C!----------------------------------------------------------------------"
C!==>Internal track storage for ZXFIND                                  "
C!----------------------------------------------------------------------"
C!                                                                      "
C!  ZXTRKS contains all necessary info for all tracks in event          "
C!         filled by ZXFILL                                             "
C!                                                                      "
C!         latest version refers to DST through PHCHRG                  "
C!                                                                      "
C!----------------------------------------------------------------------"
C! KEYWORDS:  ZXTRKS  TEMPLATE                                          "
C!*--------------------------------------------------------------------*"
C!                                                                      "
C!                                                                      "
C!*---------------------------------------------------------------------
C!     OWNER: GLADDING, Gary         CO-OWNER:  Schindler, Rafe         "
C!   SECTION: ZXFIND                     FILE:  ZXTRKS TEMPLATE         "
C!                                                                      "
C!                    Date            Explanation of Changes            "
C!                ----------- ------------------------------------------"
C! Version   0.00;"  1 Sep 1989 Program creation date                   "
C! Version   0.01;"  4 Oct 1989 Modify content                          "
C! Version   1.00;" 28 Nov 1989 ZX version                              "
C! Version   2.00;" 26 Mar 1990 add thrust variables                    "
C! Version   3.00;" 28 May 1990 change: PTYPTRU,PTYPE,PIDVEC(5)         "
C! Version   4.00;" 18 Jul 1991 delete LLIK, add PHPART, IMPACT         "
C! Version   4.10;"  1 Aug 1991 add entire error matrix DXYZPRI(6)      "
C! Version   4.50;" 18 Feb 1993 add impact3, b3norm, var(2)             "
C! Version   4.51;" 31 Mar 1993 status(4) redefined                     "
C! Version   4.52;"  7 Jun 1993 comments to define HLXPAR               "
C! Version   4.53;" 28 Jun 1993 redefine PTYPE packed 3 integers        "
C! Version   4.54;" 26 Nov 1993 redefine TSINORM and ETANORM            "
C! Version   5.00;" 27 Dec 1993 PHPART -> PHCHRG, PHTRKID & staus(4)=32 "
C! Version   5.10;" 21 Oct 1996 update meaning of status(4) = 30 (SW)   "
C!----------------------------------------------------------------------"


      INTEGER*4 MAX_ZXTRKS
      PARAMETER (MAX_ZXTRKS=100)
      INTEGER*4 IBNK_ZXTRKS(9), ITRK_ZXTRKS(12,MAX_ZXTRKS)
      REAL*4    RBNK_ZXTRKS(16), RTRK_ZXTRKS(52,MAX_ZXTRKS)
      COMMON /ZXTRKS_BANK/ IBNK_ZXTRKS, RBNK_ZXTRKS,
     >                    ITRK_ZXTRKS, RTRK_ZXTRKS
C234567         
 
C BANK  ZXTRKS  CONTEXT = DEVENT  DEMAND_ZERO MAXID=1 "header: 1/event   "
C
C First Column is a index for IBNK_ZXTRKS array
C  1   INTEGER*4 RTRNPRI       " RTN CODE FROM PRIMRY VTX FINDER see note 1"
C  2   INTEGER*4 NTKPRI        " #  OF TKS CONSISTENT WTH PRIM VTX-TYP 0,1 "
C  3:4 HEX       IDTRKS(2)     " 1 BIT SET FOR EACH TRACK WITH VTX-TYP =  0"
C  5   INTEGER*4 NCHRGIN       " No. of chrg trks inside vtx chm.    "
C  6   INTEGER*4 NINPLUS       " trks 1 - NINPLUS are in positive hemisphere"
C  7   INTEGER*4 NCHRGOUT      " No. of chrg trks outside vtx chm.   "
C  8   INTEGER*4 NCHRGBEY      " No. of chrg trks beyond vtx chm.    "
C  9   INTEGER*4 NCHRG         " No. of chrg trks in event=sum of above 3 "
C
C First Column is a index for RBNK_ZXTRKS array
C  1     REAL*4    RMSPRI        " RMS OF PRIMARY VERTEX FIT               "
C  2:4   REAL*4    XYZPRI(3)     " (X,Y,Z) OF PRIMARY VERTEX (CM)          "
C  5:10  REAL*4    DXYZPRI(6)    " ERROR MATRIX OF PRIMARY VERTEX (CM)     "
C 11:13  REAL*4    THRUST(3)     " unit vector in direction of thrust axis "
C 14:16  REAL*4    BEAMPOS(3)    " (x,y,z) posn used for impact, lvtx, hlxpar"
 
 
C BLOCK TRK(NCHRG)          " The track block (ordered in,out,beyond)  "
C First column is a index for ITRK_ZXTRKS(i, ntrk)
C MCID and PHTRKID is saved as Integer*4
C 1  POINTER VTXTRU-->VTXTRU " pointer to true MC vertex for this track "
C 2  POINTER PHCHRG-->PHCHRG " pointer to dst bank PHCHRG for this track"
C 3  INTEGER*2 MCID          " MCPART id of monte carlo track           "
C 4  INTEGER*2 PHTRKID       " PHTRK  id of track (agree with PHCHRG)   "
C 5  INTEGER   PTYPE         " packed lepton identification: see note 2  "
C 6:12  INTEGER*4 STATUS(6)     " status bits from ZXVEE-ZX2PG   see note 1"
C
C
C First column is a index for RTRK_ZXTRKS(i,ntrk)
C  1     REAL*4    CHARGE        " charge                                   "
C  2     REAL*4    TKPAR0        "  *  track parameters and error matrix  * "
C  3:7   REAL*4    TKPAR(5)      "  *   just inside the beampipe          * "
C  8:22  REAL*4    DTKPAR(15)    "  *    standard SLD co-ord system       * "
C 23     REAL*4    BNORM         " normalized 2-D impact parameter (no IP err)"
C 24     REAL*4    IMPACT        "        2-D impact parameter (cm)         "
C 25:30  REAL*4    HLXPAR(6)     "  *  track parameters and error matrix   *"
C 31:45  REAL*4    DHLXPAR(15)   "  * at POCA to beamline  HX co-ord system*"
C 46     REAL*4    COSTHRST      " COS of angle between track and thrust axis"
C 47     REAL*4    RADNORM       " radnorm= sqrt(bnorm**2 + znorm**2) (sigma)"
C 48     REAL*4    ZNORM         " znorm = displace from primary in z (sigma)"
C 49     REAL*4    IMPACT3        " 3-dim impact parameter (cm)              "
C 50     REAL*4    B3NORM         " norm 3-dim impact param INCLUDES IP ERROR "
C 51:52  REAL*4    VAR(2)         " VAR(1) = B3NORM without IP error         "
C!                             VAR(2) = not filled
C ENDBLOCK
C!
C! Notes:
C!
C! (1) STATUS(1-2) for ZXVEE, (3) for ZXPRI, (4) for swimming
C!     STATUS(5-6) for ZX2PG
C!
C!     ************************* ZXVEE useage ***********************
C!     Bit 0 (LSB of STATUS(1)) = 1 if TRK is used in some ZXVEED block   losest
C!     Bit N of STATUS(1) {N=1,31} = 1 if vee N contains current TRK
C!     Bit M of STATUS(2) {M=1,31} = 1 if vee (M+31) contains current TRK
C!     vee number = VTX index in ZXVEED
C!
C!     ************************* ZXPRI useage ************************
C!
C!     RTRNPRI  =   1   CONVERGED FIT
C!                  2   UNCONVERGED
C!                 12   FAILURE ON ENTRY
C!
C!     STATUS(3)=   0   CONSISTENT WITH PRIMARY VERTEX (postfit)
C!                  1   MAYBE IN PRIMARY VERTEX (postfit (CT2)
C!                  2   NOT FROM PRIMARY VERTEX (prefit) (CT1)
C!                  3   OUTSIDE OR BEYOND  VERTEX CHAMBER
C!                  4   REMOVED BY GSQ ZXVEE PACKAGE
C!                  5   REMOVED BY R OR PT CUT
C!                  6   SWIM FAILURE  (STATUS(4) <> 0)
C!
C!     ONLY TYPES 0 and 1 ARE USED IN FIT TO PRIMARY VERTEX
C!
C!        note:     if ZXFINDP.dobeam = false, status(3) calculated
C!                                             by ZXZPRI using cut on
C!                                             sqrt(bnorm**2+znorm**2)
C!
C!     ************************* swim   useage ****************
C!
C!     STATUS(4)=   0 * swim to ip a success
C!                  1   track swims to beampipe, but error matrix fails
C!                  2 * track leaves fiducial volume (see PMLOADP for def)
C!                  3   track reaches beampipe at |z| > 124.8
C!                  4 * track turns over before it reaches beampipe
C!                  5-8 shouldn't really happen.. contact GSQ if it does
C!                 11   swim fr bpipe to bline fails: too big a dip angle
C!                 12   swim fr bpipe to bline fails: too many iter
C!                 13   shouldn't really happen.. contact GSQ if it does
C!                 15   trk turns over but swim to bline fails: dip angle
C!                 16 * trk turns over but swim to bline fails: iter
C!                 17   shouldn't really happen.. contact GSQ if it does
C!                 21   ZXswim to beamline fails: too big a dip angle
C!                 22   zxswim to beamline fails: too many iterations
C!                 23   shouldn't really happen.. contact GSQ if it does
C!                 30 * beyond track with successful swim
C!                 32   original track had bad error matrix .. no swim
C!
C!          note: * indicates only significant cases
C!                HLXPAR for status(4) = 0, 4, 30 are reported at poca to beam
C!                HLXPAR for others are reported at tkpar0
C!
C!     ************************* ZX2PG useage ***********************
C!     Bit 0 (LSB of STATUS(5)) = 1 if TRK is used in some ZX2PGD block   losest
C!     Bit N of STATUS(5) {N=1,31} = 1 if 2pg N contains current TRK
C!     Bit M of STATUS(6) {M=1,31} = 1 if 2pg (M+31) contains current TRK
C!     2pg number = VTX index in ZX2PGD
C!            note: if VTX index > 63, only BIT 0 of STATUS(5) set
C!
C!
C! (2) PTYPE is a packed word containing lepton identification information.
C!           from KPRTID (electrons) and WUFIND (muons).
C!
C!           PTYPE = PHTRKID + (256*EID) + (65536*MUID)
C!
C!                   PHTRKID = id of PHTRK bank (= id of KTRKLAC, PHWTRK)
C!                   EID  = 2  if KTRKLAC%(prob_em) >= KTRKLAC%(prob_mu) &
C!                                KTRKLAC%(prob_em) >= KTRKLAC%(prob_pi)
C!                        = 1  if KTRKLAC%(prob_mu) >= KTRKLAC%(prob_em) &
C!                                KTRKLAC%(prob_mu) >= KTRKLAC%(prob_pi)
C!                        = 0  if KTRKLAC%(prob_pi) >= KTRKLAC%(prob_em) &
C!                                KTRKLAC%(prob_pi) >= KTRKLAC%(prob_em)
C!                        =255 if no KTRKLAC bank exists
C!                   MUID = PHWTRK(phtrkid)%(idstat)
C!
C!   note: if PTYPE is filled from ZXFRMPH (ie from reading MDST), then
C!                  MUID = PHWIC%(idstat)
C!                  EID  = PHKELID%(idstat)
C!
C! here be IDA gymnastics to unpack PTYPE (since all variables are real)
C!
C!          ptype = zxtrks.trk(n).ptype
C!          phtrkid = ptype - float(int(ptype/256))*256
C!          eid = float(int(ptype/256))
C!          eid = eid - float(int(ptype/65536))*256
C!          muid = float(int(ptype/65536))
C!
C!
C! (3) HLXPAR(1) = phi
C!     HLXPAR(2) = 1/pt
C!     HLXPAR(3) = tan(lambda)
C!     HLXPAR(4) = x
C!     HLXPAR(5) = y
C!     HLXPAR(6) = z
C!
C!     note: HLXPAR(4-6) are reported as (x,y,z) for convenience. the
C!     error matrix DHLXPAR(1-15) has elements which correspond to
C!     parameter 4 = TSI and parameter 5 = ETA, two distances to a
C!     given point defined by:
C!                       TSI =  TN dot DELTA
C!                       ETA =  TQ dot DELTA
C!
C!     where  DELTA = (x-x0, y-y0, z-z0)    {(x0,y0,z0) = given origin}
C!            TN    =  PHAT cross TM        (PHAT = momentum unit vector)
C!                                          (TM   = mag field unit vector)
C!            TQ    = (1/(1-(PHAT dot TM)**2) * (TM - (PHAT dot TM)PHAT)
C!
C!     to convert HLXPAR(1-3) to a momentum vector (PX,PY,PZ):
C!
C!            PTOT = pt/cos(lambda)
C!            PX   = PTOT * cos(lambda) * cos(phi)
C!            PY   = PTOT * cos(lambda) * sin(phi)
C!            PZ   = PTOT * sin(lambda)
C!
CENDBANK
