
subdirs = mcfio stdhep examples

install:
	for i in $(subdirs) ; do \
	( cd $$i ; make $@ ) ; done

%:
	for i in $(subdirs) ; do \
	( cd $$i ; make $@ ) ; done



