C *********************************************************************
C This file includes three subroutines to call JIM in JSF.
C These routines are prepared from Geant's grun.
C
C *********************************************************************

*CMZ :          03/11/94  15.43.38  by  John Apostolakis CERN GP-MIMD 2
*-- Author :
      SUBROUTINE JSFJIMGRUN(NRET)
C
C  NRET=IEVENT for normal return
C      = -1 at the end of run
*.
*.    ******************************************************************
*.    *                                                                *
*.    *       Steering routine to process all the events               *
*.    *                                                                *
*.    *    ==>Called by : <USER>, main program                         *
*.    *       Author    R.Brun  *********                              *
*.    *                                                                *
*.    ******************************************************************
*.
#include "geant321/gcbank.inc"
#include "geant321/gcflag.inc"
#include "geant321/gcunit.inc"
#include "geant321/gctime.inc"

#if defined(CERNLIB_PARA)
#include "geant321/multiprox.inc"
#endif

      SAVE IFIRST
      DATA IFIRST/0/
*.
*.    ------------------------------------------------------------------
*.
*             Keep starting time
*
      IF(IFIRST.EQ.0)THEN
         IFIRST=1
         CALL TIMEL(TIMINT)
      ENDIF
#if defined(CERNLIB_PARA)
*      Parallel : Does not change nevent on nodes - using it as maximum
*                   number that one node can process and regulating
*                   end by end of input file ...
*                 For programs that have no input file, gprun should
*                   divide the total number of events evenlnevey between tasks.
*
      call gprun
*
#endif

      IF(IEVENT.LT.NEVENT) THEN
         IEVENT=IEVENT+1
*
*             Initialises event partition
*
         CALL GTRIGI
*
*             Process one event (trigger)
*
         CALL GTRIG
*
*             Clear event partition
*
         CALL GTRIGC
*
         IF(IEORUN.EQ.0) THEN
*
*             Check time left
*
            IF(ITIME.LE.0)GO TO 10
            IF(MOD(IEVENT,ITIME).NE.0)GO TO 10
            CALL TIMEL(TIMNOW)
            IF(TIMNOW.GT.TIMEND)GO TO 10
            WRITE(CHMAIL,10000)TIMEND
            CALL GMAIL(0,2)
            IEORUN = 1
         ENDIF
      ENDIF
*
      nret = -1
10000 FORMAT(5X,'***** THE JOB STOPS NOW BECAUSE THE TIME LEFT IS LESS',
     +' THAN     ',F8.3,' SECONDS *****')
      return

 10   continue
      nret=ievent
      return
      END


C*********************************************************************
      subroutine jsfjimopen(iu,fname,iret)
      character*(*) fname
      integer*4 iu,iret

      open(iu,file=fname,err=100)
      iret=0
      return

 100  continue
      iret=-1
      return
      end

C********************************************************************
      subroutine jsfjimclose(iu)
      close(iu)
      return
      end

