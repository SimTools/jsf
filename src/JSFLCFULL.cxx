//*LastUpdate :  jsf-1-4  11-Feburary-1999  By Akiya Miyamoto
//*-- Author  : A.Miyamoto  11-September-1998

///////////////////////////////////////////////////////////////////
//
//  JSF LCFULL
//
//  Interface to execute LCFULL in ROOT environment.
//  This routine does (1) TBS initialize
//  
//////////////////////////////////////////////////////////////////

#include <strings.h>
#include "JSFLCFULL.h"

ClassImp(JSFLCFULL)

JSFLCFULL *gJSFLCFULL;

//_____________________________________________________________________________
JSFLCFULL::JSFLCFULL(const char *name, const char *title)
       : JSFModule(name,title)
{
  gJSFLCFULL=this;
  prinit_root_();
  prgenf_.numgen=99999999  ; prgenf_.iumgen=0 ; 
  prdyna_.ndynat=-1 ; 

}

// ---------------------------------------------------------------
JSFLCFULL::~JSFLCFULL()
{
}


// ---------------------------------------------------------------
Bool_t JSFLCFULL::Initialize()
{

  prjobi_();

  bzero((char*)&prrsta_.nrdrec, sizeof(COMMON_PRRSTA));
  bzero((char*)&presta_.nctbgn, sizeof(COMMON_PRESTA));
  
  return kTRUE;
}


// ---------------------------------------------------------------
Bool_t JSFLCFULL::BeginRun(Int_t nrun)
{

  prgevt_(&fRecid);
  prrsta_.nrdrec ++;
  if( fRecid >= 1 && fRecid <= 9 ) { prrsta_.nstrec[fRecid-1][0] ++;}

  pruhed_(&fRecid);

  return kTRUE;
}

// ---------------------------------------------------------------
Bool_t JSFLCFULL::Process(Int_t ev)
{

//  Int_t idebug=prjflg_.njbdbg[0][1]/10;
  Int_t ievseq=0;

  prgevt_(&fRecid);

  prrsta_.nrdrec ++;
  if( fRecid >= 1 && fRecid <= 9 ) { prrsta_.nstrec[fRecid-1][0] ++;}

  pruhed_(&fRecid);

  if ( fRecid == 1 ) { ievseq ++ ; }

  //  return fRecid;
  return kTRUE;
}

// ---------------------------------------------------------------
void JSFLCFULL::TBGET( Int_t idrec, const Char_t *bankname, Int_t elem,
	Int_t &nw, Int_t data[], Int_t &iret)
{
//  Get Int_t data from TBS buffer.
   Int_t lenb=strlen(bankname);
   tbget_(&idrec, bankname, &elem, &nw, data, &iret, lenb);
   return ;

}

// ---------------------------------------------------------------
void JSFLCFULL::TBGET( Int_t idrec, const Char_t *bankname, Int_t elem,
	Int_t &nw, Float_t data[], Int_t &iret)
{
//  Get Int_t data from TBS buffer.
   Int_t lenb=strlen(bankname);
   tbget_(&idrec, bankname, &elem, &nw, (Int_t*)data, &iret, lenb);
   return ;

}


// ---------------------------------------------------------------
void JSFLCFULL::TBPUT( Int_t idrec, const Char_t *bankname, Int_t elem,
	Int_t nw, Int_t data[], Int_t &iret)
{
//  Put Int_t data to TBS buffer.
   Int_t lenb=strlen(bankname);
   tbput_(&idrec, bankname, &elem, &nw, data, &iret, lenb);
   return ;

}


// ---------------------------------------------------------------
void JSFLCFULL::TBCRTE( Int_t idrec, const Char_t *bankname, Int_t isaloc,
	Int_t isext, Int_t &iret)
{
//  Put Int_t data to TBS buffer.
   Int_t lenb=strlen(bankname);
   tbcrte_(&idrec, bankname, &isaloc, &isext, &iret, lenb);
   return ;

}

// ---------------------------------------------------------------
void JSFLCFULL::TBNOEL( Int_t idrec, const Char_t *bankname,
	Int_t &nelm, Int_t neary[])
{
//  Get Float_t data from TBS buffer.
   Int_t lenb=strlen(bankname);
   tbnoel_(&idrec, bankname, &nelm, neary, lenb);

   return ;

}

// ---------------------------------------------------------------
void JSFLCFULL::TBDELB( Int_t idrec, const Char_t *bankname,  Int_t &iret)
{
//  Get Float_t data from TBS buffer.
   Int_t lenb=strlen(bankname);
   tbdelb_(&idrec, bankname, &iret, lenb);

   return ;

}

// ---------------------------------------------------------------
void JSFLCFULL::TBINFO()
{
    tblsum_();
    tblsub_();
    tblbnm_();
    Int_t id=1;
    tblist_(&id);
}

// ---------------------------------------------------------------
void JSFLCFULL::TBINFO(const Char_t * option)
{
   if( strcmp(option,"tblsum")==0 ) {
     tblsum_();
   }
   else if( strcmp(option,"tblsub")==0 ) { 
     tblsub_();
   }
   else if( strcmp(option,"tblbnm")==0 ) {
     tblbnm_();
   }
   else if( strcmp(option,"tblist")==0 ) { 
     Int_t id=1;
     tblist_(&id);
   }
   else {
     printf("Warning .. TBINFO is called with unrecognized option %s\n",option);
   }
   return ;
}

