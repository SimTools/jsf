      SUBROUTINE JMBFHD
*.
*...JMBFHD   store HEADER bank
*.
*. INPUT     : none
*. OUTPUT    : none
*.
*. SEQUENCE  :
*. COMMON    : GCFLAG GCNUM  MCHEAD 
*. CALLS     : KZCBR1 KZPUT1 
*. CALLED    : GUDIGI
*.
*. BANKS L   : HEADER
*. BANKS U   :
*. BANKS M   :
*. BANKS D   : 
*.
*. REPORT CONDITIONS
*.
*. AUTHOR    : Junichi Kanzaki
*. VERSION   : 1.00
*. CREATED   : 13-Mar-1996
*. LAST MOD  : 13-Mar-1996
*.
*. MODIFICATION LOG
*.**********************************************************************
      IMPLICIT NONE

*--   Common
#include "geant321/gcflag.inc"
#include "geant321/gcnum.inc"

#include "mchead.inc"

*--   Parameter for bank 'HEADER'
      CHARACTER*32 CFMHED,CFSHED
c      CHARACTER*32 CNAHED,CFMHED,CFSHED
      INTEGER NWMHED,LSEHED,IERHED
c      PARAMETER (CNAHED='HEADER')
      PARAMETER (NWMHED=20)
      PARAMETER (CFMHED='3I 2F 15I')
      PARAMETER (LSEHED=0)
      PARAMETER (CFSHED=' ')
      INTEGER NMXHED
      PARAMETER (NMXHED=NWMHED)
      INTEGER   IDTHED(NMXHED)
      REAL      DATHED(NMXHED)
      EQUIVALENCE (IDTHED(1),DATHED(1))

      INTEGER NSEHED
      PARAMETER (NSEHED=0)

      INTEGER I

*--   Set data
      IDTHED(1) = 0
      IDTHED(2) = 0
      IDTHED(3) = IDEVT
      DATHED(4) = EBMMC
      DATHED(5) = BKGMC

      DO 10 I=6,NWMHED
      IDTHED(I) = 0
 10   CONTINUE

*--   Create new ZBS bank with the 1-dim. fixed length segment.
      CALL KZBCR1('HEADER',NWMHED,CFMHED,NSEHED,LSEHED,CFSHED, IERHED)

*--   Put data words into the given segment of the bank
      CALL KZREP1('HEADER',0,NWMHED,IDTHED)

      RETURN
      END
