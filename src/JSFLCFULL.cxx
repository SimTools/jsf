//*LastUpdate :  jsf-1-9  10-May-1999  By Akiya Miyamoto
//*LastUpdate :  jsf-1-4  11-Feburary-1999  By Akiya Miyamoto
//*-- Author  : A.Miyamoto  11-September-1998

///////////////////////////////////////////////////////////////////
//
//  JSF LCFULL
//
//  Interface to execute LCFULL in ROOT environment.
//  This routine does (1) TBS initialize
//  
//$Id:  
//  
//////////////////////////////////////////////////////////////////

#include <strings.h>
#include "JSFLCFULL.h"

extern "C" {
extern void prinit_root_();
extern void prjobi_();
extern void prjobt_();
extern void prgevt_(int *idrec);
extern void pruhed_(int *idrec);

extern void tbget_(int *id, const char *bankname, int *elem, int *nw, 
		   int data[], int *iret, int len1);
extern void tbnoel_(int *id, const char *bankname, int *nelm, 
		    int  nary[], int len);
extern void tbput_(int *id, const char *bname, int *ne, int *nw,
		   int data[], int *iret, int len);
extern void tbcrte_(int *id, const char *bname, int *isaloc, 
		    int *isext, int *iret, int len);
extern void tbdelb_(int *id, const char *bname, int *iret, int len);
extern void tblbnm_();
extern void tblist_(int *id);
extern void tblsub_();
extern void tblsum_();

extern void jclust_(Int_t *numprt, Float_t *prtlst, const Int_t *lngdat,
		    const Int_t *ipx0, const Int_t *maxcut, Float_t *ycut,
		    Int_t *numcls, Int_t *numjet, Int_t *jettbl,
		    Float_t *jetlst, Int_t *status);
extern void spher_(Int_t *numprt, Float_t *prtlst, const Int_t *lngdat, 
		   const Int_t *ipx0, 
		   Float_t *sph, Float_t *apl, Float_t *eigvct);
extern void thrust_(Int_t *numprt, Float_t *prtlst, const Int_t *lngdat, 
		    const Int_t *ipx0, 
		   Float_t *thr, Float_t *obl, Float_t *eigvct);

extern void eprobx_(Double_t *x, const Int_t *ntyp, Double_t *xbeam);
};



typedef struct {
  Int_t rtimlm, rcpumx,
        nevmax, ndfreq, ievseq,
        nfstev, nlstev, nfstdb, nlstdb, nfstot, nlstot,
        kllvlr, kllvlp,
        nfstex, nlstex, nfstrn, nlstrn;
} COMMON_PRJCTL;

typedef struct {
  Int_t nrdrec,nwtrec;
  Int_t nstrec[10][6];
  Int_t mrdrec,mwtrec;
  Int_t mstrec[10][6];
} COMMON_PRRSTA;

typedef struct {
  Int_t nctbgn, nctend;
  Int_t nstevt[64][5];
  Int_t mctbgn, mctend;
  Int_t mstevt[64][5];
} COMMON_PRESTA ;

typedef struct {
  Int_t xtword, xerror, xrectp, xexpno, xrunno,
        xevtno, xdate,  xtime,  xtdacu, xruntp,
        xcrew , xfmerr, xevtid, xdummy[3],
        xerrs1, xerrs2;
} COMMON_PRHEAD;

typedef struct {
  Int_t njblvl[3][30], njbdbg[3][30];
} COMMON_PRJFLG;

typedef struct {
  Int_t ninuni, ninfil, iinfil,
        notuni, notfil, iotfil, notdev;
  Float_t xosize, zosize;
} COMMON_PRFCTL;

typedef struct {
  Int_t numgen, iumgen;
} COMMON_PRGENF;

typedef struct {
  Int_t ndynat, nowexp, nowrun, nxtfil, nowpnt,
        numdyn, nrndyn[50][3],
        nfinfo[50][4], nofall, nofinf[50][5];
} COMMON_PRDYNA;


