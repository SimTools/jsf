
has_clhep  = $(shell opts-config --has_CLHEP)
has_leda   = $(shell opts-config --has_LEDA)
has_lcio   = $(shell opts-config --has_LCIO)
has_jim    =

subdirs    =
ifeq ($(has_clhep),"yes")
subdirs   += stdhep
endif
ifeq ($(has_leda),"yes")
subdirs   += Anlib zvtop
endif
ifeq ($(has_lcio),"yes")
subdirs   += lcio
endif
ifeq ($(has_jim),"yes")
subdirs   += jim
endif

install:
	for i in $(subdirs) ; do \
	( cd $$i ; make $@ ) ; done

%:
	for i in $(subdirs) ; do \
	( cd $$i ; make $@ ) ; done



