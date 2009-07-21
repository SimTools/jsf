#!/bin/bash

. ../setup.bash

infile=../writestdhep/ww.stdhep


ln -sf ${infile} sample.stdhep

./inputExample

h2root sample.hbook sample.root

rm sample.hbook
rm sample.stdhep

