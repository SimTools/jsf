############################################################################
## (Description)
##    Imake file for Anlib: C++ analysis library
## (Update Record)
##    99/05/22  K.Fujii     Original version.
##    01/08/01  K.Ikematsu  Fixed dependency check for
##                          generating dictionary.
##    05/02/12  A.Miyamoto  Convert from Imakefile to Makefile
## $Id$
############################################################################

include $(LCBASEDIR)/conf/common.gmk

# Package Name 

INSTALLDIR	= $(JSFLIBDIR)
LIBNAME		= JSFAnlib
SONAME		= lib$(LIBNAME).$(SOEXT).$(SOREV)
JSFSOLIB	= $(SONAME)
INCPATH		= -I$(LEDAROOT)/include

# Headers 

# Routines to compile 

OBJS		= ANLTrack.$(ObjSuf) \
		  ANLVTXTagger.$(ObjSuf) \
		  FlavourGetter.$(ObjSuf) \
		  ANLCheatedJetFinder.$(ObjSuf) \
		  ANLGVTXTagger.$(ObjSuf)

# You don't have to edit anything below here 

DICTNAME	= $(LIBNAME)Dict

ALLOBJS 	= $(OBJS) $(DICTNAME).$(ObjSuf)

SRCS		= $(subst .$(ObjSuf),.$(SrcSuf),$(OBJS))

HDRS		= $(subst .$(ObjSuf),.h,$(OBJS))

# /* Make Targets */

all: depend $(JSFSOLIB)

$(JSFSOLIB): $(ALLOBJS)
	$(LD) -o $@ $(SOFLAGS) $(ALLOBJS)

install: $(JSFSOLIB)
	install -p -m 755 $(JSFSOLIB) $(JSFLIBDIR)
	(cd $(JSFLIBDIR) ; ln -fs $(JSFSOLIB) lib$(LIBNAME).$(SOEXT) )  
	install -p -m 644 $(HDRS) $(JSFINCDIR)

$(DICTNAME).$(ObjSuf): $(DICTNAME).$(SrcSuf)

clean:
	@rm -f *.$(ObjSuf) core $(DICTNAME).$(SrcSuf) $(DICTNAME).h
	@rm -f *~ $(JSFSOLIB)

depend: $(SRCS) $(HDRS)
	rm -f makefile.depend
	touch makefile.depend
	for i in $(SRCS); do \
	( rmkdepend -f- -a -- $(RMKDEPENDFILES) -- $$i >> makefile.depend ) ; done

distclean: clean
	@rm -f $(JSFSOLIB) *.$(SOEXT)
	@rm -f *.root *.tdr *.out *~ core

$(DICTNAME).$(SrcSuf): $(HDRS) LinkDef.h
	@echo "Generating dictionary ..."
	rootcint -f $(DICTNAME).$(SrcSuf) \
		-c $(DICTINCLUDES) \
		$(RCINTDEFINES) $(HDRS) LinkDef.h

-include  makefile.depend
