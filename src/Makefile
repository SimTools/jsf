
SHELL   = /bin/bash
MFLAGS	=
CURRDIR	= .

SUBDIRS = kern generators quicksim gui

.PHONY: all kern generators quicksim gui
	
all: kern generators quicksim gui

install: all

kern:
	( cd kern ; xmkmf -a ; make all ; make install )

generators:
	( cd generators ; make all ; make install )

quicksim:
	( cd quicksim ; make all ; make install )

gui:
	( cd gui && xmkmf -a && make all && make install )

%:
	for i in $(SUBDIRS); do \
	(cd $$i; make $@ ) ; done
	
