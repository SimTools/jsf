
include $(LCBASEDIR)/conf/common.gmk

SharedLib = libTHerwig.$(DllSuf)

HDRS = THerwig.h HerwigGenerator.h

SRCS = THerwig.$(SrcSuf) HerwigGenerator.$(SrcSuf)

OBJS = herwig6500.$(ObjSuf) circe.$(ObjSuf) \
	herwig_common_block_address.$(ObjSuf) \
	herwig_address.$(ObjSuf)

ALLHDRS = $(HDRS) THerwigCommons.h THerwigMacros.h circe.h HERWIG65.INC

# herwig65_test.$(ObjSuf) 	herwig_util.$(ObjSuf) 


OBJS += THerwig.$(ObjSuf) HerwigGenerator.$(ObjSuf) libTHerwig.$(ObjSuf)

CLEANOBJS = $(subst herwig6500.o, ,$(OBJS))

DEPENDS = $(DEPENDFLAGS) $(shell jsf-config --includes) \
	$(shell cxxdepends)

all: depend HERWIG65.INC herwig65_test.f lib

HERWIG65.INC:
	ln -s herwig6500.inc HERWIG65.INC

herwig65_test.f:
	ln -s herwig65.tst herwig65_test.f

lib: $(OBJS) 
	@rm -f $(SharedLib)
	$(LD) $(OBJS) -o $(SharedLib) $(LDFLAGS) 

install: lib
	$(INSTALL) -m 755 $(SharedLib) $(JSFLIBDIR)
	$(INSTALL) -m 644 $(ALLHDRS) $(JSFINCDIR)

uninstall:
	rm -f $(JSFLIBDIR)/$(SharedLib)
	( cd $(JSFINCDIR) ; rm -f $(ALLHDRS) )

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
	rm -f $(CLEANOBJS) libTHerwig.* *.o

distclean: clean uninstall
	rm -f testmain libTHerwig.so
	rm -f HERWIG65.INC herwig65_test.f

libTHerwig.cxx: $(HDRS) THerwigLinkDef.h
	rootcint -f $@ -c $(shell jsf-config --includes) \
	$(HDRS) THerwigLinkDef.h

depend:: $(SRCS) $(HDRS)
	rm -f makefile.depend
	touch makefile.depend
	for i in $(SRCS) ; do \
	( rmkdepend -f- -- $(DEPENDS) -- $$i >> makefile.depend ) ; done

web:
	jsf -b -q html.C

include makefile.depend



