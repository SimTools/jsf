      SUBROUTINE JMFFRD
*.
*...JMFFAL   Read data file name & its logical number
*.
*. INPUT     : none
*. OUTPUT    : none
*.
*. COMMON    : GEFILE
*. CALLS     : FFKEY FFGO  JUALLC
*. CALLED    : UGINIT
*.
*. REPORT CONDITIONS
*.
*. AUTHOR    : Naoyasu HOSODA
*. VERSION   : 1.00
*. CREATED   : 18-May-1994
*. LAST MOD  : 11-May-1995
*.
*. MODIFICATION LOG
*. 22-Jul-1999 J.Kanzaki read a flag for field-map of magnetic field
*. 04-Mar-1999 Nakamura add ITFFRD
*. 24-Feb-1999 Nakamura change KEY GEAN -> DRAW
*                     Now user should use input.dat for GEANT FFKEY.
*                     EVT file doesn't open if KIOPAL (open in KIOPAL)
*. 11-Jan-1999 Y.Fujii,  Added CTAGJD(IDETBM) = 'BM  '
*.                     and other BM,LM related lines. 
*.  3-Sep-1996 Y.Fujii,  Added FFINIT  
*. 28-Aug-1996 Y.Fujii,  Added IR read.
*.  6-Mar-1996 Nakamura,I , Replace ordinary READ to FFREAD.
*. 16-May-1995 Y.Fujii,  Moved juallc in jmppjm-if to make compatibility
*                      between jim and jim++.
*                      Now jim can use input redirection, while jim++
*                      reads data from input.dat.
*. 13-May-1995 Y.Fujii,  Introduced JMPPJM parameter to limit
*.                     readout files in jim++ case.
*. 12-May-1995 Y.Fujii,  Introduced DNAM to achieve that file-readout
*.                     order is arbitrary.
*. 11-May-1995 Nakamura.I ,Replace DYALLC with JUALLC
*.                       Comment out File Open Statement
*. 26-May-1994 N.Hosoda, Read data by using not FFREAD but fortran read 
*. 28-Jun-1994 N.Hosoda, Use not OPEN but DYALLC
*. 27-Jul-1994 N.Hosoda, Use detector name list
*.
*.**********************************************************************
      IMPLICIT NONE

#include "geant321/gckine.inc"
#include "geant321/gctrak.inc"
#include "geant321/gccuts.inc"
#include "geant321/gctime.inc"
#include "geant321/gcflag.inc"

*--   Common for user
#include "jmcntl.inc"
#include "jmfile.inc"
#include "jmdet.inc"
#include "jmstep.inc"
#include "jmkine.inc"
#include "jmbfld.inc"
#include "mchead.inc"

      INTEGER    NWFFRD
      PARAMETER (NWFFRD=1000)
      REAL       SPACE
      COMMON / CFREAD / SPACE(NWFFRD)   
      SAVE   / CFREAD /

      INTEGER NSIZE
      PARAMETER (NSIZE=4)

      CHARACTER*1 CBLK
      PARAMETER (CBLK=' ')

      CHARACTER*120 CMSG

      INTEGER IFILE
      LOGICAL LEXIST

      INTEGER LNBLNK

      INTEGER I,IERR

*-- Due to jim++ trouble, only detector geomerty is read for jim++.
*-- open input.dat only for jim++
      
      IF (JMPPJM.EQ.1) THEN
         CALL JUALLC(5, 'input.dat',0, IERR)
      ENDIF

*-- Set number of usable KEYWORDS
      CALL FFINIT(NWFFRD)        

*-- Set FFREAD
      lusijm = 4
      CALL FFSET('LINP',LUSIJM)
      CALL FFSET('LOUT',LUSOJM)
      CALL FFSET('SIZE',NSIZE)

*-- Initialize filename as ' ' 
      DO 10 I=1,NFMXJM
*--       blank
         FNAMJM(I) = CBLK
*--       null
         LUNMJM(I) = 0
         IF (I.EQ.IFILFO) THEN
*--       direct access
            FORMJM(I) = -1
         ELSE
*--       unformatted
            FORMJM(I) = 1
         ENDIF
 10   CONTINUE

*-- set FFKEY name
*--    event four-vector file
      CTAGJM(IFILFI) = 'VECI'
