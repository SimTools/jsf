C********************************************************************
C*
C*  Subroutines to read JIM data.
C*
C*(Author)
C*  A.Miyamoto 17-Apr-1999  Original version prepared from ntmake.F 
C*                          in jlcsim.
C********************************************************************

      SUBROUTINE READJIMINIT(IUNIT, FNAME)
C*
C*(Input)  
C*    Iunit(I*4) : Fortran logical unit number to read data
C*    Fname(Character*(*)) : 
C*               : File name of JIM *.bnk file, length is less than 128 
C*                       
C*

C      PROGRAM NTMAKE
*
*  This program reconstruct mass of H generated in Nu-Nu-H production
C  where H decays into gamma-gamma pair.  
*  The data are read from the file of the ZBS BANK system.
*
C     ZHRECON was modified to NNHREC.			10-Sep-1998, Y.Fujii
C     WRECON was modified to ZHRECON.			30-May-96, Y.Fujii
*     Original program for jim was modified for fjim.	12-Apr-96, Y.Fujii
* 
*     Ishii-san's single-W reconstruction program 
*     was modified 25-Mar-96 by Y.Fujii

      IMPLICIT NONE

#include "jlunit.inc"
#include "jlfile.inc"
#include "jlanal.inc"
#include "jmcntl.inc"
#include "trbuff.inc"
#include "pawc.inc"

      INTEGER IERR,ICYCLE

      INTEGER LNBLNK

      INTEGER*4 IUNIT
      CHARACTER*(*) FNAME

*-- HBOOK etc. Initialization
      CALL HLIMIT(NHBOOK)

      CALL KZOPTN('NO-ZEBRA-INIT')

      CALL KZINIT
      
      CALL JLFFKY


C .. Varaibales in jlunit.inc
      LUSTDI = IUNIT
      LUSTDO = 6
      LUEVTI = 20
      LUNTPL = LUEVTI+NPEVTB+1
      LUEVTO = LUNTPL+1
      LUTEMP = 80

      IVEVTI = 1
      NFEVTI = 0
      CFEVTI = ' '
C      CALL FFKEY('EVTV',IVEVTI,1,'INTEGER')
C      CALL FFKEY('EVTI',CFEVTI,32,'MIXED')
C      CFEVTI = 'jetset_qq_fast.bnk'
      CFEVTI = FNAME
      NFEVTB = 0
      CFEVTB = ' '
C      CALL FFKEY('EVTB',CFEVTB,32,'MIXED')
      FNNTPL = 'jim.new.ntpl'
C      CALL FFKEY('NTPL',FNNTPL,32,'MIXED')
      FNEVTO = ' '
C      CALL FFKEY('EVTO',FNEVTO,32,'MIXED')

C .. Number of event to analize.
      NEVANA = 3
C      CALL FFKEY('NANA',NEVANA,1,'INTEGER')
      IPRINT = 0
C      CALL FFKEY('PRNT',IPRINT,1,'INTEGER')

C      CALL FFGO

      call fuevti


C================  NTUPLE definitions =========================

      IEVENT = 0      

      RETURN 
      END


c*******************************************************************
C      SUBROUTINE READJIMREAD(IUNIT, IENDI, PRODUC, 
C     >   IVERS, NGEN, NCMB, NTRK,
C     >   NEMH, NHDH, HEAD, GENDAT, IGENDAT, 
C     >   CMBT, TRKF, TRKD, EMH, HDH, 
C     >   NVTX, NPVTX, VTXD, IVTX, NRET)

      SUBROUTINE READJIMREAD(NRET)

      IMPLICIT NONE
      INTEGER*4 NRET

C     NRET = 1 FOR NORMAL RETURN,
C     NRET = -1 FOR END OF FILE
C     NRET = -2 FOR READ ERROR.

C      INTEGER*4 IUNIT,IENDI,IVERS,NGEN,NCMB,NTRK,NEMH,NHDH
C      REAL*4  HEAD(2), GENDAT(10,*),CMBT(8,*),TRKF(23,*)
C      INTEGER*2 IGENDAT(4,*)
C      REAL*8  TRKD(15,*)
C      INTEGER*4  EMH(3,*), HDH(3,*)
C      INTEGER*4 NVTX(*)
C      REAL*8   VTXD(5,*)
C      INTEGER*4 IVTX(2,*)
      
C      CHARACTER*4  PRODUC
     

#include "jlunit.inc"
#include "jlfile.inc"
#include "jlanal.inc"
#include "jmcntl.inc"
#include "trbuff.inc"
#include "pawc.inc"


      INTEGER IERR,ICYCLE

      INTEGER LNBLNK

      INTEGER*4  LBANK, IBUFF(20), NBUFF

      NRET = 1

 1000 CONTINUE 

      IEVENT = IEVENT + 1


C      WRITE(6,600) IEVENT
C 600  FORMAT(//1H ,'<< Event #: ',I5,' >>'/)
         
      CALL GETMCH

*-- Read Data from Bank MCEVENT 

      CALL GETGEN

*-- Read Track Data
      if( IGEOJM.EQ.IG2TJM) THEN
	call cthsim
	call ctanal
	call gettrk
      else
	call ctcyla
      endif
      
      CALL GETCAL(IERR)

CCCCC CALL BCOUT
CCCCC CALL ECOUT

CCCCC CALL GETCLS

      CALL VXANAL(IERR)

      CALL GETVTX

 998  continue
c      IF (NEVANA.EQ.0 .OR. 
c     &     NEVANA.GT.0 .AND. IEVENT.LT.NEVANA) GO TO 1000

 999  CONTINUE

*-- Terminate

     
cc      call readjimtest2

      RETURN
      END


C************************************************************
      subroutine readjimtest2

#include "trbuff.inc"

      integer*4 idata(100)
      real*4    rdata(100)
      equivalence (idata(1), rdata(1))

      
      print *,' ntrk,nemh,nhdh=',ntrk,nemh,nhdh,' ntrk=',ntrk

      do 100 i = 1, nemh
         print *,' i=',i,' emh=',emh(1,i),emh(2,i)
 100  continue


      return
      end

C************************************************************
      subroutine readjimtest1

#include "trbuff.inc"

      integer*4 idata(100)
      real*4    rdata(100)
      equivalence (idata(1), rdata(1))

      
      print *,' endian=',endian,' ierflg=',ierflg
      print *,' ivers=',ivers,' ngen=',ngen,' ncmb=',ncmb
      print *,' ntrk,nemh,nhdh=',ntrk,nemh,nhdh,' ntrk=',ntrk

      call kzbloc('MCCTPAR', LBANK)
      print *,' MCCTPAR ... KZBLOC =',LBANK
      call kzbloc('MCBCPAR', LBANK)
      print *,' MCBCPAR ... KZBLOC =',LBANK
      call kzbloc('MCBEHIT', LBANK)
      print *,' MCBEHIT ... KZBLOC =',LBANK


      call kzbloc('MCCTPAR',LBANK)
      print *,'kzloc(MCCTPAR) .. lbank=',lbank

      call kzget('MCBEHIT',0,ndata,ibuff)
      print *,' MCEBHIT ... ndata=',ndata

      call kzget('MCCTPAR', 0, ndata, idata)
      print *,' ndata =',ndata
      print *,' idata(1:2)=',idata(1),idata(2)

      return
      end

C************************************************************
      SUBROUTINE READJIMEND

#include "jlunit.inc"

      CALL KZEND(0)
      
      CALL MSGFIN

      CLOSE (LUNTPL)

      RETURN
      END

