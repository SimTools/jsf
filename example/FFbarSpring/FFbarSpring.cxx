//*LastUpdate:  jsf-1-12 31-July-1999 Akiya Miyamoto
//*LastUpdate:  v.01.01 undefined  by undefined
//*-- Author :  Akiya Miyamoto

//////////////////////////////////////////////////////////////////
//
//  FFbarSpring
//  
// Sample program in example/FFbarSpring directory.
//  
// Files in this directory is created automatically as a template files 
// for Bases/Spring program by a script, buildjsf.  
// 
// If you have selected INCLUDE SAMPLE option, sample program can be 
// created and executed as follows.
// 
// To create executable, do
// 
// $ xmkmf
// $ make jsf
// 
// This will create ROOT application, jsf, in this directory.  Example 
// to run applications are,
// 
// $ jsf begin_html <a href="example/FFbarSpring/bases.C.html">bases.C</a> end_html   --> Bases calculation.
// $ jsf begin_html <a href="example/FFbarSpring/sim.C.html">sim.C</a> end_html     --> JSF quicksim using bases results.
// $ jsf begin_html <a href="example/FFbarSpring/nsim.C.html">nsim.C</a> end_html     --> Run next run usung seeds at the end of sim.C script.
// $ jsf begin_html <a href="example/FFbarSpring/read.C.html">read.C</a> end_html    --> Read data 
// $ jsf begin_html <a href="example/FFbarSpring/anal.C.html">anal.C</a> end_html    --> JSF quick sim and analysis. 
//  
//$Id$
//
//////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "FFbarSpring.h"

ClassImp(FFbarSpring)
ClassImp(FFbarSpringBuf)
ClassImp(FFbarBases)


//_____________________________________________________________________________
FFbarSpring::FFbarSpring(const char *name, const char *title,
			 FFbarBases *bases)
  : JSFSpring(name, title, bases)
{
  fEventBuf = new FFbarSpringBuf("FFbarSpringBuf", 
	 "FFbarSpring event buffer", this);
  if( !bases ) { 
    FFbarBases *bs=new FFbarBases();
    SetBases(bs);
  }
}



//_____________________________________________________________________________
FFbarSpring::~FFbarSpring()
{
  if( fEventBuf ) { delete fEventBuf; fEventBuf=NULL ; }
}

//_____________________________________________________________________________
Bool_t FFbarSpringBuf::SetPartons()
{

  TClonesArray &partons = *fPartons;
  FFbarBases *bases=(FFbarBases*)((FFbarSpring*)Module())->GetBases();

//  <<+LLbar>>
  fNparton = 2;
  Double_t ecm=bases->GetEcm();
  fCosth = bases->GetCosth();
  fPhi   = bases->GetPhi();

  Double_t sinth=TMath::Sqrt( (1.0-fCosth)*(1.0+fCosth) ); 
  TVector p1(4);
  TVector p2(4);
  p1(1)=ecm*sinth*TMath::Cos(fPhi);
  p1(2)=ecm*sinth*TMath::Sin(fPhi);
  p1(3)=ecm*fCosth;
  p1(0)=ecm;
  
  p2(1)=-p1(1);
  p2(2)=-p1(2);
  p2(3)=-p1(3);

  p2(0)=p1(0);
  Int_t id=bases->GetID();
  Double_t charge=bases->GetCharge();

// In case of generating u-ubar
  Int_t ISLEV =0 ; // See comment of lufrag.F
  Int_t ICF   =0 ;
  if( id < 10 ) { ISLEV = 101 ; ICF = 1 ; }

  new(partons[0]) JSFSpringParton(1,  id, 0.0,  charge, p1,
		  0, 0, 0, 0, ICF, ISLEV );
  new(partons[1]) JSFSpringParton(2, -id, 0.0, -charge, p2,
		  0, 0, 0, 0, ICF, ISLEV );
//  <<-LLbar>>

  return kTRUE ;
}


//_____________________________________________________________________________
FFbarBases::FFbarBases(const char *name, const char *title)
           : JSFBases(name, title)
{
//  Constructor of bases.  Default parameter should be initialized here
//

//  <<+LLbar>>
  //  fNDIM=2;

// Get parameters from jsf.conf, if specified.
  
  sscanf(gJSF->Env()->GetValue("FFbarBases.CosthRange","-1.0 1.0"),
	                       "%lg%lg",&fXL[0],&fXU[0]);
  sscanf(gJSF->Env()->GetValue("FFbarBases.PhiOverPiRange","0.0 2.0"),
	                       "%lg%lg",&fXL[1],&fXU[1]);
  fXL[1]=fXL[1]*kPi;
  fXU[1]=fXU[1]*kPi;
  sscanf(gJSF->Env()->GetValue("FFbarBases.Ecm","300.0"),"%g",&fEcm);
  fID = gJSF->Env()->GetValue("FFbarBases.ID",11);
  Double_t uq=2.0/3.0;  Double_t dq=-1.0/3.0;
  Double_t zr=0.0; Double_t one=-1.0;
  Double_t cdata[20]={ dq,uq,dq,uq,dq,    uq,dq,uq,zr,zr,
		       one,zr,one,zr,one,   zr,one,zr,zr,zr};
  if( fID < 1 || fID > 20 ) {
    Error("FFbarBases","Particle ID(%d) is outof range",fID);
  }
  fCharge=cdata[fID-1];

  DefineVariable(fCosth, fXL[0], fXU[0], 1, 1);
  DefineVariable(fPhi, fXL[1], fXU[1], 0, 1);


  SetNoOfSample(2000);
  SetTuneValue( 1.5 );
  SetIteration1( 0.05, 100);
  SetIteration2( 0.05, 100);

  Xh_init(1, fXL[0], fXU[0], 50, "Costh");
  Xh_init(2, fXL[1], fXU[1], 50, "Phi");

  H1Init("hCosth","Costh distrubution",50,fXL[0],fXU[0]);
  //  hCosth=new TH1F("hCosth","Costh distrubution",50,fXL[0],fXU[0]);

}

//_____________________________________________________________________________
Double_t FFbarBases::Func()
{
//  Bases Integrand
//

  Double_t val=(kAlpha*kAlpha/4.0/fEcm/fEcm)*(1.0+fCosth*fCosth)*kGev2fb;

  Xh_fill(1, fCosth, val);
  Xh_fill(2, fPhi, val);

  //  hCosth->Fill(fCosth,val); 
  H1Fill("hCosth", fCosth, val);

//  <<-LLbar>>
  return val ;
}