*--    output bank
      CTAGJM(IFILFO) = 'EVTO'
*--    postscript output file
      CTAGJM(IFILPS) = 'PS  '
*--    parameter file for drawing tool
      CTAGJM(IFILJG) = 'DRAW'
*--    output Hbook file
      CTAGJM(IFILHB) = 'HBK '

*--    formatted
      FORMJM(IFILPS) = 0
*--    formatted
      FORMJM(IFILJG) = 0

*-- FFKEY 
      CALL FFKEY(CTAGJM(IFILFI),FNAMJM(IFILFI),LFNMJM,'MIXED')
      CALL FFKEY(CTAGJM(IFILFO),FNAMJM(IFILFO),LFNMJM,'MIXED')
      CALL FFKEY(CTAGJM(IFILPS),FNAMJM(IFILPS),LFNMJM,'MIXED')
      CALL FFKEY(CTAGJM(IFILJG),FNAMJM(IFILJG),LFNMJM,'MIXED')
      CALL FFKEY(CTAGJM(IFILHB),FNAMJM(IFILHB),LFNMJM,'MIXED')

*-- initialize detector parameter file names
      DO 20 I=1,NDMXJD
         FNAMJD(I) = CBLK
         LUNMJD(I) = 0
         FORMJD(I) = 0
 20   CONTINUE 

      CTAGJD(IDETVX) = 'VX  '
      CTAGJD(IDETCT) = 'CT  '
      CTAGJD(IDETBC) = 'BC  '
      CTAGJD(IDETEC) = 'EC  '
      CTAGJD(IDETMU) = 'MU  '
      CTAGJD(IDETIR) = 'IR  '
      CTAGJD(IDETBM) = 'BM  '
      CTAGJD(IDETLM) = 'LM  '
      CTAGJD(IDETIT) = 'IT  '

      CALL FFKEY(CTAGJD(IDETVX),FNAMJD(IDETVX),LFNMJD,'MIXED')
      CALL FFKEY(CTAGJD(IDETCT),FNAMJD(IDETCT),LFNMJD,'MIXED')
      CALL FFKEY(CTAGJD(IDETBC),FNAMJD(IDETBC),LFNMJD,'MIXED')
      CALL FFKEY(CTAGJD(IDETEC),FNAMJD(IDETEC),LFNMJD,'MIXED')
      CALL FFKEY(CTAGJD(IDETMU),FNAMJD(IDETMU),LFNMJD,'MIXED')
      CALL FFKEY(CTAGJD(IDETIR),FNAMJD(IDETIR),LFNMJD,'MIXED')
      CALL FFKEY(CTAGJD(IDETBM),FNAMJD(IDETBM),LFNMJD,'MIXED')
      CALL FFKEY(CTAGJD(IDETLM),FNAMJD(IDETLM),LFNMJD,'MIXED')
      CALL FFKEY(CTAGJD(IDETIT),FNAMJD(IDETIT),LFNMJD,'MIXED')

*--   number of events to be skipped.
      NSKPJM = 0
      CALL FFKEY('SKIP', NSKPJM , 1, 'INTEGER')

*-- Histograming flags
      HISTJM = 0
      CALL FFKEY('HIST', HISTJM , 1, 'INTEGER')

*-- gstmed parameter auto switch. 0=AUTO, 1=USER DIFINE.
      IGAUTO = 0
      CALL FFKEY('IGAT', IGAUTO , 1, 'INTEGER')      

*-- Detector simulation flags 0=full, 1=smear -1=skip
      DO 30 I=1,NDMXJD
         ISIMJD(I) = IFULJD
 30   CONTINUE 
      CALL FFKEY('SIMD', ISIMJD, NDMXJD, 'INTEGER')

*-- KINE parameters    
      CKINJM = CBLK
      DO 35 I=1,NKINJM
         RKINJM(I) = 0.
 35   CONTINUE 
      CALL FFKEY('GEN ', CKINJM, LKINJM, 'MIXED')
      CALL FFKEY('GENP', RKINJM, NKINJM, 'REAL')

*-- Cuts on forward particles in gustep
      IFWCJM = 1
      THFWJM = 0.05
CCCCC THFWJM = 0.25
      ZFWJM  = 10.
      PTFWJM = 0.0
