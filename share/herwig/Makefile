include  ../../conf/configure.in
UNAME  = $(shell uname)

PYTHIANAME=pythia$(PYTHIA_VERSION)$(PYTHIA_SUBVERSION)

ifeq ($(UNAME),AIX)
CXX           = xlC
##  FFLAGS    = -C -g -qcharlen=32767 -qstrict -qarch=pwr3 
FFLAGS    = -C -g -qcharlen=32767 \
	  -qextname -qfixed -qsave -qinitauto -brtl -bshared
CFLAGS    = -O $(shell root-config --cflags)
CXXFLAGS      = $(EXTRACXXOPT) -DAIX \
	  -I$(JSFROOT)/include $(shell root-config --cflags) \
	  -I/usr/vacpp/include -brtl -bshared
LD        =  /usr/vacpp/bin/makeC++SharedLib -brtl -bshared -p 0
LDFLAGS   = -shared -rdynamic \
	-L$(JSFROOT)/lib -lJSF -lJSFQuickSim -lJSFBeamGeneration \
	$(shell root-config --libs) -lEG -lxlf90 \
	-Wl,-bnoquiet
SharedLib = libTHerwig.a
INSTALL   = cp -fp
else
ifeq ($(UNAME),Darwin)
FFLAGS    = -O0 -g -fno-second-underscore
CFLAGS    =
LD        = $(FC)
LDFLAGS   = -r -flat_namespace -nostdlib -undefined suppress
SharedLib = libTHerwig.a
INSTALL   = install -c
else
FFLAGS    = -O2 -g -fPIC -fno-second-underscore
CFLAGS    = -fPIC $(shell root-config --cflags)
CXXFLAGS    = -Wall -fPIC $(shell root-config --cflags)
LD        = $(FC)
LDFLAGS   = -shared -rdynamic -Wl,-soname,$(SharedLib) \
		$(shell root-config --libs)
#		$(shell root-config --libs) -lEG
SharedLib = libTHerwig.so
INSTALL   = install -c
endif
endif

DEPENDFLAGS = $(CXXFLAGS)
SrcSuf = cxx
ObjSuf = o

.SUFFIXES: .$(SrcSuf)

.$(SrcSuf).$(ObjSuf):
	$(CXX) $(CXXFLAGS) -c $<


HDRS = THerwig.h HerwigGenerator.h

SRCS = THerwig.$(SrcSuf) HerwigGenerator.$(SrcSuf)


OBJS = herwig6500.$(ObjSuf) circe.$(ObjSuf) \
	herwig_common_block_address.$(ObjSuf) \
	herwig_address.$(ObjSuf)

# herwig65_test.$(ObjSuf) 	herwig_util.$(ObjSuf) 


OBJS += THerwig.$(ObjSuf) HerwigGenerator.$(ObjSuf) libTHerwig.$(ObjSuf)

CLEANOBJS = $(subst herwig6500.o, ,$(OBJS))

CXXFLAGS += -I./ -I$(JSFROOT)/include

DEPENDS = $(DEPENDFLAGS) -I/usr/lib/gcc-lib/i386-redhat-linux/2.96/include

all: HERWIG65.INC herwig65_test.f install

HERWIG65.INC:
	ln -s herwig6500.inc HERWIG65.INC

herwig65_test.f:
	ln -s herwig65.tst herwig65_test.f

lib: $(OBJS) 
	@rm -f $(SharedLib)
	$(LD) $(OBJS) -o $(SharedLib) $(LDFLAGS) 

install: lib
	install $(SharedLib) $(JSFROOT)/lib

testrun: testmain.o $(SharedLib)
	g77 -o testmain  testmain.o -L$(JSFROOT)/lib -lTHerwig
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@echo "%  Herwig test program will start                   %"
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@date
	@./testmain
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@echo "%  Herwiwg test program has completed               %"
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@date

cleanall:
	rm -f *.o core *~ a.out testmain
	rm -f herwig65_test.f HERWIG65.INC $(SharedLib)
	rm -f fort.77 fort.88

clean:
	rm -f $(CLEANOBJS) libTHerwig.*

distclean: clean
	rm -f testmain libTHerwig.so


libTHerwig.cxx: $(HDRS) THerwigLinkDef.h
	rootcint -f $@ -c -I$(JSFROOT)/include \
	$(HDRS) THerwigLinkDef.h

depend:: $(SRCS) $(HDRS)
	rm -f makefile.depend
	touch makefile.depend
	for i in $(SRCS) ; do \
	( rmkdepend -f- -- $(DEPENDS) -- $$i >> makefile.depend ) ; done

web:
	jsf -b -q html.C

include makefile.depend



