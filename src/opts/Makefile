
has_clhep  = $(shell opts-config --has_CLHEP)
has_anlib  = $(shell opts-config --has_Anlib)
has_lcio   =
has_jim    =

subdirs    =
ifdef has_clhep
subdirs   += stdhep
endif
ifdef has_anlib
subdirs   += Anlib zvtop
endif
ifdef has_lcio
subdirs   +=lcio
endif
ifdef has_jim
subdirs   +=jim
endif

%:
	for i in $(subdirs) ; do \
	( cd $$i ; make $@ ) ; done



