C Utility programs to use it from C++

      subroutine setbeams(pname1, pname2, pb1, pb2, ipr)
      include  'HERWIG65.INC'
      character*(*) pname1, pname2
      double precision pb1, pb2
      integer ipr

      part1=pname1
      part2=pname2
      pbeam1=pb1
      pbeam2=pb2
      iproc=ipr

      return
      end


      subroutine myanal
      include 'HERWIG65.INC'
      print *,'**************'
      print *,' maxpr=',maxpr
      return
      end
