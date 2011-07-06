//*LastUpdate:  jsf-1-15 4-Feburary-2000
//*LastUpdate:  jsf-1-14 11-Feburary-2000
//*LastUpdate:  jsf-1-9 17-May-1999 Akiya Miyamoto
//*LastUpdate:  jsf-1-7 8-April-1999 Akiya Miyamoto
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
//         bases->Bases();
//         bases->Write();
//         file->Write();
//       }
//
// (Update)
//   8-Apr-1999 A.Miyamoto  forgetted Int_t is inserted in consit of 
//              function Streamer.
//  17-May-1999 A.Miyamoto  Add Xhsave function.
//  24-Jul-1999 A.Miyamoto  Environment variable, JSFBases.RandomSeed 
//              is added to set seed of random variable.
//  11-Feb-2000 A.Miyamoto  Uses new C++ BasesSpring library 
//   3-Jul-2000 A.Miyamoto  Output 2D bases histogram in SPRING
//                          In spring, allow multiple H1FILL call per functions. 
//$Id$
//
//////////////////////////////////////////////////////////////////
// 

#include "TH1.h"
#include "TH2.h"
#include "THashTable.h"
#include "TIterator.h"
#include "TKey.h"

#include "BasesSpring.h"
#include "JSFBases.h"
#include "JSFSpring.h"

ClassImp(JSFBases)
ClassImp(JSFBasesTempHist)

Int_t JSFBasesTempHist::fMax=20;

//_____________________________________________________________________________
JSFBases::JSFBases(const char *name, const char *title)
       : TNamed(name,title), BasesSpring()
{
  fIsSpring = kFALSE;
  fBSHash1=new THashTable();
  fBSHash2=new THashTable();
  fSPHash1=new THashTable();
  fSPHash2=new THashTable();
  fTempHash1=new THashTable();
  fTempHash2=new THashTable();
}


//_____________________________________________________________________________
JSFBases::~JSFBases()
{
  if( fBSHash1 ) fBSHash1->Delete();
  if( fBSHash2 ) fBSHash2->Delete();
  delete fBSHash1;
  delete fBSHash2;

  if( fSPHash1 ) fSPHash1->Delete();
  if( fSPHash2 ) fSPHash2->Delete();
  delete fSPHash1;
  delete fSPHash2;

  if( fTempHash1 ) fTempHash1->Delete();
  if( fTempHash2 ) fTempHash2->Delete();

  delete fTempHash1;
  delete fTempHash2;

}

//_____________________________________________________________________________
void JSFBases::SetSpring(JSFSpring *spring)
{
// Set Address of spring module
  fSpring=spring;
  if( fSpring->GetBases() != this ) fSpring->SetBases(this); 
}

//_________________________________________________________
void JSFBases::Param(int sample, double tune, int itr1, 
		  double ac1, int itr2, double ac2 )
{
  //*         ===================================================                *
  //******************************************************************************
  //* ( Function )                                                               *
  //*   To set the bases parameters.                                             *
  //* ( Output = Input )                                                         *
  //*   ncall  = sample  : the number of sampling points per iteration ( given ) *
  //*   itmx1  = itr1    : maximum number of iterations for grid opt. step       *
  //*   acc1   = ac1     : required accuracy for grid opt. step                  *
  //*   itmx2  = itr2    : maximum number of iterations for integration step     *
  //*   acc2   = ac2     : required accuracy for integration step                *
  //*   coded by S.kawabata June '99 at KEK                                      *
  //******************************************************************************/

  param(sample, tune, itr1, ac1, itr2, ac2);
}

//_________________________________________________________
void JSFBases::SetNoOfSample( int sample )
{ 
  setNoOfSample(sample); 
}

//_________________________________________________________
void JSFBases::SetTuneValue ( double tune )
{ 
  setTuneValue(tune); 
}

//_________________________________________________________
void JSFBases::SetIteration1( double ac1, int itx1 ) 
{ 
  setIteration1(ac1, itx1); 
}

//_________________________________________________________
void JSFBases::SetIteration2( double ac2, int itx2 ) 
{ 
  setIteration2(ac2, itx2);
}

//_________________________________________________________
void JSFBases::DefineVariable( double &v, double low, double high, int wld, int grd )
{
  defineVariable(v, low, high, wld, grd); 
}

//_________________________________________________________
void JSFBases::DefineVariable( int ndim, int nwild, 
                                    double x_l[], double x_u[], int jg[] )
{
  defineVariable(ndim, nwild, x_l, x_u, jg );
}

