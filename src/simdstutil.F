c*******************************************************************
      subroutine simdstopen(iunit, fname)
      character*(*) fname
      
      open(iunit,file=fname,form="unformatted",
     >     status="unknown")
      return
      end

c*******************************************************************
      subroutine simdstwrite(iunit, iendi, produc, 
     >   ivers, ngen, ncmb, ntrk,
     >   nemh, nhdh, head, gendat, igendat, cmbt, trkf, trkd, emh, hdh, 
     >   nvtx, npvtx, vtxd, ivtx )

      integer*4 iunit,iendi,ivers,ngen,ncmb,ntrk, nemh, nhdh
      real*4  head(2), gendat(10,ngen), cmbt(8,ncmb), trkf(23,ntrk)
      integer*2 igendat(4,ngen)
      real*8  trkd(15,ntrk)
      integer*4  emh(3,nemh), hdh(3,nhdh)
      integer*4 nvtx(ntrk)
      real*8   vtxd(5,npvtx)
      integer*4 ivtx(2,npvtx)

      character*4  produc
     
      write(iunit) iendi, produc, ivers, ngen, ncmb,
     >  ntrk, nemh, nhdh, (head(i),i=1,2),
     >  ((gendat(kg,ig),kg=1,10),ig=1,ngen),
     >  ((igendat(kg,ig),kg=1,4),ig=1,ngen),
     >  ((cmbt(kt,it),kt=1,8),it=1,ncmb),
     >  ((trkf(kt,it),kt=1,23),it=1,ntrk),
     >  ((trkd(kd,id),kd=1,15),id=1,ntrk),
     >  ((emh(ke,ie),ke=1,3),ie=1,nemh),
     >  ((hdh(kh,ih),kh=1,3),ih=1,nhdh),
     >  (nvtx(nv),nv=1,ntrk),
     >  npvtx, 
     >  ((vtxd(lv,iv),lv=1,5),iv=1,npvtx),
     >  ((ivtx(lv,iv),lv=1,2),iv=1,npvtx)


      return
      end


c*******************************************************************
      subroutine simdstread(iunit, iendi, produc, 
     >   ivers, ngen, ncmb, ntrk,
     >   nemh, nhdh, head, gendat, igendat, 
     >   cmbt, trkf, trkd, emh, hdh, 
     >   nvtx, npvtx, vtxd, ivtx, nret)

c     nret = 1 for normal return,
c     nret = -1 for end of file
c     nret = -2 for read error.

      integer*4 iunit,iendi,ivers,ngen,ncmb,ntrk,nemh,nhdh
      real*4  head(2), gendat(10,*),cmbt(8,*),trkf(23,*)
      integer*2 igendat(4,*)
      real*8  trkd(15,*)
      integer*4  emh(3,*), hdh(3,*)
      integer*4 nvtx(*)
      real*8   vtxd(5,*)
      integer*4 ivtx(2,*)
      
      character*4  produc
     
      nret=1
      if(nret .eq.10 ) then
        read(iunit,end=200,err=300) iendi,produc, ivers,ngen,ncmb
        print *,' iendi,produc=',iendi,produc,ivers,ngen,ncmb
      else
        read(iunit,end=200,err=300) iendi, produc, ivers, ngen, 
     >  ncmb, ntrk, nemh, nhdh, 
     >  (head(i),i=1,2),
     >  ((gendat(kg,ig),kg=1,10),ig=1,ngen),
     >  ((igendat(kg,ig),kg=1,4),ig=1,ngen),
     >  ((cmbt(kt,it),kt=1,8),it=1,ncmb),
     >  ((trkf(kt,it),kt=1,23),it=1,ntrk),
     >  ((trkd(kd,id),kd=1,15),id=1,ntrk),
     >  ((emh(ke,ie),ke=1,3),ie=1,nemh),
     >  ((hdh(kh,ih),kh=1,3),ih=1,nhdh),
     >  (nvtx(nv),nv=1,ntrk),
     >  npvtx, 
     >  ((vtxd(lv,iv),lv=1,5),iv=1,npvtx),
     >  ((ivtx(lv,iv),lv=1,2),iv=1,npvtx)

      endif

      return
 200  continue
      nret=-1
      return
 300  continue
      print *,'In SIMDSTREAD .. read error'
      nret = -2
      return
      end

c*******************************************************************
      subroutine simdstclose(iunit)
      close(iunit)
      return
      end
