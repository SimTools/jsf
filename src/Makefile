
SHELL   = /bin/bash
MFLAGS	=
CURRDIR	= .

SUBDIRS = kern generators tools lclibdep gui

.PHONY: all kern generators tools lclibdep gui
	
all: kern generators tools lclibdep gui

install: all

kern:
	( cd kern ; xmkmf -a ; make all ; make install )

generators:
	( cd generators ; make all ; make install )

lclibdep:
	( cd lclibdep ; make all ; make install )

tools:
	( cd tools ; xmkmf -a ; make all ; make install )

gui:
	( cd gui ; xmkmf -a ; make all ; make install )

%:
	for i in $(SUBDIRS); do \
	(cd $$i; make $@ ) ; done
	
