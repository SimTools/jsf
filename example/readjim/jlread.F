C      
C Original jlread.F was devided to jlread0 and jlread1,
C the former for the initialization and get marameters,
C the latter for to read event data.
C
C A.Miyamoto  23-July-2000
C


      SUBROUTINE JLREAD0(IFLAG)
C
C  Read one record from a file and set parameters.
C
      IMPLICIT NONE

#include "jlunit.inc"
#include "jlfile.inc"

      
      INTEGER IFLAG
      
      INTEGER IERR
      
      CHARACTER*120 CFILE,CTEXT

      INTEGER IFEVTI
c      SAVE IFEVTI
      common/jlread_common/ ifevti

      INTEGER IFIRST
      DATA IFIRST/ 0/
      SAVE IFIRST

      INTEGER LNBLNK
      
      INTEGER NDUP,IDUP
      INTEGER IFILE

      LOGICAL LEXIST

      INTEGER ILOOP
      INTEGER NLOOP
      PARAMETER (NLOOP=10)


      IF (IFIRST.EQ.0) THEN
         
*-- Read the first event, which should be a parameter event, 
*-- not a data event.

         IFIRST = 1
         IFEVTI = 1

         CFILE = FNEVTI(IFEVTI)
         INQUIRE(FILE=CFILE(:LNBLNK(CFILE)),
     &        EXIST=LEXIST)

         IF (.NOT.LEXIST) THEN

            WRITE(CTEXT,'(''File does not exist:'',A)')
     &           CFILE(:LNBLNK(CFILE))
            CALL MSGOUT('JLREAD',CTEXT,101)
            STOP
            
         ENDIF
         
         IF (IVEVTI.EQ.1) THEN
            CALL KZOPEN(LUEVTI,CFILE(:LNBLNK(CFILE)),IERR)
         ELSEIF (IVEVTI.EQ.0) THEN
            CALL KZXOPE(LUEVTI,CFILE(:LNBLNK(CFILE)),IERR)
         ENDIF
C         CALL JUALLC(LUEVTI,CFILE(:LNBLNK(CFILE)),1,IERR)
C         CALL KZFILE(LUEVTI)
         
         CALL KZECLR
         CALL KZREAD(LUEVTI,IERR)
         IF (IERR.NE.0) THEN
            CALL MSGOUT('JLREAD','Error in reading parameter event',
     &           IERR)
         ENDIF
         
         CALL JLGJMP(IERR)
         IF (IERR.NE.0) THEN
            CALL MSGOUT('JLREAD','Error in getting JIM parameters',IERR)
         ENDIF
                  
         CALL JLGCTP(IERR)
         IF (IERR.NE.0) THEN
            CALL MSGOUT('JLREAD','Error in getting CT parameters',IERR) 
         ENDIF
                  
         CALL JLGBCP(IERR)
         IF (IERR.NE.0) THEN
            CALL MSGOUT('JLREAD','Error in getting BC parameters',IERR) 
         ENDIF
         
         CALL JLGECP(IERR)
         IF (IERR.NE.0) THEN
            CALL MSGOUT('JLREAD','Error in getting EC parameters',IERR) 
         ENDIF
         
         CALL JLGVXP(IERR)
         IF (IERR.NE.0) THEN
            CALL MSGOUT('JLREAD','Error in getting VX parameters',IERR) 
         ENDIF
         
         CALL JLGITP(IERR)
         IF (IERR.NE.0) THEN
            CALL MSGOUT('JLREAD','Error in getting IT parameters',IERR) 
         ENDIF
         
         CFILE = FNEVTO
         IF (LNBLNK(CFILE).GT.0) THEN

            CALL KZOPEN(LUEVTO,CFILE(:LNBLNK(CFILE)),IERR)
            CALL KZWRIT(LUEVTO)

         ENDIF


         IF (NFEVTB.GT.0) THEN
            DO 10 IFILE=1,NFEVTB
               CALL KZREAD(LUEVTB(IFILE),IERR)
 10         CONTINUE
         ENDIF

      ENDIF

      return 
      end



      SUBROUTINE JLREAD1(IFLAG)
C
C  Read one record from a file and set parameters.
C
      IMPLICIT NONE

#include "jlunit.inc"
#include "jlfile.inc"
      
      INTEGER IFLAG
      
      INTEGER IERR
      
      CHARACTER*120 CFILE,CTEXT

      INTEGER IFEVTI
c      SAVE IFEVTI
      common/jlread_common/ ifevti

C      INTEGER IFIRST
C      DATA IFIRST/ 0/
C      SAVE IFIRST

      INTEGER LNBLNK
      
      INTEGER NDUP,IDUP
      INTEGER IFILE

      LOGICAL LEXIST

      INTEGER ILOOP
      INTEGER NLOOP
      PARAMETER (NLOOP=10)

         
*-- Read one data event
 1000 CONTINUE

      CALL KZECLR

      CALL KZREAD(LUEVTI,IERR)

      IF (IERR.EQ.1) THEN
         
*-- End of file is found
         IFEVTI = IFEVTI+1
         CALL KZENDI(LUEVTI)

         IF (IFEVTI.GT.NFEVTI) THEN
            IFLAG = 1
            RETURN
         ENDIF

         CFILE = FNEVTI(IFEVTI)
         INQUIRE(FILE=CFILE(:LNBLNK(CFILE)),
     &        EXIST=LEXIST)

         IF (.NOT.LEXIST) THEN

            WRITE(CTEXT,'(''File does not exist:'',A)')
     &           CFILE(:LNBLNK(CFILE))
            CALL MSGOUT('JLREAD',CTEXT,101)
            STOP
            
         ENDIF
         
         CALL KZOPEN(LUEVTI,CFILE(:LNBLNK(CFILE)),IERR)
C         CALL JUALLC(LUEVTI,CFILE(:LNBLNK(CFILE)),1,IERR)

         GO TO 1000

      ELSEIF (IERR.NE.0) THEN

         IFLAG = IERR
         RETURN

      ENDIF
      
      IF (NFEVTB.GT.0) THEN

*-- Read background events
         DO 1010 IFILE=1,NFEVTB

            NDUP = NDEVTB(IFILE)

            DO 1020 IDUP=1,NDUP

               ILOOP = 0

 1030          CONTINUE 

               ILOOP = ILOOP+1
               IF (ILOOP.GT.NLOOP) THEN
                  CALL MSGOUT('JLREAD','Maximum Loop count reached',
     &                 ILOOP)
                  STOP
               ENDIF
               
               CALL KZREAD(LUEVTB(IFILE),IERR)

               IF (IERR.EQ.1) THEN

                  CALL KZRWND(LUEVTB(IFILE))
                  GOTO 1030

               ELSEIF (IERR.NE.0) THEN
                  IFLAG = IERR+1000
                  RETURN
               ENDIF
                  
 1020       CONTINUE

 1010    CONTINUE

      ENDIF

      IFLAG = 0
      
      CALL KZEFST

      RETURN
      END
