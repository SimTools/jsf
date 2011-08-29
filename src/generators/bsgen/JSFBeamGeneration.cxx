///////////////////////////////////////////////////////////////////
//
// JSFBeamGeneration
//
// Generate JLC beam momentum
//
//$Id$
//
//////////////////////////////////////////////////////////////////

#include <iostream>
#include "TSystem.h"
#include "TFile.h"
#include "JSFBeamGeneration.h"
#ifndef NOJSF
#include "JSFSteer.h"
#endif
#include <sstream>



ClassImp(JSFBeamGeneration)
ClassImp(JSFBeamGenerationCain)

TFile *gJSFBeamFile;
static JSFBeamGenerationCain *gJSFBeamGenerationCain=0;
JSFBeamGenerationCain *gjsfbs2dfunction; 

Double_t JSFBeamGenerationCain::fLastEMinus0=0.0;
Double_t JSFBeamGenerationCain::fLastEPlus0=0.0;

extern "C" {

  Double_t JSFBS2DFunction(Double_t *x, Double_t *p) 
  { 
    return gjsfbs2dfunction->BS2DFunction(x,p);
  } 
//___________________________________________________
  TF2* JSFInitBS2DFunction(JSFBeamGenerationCain *bm)
{
    gjsfbs2dfunction=bm;
    TF2 *bsfunc=new TF2("JSFBS2DFunction", JSFBS2DFunction,
		        bm->GetBndOrder(),0.0,bm->GetBndOrder(),0.0,bm->GetPCntSize());
    bsfunc->SetParameters(bm->GetPCnt());
    return bsfunc;
}

//___________________________________________________
void jsfbeamgen_(const Double_t *xebm, const Double_t *xebp, 
		   const Double_t *xbsm, const Double_t *xbsp,
		   Double_t *xm,  Double_t *xp, Double_t *weight)
{
    Double_t wgt=gJSFBeamGenerationCain->GetWeight(*xebm, *xebp, 
						      *xbsm, *xbsp, *xm, *xp);
    *weight=wgt;
}

//___________________________________________________
void jsfbeamgend_(const Double_t *xebm, const Double_t *xebp, 
		   const Double_t *xbsm, const Double_t *xbsp,
		  const Double_t *enominal,
		   Double_t *em,  Double_t *ep, 
		   Double_t *em0, Double_t *ep0, Double_t *weight)
{
  //(Function)
  //  For given four random numbers, get energy of e- and e+ beam
  //(Inputs)
  // xebm: Random number(0<xebm<1) for initial beam energy spread of e-
  // xebp: Random number(0<xebp<1) for initial beam energy spread of e+
  // xbsm: Random number(0<xbsm<1) for Beamstrahlung of e- beam
  // xbsp: Random number(0<xbsp<1) for Beamstrahlung of e+ beam
  // enominal: Nominal Beam energy (used only at the time of initialization,
  //            if not defined by Generator.BSNominalEnergy 
  //(Outputs)
  // em:   Generated e- beam energy (GeV)
  // ep:   Generated e+ beam energy (GeV)
  // em0:  e- energy after initial beam energy spread ( without BS )
  // ep0:  e+ energy after initial beam energy spread ( without BS )

    if( gJSFBeamGenerationCain == 0 ) {
      JSFBeamGenerationCain::EIBType bstype=JSFBeamGenerationCain::kUniform;
      double bswidthMinus=-1.0;
      double bswidthPlus=-1.0;
#ifdef NOJSF
      const Char_t *bsfile="bsfile.root";
      const Char_t *bspara="350_nominal.root";
      double bswidth=0.0005;
      double e0=175.0;
#else
      const Char_t *bsfile=gJSF->Env()->GetValue("JSFBeamGeneration.FileName","bsfile.root");
      const Char_t *bspara=gJSF->Env()->GetValue("JSFBeamGeneration.ParName","350_nominal");
      Double_t bswidth=0;
      Double_t e0=0;
      std::stringstream sbswidth(gJSF->Env()->GetValue("JSFBeamGeneration.Width","0.0005"));
      sbswidth >> bswidth ;
      std::stringstream sbswidthMinus(gJSF->Env()->GetValue("JSFBeamGeneration.WidthEMinus","-1.0"));
      sbswidthMinus >> bswidthMinus ;
      std::stringstream sbswidthPlus(gJSF->Env()->GetValue("JSFBeamGeneration.WidthEPlus","-1.0"));
      sbswidthPlus >> bswidthPlus ;
      // 1 = Uniform, 2=Gaus
      Int_t ibtype=gJSF->Env()->GetValue("JSFBeamGeneration.IBType",0);
      if ( ibtype == 2 ) { bstype=JSFBeamGenerationCain::kGauss; }
      std::stringstream se0(gJSF->Env()->GetValue("JSFBeamGeneration.NominalEnergy","-100.0"));
      se0 >> e0 ;
      if ( e0 < 0.0 ) { e0 = *enominal; }

#endif
      TDirectory *cdir=gDirectory;
      gJSFBeamFile=new TFile(bsfile);
      gJSFBeamGenerationCain=(JSFBeamGenerationCain*)gJSFBeamFile->Get(bspara);
      if( bswidthMinus > 0.0 && bswidthPlus > 0.0 ) {
	gJSFBeamGenerationCain->SetIBParameters(e0, bswidthMinus, bswidthPlus, bstype);
      }
      else {
        gJSFBeamGenerationCain->SetIBParameters(e0, bswidth, bstype);
      }
      gJSFBeamGenerationCain->Print();
      gJSFBeamGenerationCain->MakeBSMap();
      cdir->cd();
    }
  
    Double_t wgt=gJSFBeamGenerationCain->GetWeight(*xebm, *xebp, 
						      *xbsm, *xbsp, *em, *ep);
    
    *em *=gJSFBeamGenerationCain->GetNominalEnergy();
    *ep *=gJSFBeamGenerationCain->GetNominalEnergy();
    *em0=gJSFBeamGenerationCain->GetLastEMinus0()*gJSFBeamGenerationCain->GetNominalEnergy();
    *ep0=gJSFBeamGenerationCain->GetLastEPlus0()*gJSFBeamGenerationCain->GetNominalEnergy();;
    *weight=wgt;
  }

//___________________________________________________
void jsfbeaminit_(Int_t *itype, Int_t *ibtyp, Double_t *bmwidth)
  {
    //(Usage)
    //   Generate beam energy, eminus and eplus
    //   energy is in unit of GeV.
    //   itype=0-27 to select Beam Strauhlung spectrum
    //   If ibtyp=1, use uniform initial beam spread, 2=Gaus form

    const Char_t *bsname[28]={"x250_n63", "jlca300", "jlca500", "jlcy300", "jlcy500",
			"trc250", "trc300", "trc350", "trc400", 
			"trc450", "trc500", "trc1000",
		"250_nominal", "350_largeY", "350_nominal", "500_lowP", // 16-19 
		"300_nominal", "350_lowP", "500_highLum", "500_lowQ",   // 20-23
		"350_highLum", "350_lowQ", "500_largeY", "500_nominal"}; // 24-27

    Char_t bsfile[256];
    sprintf(bsfile,"%s/data/bsdata/%s.root",
	    gSystem->Getenv("JSFROOT"),bsname[*itype]);

    TDirectory *cdir=gDirectory;
    gJSFBeamFile=new TFile(bsfile);
    gJSFBeamGenerationCain=(JSFBeamGenerationCain*)gJSFBeamFile->Get(bsname[*itype]);
    if( *ibtyp == 1 ) {
      gJSFBeamGenerationCain->SetIBParameters(*bmwidth,
					      JSFBeamGenerationCain::kUniform);
    }

    gJSFBeamGenerationCain->Print();
    gJSFBeamGenerationCain->MakeBSMap();
    cdir->cd();

  }


//___________________________________________________
void jsfbeaminit1_(char *bsfile, char *parname, 
		   int *itype, double *eb0, double *width, int lbsfile, int lparname)
{
  //(Usage)
  // bsfile: file name of beamstraulung data
  // parname: beam strahlung parameter name, which is used to get bsdata
  // itype:
  //   -1: Use default value defined in the bsfile
  //    0: Initial energy spread is uniform
  //    1: Initial energy spread is gaussian
  // eb0:  Nominal beam energy (GeV), valid when itype > -1
  // width: reative initial beam energy spread, valid when itype >-1
  //   Halfwidth (itype=0) or sigma (itype=1)
  
  TDirectory *cdir=gDirectory;
  std::cerr << " bsfile=" << bsfile << "====" << std::endl;
  std::cerr << "lbsfile=" << lbsfile << "  lparname=" << lparname << std::endl;

  gSystem->Exec("pwd");

  gJSFBeamFile=new TFile(bsfile);
  gJSFBeamGenerationCain=(JSFBeamGenerationCain*)gJSFBeamFile->Get(parname);

  if( *itype == 0 ) {
    gJSFBeamGenerationCain->SetIBParameters(*eb0, *width, 
					    JSFBeamGenerationCain::kUniform);
  }
  else if ( *itype == 1 ) {
    gJSFBeamGenerationCain->SetIBParameters(*eb0, *width, 
					      JSFBeamGenerationCain::kGauss);
  }

  gJSFBeamGenerationCain->Print();
  gJSFBeamGenerationCain->MakeBSMap();
  cdir->cd();

}



//___________________________________________________
void jsfbeamgenr_(const Double_t *xebm, const Double_t *xebp, 
		   const Double_t *xbsm, const Double_t *xbsp,
		  const Float_t *enominal,
		   Float_t *em,  Float_t *ep, 
		   Float_t *em0, Float_t *ep0, Float_t *weight)
{
  Double_t eb0=*enominal;
  Double_t emd=*em;
  Double_t epd=*ep;
  Double_t em0d=*em0;
  Double_t ep0d=*ep0;
  Double_t wd=*weight;
  jsfbeamgend_(xebm, xebp, xbsm, xbsp, &eb0, &emd, &epd, 
	       &em0d, &ep0d, &wd);
  *em=emd;
  *ep=epd;
  *em0=em0d;
  *ep0=ep0d;
  *weight=wd;

}


//___________________________________________________
void jsfbeamgen0_(Double_t *eminus, Double_t *eplus,
		  Double_t *eminus0, Double_t *eplus0)
  //(Function)
  //  Generate beam energy of weight 1.
  //  beaminit function must be called prior to call this function
  //(Outputs)
  //  eminus: eminus energy after BS and initial energy spread in unit of GeV. 
  //  eplus:  eplus energy after BS and initial energy spread in unit of GeV. 
  //  eminus0: eminus energy after initial energy spread but before BS in unit of GeV. 
  //  eplus0: eplus energy after initial energy spread but before BS in unit of GeV. 
{
   gJSFBeamGenerationCain->Generate(*eminus, *eplus);
   *eminus0=gJSFBeamGenerationCain->GetLastEMinus0();
   *eplus0=gJSFBeamGenerationCain->GetLastEPlus0();
}

}



