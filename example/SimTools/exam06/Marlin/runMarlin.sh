#!/bin/bash -vx

. /proj/soft/ilcsoft/v01-06/Marlin/v00-10-04/build_env.sh
# . config.sh 

ln -s /proj/soft/ilcsoft/v01-06/LCFI_MokkaBasedNets/v00-01/ILD_00 nets

# =======================================================
# Start Marlin
# =======================================================

Marlin stdreco.xml 

