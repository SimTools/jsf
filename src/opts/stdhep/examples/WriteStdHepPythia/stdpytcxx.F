      subroutine stdpytcxxinit(nevt)
C
C stupid test program for generating pythia events and looking at
C    them with c++ code
C generate pythia - convert to hepevt common block - convert to c++ class
C
C this is the initialization piece
C
#include "CLHEP/StdHep/include/stdlun.inc"
#include "pydat1.inc"

      integer nevt
      real bn
C...   PYTHIA initialization variables.
      character*4 frame
      character*5 beam,target
      double precision win
C
C initialize HEP logical units
      lnhwrt=0
      lnhrd=0
      lnhdcy=0
      lnhout=22
      mstu(11)=lnhout
      open(unit=lnhout,file='examPythiaToStdHep.lpt',status='new')
C
C      call bookhist
C
C...   Initialize PYTHIA
      frame  = 'cms'
      beam   = 'e-'
      target = 'e+'
      win    = 500.0d0
c      call pygive('msel=5')
c      call pygive('mstj(21)=0')
      call pyinit(frame,beam,target,win)

C          Fill Stdhep common block 1 (only valid for Pythia)
      call stdflpyxsec(nevt)
      return
      end

      subroutine stdpytcxx_xsec(nevt)
C          Fill Stdhep common block 1 (only valid for Pythia)
      integer nevt
      call stdflpyxsec(nevt)
      return
      end

      subroutine stdpytcxxend
C
C
#include "CLHEP/StdHep/include/stdlun.inc"

C...print histograms
C      call houtpu(lnhout)
C      call histdo
      close(unit=lnhout)
      return
      end
