############################################################################
## (Description)
##    Imake file for XCXC analysis.
## (Update Record) 
##    99/05/22  K.Fujii   Original version.
##    05/02/15  A.Miyamoto Modified to use LEDAROOT
############################################################################

include $(LCBASEDIR)/conf/common.gmk

INCPATH	+= -I$(LEDAROOT)/include 

# Package Name

PACKAGENAME	= UserAnalysis

#  You don't have to edit anything below here

OBJS		= $(PACKAGENAME).$(ObjSuf)

SRCS		= $(subst .$(ObjSuf),.$(SrcSuf),$(OBJS))

JSFPROGRAM	= $(PACKAGENAME)

ANLLIB		= -L$(LEDAROOT)/lib -lAnlib -lS4Utils 

LDFLAGS         = $(shell root-config --libs)

#/*
# * Make Targets
# *
# */

all:  $(JSFPROGRAM)

$(JSFPROGRAM): $(PACKAGENAME).$(ObjSuf)
	$(LD) -o $(JSFPROGRAM) $(PACKAGENAME).$(ObjSuf) $(ANLLIB) \
	$(LDFLAGS)

clean: 
	@rm -f *.$(ObjSuf) core

depend: $(SRCS)
	for i in $(SRCS); do \
	rmkdepend -a -- $(CXXFLAGS) $(INCPATH) $(DEPENDFILES) -- $$i; done

distclean: clean
	@rm -f $(JSFPROGRAM)
	@rm -f *.root *.tdr *.out *~ core
