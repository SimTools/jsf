//*LastUpdate:  v0.1.01 13-October-1998 by Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  13-October-1998

//////////////////////////////////////////////////////////////////
//
//  CCDCorrectDark
//  
//  Subtract dark data from ADC value
//
//////////////////////////////////////////////////////////////////


#include "JSFSteer.h"
#include "CCDRawData.h"
#include "CCDCorrectDark.h"
#include "CCDDBS.h"

ClassImp(CCDCorrectedADCBuf)
ClassImp(CCDCorrectDark)

//___________________________________________________________________________
CCDCorrectDark::CCDCorrectDark(const char *name, const char *title)
       : JSFModule(name,title)
{
//  
    fEventBuf=new CCDCorrectedADCBuf(this);

    SetMakeBranch(kFALSE) ; // Does not create branch for this module.
}

//___________________________________________________________________________
CCDCorrectDark::~CCDCorrectDark()
{
   if( fEventBuf ) delete fEventBuf;
}

//___________________________________________________________________________
Bool_t CCDCorrectDark::BeginRun(Int_t nrun)
{
//  
//  
  CCDRunIndex run;
  run.ReadDBS(nrun);  // Get Run index info for this run.
  Int_t *darkrun=run.GetDarkRunNumber();
  printf(" Using run%d as the dark data.\n",darkrun[0]);
  fDark=new CCDDarkData(darkrun[0]);
  
  return kTRUE;
}

//___________________________________________________________________________
Bool_t CCDCorrectDark::Process(Int_t ev)
{
// Read one event record and 
// Loop until obtain valid event record.

  CCDRawData *raw=(CCDRawData*)gJSF->FindModule("CCDRawData");
  CCDRawDataBuf *buf=(CCDRawDataBuf*)raw->EventBuf();
  CCDEnvironmentBuf *env=(CCDEnvironmentBuf*)raw->GetEnvironment();

  ((CCDCorrectedADCBuf*)fEventBuf)->AllocateBuf(env);

  CorrectDark(buf, env);

  return kTRUE;

}

//___________________________________________________________________________
void CCDCorrectedADCBuf::AllocateBuf(CCDEnvironmentBuf *env)
{
//  Allocate Buffer to save ADC data.

  if( fNCCD == env->GetNCCD() ) return ;

  if( fNCCD != 0 ) { 
     delete fOffset ; delete fNx ; delete fNy ; delete fADC ; }
  fNCCD = env->GetNCCD();
  fNx = new Int_t[fNCCD];
  fNy = new Int_t[fNCCD];
  fOffset = new Int_t[fNCCD+1];

  fOffset[0]=0;
  for(Int_t i=0;i<fNCCD;i++){
    fNx[i]=env->GetNx(i);
    fNy[i]=env->GetNy(i);
    if( i != 0 ) fOffset[i] = fOffset[i-1] + fNx[i-1]*fNy[i-1];
  }
  fOffset[fNCCD]=fOffset[fNCCD-1] + fNx[fNCCD-1]*fNy[fNCCD-1];

  fADC = new Float_t[fOffset[fNCCD]];

}


//___________________________________________________________________________
void CCDCorrectDark::CorrectDark(CCDRawDataBuf *buf, CCDEnvironmentBuf *env)
{
//  Subtrack Dark data from ADC.
 
  Int_t nccd=env->GetNCCD();
  Int_t ic,ix,iy,k;
  CCDMinMax  ipref;    // reference pixel is ipref.min <= ix <= ipref.max
  CCDXYMinMax ips ;    // reference to sensitive region.
  for(ic=0;ic<nccd;ic++) {
    env->GetDarkReference(ic, ipref);
    env->GetSensitiveRegion(ic, ips);
    Short_t *hadc=buf->ADC(ic);
    Float_t *adc=((CCDCorrectedADCBuf*)EventBuf())->ADC(ic);
    Int_t nx=env->GetNx(ic);
    Float_t xinvinact=1.0/(Float_t)(ipref.max-ipref.min);
    for(iy=ips.y.min;iy<ips.y.max;iy++){
      Float_t dsum=0.0;
      for(k=ipref.min;k<ipref.max;k++) {
	dsum += (Float_t)hadc[nx*iy+k];
      } 
      dsum*=xinvinact;
      for(ix=0;ix<ipref.min;ix++)  {
	adc[nx*iy+ix] = hadc[nx*iy+ix] - 
		( fDark->fDark[fDark->fOffset[ic]+nx*iy+ix] + dsum ) ; 
	}
	
    } // Y-loop
  } // CCD-loop
  
}


//___________________________________________________________________________
CCDCorrectedADCBuf::CCDCorrectedADCBuf(const char *name, 
	const char *title, CCDCorrectDark *module) 
	: JSFEventBuf(name, title, (JSFModule*)module)
{
   fNCCD=0;
   fADC=0;
   fNx=0;
   fOffset=0;
   fNy=0;
}

//___________________________________________________________________________
CCDCorrectedADCBuf::CCDCorrectedADCBuf(CCDCorrectDark *module, 
	const char *name, const char *title) 
	: JSFEventBuf(name, title, (JSFModule*)module)
{
   fNCCD=0;
   fADC=0;
   fNx=0;
   fOffset=0;
   fNy=0;
}

//___________________________________________________________________________
CCDCorrectedADCBuf::~CCDCorrectedADCBuf()
{
   if( fADC ) delete fADC;
   if( fNx  ) delete fNx;
   if( fNy  ) delete fNy ;
   if( fOffset ) delete fOffset;
}

