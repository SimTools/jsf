#ifndef __JSFBases__
#define __JSFBases__

//////////////////////////////////////////////////////////////////////////
//
// JSFBases
//
// A general class to run bases in root
//
//////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TMath.h"
#include "TDatime.h"

// #ifndef __JSFSpring__
// #include "JSFSpring.h"
// #endif


// =====================================================================
//  COMMONs for Bases
// =====================================================================
typedef struct {
  Double_t xl[50], xu[50];
  Int_t    ndim, nwild, ig[50], ncall;
} COMMON_BPARM1;             //  Common for BASES

typedef struct {
   Double_t acc1, acc2;
   Int_t    itmx1, itmx2;
} COMMON_BPARM2;             //  Common for BASES


static const Int_t kLenBSRSLT=9;
typedef struct {
   Double_t avgi, sd, chi2a, stime;
   Int_t    itf;
} COMMON_BSRSLT;             //  Common for bases result

static const Int_t kMXDIM=50;
static const Int_t kNDMX=50;
static const Int_t kLENG=32768;
static const Int_t kITM=50;
static const Int_t kNHS=50;
static const Int_t kNSC=50;

typedef struct {
     Double_t xl[kMXDIM], xu[kMXDIM];
     Int_t    ndim, nwild, ig[kMXDIM], ncall;
} COMMON_BASE1; 

typedef struct {
  Double_t acc1,acc2; 
  Int_t    itmx1,itmx2;
} COMMON_BASE2;

typedef struct {
  Double_t scalls, wgt, ti, tsi,tacc;
  Int_t    it;
} COMMON_BASE3;

typedef struct {
  Double_t xi[kNDMX*kNDMX],dx[kMXDIM],dxd[kLENG],dxp[kLENG];
  Int_t nd,ng,npg,ma[kMXDIM];
} COMMON_BASE4;

typedef struct {
  Int_t itrat[2*kITM];
  Float_t time[3*kITM],eff[2][kITM],wrong[2][kITM];
  Double_t reslt[2*kITM],acstd[2][kITM];
  Float_t trslt[2*kITM],tstd[2][kITM],pcnt[2][kITM];
} COMMON_BASE5;

typedef struct {
  Float_t rdm[33];
  Int_t ia1[12];
} COMMON_RANDM;

typedef struct {
  Int_t xhash[13*(kNHS+1)],dhash[14][kNSC],ifbase[kNHS];
  Int_t nhist,mapl[kNHS*4],nscat,mapd[kNSC*4],nw;
} COMMON_PLOTH;

static const Int_t kLenIBUF= 281*kNHS + 2527*kNSC ;
typedef struct {
  Int_t ibuf[ kLenIBUF ];
} COMMON_PLOTB;

extern COMMON_BPARM1 bparm1_;
extern COMMON_BPARM2 bparm2_;
extern COMMON_BSRSLT bsrslt_;
extern COMMON_BASE1 base1_;
extern COMMON_BASE2 base2_;
extern COMMON_BASE3 base3_;
extern COMMON_BASE4 base4_;
extern COMMON_BASE5 base5_;
extern COMMON_RANDM randm_;
extern COMMON_PLOTH ploth_;
extern COMMON_PLOTB plotb_;


class JSFSpring;

// =====================================================================
//  JSFBases class
// =====================================================================
class JSFBases : public TNamed {
protected:
   JSFSpring *fSpring; //!
   Bool_t    fIsInitialized; //!
public:

   Bool_t    fInBases;  //! kTRUE, when func is called by bases.
   Double_t  fESTIM;  // Results of the integration
   Double_t  fSIGMA;  // Its estimate
   Double_t  fCTIME;  // CPU time.
   Int_t     fIT1;    // number of iteration in first step.
   Int_t     fIT2;    // number of iteration in second step.

   Double_t  fACC1;   // precision at 1st step.
   Double_t  fACC2;   // precision at 2nd step.
   Int_t     fITMX1;  // max number of iteration at 1st step.
   Int_t     fITMX2;  // max number of iteration at 2nd step.
   Int_t     fNDIM;   // dimension
   Int_t     fNWILD;  // number of wild variable
   Int_t     fNCALL;

   Double_t  fXL[50];
   Double_t  fXU[50];
   Int_t     fIG[50];

   Bool_t     fPrintInfo;  //!  !=0 to print Bases Information.
   Bool_t     fPrintHist; //!  !=0 to print Bases Historgram.

public:
   JSFBases(const char *name="JSFBases", const char *title="JSFBases");

   virtual ~JSFBases(){ printf(" Bases destructor is called.\n");}

   
   void DoBases();          // Do bases integration
   void Print();            // Print bases information.
     

   virtual void Initialize(); // Bases initialization
   virtual void Userin();   // Bases user initialization
   virtual void Userout();  // Bases user output 
   virtual Double_t Func(Double_t x[])=0; // Bases integration function.

   void Xhinit(Int_t id, Double_t xl, Double_t xu, Int_t nxbin, Char_t *title);
   void Dhinit(Int_t id, Double_t xl, Double_t xu, Int_t nxbin, 
	       Double_t yl, Double_t yu, Int_t nybin, Char_t *title);
   void Xhfill(Int_t id, Double_t x, Double_t val);
   void Dhfill(Int_t id, Double_t x, Double_t y, Double_t val);

   void Xhsave(Int_t lunit, Int_t id);

   
   void SetACC1(const Double_t acc1){ fACC1=acc1; return; }
   void SetACC2(const Double_t acc2){ fACC2=acc2; return; }
   void SetITMX1(const Int_t itmx1){ fITMX1=itmx1; return; }
   void SetITMX2(const Int_t itmx2){ fITMX2=itmx2; return; }

   void SetNDIM(const Int_t ndim){ fNDIM=ndim; return; }
   void SetNWILD(const Int_t nwild){ fNWILD=nwild; return; }
   void SetNCALL(const Int_t ncall){ fNCALL=ncall; return; }
   
   void SetLimit(const Int_t i, const Double_t xl, const Double_t xu){
     fXL[i]=xl ; fXU[i]=xu; return ; }

   virtual void SetSpring(JSFSpring *spring);  // Set Spring address
   virtual void SetSpring(Int_t address);  // Set Spring address
   virtual JSFSpring *Spring(){ return fSpring;}

   ClassDef(JSFBases,1)  // BASES basic class
};


#endif
