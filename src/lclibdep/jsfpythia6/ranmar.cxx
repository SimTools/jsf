#include "JSFRandom.h"

extern "C" void ranmar_(float *rd, int *ndim)
{
  for(Int_t i=0;i<*ndim;i++) {
    double val=JSFRandom::Instance()->Rndm();
    rd[i]=val;
  }
};