//______________________________________________________
JSFBeamGeneration::JSFBeamGeneration()
{
  fNGenerated=0;
  fBeamDataFormat=0;
  fLuminosity=0;
  fNominalE=0;

  fEEInitial=0;
  fEPInitial=0;

  fParameterName="dummy";


}
//______________________________________________________
JSFBeamGeneration::JSFBeamGeneration(Int_t format, Double_t lum, Double_t nominalE, Char_t *name)
{
  fNGenerated=0;

  fBeamDataFormat=format;
  fNominalE=nominalE;
  fLuminosity=lum;

  fEEInitial=fNominalE;
  fEPInitial=fNominalE;

  fParameterName=name;

}


//_____________________________________________________
void JSFBeamGeneration::Print()
{
  printf("JSFBeamGeneration \n");
  printf("  Initial Energy Spread  : %g (relative)\n",GetIBWidth());
  printf("  Beamstrahlung          : %s\n",GetParameterName().Data());
  printf("  Nominal Beam Energy    = %g (GeV)\n",GetNominalEnergy());
  printf("  Luminosity             = %g (x10^{33})\n",fLuminosity);
}


//______________________________________________________
void JSFBeamGeneration::Generate(Double_t &eminus, Double_t &eplus)
{

  IncrementEventNumber();

  GenEnergySpread(fEEInitial, fEPInitial);
  GenBeamStrahlung(eminus, eplus);

}
//_______________________________________________________
void JSFBeamGeneration::GenEnergySpread(Double_t &eminus, Double_t &eplus)
{
  eminus=fNominalE;
  eplus =fNominalE;
}


