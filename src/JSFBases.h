#ifndef __JSFBases__
#define __JSFBases__

//////////////////////////////////////////////////////////////////////////
//
// JSFBases
//
// Interface to BasesSpring package.
//
//$ID$
//
//////////////////////////////////////////////////////////////////////////

#include "TNamed.h"
#include "TFile.h"
#include "TObjArray.h"
#include "THashTable.h"
#include "BasesSpring.h"

class JSFSpring;

class JSFBasesTempHist : public TNamed
{
 public:
  Bool_t   flag;
  Double_t x;
  Double_t y;
  Double_t wgt;

  JSFBasesTempHist(){flag=kFALSE; x=0; y=0; wgt=0;}
  JSFBasesTempHist(const char *name, const char *title="JSFBasesTempHist"):
    TNamed(name,title){flag=kFALSE; x=0; y=0; wgt=0;}
  virtual ~JSFBasesTempHist(){}

  ClassDef(JSFBasesTempHist,1) //
};


class JSFBases: public BasesSpring, public TNamed
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
