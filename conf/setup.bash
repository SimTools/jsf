#########################################################
#
# Example to setup environment parameters for bash
#
#########################################################
export ROOTSYS=/proj/soft/Release/1.04/root
export LCBASEDIR=/home/miyamoto/soft/LC/lcbase
export JSFROOT=/home/miyamoto/soft/LC/jsf
export LCLIBROOT=/home/miyamoto/soft/LC/lclib

export PATH=$PATH:$LCBASEDIR/bin:$JSFROOT/bin:$ROOTSYS/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$JSFROOT/lib:$ROOTSYS/lib
export IMAKEINCLUDE="-I$LCBASEDIR"