//_______________________________________________________
void JSFBeamGeneration::GenBeamStrahlung(Double_t &eminus, Double_t &eplus)
{
  eminus=GetInitialElectronEnergy();
  eplus =GetInitialPositronEnergy();
}

//______________________________________________________
void JSFBeamGeneration::Generate(TLorentzVector &ecm, TLorentzVector &vtx)
{

  IncrementEventNumber();

  GenEnergySpread(fEEInitial, fEPInitial);

  GenBeamStrahlung(ecm, vtx);

}


//_______________________________________________________
void JSFBeamGeneration::GenBeamStrahlung(TLorentzVector &pecm, TLorentzVector &pvtx)
{
  Double_t esum =GetInitialElectronEnergy()+GetInitialPositronEnergy();
  Double_t pzsum=GetInitialElectronEnergy()-GetInitialPositronEnergy();
  
  pecm.SetPxPyPzE(0.0, 0.0, pzsum, esum);
  pvtx.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);

}


//______________________________________________________
JSFBeamGenerationCain::JSFBeamGenerationCain()
{
  for(Int_t i=0;i<3;i++){ fLumRatio[i]=0.0; }
  fBndOrder=-5.0;
  for(Int_t i=0;i<3;i++){ fPEdge1[i]=0.0; }
  fNEdge2=0;
  fPEdge2=NULL;

  fNfits=0;
  fNpmax=0;
  fPCnt=NULL;
  fIBType=kGauss;
  fIBType=kUniform;
  fIBWidth=0.0;

  fBS2DFunction=NULL;

  fMdiv=0;
  fMapsize=0;
  fMxmin=NULL;
  fMxmax=NULL;
  fMymin=NULL;
  fMymax=NULL;
  fMaint=NULL;
  fMvmax=NULL;

}

