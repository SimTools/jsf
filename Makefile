

all: include/JSFConfig.h
	( cd src ; make all)
	( cd include ; ln -sf $(LCBASEDIR)/include/LCBASEConfig.h .)
	( cd include ; ln -sf $(LCBASEDIR)/include/LCBASE.inc .)
	( cd include ; ln -sf $(LCBASEDIR)/include/LCBASEDefines.h .)


makefiles: include/JSFConfig.h
	( cd src ; make makefiles )

install: all
	( cd src ; make install )

clean:
	( cd src ; make clean )

distclean:
	( cd src ; make distclean )
	rm -rf include lib tmp bin/jsf

include/JSFConfig.h:
	(cd bin ; ./makespec )
 

