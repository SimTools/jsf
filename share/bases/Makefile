#
# Makefile to build bases libraries
#

.PHONY: lib eg

all: lib eg

lib: src/Makefile
	( cd src ; xmkmf -a )
	( cd src ; make ; make install )

eg: example/nunugam/Makefile
	( cd example/nunugam ; make )

src/Makefile: src/Imakefile
	( cd src ; xmkmf -a )

example/nunugam/Makefile: example/nunugam/Imakefile
	( cd example/nunugam ; xmkmf -a )

clean: src/Makefile example/nunugam/Makefile
	( cd src ; make clean )
	( cd example/nunugam ; make clean )

cleanall: src/Makefile example/nunugam/Makefile
	( cd src ;  make distclean ; rm -f Makefile)
	( cd example/nunugam ; make cleanall ; rm -f Makefile )