//______________________________________________________
JSFBeamGenerationCain::~JSFBeamGenerationCain()
{
  if ( fPEdge2 ) { delete fPEdge2; }
  if ( fPCnt )   { delete fPCnt; }

  if ( fBS2DFunction ) { delete fBS2DFunction; }
  if ( fMxmin ) { delete fMxmin; }
  if ( fMxmax ) { delete fMxmax; }
  if ( fMymin ) { delete fMymin; }
  if ( fMymax ) { delete fMymax; }
  if ( fMaint ) { delete fMaint; }
  if ( fMvmax ) { delete fMvmax; }

}


//______________________________________________________
void JSFBeamGenerationCain::SetBSParameters(const Int_t beamdataformat, const TString parametername,
     const Double_t luminosity, const Double_t nominalE, const Double_t lumratio[3],  
     const Double_t bndorder, const Double_t pedge1[3], const Double_t edgelimit, 
     const Int_t fnedge2,  const Double_t pedge2[],  const Int_t nfits, 
    const Int_t npmax, const Double_t pcnt[])
{
  fBeamDataFormat=beamdataformat;
  fParameterName=parametername;
  fLuminosity=luminosity;
  fNominalE=nominalE;
  printf(" fNominalE in JSFBeamGeneration is %g\n",fNominalE);

  fBndOrder=bndorder;
  for(Int_t i=0;i<3;i++){ fLumRatio[i]=lumratio[i]; }
  for(Int_t i=0;i<3;i++){ fPEdge1[i]=pedge1[i]; }
  fPEdgeLimit=edgelimit;
  fNEdge2=fnedge2;
  fPEdge2=new Double_t[fNEdge2];
  for(Int_t i=0;i<fNEdge2;i++){ fPEdge2[i]=pedge2[i]; }

  fNfits=nfits;
  fNpmax=npmax;
  fPCntSize=fNfits*fNpmax;
  fPCnt=new Double_t[fNfits*fNpmax];
  for(Int_t i=0;i<fNfits*fNpmax;i++){ fPCnt[i]=pcnt[i]; }

  fBS2DFunction=JSFInitBS2DFunction(this);

  printf(" fNominalE in JSFBeamGeneration is %g\n",fNominalE);
  printf(" GetNominalEnergy is %g\n",GetNominalEnergy());
#if defined(__BUILD_BITS__) && __BUILD_BITS__ == -m64 
  printf(" Address of this object is %lx\n",(ULong_t)this);
#else
  printf(" Address of this object is %x\n",(UInt_t)this);
#endif
}

