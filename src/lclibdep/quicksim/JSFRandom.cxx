
#include "JSFRandom.h"

JSFRandom *JSFRandom::me=0;

ClassImp(JSFRandom)

extern "C" float ran_(int *seed)
{
  double val=JSFRandom::Instance()->Rndm();
  return (float)val;
};

extern "C" float rann_(int *seed)
{
  double val=JSFRandom::Instance()->Gaus();
  return (float)val;
};


JSFRandom* JSFRandom::Instance()
{
   if( !me ) me=new JSFRandom();
   return me;	
}


