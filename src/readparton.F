c*******************************************************************
      subroutine readpartonopen(iunit, itype, fname)
      character*(*) fname
      
      if( itype .eq. 0 ) then
        open(iunit,file=fname,form="unformatted",
     >     status="old")
      else
        open(iunit,file=fname,form="formatted",
     >     status="old")
      endif

      return
      end

c*******************************************************************
      subroutine writeparton(iunit, itype, iendi, nvers, 
     >  nevent, npart, idat, rdat )
      
      integer*4  itype
      integer*4  iunit, iendi, nvers, nevent, npart
      integer*4  idat(7,npart)
      real*4     rdat(6,npart)

      if( itype .eq. 0 ) then
         write(iunit) iendi, nvers, nevent, npart,
     >       ((idat(i,j),i=1,7),j=1,npart),
     >       ((rdat(i,j),i=1,6),j=1,npart)
      else
         write(iunit,*) nvers, nevent, npart
         do 200 j=1, npart
            write(iunit,*) (idat(i,j),i=1,7),(rdat(k,j),k=1,6)
 200     continue
      endif

      return
      end

c*******************************************************************
      subroutine readparton(iunit, itype, iendi, nvers,
     >  nevent, npart, idat, rdat, nret )

c     nret = 1 for normal return,
c     nret = -1 for end of file
c     nret = -2 for read error.
      
      integer*4  itype
      integer*4  iunit, iendi, nvers, nevent, npart
      integer*4  idat(7,npart)
      real*4     rdat(6,npart)
      character*256 input

      if( itype .eq. 0 ) then
         read(iunit,end=300,err=900) 
     >       iendi, nvers, nevent, npart,
     >       ((idat(i,j),i=1,7),j=1,npart),
     >       ((rdat(i,j),i=1,6),j=1,npart)
      else
 200     continue
         read(iunit,'(A)',end=300,err=900) input
         if(input(1:1).eq.'#') go to 200
         read(input,*)  nvers, nevent, npart
         do 250 j=1, npart
 220        continue
            read(iunit,'(A)',end=300,err=900) input
            if( input.eq.'#') go to 220
            read(input,*)
     >      (idat(i,j),i=1,7),(rdat(k,j),k=1,6)
 250     continue
      endif

      nret=1
      return

 300  nret=-1
      return

 920  continue
      print *,'Error in ReadParton .. ',
     > 'Got end of file in the middle of event.'
      nret=-2
      return

 900  continue
      print *,'Error in ReadParton ..',
     > 'Read error detected.'
      nret=-2
      return
      end

c*******************************************************************
      subroutine readpartonclose(iunit)
      close(iunit)
      return
      end
