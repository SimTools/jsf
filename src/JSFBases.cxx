//*LastUpdate:  v0.3.08 09/29/1998  by A.Miyamoto
//*-- Author :  Akiya Miyamoto  09/22/1998

//////////////////////////////////////////////////////////////////
//
//  JSFBases
//
//  JSF Interfaces to BASES/SPRING 
//
//  User's basis class should be derived from this class. 
//  Typical macro is,
//      {
//       // An example of Bases. 
//       // Calculate cross section of e+e- --> gamma* --> f+f- 
//         gROOT->Reset(); 
//         TFile file("bases.root","RECREATE");  // Output file.
//         bases = new LLbarBases();
//         bases->SetEcm(300.0);            // Set ECM
//         bases->SetParton(1, 2.0/3.0 );   // Set Parton
//         bases->DoBases();
//         bases->Write();
//         file->Write();
//       }
//
//
//////////////////////////////////////////////////////////////////
// 

#include "TNamed.h"
#include "JSFBases.h"
#include "JSFSpring.h"

ClassImp(JSFBases)

extern "C" {
extern void bsinit_();
extern void bsinfo_();
extern void bhplot_();
extern void bases_(const void *adr, double *estim, double *sigma, double *ctime,
		   int *it1, int *it2);
extern void xhinit_(int *id, double *xl, double *, int *nbin,
		    char *title, int len);
extern void dhinit_(int *id, double *xl, double *xu, int *nxbin, 
		    double *yl, double *yu, int *nybin, char *title, int len);
extern void xhfill_(int *id, double *x, double *val);
extern void dhfill_(int *id, double *x, double *y, double *val);
};


JSFBases *gJSFBases;

Double_t Basesfunc(double x[]);
Double_t Basesfunc(double x[])
{
  double val=gJSFBases->Func(x);
  return val; 
}



//_____________________________________________________________________________
JSFBases::JSFBases(const char *name, const char *title)
       : TNamed(name,title)
{
  // Default constructor of JSFBases.
  // Default parameter of derived class should be initialized in the
  // constructor.

  fACC1=-1.0;
  fACC2=-1.0;
  fITMX1=-1;
  fITMX2=-1;
  fNDIM=-1;
  fNWILD=-1;
  fNCALL=-1;   // Negative value means to use default value.

  for(Int_t i=0;i<(Int_t)(sizeof(fIG)/4);i++){
    fIG[i]=1;
    fXU[i]=-1.0e6;
  }

  fPrintInfo=kTRUE;
  fPrintHist=kTRUE;

  fIsInitialized = kFALSE;
  gJSFBases=0;

}

//_____________________________________________________________________________
void JSFBases::Xhinit(Int_t id, Double_t xl, Double_t xu, Int_t nbin, Char_t *title)
{
  Int_t len=strlen(title);
  xhinit_(&id, &xl, &xu, &nbin, title, len);
}

//_____________________________________________________________________________
void JSFBases::Dhinit(Int_t id, Double_t xl, Double_t xu, Int_t nxbin, 
		      Double_t yl, Double_t yu, Int_t nybin, Char_t *title)
{
  Int_t len=strlen(title);
  dhinit_(&id, &xl, &xu, &nxbin, &yl, &yu, &nybin, title, len);
}

//_____________________________________________________________________________
void JSFBases::Xhfill(Int_t id, Double_t x, Double_t val)
{
  xhfill_(&id, &x, &val);
}

//_____________________________________________________________________________
void JSFBases::Dhfill(Int_t id, Double_t x, Double_t y, Double_t val)
{
  dhfill_(&id, &x, &y, &val);
}

//_____________________________________________________________________________
void JSFBases::SetSpring(JSFSpring *spring)
{
// Set Address of spring module
  fSpring=spring;
  if( fSpring->Bases() != this ) fSpring->SetBases(this); 
}

//_____________________________________________________________________________
void JSFBases::SetSpring(Int_t address)
{
// Set Address of spring module
  fSpring=(JSFSpring*)address;
  if( fSpring->Bases() != this ) fSpring->SetBases(this); 
}


//_____________________________________________________________________________
void JSFBases::DoBases()
{
//  Main function to perform Bases. Does initialization, integration
//  and output

  if( !fIsInitialized ) Initialize();

  Userin();   // User initialization.

  fInBases=kTRUE;
  bases_(Basesfunc, &fESTIM, &fSIGMA, &fCTIME, &fIT1, &fIT2);

  printf(" *** BASES completed ***\n");
  printf("     ESTIM = %g +- %g \n",fESTIM, fSIGMA);
  printf("     CTIME = %g  IT1=%d  IT2=%d \n",fCTIME, fIT1, fIT2);

  Print();  // Print result
  
  Userout();           // Does user out process.
     
}

//_____________________________________________________________________________
void JSFBases::Initialize()
{

  if( !gJSFBases ) {
    bsinit_();  // bases initialization 
  }
  gJSFBases=this;
}


