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

  static void KZGET0(const char *cname, const int iseg, int &leng, int idat[]);  
  static void KZGET0(const char *cname, const int iseg, int &leng, float rdat[]);  
  static void KZGET(const char *cname, const int iseg, int &leng, int idat[]);  
  static void KZGET(const char *cname, const int iseg, int &leng, float rdat[]);  
  static void KZGET2(const char *cname, const int iseg, const int isseg, 
		     int &leng, int idat[]);  
  static void KZGET2(const char *cname, const int iseg, const int isseg, 
		     int &leng, float rdat[]);  
  static void KZGETH(int idat[]);  
  static void KZGETV(const char *cname, int &ivers);  
  static void KZBLOC(const char *name, Int_t &lbank);
  static void KZNSEG(const char *name, Int_t &nseg);
  static void KZBTYP(const char *name, Int_t &ityp);

  ClassDef(JSFJLCSIM,0)  // Interfaces to JLCSIM routines.
};

#endif