//_____________________________________________________
void JSFBeamGenerationCain::SetIBParameters( const Double_t width, const EIBType type)
{
  fIBType=type;
  fIBWidth=width;
  fIBWidthEMinus=-1.0;
  fIBWidthEPlus=-1.0;
}

//_____________________________________________________
void JSFBeamGenerationCain::SetIBParameters(const Double_t nominalE,
				      const Double_t width, const EIBType type )
{
  fIBType=type;
  fIBWidth=width;
  fNominalE=nominalE;
  fIBWidthEMinus=-1.0;
  fIBWidthEPlus=-1.0;
}

//_____________________________________________________
void JSFBeamGenerationCain::SetIBParameters(const Double_t nominalE,
		      const Double_t widthEMinus, const Double_t widthEPlus, const EIBType type )
{
  fIBType=type;
  //  fIBWidth=width;
  fNominalE=nominalE;
  fIBWidthEMinus=widthEMinus;
  fIBWidthEPlus=widthEPlus;
  if( fIBWidthEMinus > 0.0 && fIBWidthEPlus > 0.0 ) {
    fIBWidth=(fIBWidthEPlus+fIBWidthEMinus)*0.5; 
  }

}

//_____________________________________________________
void JSFBeamGenerationCain::SetIBParametersByEnv(const Double_t nominalE)
{
#ifdef NOJSF
  std::cout << "Fatal error in JSFBeamGenerationCain::SetIBParametersByEnv " << std::endl;
  std::cout << "SetIBParametersByEnv was called, but this library is not built with JSF support" << std::endl;
  exit(0);
#else
  std::stringstream sbswidth(gJSF->Env()->GetValue("JSFBeamGeneration.Width","0.00000"));
  sbswidth >> fIBWidth ;
  std::stringstream sbswidthMinus(gJSF->Env()->GetValue("JSFBeamGeneration.WidthEMinus","-1.0"));
  sbswidthMinus >> fIBWidthEMinus ;
  std::stringstream sbswidthPlus(gJSF->Env()->GetValue("JSFBeamGeneration.WidthEPlus","-1.0"));
  sbswidthPlus >> fIBWidthEPlus ;
  // 1 = Uniform, 2=Gaus
  fIBType=JSFBeamGenerationCain::kUniform;
  Int_t ibtype=gJSF->Env()->GetValue("JSFBeamGeneration.IBType",0);
  if ( ibtype == 2 ) { fIBType=JSFBeamGenerationCain::kGauss; }
  //  fIBWidth=width;
  if(nominalE > 0.0 ) fNominalE=nominalE;
  if( fIBWidthEMinus > 0.0 && fIBWidthEPlus > 0.0 ) {
    fIBWidth=(fIBWidthEPlus+fIBWidthEMinus)*0.5; 
  }
#endif
}


//_____________________________________________________
void JSFBeamGenerationCain::Print()
{
  JSFBeamGeneration::Print();

  TString btname[2];
  btname[0]="Uniform";
  btname[1]="Gauss";
  printf("  Type of Initial beam energy spread : %s\n",btname[GetIBType()-1].Data());
  printf("  Energy spread of EMinus Beam : %g\n",GetIBWidthEMinus());
  printf("  Energy spread of EPlus  Beam : %g\n",GetIBWidthEPlus());
  printf("  Relative luminosity of each region (%g, %g, %g)\n",
	 fLumRatio[0], fLumRatio[1], fLumRatio[2]);



}

//_____________________________________________________
void JSFBeamGenerationCain::GenEnergySpread(Double_t &eminus, Double_t &eplus)
{

  if( GetIBWidth() <= 0.0 ) {
    eminus=GetNominalEnergy();
    eplus =GetNominalEnergy();
    return;
  }
  
  switch( GetIBType() ) {
    case kUniform:
      eminus=GetNominalEnergy()*(1+2*GetIBWidthEMinus()*(0.5-GetRndm()));
      eplus =GetNominalEnergy()*(1+2*GetIBWidthEPlus()*(0.5-GetRndm()));
      break;
    case kGauss: 
      eminus=GetNominalEnergy()*GetGauss(1.0, GetIBWidthEMinus());
      eplus =GetNominalEnergy()*GetGauss(1.0, GetIBWidthEPlus());
      break;
    default:
      printf("Error at JSFBeamGenerationCain::GenEnergySpread\n");
      printf("Initial Beam type %d was not defined.\n",GetIBType());
  }

}



