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
  static void KZGET2(char *cname, int iseg, int isseg, 
		     int &leng, int idat[]);  
  static void KZGETH(int idat[]);  
  static void KZGETV(char *cname, int &ivers);  

  ClassDef(JSFJLCSIM,0)  // Interfaces to JLCSIM routines.
};

#endif


