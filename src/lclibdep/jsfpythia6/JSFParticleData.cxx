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
  extern int pychge_(int *kf);
  extern int pycomp_(int *kf);
};

struct COMMON_PYDAT2_t {
  int KCHG[4][500];
  double PMAS[4][500];
  double PARF[2000];
  double VCKM[4][4];
};
extern COMMON_PYDAT2_t pydat2_;


using namespace std;

//___ Get Particle Charge
float JSFParticleData::GetCharge(int id)
{
  return ((float)pychge_(&id))/3.0;
}

//___ Get CTau
float JSFParticleData::GetCTau(int id)
{
    int kc=pycomp_(&id);
    float xctau=pydat2_.PMAS[3][kc-1]*0.1;
    return xctau;
}
