//*LastUpdate:  v0.1.01 8-October-1998 by Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  8-October-1998

//////////////////////////////////////////////////////////////////
//
//  CCDRawData3
//  
//  Functions specially for Beam03 condition.s
//
//////////////////////////////////////////////////////////////////


#include "JSFSteer.h"
#include "CCDRawData.h"

ClassImp(CCDEnvironmentBuf3)

//___________________________________________________________________________
CCDEnvironmentBuf3::CCDEnvironmentBuf3()
{
// CCD Environment Unpacker for Beam test 3
  fTestID=3;
}

//___________________________________________________________________________
CCDEnvironmentBuf3::~CCDEnvironmentBuf3()
{
// CCD Environment Unpacker for Beam test 3
}

//___________________________________________________________________________
void CCDEnvironmentBuf3::GetDarkReference(Int_t nccd, CCDMinMax& minmax)
{
  BEAM03_ENVRECHEAD *h=(BEAM03_ENVRECHEAD*)fEnvdata;
  Int_t iadc=fMap[nccd].adcid;
  Int_t loc=sizeof(BEAM03_ENVRECHEAD)/4+iadc*h->nwclock;
  BEAM03_ADCFORMAT *a=(BEAM03_ADCFORMAT*)&fEnvdata[loc];

  Int_t inact=a->hf.inact + a->hf.brackref + a->hf.isolation +
	      a->heffect + a->hb.isolation + a->hb.brackref;
  Int_t ninact=a->hb.brackref;
  if( a->hb.inact != 0 ) {  // In case of EEV CCD
     ninact = a->hb.inact;
     inact  = inact + ninact + 2 ; // Add 2 to correct data error.
  }
  minmax = CCDMinMax(inact-ninact, inact);
  return;

}

//___________________________________________________________________________
void CCDEnvironmentBuf3::GetSensitiveRegion(Int_t nccd, CCDXYMinMax& minmax)
{
  BEAM03_ENVRECHEAD *h=(BEAM03_ENVRECHEAD*)fEnvdata;
  Int_t iadc=fMap[nccd].adcid;
  Int_t loc=sizeof(BEAM03_ENVRECHEAD)/4+iadc*h->nwclock;
  BEAM03_ADCFORMAT *a=(BEAM03_ADCFORMAT*)&fEnvdata[loc];

  Int_t h1st =a->hf.inact+a->hf.brackref + a->hf.isolation ;
  Int_t hlast= h1st + a->heffect ;
  Int_t v1st =a->vf.inact+a->vf.brackref + a->vf.isolation ;
  Int_t vlast=v1st + a->veffect ;

  CCDMinMax xrange=CCDMinMax(h1st, hlast);
  CCDMinMax yrange=CCDMinMax(v1st, vlast);
  
  minmax=CCDXYMinMax(xrange, yrange);

  return ;
}


//___________________________________________________________________________
void CCDEnvironmentBuf3::UnPack(Int_t hd[], Int_t data[])
{
//  Unpack Environment data for Beam test 3

   CCDEnvironmentBuf::UnPack(hd, data);

// Set Number of CCD

   BEAM03_ENVRECHEAD *h=(BEAM03_ENVRECHEAD*)fEnvdata;
   if( h->formatid != 19980601 ) {
       Warning("GetNCCD","First word (%d) is invalid",h->formatid);
       return ;
   }

   if( fNCCD != h->numccd ) {
     if( fNCCD != 0 ) Warning("UnPack", 
	"Number of CCD is changed from %d to %d", fNCCD, h->numccd);
     fNCCD=h->numccd;
     if( fMap ) delete [] fMap;
     fMap=new CCDtoADCMap[fNCCD];

   // Prepare fMap ( ccd# to (ADC, Loc#)

     Int_t nadc=h->numadc;
     Int_t iped=sizeof(BEAM03_ENVRECHEAD)/4+nadc*h->nwclock;

     for(Int_t ia=0;ia<nadc;ia++){
    	BEAM03_ENVADCHEAD *ah=(BEAM03_ENVADCHEAD*)&fEnvdata[iped];
        iped += sizeof(BEAM03_ENVADCHEAD)/4 ;
	for(Int_t ic=0;ic< ah->numccd; ic++) {
	   BEAM03_ENVCCD *ec=(BEAM03_ENVCCD*)&fEnvdata[iped];
	   iped += sizeof(BEAM03_ENVCCD)/4;
	   fMap[ec->ccdpos-1].adcid=ia;
	   fMap[ec->ccdpos-1].loc=ec->ccdorder;
        }
      }
   }
}


