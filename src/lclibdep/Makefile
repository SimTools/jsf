
SHELL   = /bin/bash
MFLAGS	=
CURRDIR	= .

SUBDIRS = quicksim jsfpythia6 jsfpythia5

all: install
install:
	for i in $(SUBDIRS); do \
	(cd $$i; echo ``making'' $@ ``in $(CURRDIR)/$$i...''; \
	xmkmf -a ; make ; make install ); \
	done

%:
	@case '${MFLAGS}' in *[ik]*) set +e;; esac; \
	for i in $(SUBDIRS); do \
	(cd $$i; echo ``making'' $@ ``in $(CURRDIR)/$$i...''; \
	make $@); \
	done
	@if [ $@ == clean -o $@ == distclean ] ; then \
	  $(RM) $(CLEANFILES) $(DISTCLEANFILES) ; fi
