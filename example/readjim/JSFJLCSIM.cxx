//*LastUpdate :  jsf-1-15  14-Jul-2000 Akiya Miyamoto
//*-- Author  : Akiya Miyamoto  14-July-2000 


///////////////////////////////////////////////////////////////////
//
// JSFJLCSIM
//
// A collection of Interface routines to use JLCSIM routines
//
//$Id: 
//
//////////////////////////////////////////////////////////////////

#include "JSFJLCSIM.h"

ClassImp(JSFJLCSIM)

extern "C" {
extern void kzget_(const char *cname, const int *iseg, int *leng, int idat[],  const int lenb);
extern void kzget0_(const char *cname, const int *iseg, int *leng, int idat[],  const int lenb);
extern void kzget2_(const char *cname, const int *iseg, const int *isseg, 
		    int *leng, int idat[],  int lenb);
extern void kzgeth_(int idat[]);
extern void kzgetv_(const char *cname,  int *ivers,  int lenb);
extern void kzbloc_(const char *cname,  int *lbank,  int lenb);
extern void kznseg_(const char *cname,  int *nseg,  int lenb);
extern void kzbtyp_(const char *cname,  int *ityp,  int lenb);
};


// ---------------------------------------------------------------
void JSFJLCSIM::KZGET(const char *cname, const int iseg, int &leng, int idat[])
{
  int lenb=strlen(cname);

  kzget_(cname, &iseg, &leng, idat, lenb);

  return ;
}

// ---------------------------------------------------------------
void JSFJLCSIM::KZGET(const char *cname, const int iseg, int &leng, float rdat[])
{
  int lenb=strlen(cname);

  kzget_(cname, &iseg, &leng, (int*)rdat, lenb);

  return ;
}

// ---------------------------------------------------------------
void JSFJLCSIM::KZGET0(const char *cname, const int iseg, int &leng, int idat[])
{
  int lenb=strlen(cname);

  kzget0_(cname, &iseg, &leng, idat, lenb);

  return ;
}

// ---------------------------------------------------------------
void JSFJLCSIM::KZGET0(const char *cname, const int iseg, int &leng, float rdat[])
{
  int lenb=strlen(cname);

  kzget0_(cname, &iseg, &leng, (int*)rdat, lenb);

  return ;
}

// ---------------------------------------------------------------
void JSFJLCSIM::KZGET2(const char *cname, const int iseg, const int isseg, int &leng, int idat[])
{
  static int lenb=strlen(cname);
  kzget2_(cname, &iseg, &isseg, &leng, idat, lenb);
  return ;
}

// ---------------------------------------------------------------
void JSFJLCSIM::KZGET2(const char *cname, const int iseg, const int isseg, int &leng, float rdat[])
{
  static int lenb=strlen(cname);
  kzget2_(cname, &iseg, &isseg, &leng, (int*)rdat, lenb);
  return ;
}

// ---------------------------------------------------------------
void JSFJLCSIM::KZGETH(int idat[])
{
  kzgeth_(idat);
  return ;
}

// ---------------------------------------------------------------
void JSFJLCSIM::KZGETV(const char *cname, int &ivers)
{
  int lenb=strlen(cname);
  kzgetv_(cname, &ivers, lenb);
  return ;
}

// ---------------------------------------------------------------
void JSFJLCSIM::KZBLOC(const char *cname, int &lbank)
{
  int lenb=strlen(cname);
  kzbloc_(cname, &lbank, lenb);
  return ;
}

// ---------------------------------------------------------------
void JSFJLCSIM::KZNSEG(const char *cname, int &nseg)
{
  int lenb=strlen(cname);
  kznseg_(cname, &nseg, lenb);
  return ;
}

// ---------------------------------------------------------------
void JSFJLCSIM::KZBTYP(const char *cname, int &ityp)
{
  int lenb=strlen(cname);
  kzbtyp_(cname, &ityp, lenb);
  return ;
}


