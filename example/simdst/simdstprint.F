c
c  Dump simdst file.
c

      parameter (ngensize=500)
      parameter (ntrksize=500)
      parameter (nemhsize=1000, nhdhsize=1000)
      parameter (nvtxbuf=ntrksize*100)
      integer*4 iunit, ivers, iendi, ngen ,ntrk, nemh, nhdh
      real*4  head(2), gendat(10,ngensize), trkf(23,ntrksize)
      integer*2  igendat(4,ngensize)
      real*4  cmbt(8,ntrksize)
      real*8  trkd(15,ntrksize)
      integer*4  emh(3,nemhsize), hdh(3,nhdhsize)
      integer*4 nvtx(ntrksize)
      real*8   vtxd(5,nvtxbuf)
      integer*4   ivtx(2,nvtxbuf)
      character*4 produc

      
      iunit=10
      call simdstopen(iunit, "simdst.dat")
      maxread=10000
      do 100 i=1, maxread
         
         call simdstread(iunit, iendi, produc, 
     >   ivers, ngen, ncmb, ntrk,
     >   nemh, nhdh, head, gendat, igendat,
     >   cmbt, trkf, trkd, emh, hdh, 
     >   nvtx, npvtx, vtxd, ivtx, nret)
         if( nret .lt.0 ) go to 900
         print *,' read .. ',iendi,' produc=',produc,
     >   ' ngen, ncmb, ntrk, nemh, nhdh=',
     >    ngen, ncmb, ntrk, nemh, nhdh,' npvtx=',npvtx

         print *,' head(1:2)=',head(1),head(2)
         print *,' igendat=',((igendat(kk,k),kk=1,4),k=1,min(3,ngen))
         print *,' emh=',(emh(1,k),emh(2,k),emh(3,k),k=1,min(10,nemh))
         print *,' hdh=',(hdh(1,k),hdh(2,k),hdh(3,k),k=1,min(10,nhdh))
         print *,' nvtx=',(nvtx(k),k=1,ntrk)
         print *,' ipvtx=',(ivtx(1,k),ivtx(2,k),
     >                     k=1,npvtx)
         do 120 k=1,npvtx
            print *,' vtxd=',k,(vtxd(kk,k),kk=1,5)
 120     continue


         print *,' Completed event #=',i
 100  continue

 900  continue
      call simdstclose(iunit)

      stop
      end


