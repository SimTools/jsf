
SHELL   = /bin/bash
MFLAGS	=
CURRDIR	= .

SUBDIRS = kern generators tools lclibdep gui opts

.PHONY: all kern generators tools lclibdep gui opts
	
all: kern generators tools lclibdep gui opts

install: all

kern:
	( cd kern ; xmkmf -a ; make all ; make install )

generators:
	( cd generators ; make install )

lclibdep:
	( cd lclibdep ; make install )

tools:
	( cd tools ; xmkmf -a ; make all ; make install )

gui:
	( cd gui ; xmkmf -a ; make all ; make install )

opts: 
	( cd opts ; make install )


%:
	for i in $(SUBDIRS); do \
	(cd $$i; make $@ ) ; done
	
