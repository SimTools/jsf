///////////////////////////////////////////////////////////////////
//
// JSFParticleData
//
// Accessor to version dependant Pythia functions
//
//$Id$
//
//////////////////////////////////////////////////////////////////

#include <iostream>

#include "JSFParticleData.h"

#if __PYTHIA_VERSION__ >= 6
#include "TPythia6.h"
#else
#define pychge_ luchge_
#define pycomp_ lucomp_

#define pydat2_ ludat2_
#endif

extern "C" {
  extern int pychge_(int *kf);
  extern int pycomp_(int *kf);
};

#if __PYTHIA_VERSION__ <= 5
typedef struct {
  int   KCHG[3][500]; 
  float PMAS[4][500];
  float PARF[2000];
  float VCKM[4][4];
} Pydat2_t;
#endif
extern Pydat2_t pydat2_;


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
