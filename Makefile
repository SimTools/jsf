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
	@echo " make web   ; make files in html directory."
	@echo " make clean     ; remove all .o files "
	@echo " make distclean ; delete everything except source files."
	@echo " "
	@echo " Don't forget to setenv as described in HowToInstall prior to do make install"


install : lib html include
	@echo "JSFROOT is set to $(JSFROOT)"
	( cd bin ; ./makespec $(JSFROOT) )
	( cd src ; xmkmf ; make )
	( cd example/FFbarSpring ; xmkmf ; make )
#	( cd example/CCDAnal ; xmkmf ; make )
	( cd example/ZHStudy ; xmkmf ; make )
	( cd example/ww ; xmkmf ; make )
	( cd example/guiexam1 ; xmkmf ; make )

clean: 
	( cd src ; make clean )
	( cd example/FFbarSpring ; make clean )
	( cd example/ZHStudy  ; make clean )
	( cd example/ww ; make clean )
	( cd example/guiexam1 ; make clean )
	( cd example/CCDAnal  ; make clean )

web:  CCDAnal
	( cd example/FFbarSpring ; ./jsf -q -b html.C ; \
		cd $(JSFROOT)/html ; mv USER_Index.html FFbarSpring_Index.html)
	( cd example/ww ; ./jsf -q -b html.C ; \
		cd $(JSFROOT)/html ; mv USER_Index.html ww_Index.html)
	( cd example/guiexam1 ; $(JSFROOT)/bin/jsf -q -b html.C ; \
		cd $(JSFROOT)/html ; mv USER_Index.html guiexam1_Index.html)
	( cd src ; ../bin/jsf -q -b html.C ; \
		cd $(JSFROOT)/html ; mv USER_Index.html JSFMain_Index.html)
	( cd example/CCDAnal ; xmkmf ; make ; ./ccd -q -b html.C ; \
		cd $(JSFROOT)/html ; mv USER_Index.html CCDAnal_Index.html)
	( cd example/readjim ; xmkmf ; make ; ./jim -q -b html.C ; \
		cd $(JSFROOT)/html ; mv USER_Index.html ReadJIM_Index.html)
	( cd example/jsfjim ; xmkmf ; make ; ./jsfjim -q -b html.C ; \
		cd $(JSFROOT)/html ; mv USER_Index.html JSFJIM_Index.html)
	( perl bin/webindx > html/JSF_Index.html )

CCDAnal:
	( cd example/CCDAnal ; xmkmf ; make )

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
	( cd example/ww ; make distclean )
	( cd example/guiexam1 ; make distclean )
	( cd html ; rm -f *.html *.h *.ps *~ ; rm -rf src )
	( rm -r lib ; rmdir html )
	( cd macro ; rm -f jsf.root , PYTHIA.SEED )


