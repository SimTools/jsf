#ifndef __JSFBases__
#define __JSFBases__

//////////////////////////////////////////////////////////////////////////
//
// JSFBases
//
// Interface to BasesSpring package.
//
//$Id:
//
//////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TFile.h"
#include "TObjArray.h"
#include "THashTable.h"
#include "BasesSpring.h"
#include "TH1.h"
#include "TH2.h"

class JSFSpring;
class BasesSpring;

class JSFBasesTempHist : public TNamed
{
 protected:
  //  static const Int_t  fMax=20;
  static Int_t  fMax;
  Int_t    fCall;
  Double_t fX[20];
  Double_t fY[20];
  Double_t fW[20];
 public:
  JSFBasesTempHist(){fCall=0;}
  JSFBasesTempHist(const char *name, const char *title="JSFBasesTempHist"):
    TNamed(name,title){fCall=0;}
  virtual ~JSFBasesTempHist(){}


  inline Int_t GetNcalls(){ return fCall; }
  inline void Fill(Double_t x, Double_t w){ 
    if( TestCall() ) { fX[fCall]=x; fW[fCall]=w ; fCall++;}  }
  inline void Fill(Double_t x, Double_t y, Double_t w){ 
    if( TestCall() ) { fX[fCall]=x; fY[fCall]=y; fW[fCall]=w ; fCall++; } }
  inline void Reset(){ fCall=0;}
  inline void Update(TH1D* h){ 
    for(Int_t i=0;i<fCall;i++){ h->Fill(fX[i]); }
    fCall=0;
  }
  inline void Update(TH2D* h){ 
    for(Int_t i=0;i<fCall;i++){ h->Fill(fX[i],fY[i]); } 
    fCall=0;
  }

  inline Bool_t TestCall(){
    if( fCall < fMax ) return kTRUE; 
    printf("Too mnay calls of H1FILL/H2FILL in a Bases function.");
    printf("it must be less than %d\n",fMax);
    return kFALSE;
  }

  ClassDef(JSFBasesTempHist,1) //
};


class JSFBases: public TNamed, public BasesSpring
{
 protected:
   JSFSpring *fSpring; //!
   Bool_t    fIsSpring; //! kTRUE when Spring is called.
   THashTable *fBSHash1; //!  Hash table for Bases TH1F histogram
   THashTable *fBSHash2; //!  Hash table for Bases TH2F histogram
   THashTable *fSPHash1; //!  Hash table for Spring TH1F histogram
   THashTable *fSPHash2; //!  Hash table for Spring TH2F histogram

   THashTable *fTempHash1; //!
   THashTable *fTempHash2; //!

 public:
   JSFBases(const char *name="JSFBases", const char *title="JSFBases");
   virtual ~JSFBases();

   virtual void SetSpring(JSFSpring *spring);  // Set Spring address
   virtual JSFSpring *GetSpring(){ return fSpring;}

   void CopyHists(TFile *f, TDirectory *dest=NULL);

//  For defining parameters for BasesSpring
    void   Param( int sample, double tune, int itr1, 
		  double ac1, int itr2, double ac2 );
    void   SetNoOfSample( int sample );
    void   SetTuneValue ( double tune );
    void   SetIteration1( double ac1, int itx1 );
    void   SetIteration2( double ac2, int itx2 );
    void   DefineVariable( double &v, double low, double high, int wld, int grd );
    void   DefineVariable( int ndim, int nwild, 
                                    double x_l[], double x_u[], int jg[] );
    virtual void Userin() {};
//  For integration  and evet generation
    void   Bases( );
    void   Spring( int mxtry );
    void   Sp_info( );

//  Utilities
    void   Forder( float v, float &fv, float &order, int &iorder );
    Double_t GetWeight( );
    Double_t GetScalls( );
    Int_t    Flag_bases( );
    Double_t GetEstimate( );
    Double_t GetError( );
    Int_t    GetNoOfIterate( );

// Histograms
    void  H1Init(Char_t *hn, Char_t *title, Int_t nbin, Double_t xlow, Double_t xhigh);
    void  H1Fill(Char_t *hn, Double_t x, Double_t fx );
    void  H2Init(Char_t *hn, Char_t *title, Int_t nbinx, Double_t xlow, Double_t xhigh,
                  Int_t nbiny, Double_t ylow, Double_t yhigh);
    void  H2Fill(Char_t *hn, Double_t x, Double_t y, Double_t fx );
    void  EndIntegration(Int_t step);

// For histogramming package
    void   Xh_init( int id, double x_low, double x_high, int bin, char title[] );
    void   Dh_init( int id, double x_low, double x_high, int Xbin,
		    double y_low, double y_high, int Ybin, char title[] );
    void   Bh_plot( );
    void   Xh_fill( int id, double x, double fx );
    void   Dh_fill( int id, double x, double y, double fx );
    void   Bh_save( );
    void   Sh_reset( );
    void   sh_reset( );
    void   Sh_update( );
    void   sh_update( );
    void   Sh_plot( );

    virtual double Func( double x[] );
    virtual double Func( );

    double func( double x[] );
    double func();

    ClassDef(JSFBases,3) // Multi-dimensional integration and generation

};

#endif
