
all: include/JSFConfig.h
	( cd src ; make all)

makefiles: include/JSFConfig.h
	( cd src ; make makefiles )

install: 
	( cd src ; make install )

clean:
	( cd src ; make clean )

distclean:
	( cd src ; make distclean )
	rm -rf include lib tmp bin/jsf

include/JSFConfig.h:
	(cd bin ; ./makespec )
 