//_____________________________________________________________________________
void JSFBases::Userin()
{
//
//   Initialize User parameters for Bases
//

  if( fACC1  > 0.0 ) { bparm2_.acc1=fACC1; }
  else { fACC1=bparm2_.acc1;}
  if( fACC2  > 0.0 ) { bparm2_.acc2=fACC2; }
  else { fACC2=bparm2_.acc2;}
  if( fITMX1 > 0   ) { bparm2_.itmx1=fITMX1; }
  else { fITMX1=bparm2_.itmx1;}
  if( fITMX2 > 0   ) { bparm2_.itmx2=fITMX2; }
  else { fITMX2=bparm2_.itmx2;}

  if( fNDIM  > 0   ) { bparm1_.ndim=fNDIM ;}
  else { fNDIM=bparm1_.ndim;}
  if( fNWILD > 0   ) { bparm1_.nwild=fNWILD ;}
  else { fNWILD=bparm1_.nwild;}
  if( fNCALL > 0   ) { bparm1_.ncall=fNCALL ;}
  else { fNCALL=bparm1_.ncall;}

  for(Int_t i=0;i<bparm1_.ndim;i++){
     bparm1_.xl[i]=fXL[i];
     bparm1_.xu[i]=fXU[i];
     bparm1_.ig[i]=fIG[i];
  }

  return ;
}

//_____________________________________________________________________________
void JSFBases::Userout()
{

}

//_____________________________________________________________________________
void JSFBases::Print()
{
  Int_t luinfo=6;
  if( fPrintInfo ) { bsinfo_(&luinfo); }   // Printout bases information.
  if( fPrintHist ) { bhplot_(&luinfo); }   // Printout bases histogram.
}

//______________________________________________________________________________
void JSFBases::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFBases.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TNamed::Streamer(R__b);

      R__b >> fESTIM;
      R__b >> fSIGMA;
      R__b >> fCTIME;
      R__b >> fIT1;
      R__b >> fIT2;
      R__b >> fACC1;
      R__b >> fACC2;
      R__b >> fITMX1;
      R__b >> fITMX2;
      R__b >> fNDIM;
      R__b >> fNWILD;
      R__b >> fNCALL;
      base2_.acc1=fACC1; base2_.acc2=fACC2 ;
      base2_.itmx1=fITMX1; base2_.itmx2=fITMX2;

      R__b.ReadStaticArray(fXU);
      R__b.ReadStaticArray(fXU);
      R__b.ReadStaticArray(fIG);

      R__b.ReadStaticArray(base1_.xl);
      R__b >> base1_.ndim;
      R__b >> base1_.nwild;
      R__b.ReadStaticArray(base1_.ig);
      R__b >> base1_.ncall;
      R__b >> base3_.scalls;
      R__b >> base3_.wgt;
      R__b >> base3_.ti;
      R__b >> base3_.tsi;
      R__b >> base3_.tacc;
      R__b >> base3_.it;
      
      R__b.ReadStaticArray(base4_.xi);
      R__b >> base4_.nd;
      R__b >> base4_.ng;
      R__b >> base4_.npg;
      R__b.ReadStaticArray(base4_.ma);

      R__b.ReadStaticArray(base5_.itrat);
      R__b.ReadStaticArray(base5_.time);
      R__b.ReadStaticArray(base5_.reslt);
      R__b.ReadStaticArray(base5_.trslt);

      R__b.ReadStaticArray(randm_.rdm);
      R__b.ReadStaticArray(randm_.ia1);

      R__b.ReadStaticArray(ploth_.xhash);
      R__b.ReadStaticArray(plotb_.ibuf);
   } else {
      R__b.WriteVersion(JSFBases::IsA());
      TNamed::Streamer(R__b);
      R__b << fESTIM;
      R__b << fSIGMA;
      R__b << fCTIME;
      R__b << fIT1;
      R__b << fIT2;
      R__b << fACC1;
      R__b << fACC2;
      R__b << fITMX1;
      R__b << fITMX2;
      R__b << fNDIM;
      R__b << fNWILD;
      R__b << fNCALL;
      R__b.WriteArray(fXL,fNDIM);
      R__b.WriteArray(fXU,fNDIM);
      R__b.WriteArray(fIG,fNDIM);

      R__b.WriteArray(base1_.xl,2*kMXDIM);
      R__b << base1_.ndim;
      R__b << base1_.nwild;
      R__b.WriteArray(base1_.ig,kMXDIM);
      R__b << base1_.ncall;
      R__b << base3_.scalls;
      R__b << base3_.wgt;
      R__b << base3_.ti;
      R__b << base3_.tsi;
      R__b << base3_.tacc;
      R__b << base3_.it;
      
      const kLenBase4xi=kNDMX*kNDMX+kMXDIM+2*kLENG;
      R__b.WriteArray(base4_.xi,kLenBase4xi);
      R__b << base4_.nd;
      R__b << base4_.ng;
      R__b << base4_.npg;
      R__b.WriteArray(base4_.ma,kMXDIM);

      R__b.WriteArray(base5_.itrat,2*kITM);
      R__b.WriteArray(base5_.time,7*kITM);
      R__b.WriteArray(base5_.reslt,4*kITM);
      R__b.WriteArray(base5_.trslt,6*kITM);

      R__b.WriteArray(randm_.rdm,33);
      R__b.WriteArray(randm_.ia1,12);

      const kPLOTH=13*(kNHS+1)+14*kNSC+kNHS+1+4*kNHS+1+4*kNSC+1;
      R__b.WriteArray(ploth_.xhash,kPLOTH);

      R__b.WriteArray(plotb_.ibuf,kLenIBUF);
   }
}

