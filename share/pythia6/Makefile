

include  ../../conf/configure.in
UNAME  = $(shell uname)

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
FFLAGS    = -O2 -g -fPIC -fno-second-underscore
CFLAGS    = -fPIC
LD        = $(FC)
LDFLAGS   = -shared -rdynamic -Wl,-soname,$(SharedLib)
SharedLib = libPythia6.so
INSTALL   = install
endif


OBJS = $(PYTHIANAME).o \
	tpythia6_called_from_cc.o \
	pythia6_common_block_address.o \
	pythia6_address.o \
	main.o 

all: lib install testrun

lib: $(OBJS)
	@rm -f $(SharedLib)
	$(LD) $(LDFLAGS) -o $(SharedLib) $(OBJS)

install: $(SharedLib)
	$(INSTALL) $(SharedLib) ../../lib/$(SharedLib)
	mkdir -p ../../include
	$(INSTALL) pythia6.inc ../../include/pythia6.inc

testrun: pythia_test $(SharedLib)
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@echo "%  Pythia test program will start                   %"
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@date	
	@pythia_test
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@echo "%  Pythia test program has completed                %"
	@echo "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%"
	@date

pythia_test: pythia_test.f $(SharedLib)
	$(FC) -o pythia_test pythia_test.f $(FFLAGS)  -L. -lPythia6

clean:
	rm -f *.o core *~

distclean: clean
	rm -f pythia_test libPythia6.so

