C*******************************************************************
C*
C*  Sample program to create a hepevt data to be read 
C*  using JSFReadGenerator class.
C*  Sample data is e+e- -> qqbar by Pythia.
C*
C*(Author)
C*  7-April-1999 Akiya Miyamoto  Original version.
C*
C*******************************************************************

#include "jetset74/pysubs.inc"
#include "jetset74/hepevt.inc"

      external pydata
      external ludata

      NEV=10
      msel=1

      call pyinit('CMS', 'e-', 'e+', 300.0)
cc      call pystat(2)

      iu=1
      open(iu,file="genevent.dat",form="unformatted",
     >     status="unknown")

      do 100 i = 1, nev
         call pyevnt
         call luhepc(1)
         print *,' nevhep, nhep=',nevhep, nhep

         iendi=1296651082
         nvers=1

         call writehepevt(iu,iendi, nvers, nevhep,
     >    nhep, isthep, idhep, jmohep, jdahep,
     >    phep, vhep)
      
 100  continue

      close(iu)
      call pystat(1)
      stop
      end