//___________________________________________________________________________
Int_t CCDEnvironmentBuf3::GetNx(Int_t ccd)
{
// Return number of x pixels ( including non-active ones)

   BEAM03_ENVRECHEAD *h=(BEAM03_ENVRECHEAD*)fEnvdata;
   Int_t iped=sizeof(BEAM03_ENVRECHEAD)/4+fMap[ccd].adcid*h->nwclock;
   BEAM03_ADCFORMAT *ad=(BEAM03_ADCFORMAT*)&fEnvdata[iped];
   return ad->sumh;
}

//___________________________________________________________________________
Int_t CCDEnvironmentBuf3::GetNy(Int_t ccd)
{
// Return number of y pixels ( including non-active ones)

   BEAM03_ENVRECHEAD *h=(BEAM03_ENVRECHEAD*)fEnvdata;
   Int_t iped=sizeof(BEAM03_ENVRECHEAD)/4+fMap[ccd].adcid*h->nwclock;
   BEAM03_ADCFORMAT *ad=(BEAM03_ADCFORMAT*)&fEnvdata[iped];
   return ad->sumv;

}


//___________________________________________________________________________
Int_t CCDEnvironmentBuf3::GetType(Int_t ccd)
{
// Return number of CCD of this setup

  return 0;
}

//___________________________________________________________________________
void CCDEnvironmentBuf3::UnPackADC(Short_t data[], Int_t &nccd, Int_t& lnx, 
	Int_t& lny,  Int_t& loffset, Int_t& locadc)
{
// UnPack CCD Rawdata, using the  parameters stored in the environment 
// record.  This is for Beam-test3 configuration, assuming
// upto 2 ADC module, with max. 3 CCD in first ADC and 1 CCD in the
// second ADC module.

  CCDEnvironmentBuf::UnPackADC(data, nccd, lnx, lny, loffset, locadc);

//  Unpack ADC data part.

  BEAM03_ENVRECHEAD *h=(BEAM03_ENVRECHEAD*)fEnvdata;

// Loo over ADC.
#if defined(R__ACC) || defined(AIX)
  Int_t  ccdind[10];
#else
  Int_t  ccdind[fNCCD];
#endif
  Int_t  iv, ih, ic, ip, now, ipadc, kc;
  Int_t iped, ipedf;
  Short_t *adc=(Short_t*)locadc;
  Int_t *nx=(Int_t*)lnx  ;  Int_t *offset=(Int_t*)loffset;
  ipadc=0;
  iped =sizeof(BEAM03_ENVRECHEAD)/4 + h->numadc*h->nwclock ;
  for(Int_t ia=0;ia<h->numadc;ia++) {
    ipedf=sizeof(BEAM03_ENVRECHEAD)/4 + ia*h->nwclock;

    BEAM03_ENVADCHEAD *ah=(BEAM03_ENVADCHEAD*)&fEnvdata[iped];
    BEAM03_ADCFORMAT  *af=(BEAM03_ADCFORMAT*)&fEnvdata[ipedf];
	
    if( ah->nwadc != 637729 && ah->nwadc != 159433) {
	Error("UnPackADC",
	"Unpack routine for this configuration is not implemented yet!!");
	break;
    }
//  Make ccdind[i] ; which data corresponds to which ccd
    for(ic=0;ic<ah->numccd;ic++){
        BEAM03_ENVCCD *ch=(BEAM03_ENVCCD*)&fEnvdata[
 	  iped+sizeof(BEAM03_ENVADCHEAD)/4 + ic*sizeof(BEAM03_ENVCCD)/4 ];
	ccdind[ic]=ch->ccdpos-1;
    }
    //  Following algorithm is special for this setup.
    switch (ia) { 
      case 0: {
        for(iv=0;iv<af->sumh;iv++){
          for(ih=0;ih<af->sumh;ih++){
  	    for(ic=0;ic<ah->numccd;ic++){
              ip=4*(ih+iv*af->sumh)+ic ;
              now = ( (data[ip]>>4) & 0x07ff ) | 
                         ( ( (data[ip]>>4) & 0x0800 ) ^ 0x0800  ) ;
	      kc=ccdind[ic];
              adc[offset[kc]+nx[kc]*iv+ih]= now;
         }}}
         ipadc+=ah->nwadc*2;
         break;
      }  // end of case 0
      case 1: {
        for(iv=0;iv<af->sumv;iv++){
          for(ih=0;ih<af->sumh;ih++){
            ip=ih+iv*af->sumh + ipadc ;
	    ic  = 0 ;
            now = ( (data[ip]>>4) & 0x07ff ) | 
                         ( ( (data[ip]>>4) & 0x0800 ) ^ 0x0800  ) ;
            kc=ccdind[ic];
            adc[offset[kc]+nx[kc]*iv+ih]=now;
       }}
       break ;
      }  // End of case 1
    } // End of switch

    iped += ah->numccd*sizeof(BEAM03_ENVCCD)/4 + sizeof(BEAM03_ENVADCHEAD)/4;
  } // End of ADC loop

}


