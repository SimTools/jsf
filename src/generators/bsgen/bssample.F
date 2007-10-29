c
c
c
      program bssample
      implicit real*8 (a-h,o-z)
      common /pawc/ h(80000)

      call hlimit(80000)

      call hbook1(1,"E(e-) w weight", 100, 10.0, 260.0, 0.0)
      call hbook1(2,"Root-S w weight", 100, 10.0, 510.0, 0.0)	

      call hbook1(11,"E(e-)", 100, 10.0, 260.0, 0.0)
      call hbook1(12,"Root-S", 100, 10.0, 510.0, 0.0)	

      eb0=250.0
      itype=0
      width=0.0005
      call jsfbeaminit1("500_nominal.root\00",
     >  "500_nominal\00", itype, eb0, width)

C
C  Loop to generate beam energies for given random numbers.
C
      do 100 i=1, 10000
         xm=rndm()
         xp=rndm()
         xbsm=rndm()
         xbsp=rndm()

         call jsfbeamgend(xm, xp, xbsm, xbsp, eb0, 
     >    em, ep, em0, ep0, weight)

         if( i .lt. 10 ) then
            print *,'em=',em,' ep=',ep
            print *,'em0=',em0,' ep0=',ep0
            print *,'weight=',weight
         endif

      ecm=2*sqrt(em*ep)
      call hfill(1, real(em), 0.0, real(weight))
      call hfill(2, real(ecm), 0.0, real(weight)) 

 100  continue

C
C  Loop to generate events with equal weight (1)
C
      do 200 i=1, 10000
        call jsfbeamgen0(em, ep, em0, ep0)
        ecm=2*sqrt(em*ep)
        if( i < 20 ) then
           print *,"Ev",i," xem, xep, ecm=",em, ep, ecm
        endif
        call hfill(11, real(em), 0.0, 1.0)
        call hfill(12, real(ep), 0.0, 1.0)
 200  continue


      call histdo
      call hrput(0, 'sample.hbook','N')

      stop
      end
 
