///////////////////////////////////////////////////////////////////
//
// JSFParticleData
//
// Accessor to version dependant Pythia functions
//
//$Id$
//
//////////////////////////////////////////////////////////////////

#ifdef __USEISOCXX__
#include <iostream>
#else
#include <iostream.h>
#endif

#include "JSFParticleData.h"

extern "C" {
  extern int luchge_(int *kf);
  extern float ulctau_(int *kf);
};

using namespace std;

//___ Get Particle Charge
float JSFParticleData::GetCharge(int id)
{
  return ((float)luchge_(&id))/3.0;
}

//___ Get CTau
float JSFParticleData::GetCTau(int id)
{
  return ((float)ulctau_(&id));
}
