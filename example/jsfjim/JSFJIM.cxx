//*LastUpdate :  jsf-1-10  29-May-1999  By Akiya Miyamoto
//*-- Author  : Akiya Miyamoto  29-May-1999


///////////////////////////////////////////////////////////////////
//
// JSFJIM
//
// Run JIM in JSF
//
//$Id$
//
//////////////////////////////////////////////////////////////////

#include "JSFJIM.h"


extern "C" {
extern void uginit_();
extern void uglast_();
extern void jsfjimgrun_(Int_t *nret);
extern void jsfjimopen_(Int_t *iu, Char_t *fname, Int_t *iret, Int_t lname);
extern void jsfjimclose_(Int_t *iu);
extern void kzeclr_();
};

typedef struct {
  Int_t jmppjm,histjm,nskpjm;
} COMMON_JMCNTL ;

extern COMMON_JMCNTL jmcntl_;

typedef struct {
  Int_t idebug,idemin,idemax,itest,idrun,idevt,ieorun;
  Int_t ieotri,ievent,iswit[10],ifinit[20],nevent,nrndm[2];
} COMMON_GCFLAG;

extern COMMON_GCFLAG gcflag_;  

#include "JSFReadJIMCommon.h"

ClassImp(JSFJIM)

Int_t gJIMIUnit=4;

//_____________________________________________________________________________
JSFJIM::JSFJIM(const char *name, const char *title, Bool_t constbuf)
       : JSFSIMDST(name,title)
{
  if( constbuf )  fEventBuf = new JSFReadJIMBankBuf("JSFReadJIMBankBuf", 
			       "JSFJIM event buffer",this);
  strcpy(fInputFile,"input.dat");
}


// ---------------------------------------------------------------
Bool_t JSFJIM::Initialize()
{

  Int_t lfInputFile=strlen(fInputFile);
  Int_t iret;
  jsfjimopen_(&gJIMIUnit, fInputFile, &iret, lfInputFile);
  if( iret != 0 ) {
    printf(" JSFJIM::Initialize failed to open a file %s\n",fInputFile);
    return kFALSE;
  }

  //  *--   SET JMPPJM=0 FOR NORMAL JIM.
  jmcntl_.jmppjm=0;

  uginit_();
  return kTRUE;
}

// ---------------------------------------------------------------
Bool_t JSFJIM::BeginRun(Int_t nrun)
{
  return kTRUE;
}

// ---------------------------------------------------------------
Bool_t JSFJIM::Process(Int_t nev)
{
//


  // Trigger one JIM event
  Int_t iret=0;
  jsfjimgrun_(&iret);
  printf(" jsfjimevent returned with iret=%d\n",iret);
  if( iret < 0 ) return kFALSE;

  // Put it into SIMDST buffer.
  JSFReadJIMBankBuf *simb=(JSFReadJIMBankBuf*)EventBuf();
  Bool_t rc=simb->UnpackDST(nev);

  //*-- Delete (ZDROP'S and one ZGARB) all existing banks 
  //*-- and clear bank directory
  kzeclr_();  // This was originally called in guout of JIM.

  return rc;
}

// ---------------------------------------------------------------
Bool_t JSFJIM::EndRun()
{
  return kTRUE;
}


// ---------------------------------------------------------------
Bool_t JSFJIM::Terminate()
{
  uglast_();

  jsfjimclose_(&gJIMIUnit);

  return kTRUE;
}