//_________________________________________________________
void JSFBases::Bases( ) 
{ 
  fIsSpring=kFALSE;
  Userin();
  BasesSpring::Bases(); 
}

//_________________________________________________________
void JSFBases::Spring( int mxtry )
{ 
  fIsSpring=kTRUE;
  BasesSpring::Spring(mxtry); 
}

//_________________________________________________________
void JSFBases::Sp_info( )
{ 
  sp_info(); 
}

//  Utilities
//_________________________________________________________
void JSFBases::Forder( float v, float &fv, float &order, int &iorder )
{
  forder( v, fv, order, iorder );
}

//_________________________________________________________
Double_t JSFBases::GetWeight( )
{ 
  return getwgt(); 
}

//_________________________________________________________
Double_t JSFBases::GetScalls( )
{ 
  return getscalls(); 
}

//_________________________________________________________
Int_t JSFBases::Flag_bases( )
{ 
  return BasesSpring::Flag_bases(); 
}

//_________________________________________________________
Double_t JSFBases::GetEstimate( )
{ 
  return getEstimate(); 
}

//_________________________________________________________
Double_t JSFBases::GetError( )
{ 
  return getError(); 
}

//_________________________________________________________
Int_t JSFBases::GetNoOfIterate( )
{ 
  return getNoOfIterate(); 
}

// For histogramming package
//_________________________________________________________
void JSFBases::H1Init(const Char_t *hn, const Char_t *title, Int_t nbin, Double_t xlow, Double_t xhigh)
{ 
// Create histogram object to save Bases and Spring result
//
  Char_t hstn[128];
  sprintf(hstn,"%sBS",hn);
  TH1D *hst=new TH1D(hstn, title, nbin, xlow, xhigh);
  fBSHash1->Add(hst);
  sprintf(hstn,"%sSP",hn);
  hst=new TH1D(hstn, title, nbin, xlow, xhigh);
  fSPHash1->Add(hst);

  JSFBasesTempHist *th=new JSFBasesTempHist(hn);
  fTempHash1->Add(th);

}

//_________________________________________________________
void JSFBases::H1Fill(const Char_t *hn, Double_t x, Double_t fx)
{ 
// Fill Histogram

  if( fIsSpring ) { 
    JSFBasesTempHist *th=((JSFBasesTempHist*)(fTempHash1->FindObject(hn)));
    th->Fill(x, fx*GetWeight() );
  }
  else { 
    if( Flag_bases() > 1 ) { // Accumulate only during second integration loop
      Char_t hstn[128];
      const Char_t *hnsuf="BS";
//      hnsuf="BS"; 
      sprintf(hstn,"%s%s",hn,hnsuf);
      ((TH1D*)(fBSHash1->FindObject(hstn)))->Fill(x, fx*GetWeight());
    }
  }

}

//_________________________________________________________
void JSFBases::H2Init(const Char_t *hn, const Char_t *title, Int_t nbinx, Double_t xlow, Double_t xhigh,
		      Int_t nbiny, Double_t ylow, Double_t yhigh)
{ 
  Char_t hstn[128];
  sprintf(hstn,"%sBS",hn);
  TH2D *hst=new TH2D(hstn, title, nbinx, xlow, xhigh, nbiny, ylow, yhigh);
  fBSHash2->Add(hst);
  sprintf(hstn,"%sSP",hn);
  hst=new TH2D(hstn, title, nbinx, xlow, xhigh, nbiny, ylow, yhigh);
  fSPHash2->Add(hst);

  JSFBasesTempHist *th=new JSFBasesTempHist(hn);
  fTempHash2->Add(th);

}

//_________________________________________________________
void JSFBases::H2Fill(const Char_t *hn, Double_t x, Double_t y, Double_t fx)
{ 
  if( fIsSpring ) { 
    JSFBasesTempHist *th=((JSFBasesTempHist*)(fTempHash2->FindObject(hn)));
    th->Fill(x, y, fx*GetWeight() );
  }
  else { 
    if( Flag_bases() > 1 ) { // Integrate only in 2nd integration loop.
      Char_t hstn[128];
      const Char_t *hnsuf="BS";
//      hnsuf="BS"; 
      sprintf(hstn,"%s%s",hn,hnsuf);
      ((TH2D*)(fBSHash2->FindObject(hstn)))->Fill(x, y, fx*GetWeight());
    }
   }
}