//_______________________________________________________
void JSFBeamGenerationCain::GenBeamStrahlung(Double_t &eminus, Double_t &eplus)
{

  Double_t rnd1=GetRndm();

  //  When both beam does not have energy loss.
  if( rnd1 < GetLumRatio(0) ) {
    Double_t bndord=TMath::Power(10, GetBndOrder());
    eminus=GetInitialElectronEnergy()*(1-bndord*GetRndm());
    eplus =GetInitialPositronEnergy()*(1-bndord*GetRndm());
  }
  // When either electron or positron has energy logg
  else if( rnd1 < GetLumRatio(1) ) { 
    Double_t bndord=TMath::Power(10, GetBndOrder()); 
    if( GetRndm() < 0.5 ) { 
      eminus=GetInitialElectronEnergy()*(1-bndord*GetRndm());
      eplus=GenBSEnergy1(GetInitialPositronEnergy());
    }
    else {
      eminus=GenBSEnergy1(GetInitialElectronEnergy());
      eplus =GetInitialPositronEnergy()*(1-bndord*GetRndm());
    }
  }
  //
  // Bot electron and positron has evergy loss.
  else {
    if ( GenBSEnergy2(eminus, eplus) ) { return ; }
    printf("Error in JSFBeamGeneration::GenBeamStruhlung.");
    printf("Failed to generate an event.\n");
    return ;
  }

  return;
}


//_______________________________________________________
Double_t JSFBeamGenerationCain::GetWeight(const Double_t xebm, const Double_t xebp, 
				      const Double_t xbsm, const Double_t xbsp,
				      Double_t &xeminus, Double_t &xeplus)
{
// Using the input four random number, generate beam energy, eminus and eplus. 
// Pointer to bases object is used to get additional random number
// xebm, xebp, xbsm, xbsp must be uniform random number between 0 to 1.
//
// Output energies, xeminus, xeplus are scaled energy [0,1+alpha]

  Double_t weight=-1.0;
  // First generate initial beam energy
  //  if(  GetIBType() != kUniform ) {
  //    printf("Fatal error in JSFBeamGenerationCain::GetWeight") ;
  //    printf("Initial beam shape other than uniform is not supported yet\n");
  //    return weight;
  //  }
  Double_t em=1.;
  Double_t ep=1.;
  if(  GetIBType() == kUniform ) {
    em=1+2*GetIBWidth()*(0.5-xebm);
    ep=1+2*GetIBWidth()*(0.5-xebp);
  }else if(GetIBType() == kGauss){
    em = 1. - GetIBWidth() * sqrt(3)/3.14 * log(1/xebm - 1.);
    ep = 1. - GetIBWidth() * sqrt(3)/3.14 * log(1/xebp - 1.);
  }else{
    printf("Fatal error in JSFBeamGenerationCain::GetWeight") ;
    printf("Initial beam shape other than uniform and gauss is not supported yet\n");
    return weight;
  }

  fLastEMinus0=em;
  fLastEPlus0=ep;

  //  xeminus=em;
  //  xeplus =ep;
  //  weight=1.0;

  // Decide beamstraulung spectrum.
  // Assume eminus and eplus symmetry

  Double_t xbnd=TMath::Sqrt(GetLumRatio(0));
  weight=0.0;
  if( xbsm < xbnd && xbsp < xbnd ) {
    xeminus=em;
    xeplus =ep;
    weight=1.0;
  }
  //
  // When either electron or positron has energy logg
  else if ( xbsm < xbnd ) {
    Double_t scrnd=(xbsp-xbnd)/(1.0-xbnd);
    xeminus=em;
    xeplus=GenBSEnergy1(ep, scrnd);
    weight=(GetLumRatio(1)-GetLumRatio(0))/(2*xbnd*(1-xbnd));
  }
  else if ( xbsp < xbnd ) {
    Double_t scrnd=(xbsm-xbnd)/(1.0-xbnd);
    xeminus=GenBSEnergy1(em, scrnd);
    xeplus=ep;
    weight=(GetLumRatio(1)-GetLumRatio(0))/(2*xbnd*(1-xbnd));
  }
  //
  // Bot electron and positron has evergy loss.
  else {
    Double_t scrndp=(xbsp-xbnd)/(1.0-xbnd);
    Double_t scrndm=(xbsm-xbnd)/(1.0-xbnd);
    weight=GetBSEnergy2Weight(scrndm, scrndp, xeminus, xeplus);
    xeminus*=em;
    xeplus*=ep;
    weight*=(1.0-GetLumRatio(1))/((1-xbnd)*(1-xbnd));
  }
  
  return weight;  

}

