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
};


// ---------------------------------------------------------------
void JSFJLCSIM::KZGET(char *cname, int iseg, int &leng, int idat[])
{
  int lenb=strlen(cname);
  kzget_(cname, &iseg, &leng, idat, lenb);
  return ;
}