//_________________________________________________________
void JSFBases::EndIntegration( Int_t step )
{ 
  if( step == 0 ) {
    /*
    TIterator *hst=fBSHash1->MakeIterator();
    TH1D *h1;
    while( (h1=(TH1D*)hst->Next()) ){
      h1->Reset();
    }

    hst=fBSHash2->MakeIterator();
    TH2D *h2;
    while( (h2=(TH2D*)hst->Next()) ){
      h2->Reset();
    }
    */

  }
  else {
    TIterator *hst=fBSHash1->MakeIterator();
    TH1D *h1;
    Double_t sc=1./GetScalls();
    while( (h1=(TH1D*)hst->Next()) ){
      Double_t fact=sc/h1->GetBinWidth(1);
      h1->Scale(fact);
    }

    hst=fBSHash2->MakeIterator();
    TH2D *h2;
    while( (h2=(TH2D*)hst->Next()) ){
      Double_t fact=sc/h2->GetBinWidth(1);
      h2->Scale(fact);
    }
  }

}

//_________________________________________________________
void JSFBases::CopyHists(TFile *f, TDirectory *dest)
{
  // Copy bases hists in f to the directory dest

  TDirectory *now=0;
  if( dest ) { now=gDirectory; dest->cd(); }

  TList *klst = f->GetListOfKeys();
  TListIter nkey(klst);
  TKey  *key;
  while ((key = (TKey*)nkey())) {
    if( strcmp(key->GetClassName(),"TH1D") == 0 ) {
      Int_t ln=strlen(key->GetName());
      const Char_t *hn=key->GetName();
      if( *(hn+ln-1) == 'S' && *(hn+ln-2) == 'B' ) {
	TH1D *hnew=(TH1D*)(fBSHash1->FindObject(hn));
	if( hnew ) hnew->Add((TH1D*)f->Get(key->GetName()));
	else {
	  printf("Warning in JSFBases::CopyHists\n");
	  printf("Bases histogram, %s, in the file, %s",hn,f->GetName());
	  printf(" is not defined in the current job.\n");
	}
      }
    }	    
    else if( strcmp(key->GetClassName(),"TH2D") == 0 ) {
      Int_t ln=strlen(key->GetName());
      const Char_t *hn=key->GetName();
      if( *(hn+ln-1) == 'S' && *(hn+ln-2) == 'B' ) {
	TH2D *hnew=(TH2D*)(fBSHash2->FindObject(hn));
	if( hnew ) hnew->Add((TH2D*)f->Get(key->GetName()));
	else {
	  printf("Warning in JSFBases::CopyHists\n");
	  printf("Bases histogram, %s, in the file, %s",hn,f->GetName());
	  printf(" is not defined in the current job.\n");
	}
      }
    }	    
  }

  if( dest ) { now->cd(); }

}

// For histogramming package
//_________________________________________________________
void JSFBases::Xh_init( int id, double x_low, double x_high, int bin, char title[] )
{ 
  xh_init( id, x_low, x_high, bin, title ); 
}

//_________________________________________________________
void JSFBases::Dh_init( int id, double x_low, double x_high, int Xbin,
                            double y_low, double y_high, int Ybin, char title[] )
{ 
  dh_init( id, x_low, x_high, Xbin, y_low, y_high, Ybin, title ); 
}

//_________________________________________________________
void JSFBases::Bh_plot( )
{ 
  bh_plot( ); 
}

//_________________________________________________________
void JSFBases::Xh_fill( int id, double x, double fx )
{ 
  xh_fill( id, x, fx ); 
}

//_________________________________________________________
void JSFBases::Dh_fill( int id, double x, double y, double fx )
{ 
  dh_fill( id, x, y, fx ); 
}

//_________________________________________________________
void JSFBases::Bh_save( )
{ 
  bh_save(); 
}

//_________________________________________________________
void JSFBases::Sh_reset( )
{ 
  sh_reset();
}

//_________________________________________________________
void JSFBases::sh_reset( )
{ 
  
  BasesSpring::sh_reset();

  TIterator *hst1=fTempHash1->MakeIterator();
  JSFBasesTempHist *th1;
  while( (th1=(JSFBasesTempHist*)hst1->Next()) ){
    th1->Reset();
  }

  TIterator *hst2=fTempHash2->MakeIterator();
  JSFBasesTempHist *th2;
  while( (th2=(JSFBasesTempHist*)hst2->Next()) ){
    th2->Reset();
  }

}

