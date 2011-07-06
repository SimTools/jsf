#
# stdhep examples makefile
#

STDHEP_DIR=$(STDHEPDIR)
MCFIO_DIR=$(STDHEPDIR)
# CRNLIB=$(shell which cernlib | sed -e "s|bin/cernlib|lib|")

include $(STDHEP_DIR)/src/stdhep_arch

SRCS = hepsplit.cxx

OBJS = $(subst .cxx,.o,$(SRCS))
INCS = -I$(STDHEP_DIR)/src/inc 
CXXFLAGS = $(INCS)
LIBS= -L$(STDHEP_DIR)/lib -lstdhep -lstdhepC -lFmcfio \
	-L/opt/gcc-4.4.0/lib64 -lgfortran -lstdc++


all: hepsplit

hepsplit: $(OBJS)
	$(CC) $(OBJS) -o hepsplit $(LIBS)

.PHONy:clean
clean: 
	rm -f *.o *~

.SUFFIXES: .o .cxx
.cxx.o:
	$(CXX) $(CXXFLAGS) -c $<