extern COMMON_PRJCTL prjctl_;
extern COMMON_PRRSTA prrsta_;
extern COMMON_PRESTA presta_;
extern COMMON_PRHEAD prhead_;
extern COMMON_PRJFLG prjflg_;
extern COMMON_PRFCTL prfctl_;
extern COMMON_PRGENF prgenf_;
extern COMMON_PRDYNA prdyna_;

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
void JSFLCFULL::SetNumgen(Int_t ng)
{ 
    prgenf_.numgen=ng ; prgenf_.iumgen=0 ; 
    prdyna_.ndynat=-1 ; return ; 
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

//_____________________________________________________________________________
void JSFLCFULL::JCLUST(Int_t numprt, Float_t *prtlst, const Int_t lngdat,
		       const Int_t ipx0, const Int_t maxcut, 
		       Float_t ycut[], Int_t numcls[], Int_t numjet[],
		       Int_t jettbl[], Float_t *jetlst, Int_t &status)
{
  //  Do Jet clustering using JADE algorithm

/*
C----------
C JCLUST
C----------
C
C JADE_CLUSTER
C     CLUSTER FINDING PROGRAM USED BY JADE COLLABORATION
C     IN "EXPERIMETAL STUDIES ON MULTIJET PRDCTION IN
C     E+E- ANNIHILATION AT PETRA ENERGIES"
C
C MODIFIED BY A.SHIRAHASHI, UNIV. OF TOKYO, 26-NOV-87
C   * SUBROUTINE NAME HAS BEEN CHANGED FROM 'JADECL' TO 'JCLUST'.
C   * PARTICLE LIST IS TRANSFERED IN 'YAMAMOTO LIBRARY' FORMAT.
C   * REMOVE DUMMY ARGUMENT 'MAXJET'.
C   * REMOVE AN ARGUMENT 'YIJ'.
C   * RETURN 'JETTBL' FOR YCUT(MAXCUT).  ( JETTBL(I) IS THE INDEX OF
C     THE JET TO WHICH I'TH PARTICLE IS ASSIGNED.  THE SIZE OF ARRAY
C     JETTBL SHOULD BE GREATER THAN 2*NUMPRT. )
C   * NUMJET(I) CONTAINS # PARTICLES IN I'TH JET FOR YCUT(MAXCUT).
C   * EXPAND FUNCTION 'ANAPRO' IN LINE.
C
C MODIFIED BY A.SHIRAHASHI, UNIV. OF TOKYO, 4-DEC-87
C   * TAKE INTO ACCOUNT THE MASSES OF PARTICLES AND CLUSTERS.
C   * THE ENERGY OF THE CLUSTER IS CALCULATED AS THE SUM OF TWO SUB-
C     CLUSTERS.
C   * THE ENERGY OF EACH PARTICLE SHOULD BE GIVEN IN PRTLST(IPX0+4,I)
C     BY THE USER.
C   * THIS VERSION IS COMPATIBLE WITH THE ALGORITHM USED BY JADE.
C   * USE LOCAL WORK AREA 'JETWOW' AND 'JETIDX' AND THE SIZE OF ARRAY
C     'JETTBL' MAY BE REDUCED TO NUMPRT.
C
C MODIFIED BY A.SHIRAHASHI, UNIV. OF TOKYO, 9-DEC-87
C   * STORE PAIR MASSES INTO ARRAY INVMAS AND REUSE THEM.
C
C   INPUT:
C     INTEGER * 4  NUMPRT
C     REAL    * 4  PRTLST( LNGDAT,NUMPRT )
C     INTEGER * 4  LNGDAT
C     INTEGER * 4  IPX0
C     INTEGER * 4  MAXCUT
C     REAL    * 4  YCUT( MAXCUT )
C
C     PRTLST : FOUR VECTORS OF PARTICLES.
C              PRTLST(IPX0+J,I) = PX, PY, PZ AND E OF I'TH PATICLES
C              FOR J = 1, 4.
C
C   OUTPUT:
C     INTEGER * 4  NUMCLS( MAXCUT )
C     INTEGER * 4  NUMJET( * )
C     INTEGER * 4  JETTBL( NUMPRT )
C     REAL    * 4  JETLST( 4,NUMPRT )
C     INTEGER * 4  STATUS
C
C
      SUBROUTINE JCLUST( NUMPRT, PRTLST, LNGDAT, IPX0,
     &                   MAXCUT, YCUT, NUMCLS,
     &                   NUMJET, JETTBL, JETLST, STATUS )
C
C
C INPUT:
      INTEGER * 4  NUMPRT, LNGDAT, IPX0
      REAL    * 4  PRTLST( LNGDAT,* )
C
      INTEGER * 4  MAXCUT
      REAL    * 4  YCUT(MAXCUT)
C
C OUTPUT:
      INTEGER * 4  NUMCLS(MAXCUT)
      INTEGER * 4  NUMJET( * )
      INTEGER * 4  JETTBL( * )
      REAL    * 4  JETLST( 4,* )
      INTEGER * 4  STATUS
C
*/

  jclust_(&numprt, prtlst, &lngdat, &ipx0, &maxcut, ycut,
	  numcls, numjet, jettbl, jetlst, &status);

}

//_______________________________________________________________
void JSFLCFULL::SPHER(Int_t numprt, Float_t *prtlst, const Int_t lngdat,
		      const Int_t ipx0, 
		      Float_t &sph, Float_t &apl, Float_t eigvct[3][4])

{

  // C...FUNCTION: CALCULATE SPHERICITY AND APLANALITY                               

/*
C-----------------------------------------------------------------------        
      SUBROUTINE SPHER ( NUMPRT, PRTLST, LNGDAT, IPX0,
     .                  SPH   , APL   , EIGVCT        )

C...FUNCTION: CALCULATE SPHERICITY AND APLANALITY                               

C...INPUTS:                                                                     
      INTEGER     NUMPRT, LNGDAT, IPX0                                          
      REAL*4      PRTLST(LNGDAT,1), RVAL                                        
                                                                                
C...OUTPUTS:                                                                    
      REAL*4      SPH, APL, EIGVCT(0:3,3)                                       
                                                                                
C...ERROR FLAGS:                                                                
C     SPH = APL = -1                                                            
                                                                                
C...MODIFIED VER. OF LUSPHER IN THE LUND MONTE CARLO V.4.3.                     
C     ALL INPUTS AND OUTPUTS ARE TRANSFERED BY SUBROUTINES CALLS.               
C     MODIFIED BY HIRO YAMAMOTO ON FEB. 26, 1983                                
                                                                                
C...ARGUMENTS: SUBROUTINE SPHER                                                 
C     NUMPRT : # PARTICLES FOR ANALYSIS FILLED IN PRTLST. ( :== NP )            
C     PRTLST : MOMENTUM LIST OF PARTICLES.                                      
C              PRTLST(IPX0+J,I) = PX, PY, PZ OF I'TH PATICLES FOR J = 1 , 3     
C              ( PRTLST(IPX0+J,I) :== P(J,I') FOR J = 1 , 3 WHEN I AND I'       
C                CORESPONDS TO THE SAME PARTICLE. )                             
C     LNGDAT : DATA LENGTH OF ONE PARTICLE IN PRTLST                            
C     IPX0   : BASE FOR THE THREE MOMENTUM IN PRTLST.                           
C              PX = PRTLST(IPX0+1), PY = PRTLST(IPX0+2), PZ = PRTLST(IPX0+3)    
C     SPH    : SPHERICITY = 3(EIGVCT(0,2)+EIGVCT(0,3))/2                        
C     APL    : APLANALITY = 3 EIGVCT(0,3) / 2                                   
C     EIGVCT : EIGEN VECTOR OF SPHERICITY TENSOR.                               
C              EIGVCT(0,J) : J'TH EIGEN VALUE                                   
C              EIGVCT(I,J) : J'TH EIGEN VECTOR                                  
C                 SPHERICITY TENSOR(I,J) = SUM[ P(A,I)*P(A,J)*P(A)**(R-2) ]     
C                                        / SUM[ P(A)**R ]                       
C                   WHERE P(A,I) IS THREE MOMENTUM OF PARTICLE A,               
C                   P(A) IS MOMENTUM OF A AND R IS A PARAMETER(:==PARE(30)).    
C                 EIGVCT(0,1) > EIGVCT(0,2) > EIGVCT(0,3)                       

*/

  spher_(&numprt, prtlst, &lngdat, &ipx0, &sph, &apl, (Float_t*)eigvct);

}

//_______________________________________________________________________
void JSFLCFULL::THRUST(Int_t numprt, Float_t *prtlst, const Int_t lngdat,
		      const Int_t ipx0, 
		      Float_t &thr, Float_t &obl, Float_t eigvct[3][4])
{
/*
C***********************************************************************        
C-----------------------------------------------------------------------        
      SUBROUTINE THRUST( NUMPRT, PRTLST, LNGDAT, IPX0,                          
     .                   THR   , OBL   , EIGVCT        )                        
C-----------------------------------------------------------------------        
                                                                                
C...FUNCTION: CALCURATE THRUST AND OBLATENESS.                                  
                                                                                
C...INPUTS:                                                                     
      INTEGER     NUMPRT, LNGDAT, IPX0                                          
      REAL        PRTLST(LNGDAT,1), RVAL                                        
      CHARACTER*3 COMAND                                                        
      CHARACTER*6 PARAM                                                         
                                                                                
C...OUTPUTS:                                                                    
      REAL        THR, OBL, EIGVCT(0:3,3)                                       
                                                                                
C...ERROR FLAGS:                                                                
C     THR = OBL = -1                                                            
                                                                                
C...MODIFIED VER. OF LUTHRU IN THE LUND MONTE CARLO V.4.3.                      
C      ALL INPUTS AND OUTPUTS ARE TRANSFERED BY SUBROUTINES CALLS.              
C      IN THE ORIGINAL PROGRAMS, IT HAPPENS THAT THE MAJOR BECOMES              
C      SMALLER THAN THE MINOR WHEN ACCURACY OR ITERATIONS ARE NOT               
C      ENOUGH.  IN THIS VERSION, THE MAXIMUM SEARCH IS CONTINUED                
C      UNTIL MAJOR BECOMES LARGER THAN MINOR.  IF THE ORDER IS                  
C      STILL REVERSED WHEN ALL INITIAL DIRECTIONS ARE USED TO SEARCH,           
C      MAJOR AND MINOR ARE EXCHANGED.                                           
C      MODIFIED BY HIRO YAMAMOTO ON FEB. 23, 1983.                              
                                                                                
C...ARGUMENTS OF SUBROUTINE THRUST:                                             
C     NUMPRT : # PARTICLES FOR ANALYSIS FILLED IN PRTLST. ( :== NP )            
C     PRTLST : MOMENTUM LIST OF PARTICLES.                                      
C              PRTLST(IPX0+J,I) = PX, PY, PZ OF I'TH PATICLES FOR J = 1 , 3     
C              ( PRTLST(IPX0+J,I) :== P(J,I') FOR J = 1 , 3 WHEN I AND I'       
C                CORESPONDS TO THE SAME PARTICLE. )                             
C     LNGDAT : DATA LENGTH OF ONE PARTICLE IN PRTLST                            
C     IPX0   : BASE FOR THE THREE MOMENTUM IN PRTLST.                           
C              PX = PRTLST(IPX0+1), PY = PRTLST(IPX0+2), PZ = PRTLST(IPX0+3)    
C     THR    : THRUST                                                           
C     OBL    : OBLATENESS                                                       
C     EIGVCT : EIGEN VECTOR                                                     
C              EIGVCT(I,J) = THRUST AXIS FOR J = 1.                             
C                            MAJOR  AXIS FOR J = 2.                             
C                            MINOR  AXIS FOR J = 3.                             
C                            THRUST VALUE IN THAT DIRECTION FOR I = 0.          
C                            NORMALIZED DIRECTION VECTOR FOR I = 1 , 3.         
C              EIGVCT(0,J) :== P(N+J,4), EIGVCT(I,J) :== P(N+J,I) FOR J = 1 , 3 
                                                                                
C...ARGUMENTS OF ENTRY THRPRM:                                                  
C     COMAND : CONTROL TO GET OR PUT PARAMETER VALUE.                           
C              COMAND = 'GET' RETURNS CURRENT PARAMETER VALUE.                  
C              COMAND = 'PUT' RESETS PARAMETER VALUE.                           
C     PARAM  : IDENTIFY PARAMETER TO CHANGE.                                    
C              PARAM = 'MSTE15', 'PARE31', 'PARE34'                             
C     RVAL   : VALUE OF MSTE(15), PARE(31), PARE(34)                            
C              BE CAREFULL THAT ALL PARAMETERS ARE PASSED BY REAL*4 TYPE.       
*/

  thrust_(&numprt, prtlst, &lngdat, &ipx0, &thr, &obl, (Float_t*)eigvct);
}

//________________________________________________________
void JSFLCFULL::EPROBX(Double_t x, const Int_t ntyp, Double_t &xbeam)
{
  //
/*
C**********************************************************************
C*
C*  -----------------------------=====
C*   SUBROUTINE  EPROBX(X, NTYP, XBEAM)
C*  -----------------------------=====
C*(Function)
C*   Generate Beam energy
C*   E at the collision = Ebeam*Xbeam
C*
C*(Input)
C*   X  : Integral variable, 0 < X < 1
C*   NTYP  : 1 = Ebeam = 250GeV
C*           2 =       = 500GeV
C*           3 =       = 750GeV
C*          11 = Ebeam = 250GeV with 0.5% initial beam energy spread.
C*          12 =       = 500GeV           (Gaussian)
C*          13 =       = 750GeV
C*          21 = Ebeam = 250GeV with 1%   initial beam energy spread.
C*          22 =       = 500GeV           (Full width, Sqauare)
C*          23 =       = 750GeV
C*          31 = JLC-I Sband 150 GeV, 0.01% width
C*          32 =       Cband   
C*          33 =       Xband 
C*          34 = JLC-I Sband 250 GeV, 0.01% width
C*          35 =       Cband   
C*          36 =       Xband 
C*          41 = JLC-I Sband 150 GeV, 0.5% width
C*          42 =       Cband   
C*          43 =       Xband 
C*          44 = JLC-I Sband 250 GeV, 0.5% width
C*          45 =       Cband   
C*          46 =       Xband 
C*(Output)
C*   EPROBX  : function to generate beam spectrum.
C*             0 < EPROBX < 1.025 ( Corresponds to 5 sigma when
C*                                  sigma is 0.5%.)
C*(Required subroutine)
C*   Sub. UDSRCH ; Do binary search of real*8 variable.
C*
C*(Author)
C*   A. Miyamoto  16-Oct-1990  Original version.
C*                11-Jun-1991  For JLC parameter on May 1991.
C*                31-Jul-1991  Add an option, 1% square Ebeam spread
C*                 6-Feb-1993  Add JLC-I parameters
C*
C**********************************************************************
*/
  eprobx_(&x, &ntyp, &xbeam);
}
