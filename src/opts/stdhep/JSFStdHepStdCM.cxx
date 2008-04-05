///////////////////////////////////////////////////////////////////
//
//  JSFStdHepStdCM
//
//$Id$ 
//  
//////////////////////////////////////////////////////////////////

#include <iostream>

#include "JSFStdHepStdCM.h"
#include "stdcm1.h"

ClassImp(JSFStdHepStdCM)

/*
Float_t JSFStdHepStdCM::fStdECom=0.0 ; //  STDECOM  - center-of-mass energy 
Float_t JSFStdHepStdCM::fStdXSec=0.0 ;   // STDXSEC  - cross-section 
Double_t JSFStdHepStdCM::fStdSeed1=0.0 ; // STDSEED1 - random number seed 
Double_t JSFStdHepStdCM::fStdSeed2=0.0 ; // STDSEED2 - random number seed
Int_t    JSFStdHepStdCM::fNEvtReq=0 ;  // NEVTREQ  - number of events to be generated 
Int_t    JSFStdHepStdCM::fNEvtGen=0 ;  // NEVTGEN  - number of events actually generated 
Int_t    JSFStdHepStdCM::fNEvtWrt=0 ;  // NEVTWRT  - number of events written to output file 
Int_t    JSFStdHepStdCM::fNEvtLH=0 ;   // NEVTLH  - number of Les Houches events written to output file
Char_t   JSFStdHepStdCM::fGeneratorName=""; //  name of Monte Carlo generator 
Char_t   JSFStdHepStdCM::fPDFName="";       //  name of PDF method used
*/
 
//_____________________________________________________________________________
void JSFStdHepStdCM::LoadStdHepCM()
{
  fStdECom=stdcm1_.stdecom;
  fStdXSec=stdcm1_.stdxsec;   
  fStdSeed1=stdcm1_.stdseed1;
  fStdSeed2=stdcm1_.stdseed2;
  fNEvtReq=stdcm1_.nevtreq;
  fNEvtGen=stdcm1_.nevtgen;
  fNEvtWrt=stdcm1_.nevtwrt;
  fNEvtLH=stdcm1_.nevtlh;

  for(Int_t i=0;i<20;i++){
    if( stdcm2_.generatorname[i] == (Char_t)0 ) break;
    fGeneratorName[i]=stdcm2_.generatorname[i];
  }
  fGeneratorName[19]=(Char_t)0;

  for(Int_t i=0;i<20;i++){
    if( stdcm2_.pdfname[i] == (Char_t)0 ) break;
    fPDFName[i]=stdcm2_.pdfname[i];
  }
  fPDFName[19]=(Char_t)0;

}

//_____________________________________________________________________________
void JSFStdHepStdCM::SetStdHepCM(Float_t ecom, Float_t xsec, Double_t seed1, 
   Double_t seed2, Int_t nevtreq, Int_t nevtgen,
   Int_t nevtwrt, Int_t nevtlh, 
	Char_t *gname, Char_t *pdfname)
{
  fStdECom=ecom;
  fStdXSec=xsec;
  fStdSeed1=seed1;
  fStdSeed2=seed2;
  fNEvtReq=nevtreq;
  fNEvtGen=nevtgen;
  fNEvtWrt=nevtwrt;
  fNEvtLH=nevtlh;

  stdcm1_.stdecom=fStdECom;
  stdcm1_.stdxsec=fStdXSec;   
  stdcm1_.stdseed1=fStdSeed1;
  stdcm1_.stdseed2=fStdSeed2;
  stdcm1_.nevtreq=fNEvtReq;
  stdcm1_.nevtgen=fNEvtGen;
  stdcm1_.nevtwrt=fNEvtWrt;
  stdcm1_.nevtlh=fNEvtLH;

  for(Int_t i=0;i<20;i++){
    if( gname[i] == (Char_t)0 ) break;
    fGeneratorName[i]=gname[i];
    stdcm2_.generatorname[i]=gname[i];
  }
  fGeneratorName[19]=(Char_t)0;
  stdcm2_.generatorname[19]=(Char_t)0;

  for(Int_t i=0;i<20;i++){
    if( pdfname[i] == (Char_t)0 ) break;
    fPDFName[i]=pdfname[i];
    stdcm2_.pdfname[i]=pdfname[i];
  }
  fPDFName[19]=(Char_t)0;
  stdcm2_.pdfname[19]=(Char_t)0;
}

void JSFStdHepStdCM::SetStdECom(Float_t val)
{  fStdECom=val;  stdcm1_.stdecom=val; }

void JSFStdHepStdCM::SetStdXSec(Float_t val)
{ fStdXSec=val; stdcm1_.stdxsec=val; }

void JSFStdHepStdCM::SetStdSeed1(Double_t val)
{ fStdSeed1=val; stdcm1_.stdseed1=val; }

void JSFStdHepStdCM::SetStdSeed2(Double_t val)
{ fStdSeed2=val; stdcm1_.stdseed2=val; }

void JSFStdHepStdCM::SetNEvtReq(Int_t val)
{ fNEvtReq=val;  stdcm1_.nevtreq=val; }

void JSFStdHepStdCM::SetNEvtGen(Int_t val)
{ fNEvtGen=val;   stdcm1_.nevtgen=val; }

void JSFStdHepStdCM::SetNEvtWrt(Int_t val)
{ fNEvtWrt=val;  stdcm1_.nevtwrt=val; }

void JSFStdHepStdCM::SetNEvtLH(Int_t val)
{ fNEvtLH=val;  stdcm1_.nevtlh=val; }

void JSFStdHepStdCM::SetGeneratorName(Char_t *val)
{
  for(Int_t i=0;i<20;i++){
    if( val[i] == (Char_t)0 ) break;
    fGeneratorName[i]=val[i];
    stdcm2_.generatorname[i]=val[i];
  }
  fGeneratorName[19]=(Char_t)0;
  stdcm2_.generatorname[19]=(Char_t)0;
}

void JSFStdHepStdCM::SetPDFName(Char_t *val)
{
  for(Int_t i=0;i<20;i++){
    if( val[i] == (Char_t)0 ) break;
    fPDFName[i]=val[i];
    stdcm2_.pdfname[i]=val[i];
  }
  fPDFName[19]=(Char_t)0;
  stdcm2_.pdfname[19]=(Char_t)0;
}


//_______________________________________________________
void JSFStdHepStdCM::Print()
{
  std::cerr << "=========JSFStdHep BeginRun/EndRun Information " << std::endl;
  std::cerr << "  stdecom=" << fStdECom
            << "  stdxsec=" << fStdXSec 
	    << "  seed1=" << fStdSeed1
	    << "  seed2=" << fStdSeed2
	    << std::endl;
  std::cerr << "  nevtreq=" << fNEvtReq 
	    << "  nevtgen=" << fNEvtGen
	    << "  nevtwrt=" << fNEvtWrt
	    << "  nevtlh="  << fNEvtLH
	    << std::endl;
  std::cerr << "  GeneratorName=" << fGeneratorName
	    << "  PDFName=" << fPDFName
	    << std::endl;
}
