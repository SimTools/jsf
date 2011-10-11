
include $(LCBASEDIR)/conf/common.gmk


LIBNAME = libJSF2LCIO

SharedLib = $(LIBNAME).$(DllSuf)

SRC     = JSF2LCIO.$(SrcSuf)

OBJS    = $(subst .$(SrcSuf),.$(ObjSuf),$(SRC)) \
	  $(LIBNAME).$(ObjSuf)

HDRS    = $(subst .$(SrcSuf),.h,$(SRC))

ifdef CLHEP_BASE_DIR
LIBS	+= -L$(CLHEP_BASE_DIR)
endif
LIBS    += -lCLHEP

ifdef CLHEP_INCLUDE_DIR
INCPATH  += -I$(CLHEP_INCLUDE_DIR)
endif

INCPATH  += -I$(LCIO)/include 
CXXFLAGS += -ansi -Wall -O3

LCIOLIBS = $(shell jsf-config --lciolib)


all: depend $(LIBNAME).$(DllSuf)

$(SharedLib): $(OBJS) 
	@rm -f $(SharedLib)
	$(LD) $(OBJS) -o $(SharedLib) $(SOFLAGS) $(LCIOLIBS)

depend: $(SRC) $(HDR)
	for i in $(SRC); do \
	( rmkdepend -f- -a -- $(RMKDEPENDFILES) -- $$i) >> makefile.depend; done

$(LIBNAME).$(SrcSuf): $(HDRS) LinkDef.h
	rootcint -f $@ -c $(DICTINCLUDES) \
	$(HDRS) LinkDef.h 

install: $(SharedLib)
	$(INSTALL) -m 755 $(SharedLib) $(JSFLIBDIR)
	$(INSTALL) -m 644 $(HDRS) $(JSFINCDIR)

uninstall:
	rm -f $(JSFLIBDIR)/$(SharedLib)
	( cd $(JSFINCDIR) ; rm -f $(HDRS) )

clean:
	rm -f *.o core *~
	rm -f makefile.depend

distclean: clean
	rm -f $(SharedLib) $(LIBNAME).*

-include makefile.depend

