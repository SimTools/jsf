
      subroutine jsf_mcfio_init()
#include "stdlun.inc"
      lnhwrt=0
      lnhrd =23
      lnhdcy=0
      lnhout=6
      lnhpdf=0
      lnhdmp=0
     
      call mcfio_init()

      return
      end

