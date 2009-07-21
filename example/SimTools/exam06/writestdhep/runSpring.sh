#!/bin/bash

. ../setup.bash

jsf -b -q --maxevt=1000 --stdhep=ww.stdhep \
  --stdhep-title="Physsim-WW" \
  --stdhep-ntries=1000 \
  gui.C

