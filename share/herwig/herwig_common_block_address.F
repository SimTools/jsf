      subroutine hwaend
      print *,'Dummy HWAEND routine was called'
      return
      end


      integer function herwig_common_block_address(common_block_name)
c-----------------------------------------------------------------------
c (Function)
c  Get address of common block address  
c  follow method used in pythia6_common_block_address
c
c (Author)
c  Akiya Miyamoto   12-Oct-2002
c
c-----------------------------------------------------------------------
#include "HERWIG65.INC"
c
      character*(*) common_block_name
      external      hwudat
c
c-----------------------------------------------------------------------
      integer       herwig_addressc, herwig_addressi, herwig_addressf
      integer       herwig_addressd
      integer       common_block_address
c-----------------------------------------------------------------------
      common_block_address = 0
c-----------------------------------------------------------------------
c  HERWIG common-blocks
c-----------------------------------------------------------------------
      if     (common_block_name.eq."HEPEVT") then
        common_block_address = herwig_addressi(NEVHEP)

      else if (common_block_name.eq."HWEVNT") then  
        common_block_address = herwig_addressd(AVWGT)

      else if (common_block_name.eq."HWBEAM") then  
        common_block_address = herwig_addressi(IPART1)
      else if (common_block_name.eq."HWBMCH") then  
        common_block_address = herwig_addressc(PART1)
      else if (common_block_name.eq."HWPROC") then  
        common_block_address = herwig_addressd(EBEAM1)
      else if (common_block_name.eq."HWPRAM") then  
        common_block_address = herwig_addressd(AFCH)
      else if (common_block_name.eq."HWPRCH") then  
        common_block_address = herwig_addressc(AUTPDF)
      else if (common_block_name.eq."HWPART") then  
        common_block_address = herwig_addressi(NEVPAR)
      else if (common_block_name.eq."HWPARP") then  
        common_block_address = herwig_addressd(DECPAR)
      else if (common_block_name.eq."HWBOSC") then  
        common_block_address = herwig_addressd(ALPFAC)
      else if (common_block_name.eq."HWPARC") then  
        common_block_address = herwig_addressi(JCOPAR)
      else if (common_block_name.eq."HWBRCH") then  
        common_block_address = herwig_addressd(ANOMSC)

      else if (common_block_name.eq."HWPROP") then  
        common_block_address = herwig_addressd(RLTIM)
      else if (common_block_name.eq."HWUNAM") then  
        common_block_address = herwig_addressc(RNAME)
      else if (common_block_name.eq."HWUSUD") then  
        common_block_address = herwig_addressd(ACCUR)
      else if (common_block_name.eq."HWMINB") then  
        common_block_address = herwig_addressd(PMBN1)
      else if (common_block_name.eq."HWDIST") then  
        common_block_address = herwig_addressd(EXAG)
      else if (common_block_name.eq."HWUCLU") then  
        common_block_address = herwig_addressd(CLDKWT)
      else if (common_block_name.eq."HWUWTS") then  
        common_block_address = herwig_addressd(REPWT)
      else if (common_block_name.eq."HW6202") then  
        common_block_address = herwig_addressd(VIPWID)
      else if (common_block_name.eq."HWHARD") then  
        common_block_address = herwig_addressd(ASFIXD)

      endif
c
      herwig_common_block_address = common_block_address
      
      return
      end
