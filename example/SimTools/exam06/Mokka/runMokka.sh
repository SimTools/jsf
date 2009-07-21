#!/bin/bash

dirilcsoft=/proj/soft/ilcsoft/v01-06/
dirmokka=$dirilcsoft/Mokka/mokka-06-07-patch01

. $dirmokka/build_env.sh

export LD_LIBRARY_PATH=$CLHEP_BASE_DIR/lib:$LD_LIBRARY_PATH

Mokka sample.steer < sample.g4


