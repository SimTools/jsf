
has_clhep  = $(shell opts-config --has_CLHEP)
has_anlib  = $(shell opts-config --has_Anlib)
has_lcio   = $(shell opts-config --has_LCIO)
has_jim    =

subdirs    =
ifdef has_clhep
subdirs   += stdhep
endif
ifdef has_anlib
subdirs   += Anlib zvtop
endif
ifdef has_lcio
subdirs   += lcio
endif
ifdef has_jim
subdirs   += jim
endif

install:
	for i in $(subdirs) ; do \
	( cd $$i ; make $@ ) ; done

%:
	for i in $(subdirs) ; do \
	( cd $$i ; make $@ ) ; done



