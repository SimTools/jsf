//*LastUpdate:  v.01.01 11-November-1998  Akiya Miyamoto
//*-- Author :  Akiya Miyamoto

//////////////////////////////////////////////////////////////////
//
//  DebugWGenerator
//  
//  Generate W for debugging
//  
//////////////////////////////////////////////////////////////////

#include <TRandom.h>
#include <TMath.h>
#include "JSFModule.h"
#include "JSFSpring.h"
#include "JSFSteer.h"
#include "DebugWGenerator.h"

extern "C" {
extern void decay2_(Double_t *wmom, Double_t *amb, Double_t *amf1,
		    Double_t *amf2, Double_t *pf1, Double_t *pf2);
};
  

ClassImp(DebugWGeneratorBuf)
ClassImp(DebugWGenerator)


//_____________________________________________________________________________
DebugWGenerator::DebugWGenerator(const char *name, const char *title,
				 JSFBases *bases)
  : JSFSpring(name, title, bases)
{
  fEventBuf = new DebugWGeneratorBuf("DebugWGeneratorBuf", 
	 "DebugWGenerator event buffer", this);
 
  fWmass  = gJSF->Env()->GetValue("DebugWGenerator.Wmass",80.0);
  fWwidth = gJSF->Env()->GetValue("DebugWGenerator.Wwidth",2.5);

  sscanf(gJSF->Env()->GetValue("DebugWGenerator.RangeP","0.0,500.0"),
	 "%g,%g",&fRp[0], &fRp[1]);
  sscanf(gJSF->Env()->GetValue("DebugWGenerator.RangeCosth","-1.0,1.0"),
	 "%g,%g",&fRcosth[0], &fRcosth[1]);
  Float_t rp1, rp2;
  sscanf(gJSF->Env()->GetValue("DebugWGenerator.RangePhiAngle","0.0,360.0"),
	 "%g,%g",&rp1, &rp2);

  fRphi[0]=rp1/180.0*TMath::Pi();
  fRphi[1]=rp2/180.0*TMath::Pi();

}


//_____________________________________________________________________________
DebugWGenerator::~DebugWGenerator()
{
  if( !fEventBuf ) delete fEventBuf;
}



//_____________________________________________________________________________
Bool_t DebugWGeneratorBuf::SetPartons()
{

  TClonesArray &partons = *fPartons;
  DebugWGenerator *gen=(DebugWGenerator*)Module();

  // Make W parameters
  Double_t amb  = gRandom->Gaus(gen->fWmass, gen->fWwidth);
  Double_t wp   = gen->fRp[0] + (gen->fRp[1]-gen->fRp[0])*gRandom->Rndm();
  Double_t wcos = gen->fRcosth[0] 
               + (gen->fRcosth[1]-gen->fRcosth[0])*gRandom->Rndm();
  Double_t wphi = gen->fRphi[0] 
               + (gen->fRphi[1]-gen->fRphi[0])*gRandom->Rndm();

  Double_t wmom[4];
  wmom[0]=TMath::Sqrt(wp*wp + amb*amb);
  Double_t sinth=TMath::Sqrt((1.0-wcos)*(1.0+wcos));
  wmom[1]=wp*sinth*TMath::Cos(wphi);
  wmom[2]=wp*sinth*TMath::Sin(wphi);
  wmom[3]=wp*wcos;

  Double_t amf=0;
  Double_t pf1[4], pf2[4];
  decay2_(wmom, &amb, &amf, &amf, pf1, pf2);

  TVector wv(4); TVector pv1(4); TVector pv2(4);
  for(Int_t i=0;i<4;i++){ wv(i)=wmom[i]; pv1(i)=pf1[i] ; pv2(i)=pf2[i]; }

  Int_t idf1=1 ; Int_t idf2=-2;
  if( gRandom->Rndm() > 0.5 ) { idf1=3 ; idf2=-4; }

  // Fill data into Parton class
  new(partons[0]) JSFSpringParton(1, 24  , amb,  1.0, wv,
		  2, 1, 0, 0, 0, 0 );
  new(partons[1]) JSFSpringParton(2, idf1 , 0.0,  0.0, pv1,
		  0, 0, 1, 0, 1, 101 );
  new(partons[2]) JSFSpringParton(3, idf2 , 0.0,  0.0, pv2,
		  0, 0, 1, 0, 1, 101 );

  fNparton=3;

  return kTRUE ;
}


//_____________________________________________________________________________
Bool_t DebugWGenerator::Process(Int_t ev)
{

  if( fEventBuf ) {
    DebugWGeneratorBuf *buf=(DebugWGeneratorBuf*)fEventBuf;
    if ( !buf->SetPartons() ) return kFALSE; 
  }
  return kTRUE;
}

#if __ROOT_FULLVERSION__ >= 30000
//_____________________________________________________________________________
void DebugWGenerator::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFSpring.

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v=R__b.ReadVersion(&R__s, &R__c);
     if( R__v > 1 ) {
       DebugWGenerator::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
       return;
     }
     JSFModule::Streamer(R__b);
     R__b >> fWmass ;  // W boson mass
     R__b >> fWwidth;  // W boson width
     R__b.ReadStaticArray(fRp);
     R__b.ReadStaticArray(fRcosth);
     R__b.ReadStaticArray(fRphi);
     R__b.CheckByteCount(R__s, R__c, DebugWGenerator::IsA());

   } else {
     DebugWGenerator::Class()->WriteBuffer(R__b, this);
   }

}

#else
//_____________________________________________________________________________
void DebugWGenerator::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFSpring.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      JSFModule::Streamer(R__b);
      R__b >> fWmass ;  // W boson mass
      R__b >> fWwidth;  // W boson width
      R__b.ReadStaticArray(fRp);
      R__b.ReadStaticArray(fRcosth);
      R__b.ReadStaticArray(fRphi);
   } else {
      R__b.WriteVersion(JSFSpring::IsA());
      JSFModule::Streamer(R__b);
      R__b << fWmass ;  // W boson mass
      R__b << fWwidth;  // W boson width
      R__b.WriteArray(fRp,2);
      R__b.WriteArray(fRcosth,2);
      R__b.WriteArray(fRphi,2);
   }
}


#endif

