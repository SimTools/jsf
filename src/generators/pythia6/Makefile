include  ../../../conf/configure.in
UNAME  = $(shell uname)

PYTHIA_VERSION=6
PYTHIA_SUBVERSION=319
PYTHIANAME=pythia$(PYTHIA_VERSION)$(PYTHIA_SUBVERSION)

ifeq ($(UNAME),AIX)
FFLAGS    = -C -g -qcharlen=32767 -qstrict -qarch=pwr3 \
	  -qextname -qfixed -qsave -qinitauto -brtl -bshared
CFLAGS    = -O
LD        =  /usr/vacpp/bin/makeC++SharedLib -brtl -bshared -p 0
LDFLAGS   = -lxlf90
SharedLib = libPythia6.a
INSTALL   = cp -fp
else
ifeq ($(UNAME),Darwin)
FC        = gfortran
FFLAGS    = -O0 -g -fno-second-underscore
CFLAGS    =
LD        = $(FC)
LDFLAGS   = -dynamiclib -flat_namespace -single_module -undefined dynamic_lookup
SharedLib = libPythia6.dylib
BUNDLE    = $(subst .dylib,.so,$(SharedLib))
INSTALL   = install -c -p
else
FFLAGS    = -O2 -g -fPIC -fno-second-underscore
CFLAGS    = -fPIC
LD        = $(FC)
LDFLAGS   = -shared -rdynamic -Wl,-soname,$(SharedLib)
SharedLib = libPythia6.so
INSTALL   = install -c -p
endif
endif

-include srclist
OBJS += tpythia6_called_from_cc.o \
	pythia6_common_block_address.o \
	pythia6_common_address.o \
	pythia6_address.o
ifneq ($(UNAME),Darwin)
OBJS +=  main.o
else
OBJS +=  idate.o pydatawrapper.o
endif

# ROOTLIBS = -L$(ROOTSYS)/lib -lEG -lEGPythia6

all: lib testrun 

srclist: $(PYTHIANAME).f
	./fsplit $(PYTHIANAME).f > srclist

lib: srclist $(OBJS)
	@rm -f $(SharedLib)
	$(LD) $(LDFLAGS) -o $(SharedLib) $(OBJS) 
ifeq ($(UNAME),Darwin)
	$(LD) -bundle -flat_namespace -undefined dynamic_lookup -o $(BUNDLE) $(OBJS)
endif

../../../lib/$(SharedLib) : install

install: $(SharedLib)
	mkdir -p ../../../lib
	$(INSTALL) $(SharedLib) ../../../lib/$(SharedLib)
	mkdir -p ../../../include
	$(INSTALL) -m 644 pythia6.inc ../../../include/pythia6.inc
ifeq ($(UNAME),Darwin)
	$(INSTALL) $(BUNDLE) ../../../lib/$(BUNDLE)
endif

uninstall:
	rm -f ../../../lib/$(SharedLib)
	rm -f ../../../lib/$(BUNDLE)
	rm -f ../../../include/pythia6.inc

testrun: pythia_test ../../../lib/$(SharedLib)
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@echo "%  Pythia test program will start                   %"
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@date
	@./pythia_test
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@echo "%  Pythia test program has completed                %"
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@date

pythia_test: pythia_test.f ../../../lib/$(SharedLib)
	$(FC) -o pythia_test pythia_test.f $(FFLAGS)  -L../../../lib -lPythia6

clean:
	rm -f *.o core *~
	rm -f $(SRCS) srclist

distclean: clean
	rm -f pythia_test libPythia6.so libPythia6.dylib

ifeq ($(UNAME),Darwin)
PYSIGH.o: PYSIGH.f
	$(FC) $(subst -O0,-O,$(FFLAGS)) -c $< -o $@

PYSIGH.f: srclist
endif
