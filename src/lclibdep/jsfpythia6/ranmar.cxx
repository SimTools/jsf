#include "JSFRandom.h"

#if __PYTHIA_VERSION__ <= 5
extern "C" {
  void pdfset_();
  void structm_();
  void dummy_(){ 
	pdfset_(); 
	structm_();
  }
};
#endif

extern "C" void ranmar_(float *rd, int *ndim)
{
  for(Int_t i=0;i<*ndim;i++) {
    double val=JSFRandom::Instance()->Rndm();
    rd[i]=val;
  }
};
