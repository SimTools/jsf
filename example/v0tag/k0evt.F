C*******************************************************************
C*
C*  Sample program to create a hepevt data to be read 
C*  using JSFReadGenerator class.
C*  Sample data is k0 event
C*
C*(Author)
C*  31-Jan-2000 Akiya Miyamoto  Original version.
C*
C*******************************************************************

#include "jetset74/pysubs.inc"
#include "jetset74/hepevt.inc"

      external pydata
      external ludata

      NEV=5000

      iu=1
      open(iu,file="k0evt.dat",form="unformatted",
     >     status="unknown")

c  kf=310 : k0_s
      kf=310
      pi=acos(-1.0)

      do 100 i = 1, nev

         pe=5.0*rlu(idummy)+0.5
         the=pi*(0.5*rlu(idummy)+0.25)
         phi=2.0*pi*rlu(idummy)
         call lu1ent(0, kf, pe, the, phi)
         call luexec
c         call lulist(1)
         call luhepc(1)
         iendi=1296651082
         nvers=1

         nevhep=i
c         print *,' i=',i,' nevhep=',nevhep,nhep
c         do 200 k=1, nhep
c            print *,' k=',k,'isthep,idhep=',isthep(k), idhep(k), 
c     >      ' jmohep(1:2,k)=',jmohep(1,k),jmohep(2,k)
c 200     continue

         call writehepevt(iu,iendi, nvers, nevhep,
     >    nhep, isthep, idhep, jmohep, jdahep,
     >    phep, vhep)
      
 100  continue

      close(iu)
c      call pystat(1)
      stop
      end

