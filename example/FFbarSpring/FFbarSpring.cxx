//*LastUpdate:  v.01.01 undefined  by undefined
//*-- Author :  undefined  undefined

//////////////////////////////////////////////////////////////////
//
//  FFbarSpring
//  
//  Sample program
//  
//////////////////////////////////////////////////////////////////

#include "JSFModule.h"
#include "JSFSpring.h"
#include "JSFSteer.h"
#include "JSFBases.h"
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
  if( !fEventBuf ) delete fEventBuf;
}



//_____________________________________________________________________________
Bool_t FFbarSpringBuf::SetPartons()
{

  TClonesArray &partons = *fPartons;
  FFbarBases *bases=(FFbarBases*)((FFbarSpring*)Module())->Bases();

//  <<+LLbar>>
  fNparton = 2;
  Double_t ecm=bases->GetEcm();

  Double_t sinth=TMath::Sqrt( (1.0-fX[0])*(1.0+fX[0]) ); 
  TVector p1(4);
  TVector p2(4);
  p1(1)=ecm*sinth*TMath::Cos(fX[1]);
  p1(2)=ecm*sinth*TMath::Sin(fX[1]);
  p1(3)=ecm*fX[0];
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
  fNDIM=2;

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

  fPrintInfo = gJSF->Env()->GetValue("FFbarBases.PrintInfo",kTRUE);
  fPrintHist = gJSF->Env()->GetValue("FFbarBases.PrintHist",kTRUE);

//  <<-LLbar>>

}

//_____________________________________________________________________________
Double_t FFbarBases::Func(Double_t x[])
{
//  Bases Integrand
//

//  <<+LLbar>>
  if( !fInBases ) { 
    ((FFbarSpringBuf*)((FFbarSpring*)fSpring->EventBuf()))->fX[0]=x[0];
    ((FFbarSpringBuf*)((FFbarSpring*)fSpring->EventBuf()))->fX[1]=x[1];
  }
  Double_t val=(kAlpha*kAlpha/4.0/fEcm/fEcm)*(1.0+x[0]*x[0])*kGev2fb;

  Xhfill(1, x[0], val);
  Xhfill(2, x[1], val);

//  <<-LLbar>>
  return val ;
}

//_____________________________________________________________________________
void FFbarBases::Userin()
{
//
//   Initialize User parameters for Bases
//
  JSFBases::Userin();  // Call JSFBases::Userin() for standard setup.

//  <<+LLbar>>
  Xhinit(1, fXL[0], fXU[0], 50, "Costh");
  Xhinit(2, fXL[1], fXU[1], 50, "Phi");
//  <<-LLbar>>

  return ;
}

//_____________________________________________________________________________
void FFbarBases::Userout()
{
}