//_________________________________________________________
void JSFBases::Sh_update( )
{ 
  sh_update(); 
}

//_________________________________________________________
void JSFBases::sh_update( )
{ 
  // Fill saved histogram inforation

  BasesSpring::sh_update();

  Char_t hstn[128];

  TIterator *hst1=fTempHash1->MakeIterator();
  JSFBasesTempHist *th1;
  while( (th1=(JSFBasesTempHist*)hst1->Next()) ){
    sprintf(hstn,"%sSP",th1->GetName());
    th1->Update(((TH1D*)(fSPHash1->FindObject(hstn))));
  }

  TIterator *hst2=fTempHash2->MakeIterator();
  JSFBasesTempHist *th2;
  while( (th2=(JSFBasesTempHist*)hst2->Next()) ){
    sprintf(hstn,"%sSP",th2->GetName());
    th2->Update(((TH2D*)(fSPHash2->FindObject(hstn))));
  }

}

//_________________________________________________________
void JSFBases::Sh_plot( )
{ 
  sh_plot(); 
}

//_________________________________________________________
Double_t JSFBases::Func( double x[] )
{
  return BasesSpring::func(x) ;
}

//_________________________________________________________
Double_t JSFBases::Func( )
{ 
  return BasesSpring::func() ; 
}

//_________________________________________________________
double JSFBases::func( double x[] )
{
  return Func(x);
}

//_________________________________________________________
double JSFBases::func()
{ 
  return Func(); 
}

//______________________________________________________________________________
void JSFBases::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFBases.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }

      if( R__v < 3 ) { 
	printf("JSFBases::Streamer .. Old version data.  Can not read.\n");
      }
      TNamed::Streamer(R__b);

      R__b >> ndim;
      R__b >> nwild;

      R__b.ReadStaticArray(ig);
      R__b >> ncall;
      R__b >> ngiven;
      R__b.ReadStaticArray(xl);
      R__b.ReadStaticArray(xu);
      R__b.ReadStaticArray(&xi[0][0]);
      R__b.ReadStaticArray(dx);
      R__b >> nd;
      R__b >> ng;
      R__b >> nsp;
      R__b >> npg;
      R__b.ReadStaticArray(ma);
      R__b.ReadStaticArray(dxd);
      R__b.ReadStaticArray(pxd);
      R__b >> it;
      R__b >> itmx1;
      R__b >> itmx2;
      R__b >> acc1;
      R__b >> acc2;
      R__b >> si;
      R__b >> swgt;
      R__b >> scalls;
      R__b >> xnd;
      R__b >> jacob;
      R__b >> dxg;
      R__b >> estim;
      R__b >> error;

      // save seed
      long seed, iy, iv[NTAB];
      R__b >> seed;
      R__b >> iy;
      R__b.ReadStaticArray(iv);
      ran1.SetSeed(seed);
      ran1.SetSeed2(iy, iv);

   } else {
      R__b.WriteVersion(JSFBases::IsA());
      TNamed::Streamer(R__b);

      R__b << ndim;
      R__b << nwild;
      R__b.WriteArray(ig,ndim);
      R__b << ncall;
      R__b << ngiven;
      R__b.WriteArray(xl,ndim);
      R__b.WriteArray(xu,ndim);
      Int_t mxndmxdim=MXND*MXDIM;
      Int_t mxdim=MXDIM;
      R__b.WriteArray(&xi[0][0],mxndmxdim);
      R__b.WriteArray(dx,mxdim);
      R__b << nd;
      R__b << ng;
      R__b << nsp;
      R__b << npg;
      R__b.WriteArray(ma,ndim);
      R__b.WriteArray(dxd,nsp);
      R__b.WriteArray(pxd,nsp);
      //      R__b << seed;
      R__b << it;
      R__b << itmx1;
      R__b << itmx2;
      R__b << acc1;
      R__b << acc2;
      R__b << si;
      R__b << swgt;
      R__b << scalls;
      R__b << xnd;
      R__b << jacob;
      R__b << dxg;
      R__b << estim;
      R__b << error;

      // save seed
      long seed=ran1.GetSeed();
      long iy, iv[NTAB];
      ran1.GetSeed2(iy, iv);
      R__b << seed;
      R__b << iy;
      R__b.WriteArray(iv, NTAB);

      
   }

}