CCCCC PTFWJM = 0.0075
      RDFWJM = 2.5
      CALL FFKEY('FWCT', IFWCJM, 1, 'INTEGER')
      CALL FFKEY('THFW', THFWJM, 1, 'REAL')
      CALL FFKEY('ZFW',  ZFWJM,  1, 'REAL')
      CALL FFKEY('PTFW', PTFWJM, 1, 'REAL')
      CALL FFKEY('RDFW', RDFWJM, 1, 'REAL')

*-- geant Cut-off parameters FFKEY 'CUTS'
      CUTGAM = 0.0003
      CUTELE = 0.0003
      CUTNEU = 0.0003
      CUTHAD = 0.0003
      CUTMUO = 0.0003

      TIMEND = 1.
      ITIME = 10000000

      ITEST = 0
      CALL FFKEY('TEST', ITEST , 1, 'INTEGER')

*--   Mafnetic field (kGauss)
      BKGMC = 20.
      CALL FFKEY('MFLD', BKGMC , 1, 'REAL')      

*--   Flag for field-map
      IFMPJM = 0
      CFMPJM = CBLK
      CALL FFKEY('BMAP', CFMPJM, LFMPJM, 'MIXED')
      
*-- FFGO
      CALL GFFGO

      CALL JMKIRD
	
      ITYPMC = IKINE
      EBMMC  = PKINE(1)/2.0

      IFILE = 0
      DO 40 I = 1,NFMXJM
         IF (FNAMJM(I).NE.CBLK) THEN
            IFILE = IFILE+1
            LUNMJM(I) = LUN0JM+IFILE
*-- open specified files
*-- skip if KIOPAL file is opened in a subroutine KIOPAL 
            IF (I.NE.IFILFI .AND. I.NE.IFILFO) THEN
               CALL JUALLC(LUNMJM(I),
     &              FNAMJM(I)(:LNBLNK(FNAMJM(I))),FORMJM(I),IERR)
            ENDIF
         ENDIF
 40   CONTINUE
      
      IFILE = 0
      DO 50 I = 1,NDMXJD
         IF (FNAMJD(I).NE.CBLK) THEN
            INQUIRE(FILE=FNAMJD(I)(:LNBLNK(FNAMJD(I))),
     &           EXIST=LEXIST)
            IF (.NOT.LEXIST) THEN
               WRITE(CMSG,
     &              '(''Specified parameter file does not exist for '',A4,  
     &              '':'',A)') CTAGJD(I),FNAMJD(I)(:LNBLNK(FNAMJD(I))) 
               CALL MSGOUT('JMFFRD',CMSG(:LNBLNK(CMSG)),2)
               STOP
            ENDIF
            IFILE = IFILE+1
            LUNMJD(I) = LUN0JD+IFILE
            CALL JUALLC(LUNMJD(I),
     &           FNAMJD(I)(:LNBLNK(FNAMJD(I))),FORMJD(I),IERR)
         ENDIF
 50   CONTINUE

*-- print file allocation list
      WRITE(LUSOJM,'(//5X,''<< File Allocation List >>''/)')
      DO 60 I = 1,NFMXJM
         IF (LUNMJM(I).GT.0) THEN
            WRITE(LUSOJM,'(1H ,A4,'':'',I4,'':'',A)') 
     &           CTAGJM(I),LUNMJM(I),FNAMJM(I)(:LNBLNK(FNAMJM(I)))
         ENDIF
 60   CONTINUE

*-- print file allocation list for detector parameter file
      WRITE(LUSOJM,'(//5X,''<< Parameter File Allocation List >>''/)')
      DO 70 I = 1,NDMXJD
         IF (LUNMJD(I).GT.0) THEN
            WRITE(LUSOJM,'(1H ,A4,'':'',I4,'':'',A)') 
     &           CTAGJD(I),LUNMJD(I),FNAMJD(I)(:LNBLNK(FNAMJD(I)))
         ENDIF
 70   CONTINUE

      IF (CFMPJM.NE.CBLK) THEN
         IF (CFMPJM.EQ.'ON') THEN
            IFMPJM = 1
         ELSE
            IFMPJM = 0
         ENDIF
      ENDIF
	
      RETURN
      END
