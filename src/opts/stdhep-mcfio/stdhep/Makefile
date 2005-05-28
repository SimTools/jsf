#
# stdhep examples makefile
#
include ../arch_mcfio


NAME      = StdHepMCFIO
LIBNAME   = $(JSFROOT)/lib/lib$(NAME).a

MCFIO_DIR = ../mcfio
INCS = -I$(MCFIO_DIR)
CFLAGS += -I$(MCFIO_DIR)

OBJS  = stdxropen.o stdxend.o  stdxrd.o stdversn.o \
	stdzero.o stdtcopy.o \
	mcf_Stdhep_xdr.o  mcf_hepev4_xdr.o  mcf_stdcm1_xdr.o \
	mcf_hepup_xdr.o

#------------------------------------------

all: $(LIBNAME)

$(LIBNAME): $(OBJS)
	$(AR) -urs $(LIBNAME) $(OBJS)

install: $(LIBNAME)

cleanobjs:
	rm -f *.o *.f *.map

clean: cleanobjs
	rm -f $(LIBNAME)

#------------------------------------------

.SUFFIXES : 			# cancel implicit rules
.SUFFIXES : .o .c .F .inc .h

%.o: %.F
	$(F77) $(INCS) $(FFLAGS) -c $*.F

%.o: %.c
	$(CC) $(CFLAGS) -c $<