//_______________________________________________________
void JSFBeamGenerationCain::GenBeamStrahlung(TLorentzVector &pecm, TLorentzVector &pvtx)
{
  Double_t eminus, eplus;
  GenBeamStrahlung(eminus, eplus);

  Double_t esum =eminus+eplus;
  Double_t pzsum=eminus-eplus;
  
  pecm.SetPxPyPzE(0.0, 0.0, pzsum, esum);
  pvtx.SetPxPyPzE(0.0, 0.0, 0.0, 0.0);

}

//_______________________________________________________
Double_t JSFBeamGenerationCain::GenBSEnergy1(const Double_t eini, const Double_t xrand)
{
  Double_t x=( xrand < 0.0 ? GetRndm() : xrand );
  Double_t ebl;
  if( x < fPEdgeLimit ) {
    ebl=fPEdge1[0]*x*x + fPEdge1[1]*x + fPEdge1[2];
  }
  else {
    ebl=fPEdge2[0];
    for(Int_t i=1;i<9;i++){
      ebl+= fPEdge2[i]*TMath::Power(x,i);
    }
    ebl*=TMath::Power(x, fPEdge2[9]);
  }
  Double_t eb=eini*(1-TMath::Power(10, ebl));

  return eb;
}


//_______________________________________________________
Bool_t JSFBeamGenerationCain::GenBSEnergy2(Double_t &eminus, Double_t &eplus)
{
  // When both electron and positron have energy loss.

  Double_t eini=GetInitialElectronEnergy();
  Double_t pini=GetInitialPositronEnergy();

  Double_t xrnd=GetRndm();
  for(Int_t i=0;i<fMapsize;i++){
    if( fMaint[i] > xrnd ) {
      Double_t xmin=fMxmin[i];
      Double_t xmax=fMxmax[i];
      Double_t ymin=fMymin[i];
      Double_t ymax=fMymax[i];
      Int_t maxloop=500;
      for(Int_t j=0;j<maxloop;j++){
	Double_t x=xmin+(xmax-xmin)*GetRndm();
	Double_t y=ymin+(ymax-ymin)*GetRndm();
	Double_t val=fBS2DFunction->Eval(x,y)/fMvmax[i];
	Double_t xxrnd=GetRndm();
	if( val > xxrnd ) {
	  eminus=eini*(1.0-TMath::Power(10.0,x));
	  eplus =pini*(1.0-TMath::Power(10.0,y));
	  return kTRUE;
	}
      }
      printf(" Cannot generate E-beam after %d loop\n",maxloop);
      return kFALSE;
    }
  }
  printf(" Unable to find proper bsmap bin\n");
  return kFALSE;


}


//_______________________________________________________
Double_t JSFBeamGenerationCain::GetBSEnergy2Weight(const Double_t xbsm, const Double_t xbsp,
			      Double_t &eminus, Double_t &eplus)
{
  // When both electron and positron have energy loss.

  Double_t a=xbsm*fBndOrder;
  Double_t b=xbsp*fBndOrder;
  eminus=1.0 - TMath::Power(10.0, a);
  eplus =1.0 - TMath::Power(10.0, b);

  Double_t scale=fBndOrder*fBndOrder;
  Double_t weight=scale*fBS2DFunction->Eval(a,b)/fTotalInt;

  return weight;

}


