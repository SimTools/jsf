      integer function pythia6_common_block_address(common_block_name)
c-----------------------------------------------------------------------
c  revision history:
c  -----------------
c *0001 Jun 29 1998 P.Murat: created
c *0002 Apr 30 1999 P.Murat: force loading in the BLOCK DATA
c-----------------------------------------------------------------------
#include "pythia6.inc"
c#include "stdhep.inc"
c
      character*(*) common_block_name
      external      pydata
c
c-----------------------------------------------------------------------
      integer       adrressc, pythia6_addressi, pythia6_addressf
      integer       pythia6_addressd
      integer       common_block_address
c-----------------------------------------------------------------------
      common_block_address = 0
c-----------------------------------------------------------------------
c  GEANT common-blocks
c-----------------------------------------------------------------------
      if     (common_block_name.eq."HEPEVT") then
        common_block_address = pythia6_addressi(NEVHEP)
      elseif (common_block_name.eq."PYJETS") then
        common_block_address = pythia6_addressi(N)
      elseif (common_block_name.eq."PYDAT1") then
        common_block_address = pythia6_addressi(MSTU)
      elseif (common_block_name.eq."PYDAT2") then
        common_block_address = pythia6_addressi(KCHG)
      elseif (common_block_name.eq."PYDAT3") then
        common_block_address = pythia6_addressi(MDCY)
      elseif (common_block_name.eq."PYDAT4") then
        common_block_address = pythia6_addressc(CHAF)
      elseif (common_block_name.eq."PYDATR") then
        common_block_address = pythia6_addressi(MRPY)
      elseif (common_block_name.eq."PYSUBS") then
        common_block_address = pythia6_addressi(MSEL)
      elseif (common_block_name.eq."PYPARS") then
        common_block_address = pythia6_addressi(MSTP)
      elseif (common_block_name.eq."PYINT1") then
        common_block_address = pythia6_addressi(MINT)
      elseif (common_block_name.eq."PYINT2") then
        common_block_address = pythia6_addressi(ISET)
      elseif (common_block_name.eq."PYINT3") then
        common_block_address = pythia6_addressd(XSFX)
      elseif (common_block_name.eq."PYINT4") then
        common_block_address = pythia6_addressi(MWID)
      elseif (common_block_name.eq."PYINT5" ) then
        common_block_address = pythia6_addressi(NGENPD)
      elseif (common_block_name.eq."PYINT6" ) then
        common_block_address = pythia6_addressc(PROC)
      elseif (common_block_name.eq."PYINT7") then
        common_block_address = pythia6_addressd(SIGT)
      elseif (common_block_name.eq."PYINT8") then
        common_block_address = pythia6_addressd(XPVMD)
      elseif (common_block_name.eq."PYINT9") then
        common_block_address = pythia6_addressd(VXPVMD)
      elseif (common_block_name.eq."PYUPPR") then
        common_block_address = pythia6_addressi(NUP)
      elseif (common_block_name.eq."PYMSSM") then
        common_block_address = pythia6_addressi(IMSS)
      elseif (common_block_name.eq."PYSSMT") then
        common_block_address = pythia6_addressd(ZMIX)
      elseif (common_block_name.eq."PYINTS") then
        common_block_address = pythia6_addressd(XXM(1))
      elseif (common_block_name.eq."PYBINS") then
        common_block_address = pythia6_addressi(IHIST(1))
      endif
c
      pythia6_common_block_address = common_block_address
      end
