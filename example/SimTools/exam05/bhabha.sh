#!/bin/bash

srcdir=/proj/soft/data5/samples/gen/ftp-lcd.slac/ilc/ILC500/generated/CMS_500
pid=$1
subid=01
procstr=${pid}\_${subid}

echo Processing ${procstr}

jsf -b -q --maxevt=100000000 \
	--infile=$srcdir/${procstr}.stdhep \
        --OutputFile=${pid}/${procstr}_forTaikan.root \
	--stdhepout=${pid}/${procstr}_forTaikan \
	--stdheptitle="${procstr}_forTaikan(|cos|<0.92&&acol>15)" \
     gui.C > ${pid}/${procstr}.log 2>&1 


