      SUBROUTINE GUOUT
*.
*...GUOUT    GEANT3 user routine
*.           User routine called at the end of each event.
*.
*. INPUT     : none
*. OUTPUT    : none
*.
*. SEQUENCE  :
*. COMMON    : GCFLAG MCHEAD GEFILE
*. CALLS     : HFILL  IRQST  ICLRWK GPHITS
*. CALLED    : GTRIG
*.
*. BANKS L   :
*. BANKS U   :
*. BANKS M   :
*. BANKS D   :
*.
*. REPORT CONDITIONS
*.
*. AUTHOR    : Naoyasu HOSODA
*. VERSION   : 1.00
*. CREATED   : 11-Mar-1994
*. LAST MOD  : 22-May-1995
*.
*. MODIFICATION LOG
*. 13-Mar-1996 J.Kanzaki, Added CT analysis routines JUANAL and JFILBK
*. 06-Mar-1996 Nakamura Isamu remove BCOUT
*. 08-May-1995 Y.Fujii,   Added VXOUT 
*. 22-May-1995 Nakamura Isamu added BCOUT
*. 18-May-1994 replace geant common by INCLUDE
*. 17-Jun-1994 Add GPDIGI
*.
*.**********************************************************************
      IMPLICIT NONE

*-- Common for GEANT
#include "geant321/gcflag.inc"

*-- Common for private
#include "jmfile.inc"
#include "mchead.inc"
      
      CHARACTER*100 STR/'Hit return'/

      INTEGER ISTAT,L


*-- Store contents of HEADER bank
      CALL JMBFHD

*-- Output an event to the file specified by LUN
      IF (LUNMJM(IFILFO).GT.0) THEN
         
CCC         CALL KZWRIT(LUNMJM(IFILFO))

      ENDIF

      CALL JMHFIL
      CALL CTHFIL
      CALL VXHFIL

      IF (IDEBUG*ISWIT(4) .GE. 1) THEN

*-- HIGZ: returns a character string typed on the keyboard
        CALL IRQST(1, 1, ISTAT, L, STR)

*-- HIGZ: clear all active workstations
        CALL ICLRWK(0,0)
      ENDIF

*-- For debug
      IF (IDEBUG.EQ.1 .AND. ISWIT(3) .GE. 1) THEN
*-- Print JHITS bank
        CALL GPHITS('*   ','*   ')
        CALL GPDIGI('*   ','*   ')
      ENDIF

*-- Delete (ZDROP'S and one ZGARB) all existing banks 
*-- and clear bank directory

CCC      CALL KZECLR

  999 CONTINUE
      RETURN
      END
