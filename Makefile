all: 

distclean: 
	( cd src ; make clean ) 
	( cd example/FFbarSpring ; make clean )
	( cd example/CCDAnal ; make clean )
	( cd html ; rm -f *.html *.h *.ps *~ ; rm -rf src )
	( cd lib ; rm * )
	( cd macro ; rm -f jsf.root , PYTHIA.SEED )
