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
extern void kzget_(char *cname, int *iseg, int *leng, int idat[],  int lenb);
extern void kzget2_(char *cname, int *iseg, int *isseg, 
		    int *leng, int idat[],  int lenb);
extern void kzgeth_(int idat[]);
extern void kzgetv_(char *cname,  int *ivers,  int lenb);
};


// ---------------------------------------------------------------
void JSFJLCSIM::KZGET(char *cname, int iseg, int &leng, int idat[])
{
  int lenb=strlen(cname);
  kzget_(cname, &iseg, &leng, idat, lenb);
  return ;
}

// ---------------------------------------------------------------
void JSFJLCSIM::KZGET2(char *cname, int iseg, int isseg, int &leng, int idat[])
{
  int lenb=strlen(cname);
  kzget2_(cname, &iseg, &isseg, &leng, idat, lenb);
  return ;
}

// ---------------------------------------------------------------
void JSFJLCSIM::KZGETH(int idat[])
{
  kzgeth_(idat);
  return ;
}

// ---------------------------------------------------------------
void JSFJLCSIM::KZGETV(char *cname, int &ivers)
{
  int lenb=strlen(cname);
  kzgetv_(cname, &ivers, lenb);
  return ;
}


