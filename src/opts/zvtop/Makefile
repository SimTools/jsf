#
# Makefile to build bases libraries
#

all: src/Makefile example/Makefile
	( cd src ; make ; make install )
	( cd example ; make )

src/Makefile: src/Imakefile
	( cd src; xmkmf -a )

example/Makefile: example/Imakefile
	( cd example; xmkmf -a )

clean: src/Makefile example/Makefile
	( cd src ; make clean )
	( cd example/ ; make clean )

cleanall: src/Makefile example/Makefile
	( cd src ; make cleanall ; rm -f Makefile )
	( cd example/ ; make cleanall ; rm -f Makefile )

distclean: cleanall

web:
	( cd src ; xmkmf -a ; make web )
