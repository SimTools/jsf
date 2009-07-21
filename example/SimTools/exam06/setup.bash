##
##  Setup environments to run root/jsf/Geant4
##  Environment parameter for LC studies
##
#######################################################
##
##  Setup for JLCLOGIN2
##
#######################################################
export LC_RELEASE=1.45
export LC_RELEASE_DIR=/proj/soft/Release/$LC_RELEASE
#
export ROOTSYS=/proj/soft/ext/root/5.18.00
export G4INSTALL=/proj/soft/ext/g4/geant4.9.1.p01
export LCIO=/proj/soft/ext/lcio/v01-09
export JDK_HOME=/usr/java/j2sdk1.4.2_12
export CERN_ROOT=/proj/soft/ext/cernlib/2006

export CLHEP_BASE_DIR=/proj/soft/ext/clhep/2.0.3.2
export CLHEP_INCLUDE_DIR=$CLHEP_BASE_DIR/include
export STDHEPDIR=/proj/soft/ext/stdhep/stdhep-5-06-01

export LCBASEDIR=$LC_RELEASE_DIR/lcbase
export LEDAROOT=$LC_RELEASE_DIR/Leda
export LCLIBROOT=$LC_RELEASE_DIR/lclib
export JSFROOT=$LC_RELEASE_DIR/jsf
export KFLIBROOT=$LC_RELEASE_DIR/physsim

#export JUPITERROOT=$LC_RELEASE_DIR/Jupiter
#export SATELLITESROOT=$LC_RELEASE_DIR/Satellites
#export URANUSROOT=$LC_RELEASE_DIR/Uranus
export SOSYMLINK=true


##### Geatn4 setup ##############
. $G4INSTALL/env.sh > /dev/null
######### end of Geant4 setup ##################################

## Set command path
LCPATH=$LCBASEDIR/bin:$JSFROOT/bin:$ROOTSYS/bin:$CERN_ROOT/bin:$LCLIBROOT/bin:$JLCSIMROOT/bin:$CLHEP_BASE_DIR/bin
J4PATH=$JUPITERROOT/bin/$G4SYSTEM:$G4INSTALL/bin/$G4SYSTEM
LCIOPATH=$LCIO/tools:$LCIO/bin:$JDK_HOME/bin
export PATH=.:~/bin:$LCPATH:$LCIOPATH:$PATH
unset LCPATH
unset J4PATH
unset LCIOPATH

## Set LD Library Path
export LD_LIBRARY_PATH=$JSFROOT/lib:$ROOTSYS/lib:$LEDAROOT/lib:$CLHEP_BASE_DIR/lib
# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$JUPITERROOT/lib/$G4SYSTEM:$G4INSTALL/lib/$G4SYSTEM:$SATELLITESROOT/lib:$URANUSROOT/lib
export IMAKEINCLUDE="-I$LCBASEDIR -I$KFLIBROOT -I$LCLIBROOT"

