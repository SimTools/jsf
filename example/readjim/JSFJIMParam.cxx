//*LastUpdate :  jsf-1-8  18-April-1999  By Akiya Miyamoto
//*-- Author  : A.Miyamoto  18-April-1999


///////////////////////////////////////////////////////////////////
//
// JSFJIMParam
//
// A JIM detector parameter class
// 
//$Id:
//
//////////////////////////////////////////////////////////////////

#include "JSFJIMParam.h"

ClassImp(JSFJIMParam)

extern "C" {
extern void kzget_(Char_t *cname, Int_t *isegm, 
		   Int_t *ndata, Int_t idata[], Int_t lcname);
};

//_____________________________________________________________________________
JSFJIMParam::JSFJIMParam()
{
}

//_____________________________________________________________________________
JSFJIMParam::~JSFJIMParam()
{
}

//_____________________________________________________________________________
void JSFJIMParam::ReadParameter()
{
  // Read detector parameter for JIM from JIM data buffer.

  //  kzget_(Char_t *cname, Int_t *isegm, Int_t *ndata, Int_t idata[], lcname);
 
  Int_t isegm=0;
  Int_t ndata; Int_t idata[100];

  /*
  FILE *fd;
  fd=fopen(file,"r");
  if( !fd ) Fatal("ReadParamFile","Unable to open file %s",file);
 
  printf("JSFQuickSimParam::ReadParamDetector \n");
  printf(" --  Read detector parameter from the file %s\n",file);
  Char_t input[256];

  // Read a file
  while(fgets(input, 256, fd) ){

    if( !strncmp("@!",input,2) ) continue;
    
    Int_t id;
    Float_t val;
    sscanf(input,"%d %g",&id, &val);
   
  // Set a data to varaiables
    Float_t *clspar=&fCLSPAR[0][0];
    if( id == 1 ) continue ;
    else if( id == 2 ) { fBfield = val; 
                    prfeld_.bfield=10.1085  ;  // kgauss
                    prfeld_.ptor=329.9837713  ;
		    prfeld_.ropt=3.03045206e-3 ;
    }
    else if( 10 < id && id < 20 ) fTrack[id-11]=val;
    else if( 30 < id && id < 50 ) fEMCal[id-31]=val;
    else if( 50 < id && id < 70 ) fHDCal[id-51]=val;
    else if( id == 70 ) fNERRVX = (Int_t)val ; //# sampling layers + 1 = NSMPVX
    else if( id == 71 ) fNSMPVX = (Int_t)val ; //# sampling layers + 1 = NSMPVX
    else if( 1000 < id && id < 1021 ) clspar[id-1001]=val;

    else { 
      if( fNERRVX ==1 || fNERRVX == 2 ) {
        if( id == 72 ) fDPHIVX = val ; // phi pitch (cm)
        else if( id == 73 ) fDZEEVX = val ; // Z pitch (cm)
	else if( 73 < id && id < 4*(fNSMPVX+1)+74 ) { // VTX layer info.
	  Int_t ilay=(id-74)/4 ;
	  fVTXLayer[ilay][id-(74+4*ilay)]=val;
	}
	else if( 4*(fNSMPVX+1)+73 < id && id < 4*(fNSMPVX+1)+79 ) { // VTX error info.
	  fVTXError[id-(4*(fNSMPVX+1)+74 )]=val;
	}
      }
      else if( fNERRVX==3 ){
	if( 73 <= id  && id <= 76 ) { fVTXError[id-73]=val; }
	else if( 76 < id && id < 4*(fNSMPVX+1)+77 ) {
	  Int_t ilay=(id-76)/4;
	  fVTXLayer[ilay][id - (4*ilay+77)]=val;
	}
      }
      else {
        Fatal("ReadParamDetector",
	      "Invalid Parameter ID %d, Error line is \n%s",id,input);
      }
    }
  }
  fclose(fd);
  */  

}

