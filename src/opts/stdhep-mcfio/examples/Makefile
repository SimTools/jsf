#
# stdhep examples makefile
#
include ../arch_mcfio

MCFIO_DIR = ../mcfio
INCS = -I$(MCFIO_DIR) -I../stdhep
CFLAGS += -I$(MCFIO_DIR)

OBJS = readPythia.o

#------------------------------------------

all: readPythia

readPythia: $(OBJS) 
	$(F77) $(FFLAGS) -o readPythia $(OBJS) \
	-L$(JSFROOT)/lib -lFmcfio -lStdHepMCFIO

install:

clean:
	rm -f *.o *.f *.map
	rm readPythia

#------------------------------------------

.SUFFIXES : 			# cancel implicit rules
.SUFFIXES : .o .c .F .inc .h

%.o: %.F
	$(F77) $(INCS) $(FFLAGS) -c $*.F

%.o: %.c
	$(CC) $(CFLAGS) -c $<

