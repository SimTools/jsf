#
# Makefile to build bases libraries
#

all:
	( cd src ; xmkmf -a ; make ; make install )
	( cd example/nunugam ; xmkmf -a ; make )

cleanall:
	( cd src ; make cleanall ; rm -f Makefile )
	( cd example/nunugam ; make cleanall ; rm -f Makefile)

