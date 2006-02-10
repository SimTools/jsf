c*******************************************************************
      subroutine readgenopen(iunit, fname)
      character*(*) fname
      
      open(iunit,file=fname,form="unformatted",
     >     status="old")
      return
      end

c*******************************************************************
      subroutine OpenFortranFile(iunit, fname, formstr, statusstr)
      integer       iunit
      character*(*) fname, formstr, statusstr
      
      open(iunit, file=fname, form=formstr, status=statusstr)  

      return
      end

c*******************************************************************
      subroutine CloseFortranFile(iunit)
      integer       iunit

      close(iunit)

      return
      end

c*******************************************************************
      subroutine WriteHepevt(iunit, iendi, nvers, nevhep,
     >  nhep, isthep, idhep, jmohep, jdahep,
     >  phep, vhep)
      
      integer*4  iunit, iendi, nvers, nevhep, nhep
      integer*4  isthep(nhep), idhep(nhep)
      integer*4  jmohep(2,nhep), jdahep(2,nhep)
      real*8     phep(5,nhep), vhep(4,nhep)
     
      write(iunit) iendi, nvers, 
     >  nevhep, nhep, (isthep(k),k=1,nhep),
     > (idhep(k),k=1,nhep),
     > ((jmohep(m,k),m=1,2),k=1,nhep),
     > ((jdahep(m,k),m=1,2),k=1,nhep),
     > ((phep(m,k),m=1,5),k=1,nhep),
     > ((vhep(m,k),m=1,4),k=1,nhep)

      return
      end

c*******************************************************************
      subroutine WriteHepevtAscii(iunit, nvers, nevhep,
     >  nhep, isthep, idhep, jmohep, jdahep,
     >  phep, vhep)
      
      integer*4  iunit, nvers, nevhep, nhep
      integer*4  isthep(nhep), idhep(nhep)
      integer*4  jmohep(2,nhep), jdahep(2,nhep)
      real*8     phep(5,nhep), vhep(4,nhep)
     
      write(iunit,*) nhep, nevhep, nvers
      do 100 ihep=1, nhep 
         write(iunit,10) 
     >    isthep(ihep), idhep(ihep), 
     >    jmohep(1,ihep), jmohep(2,ihep),
     >    jdahep(1,ihep), jdahep(2,ihep),
     >    (phep(m,ihep),m=1,5),(vhep(m,ihep),m=1,4)
100   continue
10    format(6i10,9(1x,d15.8))
      return
      end

c*******************************************************************
      subroutine ReadgenHepevt(iunit, maxhep,iendi, nvers, nevhep,
     >  nhep, isthep, idhep, jmohep, jdahep,
     >  phep, vhep, nret)
c     nret = 1 for normal return,
c     nret = -1 for end of file
c     nret = -2 for read error.
      
      integer*4  iunit, maxhep,iendi, nvers, nevhep, nhep
      integer*4  isthep(maxhep), idhep(maxhep)
      integer*4  jmohep(2,maxhep), jdahep(2,maxhep)
      real*8     phep(5,maxhep), vhep(4,maxhep)

      read(iunit,end=200,err=300) iendi, nvers, 
     >  nevhep, nhep, (isthep(k),k=1,nhep),
     > (idhep(k),k=1,nhep),
     > ((jmohep(m,k),m=1,2),k=1,nhep),
     > ((jdahep(m,k),m=1,2),k=1,nhep),
     > ((phep(m,k),m=1,5),k=1,nhep),
     > ((vhep(m,k),m=1,4),k=1,nhep)

      nret=0
      return
 200  continue
      nret=-1
      return
 300  continue
      print *,'In the subroutine, readgenhepevt .. read error'
      nret = -2
      return
      end

c*******************************************************************
      subroutine ReadgenHepevtAscii(iunit, maxhep, nvers, nevhep,
     >  nhep, isthep, idhep, jmohep, jdahep,
     >  phep, vhep, nret)
c     nret = 1 for normal return,
c     nret = -1 for end of file
c     nret = -2 for read error.
      
      integer*4  iunit, maxhep,nvers, nevhep, nhep
      integer*4  isthep(maxhep), idhep(maxhep)
      integer*4  jmohep(2,maxhep), jdahep(2,maxhep)
      real*8     phep(5,maxhep), vhep(4,maxhep)


      read(iunit,*,end=200,err=300) nhep, nevhep, nvers
      do 100 ihep=1, nhep 
         read(iunit,10,end=310,err=300) 
     >    isthep(ihep), idhep(ihep), 
     >    jmohep(1,ihep), jmohep(2,ihep),
     >    jdahep(1,ihep), jdahep(2,ihep),
     >    (phep(m,ihep),m=1,5),(vhep(m,ihep),m=1,4)
100   continue
10    format(6i10,9(1x,d15.8))
      nret=0
      return
 200  continue
      nret=-1
      return
 300  continue
      print *,'In the subroutine, readgenhepevt .. read error'
      nret = -2
 310  continue
      print *,'In the subroutine, readgenhepevt .. read error'
      print *,'Got EOF while reading event data'
      nret = -2
      return
      end

c*******************************************************************
      subroutine readgenclose(iunit)
      close(iunit)
      return
      end
