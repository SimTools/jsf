#ifndef __JSFRANDOM__
#define __JSFRANDOM__

// Overrid a function, ran used in lclib
//
#include "TRandom3.h"

class JSFRandom : public TRandom3 {
public:
  static JSFRandom *Instance();
private:
  static JSFRandom* me;

  ClassDef(JSFRandom,1) // Random Number for Quick Sim
  
};

#endif

