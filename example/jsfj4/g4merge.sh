#!/bin/bash

echo -n "G4INSTALL($G4INSTALL) directory .. "
if [ ! -d $G4INSTALL ] ; then 
  echo does not exist.
  exit
fi
echo exist

#
echo -n "G4INSTALL/lib/G4SYSTEM($G4INSTALL/lib/$G4SYSTEM) directory .."
if [ ! -d $G4INSTALL/lib/$G4SYSTEM ] ; then
  echo does not exist.
  exit 
fi
echo exist
#

WORKDIR=g4tmp
rm -fr $WORKDIR
mkdir $WORKDIR
cd $WORKDIR

echo "Extractling Geant4 library objects"
for f in $G4INSTALL/lib/$G4SYSTEM/lib*.a ; do
  ar x $f
done

rm -f ../libG4all.a
echo "Merging all Geant4 objects into single library"
ar cr ../libG4all.a *.o
rm -f $WORKDIR
