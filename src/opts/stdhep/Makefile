
include $(LCBASEDIR)/conf/common.gmk

NAME	  = JSFReadStdHep

LIBNAME   = lib$(NAME)

SharedLib = $(LIBNAME).$(DllSuf)

SRCS      = JSFReadStdHep.$(SrcSuf)

OBJS      = $(subst .$(SrcSuf),.$(ObjSuf),$(SRCS)) \
	    dummystdhep.o \
	    $(LIBNAME).$(ObjSuf)

HDRS      = $(subst .$(SrcSuf),.$(HdrSuf),$(SRCS))

ifdef CLHEP_BASE_DIR
LIBS	+= -L$(CLHEP_BASE_DIR)/lib
endif
LIBS    += -lCLHEP

ifdef CLHEP_INCLUDE_DIR
INCPATH  += -I$(CLHEP_INCLUDE_DIR)
FFLAGS   += -I$(CLHEP_INCLUDE_DIR)
endif

all: depend $(SharedLib)

$(SharedLib): $(OBJS) 
	@rm -f $(SharedLib)
	$(LD) $(OBJS) -o $(SharedLib) $(SOFLAGS) $(LIBS) 

install: $(SharedLib)
	$(INSTALL) -m 755 $(SharedLib) $(JSFLIBDIR)
	$(INSTALL) -m 644 $(HDRS) $(JSFINCDIR)

uninstall:
	rm -f $(JSFLIBDIR)/$(SharedLib)
	( cd $(JSFINCDIR) ; rm -f $(HDRS) )

clean:
	rm -f makefile.depend
	rm -f $(CLEANOBJS) $(LIBNAME).* *.o

distclean: clean uninstall
	rm -f *.o core *~ a.out $(SharedLib)

$(LIBNAME).$(SrcSuf): $(HDRS)
	rootcint -f $@ -c $(DICTINCLUDES) \
	$(HDRS) JSFStdHepLinkDef.h 

depend: $(SRCS) $(HDRS)
	rm -f makefile.depend
	touch makefile.depend
	for i in $(SRCS) ; do \
	( rmkdepend -f- -- $(RMKDEPENDFILES) -- $$i >> makefile.depend ) ; done

web:
	jsf -b -q html.C

-include makefile.depend

