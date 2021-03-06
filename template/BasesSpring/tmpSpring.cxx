//*LastUpdate:  <<Version>> <<Date>>  by <<Author>>
//*-- Author :  <<Date>>  <<Date>>

//////////////////////////////////////////////////////////////////
//
//  <<Spring>>
//  
//  <<Comment>>
//  
//////////////////////////////////////////////////////////////////

#include "JSFModule.h"
#include "JSFSpring.h"
#include "JSFSteer.h"
#include "JSFBases.h"
#include "<<Spring>>.h"

ClassImp(<<Spring>>)
ClassImp(<<Spring>>Buf)
ClassImp(<<Bases>>)



//_____________________________________________________________________________
<<Spring>>::<<Spring>>(const char *name, const char *title,
			 <<Bases>> *bases)
  : JSFSpring(name, title, bases)
{
  fEventBuf = new <<Spring>>Buf("<<Spring>>Buf", 
	 "<<Spring>> event buffer", this);
  if( !bases ) { 
    <<Bases>> *bs=new <<Bases>>();
    SetBases(bs);
  }
}


//_____________________________________________________________________________
<<Spring>>::~<<Spring>>()
{
  if( !fEventBuf ) delete fEventBuf;
}



//_____________________________________________________________________________
Bool_t <<Spring>>Buf::SetPartons()
{

  TClonesArray &partons = *fPartons;
  <<Bases>> *bases=(<<Bases>>*)((<<Spring>>*)Module())->GetBases();

<<+LLbar>>
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
<<-LLbar>>

  return kTRUE ;
}


//_____________________________________________________________________________
<<Bases>>::<<Bases>>(const char *name, const char *title)
           : JSFBases(name, title)
{
//  Constructor of bases.  Default parameter should be initialized here
//

<<+LLbar>>

// Get parameters from jsf.conf, if specified.
  
  sscanf(gJSF->Env()->GetValue("<<Bases>>.CosthRange","-1.0 1.0"),
	                       "%lg%lg",&fXL[0],&fXU[0]);
  sscanf(gJSF->Env()->GetValue("<<Bases>>.PhiOverPiRange","0.0 2.0"),
	                       "%lg%lg",&fXL[1],&fXU[1]);
  fXL[1]=fXL[1]*kPi;
  fXU[1]=fXU[1]*kPi;
  sscanf(gJSF->Env()->GetValue("<<Bases>>.Ecm","300.0"),"%g",&fEcm);
  fID = gJSF->Env()->GetValue("<<Bases>>.ID",11);
  Double_t uq=2.0/3.0;  Double_t dq=-1.0/3.0;
  Double_t zr=0.0; Double_t one=-1.0;
  Double_t cdata[20]={ dq,uq,dq,uq,dq,    uq,dq,uq,zr,zr,
		       one,zr,one,zr,one,   zr,one,zr,zr,zr};
  if( fID < 1 || fID > 20 ) {
    Error("<<Bases>>","Particle ID(%d) is outof range",fID);
  }
  fCharge=cdata[fID-1];

  DefineVariable(fCosth, fXL[0], fXU[0], 1, 1);
  DefineVariable(fPhi,   fXL[1], fXU[1], 0, 1);

  SetNoOfSample(2000);
  SetTuneValue( 1.5 );
  SetIteration1( 0.05, 100);
  SetIteration2( 0.05, 100);

  H1Init("hCosth","Costh",50,fXL[0],fXU[0]);
  H1Init("hPhi","Phi",50,fXL[1],fXU[1]);
  
<<-LLbar>>

}

//_____________________________________________________________________________
Double_t <<Bases>>::Func()
{
//  Bases Integrand
//

<<+LLbar>>
    //  if( !fInBases ) { 
    //    ((<<Spring>>Buf*)((<<Spring>>*)fSpring->EventBuf()))->fCosth=fCosth;
    //    ((<<Spring>>Buf*)((<<Spring>>*)fSpring->EventBuf()))->fPhi=fPhi;
    //  }

  Double_t val=(kAlpha*kAlpha/4.0/fEcm/fEcm)*(1.0+fCosth*fCosth)*kGev2fb;

  H1Fill("hCosth",fCosth,val);
  H1Fill("hPhi",fPhi,val);

<<-LLbar>>
  return val ;
}

