all: help

help: 
	@echo "How to use makefile "
	@echo " make help      ; display this message "
	@echo " make install   ; create library and binaries in example directory"
	@echo " make clean     ; remove all .o files "
	@echo " make distclean ; delete everything except source files."
	@echo " "
	@echo " Don't forget to setenv ROOTSYS and JSFROOT prior to do make install"


install : lib html include
	@echo "JSFROOT is set to $JSFROOT"
	( cd src ; xmkmf ; make )
	( cd example/FFbarSpring ; xmkmf ; make )
	( cd example/CCDAnal ; xmkmf ; make )
	( cd example/ZHStudy ; xmkmf ; make )

clean: 
	( cd src ; make clean )
	( cd example/FFbarSpring ; make clean )
	( cd example/CCDAnal  ; make clean )
	( cd example/ZHStudy  ; make clean )

lib: 
	mkdir lib

html:
	mkdir html

include:
	ln -s src include

distclean: 
	( cd src ; make distclean ) 
	( cd example/FFbarSpring ; make distclean )
	( cd example/CCDAnal ; make distclean )
	( cd example/ZHStudy ; make distclean )
	( cd html ; rm -f *.html *.h *.ps *~ ; rm -rf src )
	( rm -r lib ; rmdir html )
	( cd macro ; rm -f jsf.root , PYTHIA.SEED )
