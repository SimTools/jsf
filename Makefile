#
# Makefile for JSF
#
#$Id$
#
all: help

help: 
	@echo "How to use makefile "
	@echo " make help      ; display this message "
	@echo " make install   ; create library and binaries in example directory"
	@echo " make allinstall: install optional package such as for JLCSIM"
	@echo " make fullinstall: iallinstall+web"
	@echo " make web   ; make files in html directory."
	@echo " make clean     ; remove all .o files "
	@echo " make distclean ; delete everything except source files."
	@echo " "
	@echo " Don't forget to setenv as described in HowToInstall prior to do make install"


install : makespec lib html include
	@echo "JSFROOT is set to $(JSFROOT)"
	( cd share/bases ; make )
	( cd src ; xmkmf -a ; make )
	( cd example/FFbarSpring ; xmkmf -a ; make )
	( cd example/ZHStudy ; xmkmf -a ; make )
	( cd example/ww ; xmkmf -a ; make )
	( cd example/guiexam1 ; xmkmf -a ; make )

makespec: $(JSFROOT)/src/JSFConfig.h

$(JSFROOT)/src/JSFConfig.h: $(JSFROOT)/conf/jsf.version
	( cd bin ; ./makespec $(JSFROOT) )

mkopsoft:
	mkopsoft

allinstall: install mkopsoft
	( cd example/CCDAnal ; xmkmf -a ; make )
	( cd example/NuNuGam ; xmkmf -a ; make )
	( cd example/mkjimbnk ; xmkmf -a ; make )
	( cd share/zvtop ; make all )

fullinstall: allinstall web

clean: 
	( cd share/bases ; make clean )
	( cd src ; make clean )
	( cd example/FFbarSpring ; make clean )
	( cd example/ZHStudy  ; make clean )
	( cd example/ww ; make clean )
	( cd example/guiexam1 ; make clean )
	( cd example/CCDAnal  ; make clean )

web: html/example
	( cd src ; xmkmf ; make web ) 
	for i in FFbarSpring guiexam1 ww CCDAnal readjim jsfjim ; do \
	( cd example/$$i ; xmkmf -a ; make web ) \
	done
	( cd share/zvtop ; make web ) 
	( perl bin/webindx > html/JSF_Index.html ) 

html/example: 
	mkdir html/example

lib: 
	mkdir lib

html:
	mkdir html

include:
	ln -s src include

distclean: 
	( cd share/bases ; make cleanall ) 
	( cd src ; make distclean ) 
	( cd example/FFbarSpring ; make distclean )
	( cd example/ZHStudy ; make distclean )
	( cd example/ww ; make distclean )
	( cd example/guiexam1 ; make distclean )
	( cd html ; rm -f *.html *.h *.ps *~ ; rm -rf src ; rm -rf exam* )
	( rm -r lib ; rmdir html )
	( cd macro ; rm -f jsf.root , PYTHIA.SEED )
	( cd example/CCDAnal ; make distclean )

