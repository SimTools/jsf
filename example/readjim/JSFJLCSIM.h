#ifndef __JSFJLCSIM__
#define __JSFJLCSIM__

///////////////////////////////////////////////////////////////////////
//
//  Static member functions to call JLCSIM routines.
//
//$Id: 
//
///////////////////////////////////////////////////////////////////////

#include <TObject.h>

// *************************************************************
class JSFJLCSIM 
{
 public:
  JSFJLCSIM(){}
  virtual ~JSFJLCSIM(){}

  static void KZGET(char *cname, int iseg, int &leng, int idat[]);  

  ClassDef(JSFJLCSIM,0)  // Interfaces to JLCSIM routines.
};

#endif


