NAME = writeStdHepHerwig

CXX = g++
OBJS = $(NAME).o \
	stdhwgcxx.o stdflhwxsec.o \
	hwghep.o hwtran.o dummy.o

CLHEPDIR = /usr
CXXFLAGS = -I$(CLHEPDIR)/include
FFLAGS= $(DFLG) -fno-second-underscore

LIBS = -L$(CLHEPDIR) -L/proj/soft/share/lib -lherwig6500 -lcirce -lCLHEP \
	-lg2c # -lnsl

$(NAME): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LIBS)

$(NAME).o: $(NAME).cc

clean:
	rm -f *.o *~

distclean: clean
	rm -f writeStdHepHerwig *.dat *.lpt  fort.77