//__________________________________________________
Double_t JSFBeamGenerationCain::BS1DFunction(Double_t *x, Double_t *p)
{
  Double_t x1=x[0]+p[1];
  Double_t t1=TMath::CosH( p[2]*(x[0]+p[4]) ) -1.0;
  Double_t t2=TMath::Exp(p[3]*x1);
  Double_t val1=p[0]*t1*t2;

  if( x[0] > -p[4] ) { val1 = 0.0 ; }

  return val1;
}

//_________________________________________________
Double_t JSFBeamGenerationCain::BS2DFunction(Double_t *xi, Double_t *par)
{
  Double_t x=xi[0];
  Double_t y=xi[1];
  Double_t p[5];
 
  p[0]=BS1DFunction(&y, &par[0]);
  for(Int_t i=1;i<5;i++){
    p[i]=PolFunc6(&y, &par[i*fNpmax]);
  }
  Double_t val=BS1DFunction(&x, p);
  if( val < 0.0 ) { val = 0.0 ; }

  return val;
}

//__________________________________________________
Double_t JSFBeamGenerationCain::PolFunc6(Double_t *xi, Double_t *par)  
{
  Double_t x=xi[0];
  Double_t val=((((( par[6]*x + par[5] )*x + par[4] ) * x + 
		    par[3] ) * x + par[2] )*x + par[1])*x + par[0] ;
  return val;
}



//___________________________________________________
void JSFBeamGenerationCain::MakeBSMap()
{
  // Create a data for beam generation.

  if( !fBS2DFunction ) {
    fBS2DFunction=JSFInitBS2DFunction(this);
  }

  Double_t totalint=fBS2DFunction->Integral(GetBndOrder(),0.0,GetBndOrder(),0.0);

  fMdiv=-(Int_t)GetBndOrder();
  fMapsize=fMdiv*fMdiv;
  fMxmin=new Double_t[fMapsize];
  fMxmax=new Double_t[fMapsize];
  fMymin=new Double_t[fMapsize];
  fMymax=new Double_t[fMapsize];
  fMaint=new Double_t[fMapsize];
  fMvmax=new Double_t[fMapsize];

  Double_t xmin,xmax,ymin,ymax,xstp,ystp;
  xstp=1.0;  ystp=1.0;
  Double_t ssum=0.0;
  Int_t ip=0;
  for(Int_t ix=0;ix<fMdiv;ix++){
    xmin=GetBndOrder()+ix*xstp;
    xmax=xmin+xstp;
    for(Int_t iy=0;iy<fMdiv;iy++){
      ymin=GetBndOrder()+iy*ystp;
      ymax=ymin+ystp;
      Double_t s=fBS2DFunction->Integral(xmin,xmax,ymin,ymax,1.0E-8);
      ssum+=s;
      fMxmin[ip]=xmin;
      fMxmax[ip]=xmax;
      fMymin[ip]=ymin;
      fMymax[ip]=ymax;
      fMaint[ip]=ssum/totalint;
      ip++;
    }
  }
  fTotalInt=totalint;
  if( TMath::Abs(ssum/totalint-1.0) > 0.001 ) {
    printf("Fatal error at JSFBeamGenerattion::MakeBSMap\n");
    printf(" Too much difference in total integration ");
    printf(" and sum of each bins.  Something is wrong.\n"); 
    exit(-1);
  }
  if( ip != fMapsize ) { 
    printf(" Error in JSFBeamGenerationCain::MakeBSMap ..# of map data(%d)",ip);
    printf(" exceeds defined size of map (%d)\n",fMapsize);
    exit(-1);
  }

  //  Set maximum of each area
  Double_t nscan=2*fMdiv;
  for(Int_t i=0;i<fMapsize;i++){
    Double_t vmax=0.0;
    Double_t vstp=0.0;
    for(Double_t x=fMxmin[i];x<=fMxmax[i];
	x+=(fMxmax[i]-fMxmin[i])/nscan)      {
	for(Double_t y=fMymin[i];y<=fMymax[i];
	    y+=(fMymax[i]-fMymin[i])/nscan)  {
	    Double_t val=fBS2DFunction->Eval(x,y);
	    if( val > vmax ) {
	      if( val-vmax > vstp ) { vstp = val-vmax; }
	      vmax=val;
	    }
	}
    }
    fMvmax[i]=vmax+vstp;
  }
	     
}





