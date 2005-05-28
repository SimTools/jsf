#
# make procedure for mcfio  - use gmake
# This makefile builds ntuBrowser with histoscope if $HISTO_DIR is defined.
#
# note that this latest version of mcfio only works with stdhep 3.0 or later
#
UNAME := $(shell uname)

LIBDIR = $(JSFROOT)/lib

include ../arch_mcfio
 
FINC = -I.
CINC = -I.
HLIB =

CL_F_SRC    = mcfio_FPrintDictionary \
               mcf_evt_xdr mcfio_FBinding mcfio_Util1 \
               mcfio_Direct mcfio_SeqDummy mcfio_Block \
               mcf_ntubldInit mcf_ntuBldDbinc \
               mcf_NTuIOFiles  mcf_NTuIOUtils \
	       mcfio_UserDictionary
CL_F_OBJS   = $(addsuffix .o,$(CL_F_SRC))

#--------------------------

all: $(LIBDIR)/libFmcfio.a

$(LIBDIR)/libFmcfio.a: $(CL_F_OBJS) 
	$(AR) -urs $(LIBDIR)/libFmcfio.a $(CL_F_OBJS) 

install: $(LIBDIR)/libFmcfio.a 
	
cleanobjs:
	rm -f *.o 
	       
clean:
	rm -f *.o 
	rm -f $(LIBDIR)/libFmcfio.a 

glib:
	@echo "libraries are debug by default"

#--------------------------

%.o: %.F
	$(F77) -c -o $*.o -DUNIX $(FINC) $(FFLAGS) $<

%_g.o: %.F
	$(F77) -c -o $*_g.o -DUNIX $(FINC) $(FFLAGS) $(DFLG) $<

%.o: %.c
	$(CC) -c -o $*.o $(CFLAGS) $(CINC) $*.c

%_g.o: %.c
	$(CC) -c -o $*_g.o $(CFLAGS) $(DFLG) $(CINC) $*.c

#--------------------------

.PHONY: clean cleanobjs glib

.SUFFIXES : 		# cancel implicit rules
.SUFFIXES : .o .c .F

#--------------------------


# DO NOT DELETE THIS LINE -- make depend depends on it.

mcf_evt_xdr.o: mcf_xdr.h
mcfio_Direct.o: mcf_xdr.h mcfio_Dict.h mcfio_Util1.h mcfio_Direct.h
mcfio_SeqDummy.o:mcfio_Sequential.h
mcfio_Util1.o: mcf_xdr.h mcfio_Dict.h mcfio_Util1.h mcfio_Direct.h \
                mcfio_Sequential.h
mcfio_block.o: mcf_xdr.h mcfio_Dict.h mcfio_Util1.h mcfio_Block.h              
mcf_evt_xdr_g.o: mcf_xdr.h
mcfio_Direct_g.o: mcf_xdr.h mcfio_Dict.h mcfio_Util1.h mcfio_Direct.h
mcfio_SeqDummy_g.o:mcfio_Sequential.h
mcfio_Util1_g.o: mcf_xdr.h mcfio_Dict.h mcfio_Util1.h mcfio_Direct.h \
                mcfio_Sequential.h
mcfio_block_g.o: mcf_xdr.h mcfio_Dict.h mcfio_Util1.h mcfio_Block.h              
