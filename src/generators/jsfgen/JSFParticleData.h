#ifndef __JSFParticleData__
#define __JSFParticleData__
//
// Interface to get particle information
//
// Implementation of this class depends on Pythia version

class JSFParticleData
{
 public:
  static float GetCharge(int id);
  static float GetCTau(int id);
};
#endif

