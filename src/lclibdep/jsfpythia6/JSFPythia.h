#ifndef __JSFParticleData__
#define __JSFParticleData__
//
// Interface to Pythia generator
//

class JSFParticleData
{
 public:
  static Float_t GetCharge(int id);
  static Float_t GetCTau(int id);
};
#endif

