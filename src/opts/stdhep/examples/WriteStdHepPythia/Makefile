CXX = g++
OBJS = writeStdHepPythia.o \
	stdpytcxx.o stdflpyxsec.o \
	lunhep.o lutran.o dummy.o

CLHEPDIR = /usr
CXXFLAGS = -I$(CLHEPDIR)/include
FFLAGS= $(DFLG) -fno-second-underscore
FFLAGS += -I/proj/soft/share/include/pythia

LIBS = -L$(CLHEPDIR) -L/proj/soft/share/lib -lpythia6222 -lCLHEP \
	-lg2c #-lnsl

writeStdHepPythia: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS)

writeStdHepPythia.o: writeStdHepPythia.cc

clean:
	rm -f $(OBJS) *~

distclean:
	rm -f writeStdHepPythia *.dat *.lpt

