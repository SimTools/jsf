
SHELL   = /bin/bash
MFLAGS	=
CURRDIR	= .

SUBDIRS1 = pythia6 bases
SUBDIRS2 = bsgen jsfgen me2shgen
SUBDIRS3 = herwig
SUBDIRS  = $(SUBDIRS1) $(SUBDIRS2) $(SUBDIRS3)

all: set1 set2 set3

install: set1 set2 set3

set1: 
	for i in $(SUBDIRS1) ; do \
	(cd $$i; echo ``making'' $@ ``in $(CURRDIR)/$$i...''; \
	make all ; make install ); done

set2: 
	for i in $(SUBDIRS2) ; do \
	(cd $$i; echo ``making'' $@ ``in $(CURRDIR)/$$i...''; \
	xmkmf -a ; make all ; make install ); done

set3: 
	for i in $(SUBDIRS3) ; do \
	(cd $$i; echo ``making'' $@ ``in $(CURRDIR)/$$i...''; \
	make all ; make install ); done
%:
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS); do \
	(cd $$i; echo ``making'' $@ ``in $(CURRDIR)/$$i...''; \
	make $@); \
	done
	@if [ $@ == clean -o $@ == distclean ] ; then \
	  $(RM) $(CLEANFILES) $(DISTCLEANFILES) ; fi

