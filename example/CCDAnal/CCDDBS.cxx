
//*LastUpdate:  v0.1.01 15-October-1998 by Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  15-October-1998

//////////////////////////////////////////////////////////////////
//
//  CCDDBS
//  
//  Collection of classes to accesss data base file for CCD
//
//////////////////////////////////////////////////////////////////


#include "JSFSteer.h"
#include "CCDDBS.h"

ClassImp(CCDRunIndex)
ClassImp(CCDPedestals)
ClassImp(CCDPedestal)
ClassImp(CCDDarkData)

//___________________________________________________________________________
CCDRunIndex::CCDRunIndex(Int_t run)
{
   if( run > 0 )  ReadDBS(run);  // Put this statement in your constructor
}

//___________________________________________________________________________
CCDRunIndex::CCDRunIndex(Int_t run, Int_t refrun, Int_t nccd, Char_t *name,
	       Float_t temp, Float_t angle, Float_t ebeam,
	       Int_t ndark, Int_t darkrun[])
	: JSFDBS(0)
{
     if( ! fFile->IsWritable() ) {
	Error("CCDRunIndex","Open data base file with write-enabled.");
        return;
     } else if ( ndark > (Int_t)(sizeof(fDarkRun)/4) ) {
	Warning("CCDRunIndex","Too many dark run is specified.");
        ndark=sizeof(fDarkRun)/4; 
     }
     fRun=run;
     fRefRun=refrun;
     fNCCD=nccd;
     strcpy(f3rdCCD,name);
     fTemp=temp;
     fAngle=angle;
     fEbeam=ebeam;
     fNdark=ndark;
     for(Int_t i=0;i<ndark;i++){
	fDarkRun[i] = darkrun[i];
     }

     WriteDBS(fRun);

}


//___________________________________________________________________________
CCDPedestals::CCDPedestals(Int_t run)
{
   if( run > 0 )  ReadDBS(run);  // Put this statement in your constructor
}

//___________________________________________________________________________
CCDPedestals::CCDPedestals(Int_t run, Int_t nccd, Int_t ncall, 
  		CCDPedestal ped[])  : JSFDBS(0)
{
     if( ! fFile->IsWritable() ) {
	Error(ClassName(),"Open data base file with write-enabled.");
        return;
     }
     fRun=run;
     fNCCD=nccd;
     fNcall=ncall;
     fPed = new CCDPedestal[fNCCD];
     for(Int_t i=0;i<fNCCD;i++) { fPed[i] = ped[i]; }

     WriteDBS(fRun);

}

//___________________________________________________________________________
CCDPedestals::~CCDPedestals()
{
  if( fPed ) delete fPed;
}

//___________________________________________________________________________
CCDPedestal::CCDPedestal(Int_t ccdid, Float_t mean , Float_t emean,
		Float_t rms, Float_t erms, Float_t chisq,
		Int_t nover5s, Int_t nunder5s)
{
   fCCDID=ccdid;
   fMean=mean;
   fEMean=emean;
   fRms=rms;
   fERms=erms;
   fChisq=chisq;
   fNover5s=nover5s;
   fNunder5s=fNunder5s;
}

//______________________________________________________________________________
void CCDPedestals::Streamer(TBuffer &R__b)
{
   // Stream an object of class CCDPedestals.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      JSFDBS::Streamer(R__b);
      R__b >> fRun;
      R__b >> fNCCD;
      R__b >> fNcall;
      fPed = new CCDPedestal[fNCCD];
      for(Int_t i=0;i<fNCCD;i++){
 	R__b >> fPed[i].fCCDID;
 	R__b >> fPed[i].fNover5s;
 	R__b >> fPed[i].fNunder5s;
 	R__b >> fPed[i].fMean;
 	R__b >> fPed[i].fEMean;
 	R__b >> fPed[i].fRms;
 	R__b >> fPed[i].fERms;
 	R__b >> fPed[i].fChisq;
      }
   } else {
      R__b.WriteVersion(CCDPedestals::IsA());
      JSFDBS::Streamer(R__b);
      R__b << fRun;
      R__b << fNCCD;
      R__b << fNcall;
      for(Int_t i=0;i<fNCCD;i++){
 	R__b << fPed[i].fCCDID;
 	R__b << fPed[i].fNover5s;
 	R__b << fPed[i].fNunder5s;
 	R__b << fPed[i].fMean;
 	R__b << fPed[i].fEMean;
 	R__b << fPed[i].fRms;
 	R__b << fPed[i].fERms;
 	R__b << fPed[i].fChisq;
      }
   }
}

//___________________________________________________________________________
CCDDarkData::CCDDarkData(Int_t run)
{
   fNCCD=0;
   fOffset=NULL;
   fNx=NULL;
   fNy=NULL;
   fDark=NULL;

   if( run > 0 )  ReadDBS(run);  // Put this statement in your constructor
}

//___________________________________________________________________________
CCDDarkData::~CCDDarkData()
{
  if( fOffset ) delete fOffset;
  if( fNx ) delete fNx;
  if( fNy ) delete fNy;
  if( fDark ) delete fDark;

}


//___________________________________________________________________________
void CCDDarkData::LoadFile(const Char_t *file)
{
//  Read dark data from the file.
//  This member should be used to prepare database file from standard binary data.
   
  FILE *fd;
  if( (fd=fopen(file,"rb")) == NULL ){
     Error("LoadDark","Unable to open dark file %s\n",file);
     return ;
  }  /* Open file, binary mode */

  printf("Loading dark data from a file %s",file);

  Int_t lw=fread(&fNCCD, 4, 1, fd);
  
  fOffset = new Int_t[fNCCD+1];
  fNx=new Int_t[fNCCD];
  fNy=new Int_t[fNCCD];
  
  struct darkinfo {
    int offset, nh, nv ; 
  };
#if defined(R__ACC) || defined(AIX)
  darkinfo di[10];
#else
  darkinfo di[fNCCD];
#endif
  int nw=3*fNCCD;
  lw=fread(di, 4, nw, fd);

  Int_t i;
  for(i=0;i<fNCCD;i++){
    fNx[i]=di[i].nh;
    fNy[i]=di[i].nv;
    fOffset[i]=di[i].offset-i;
  }
  fOffset[fNCCD]=fOffset[fNCCD-1]+di[fNCCD-1].nh*di[fNCCD-1].nv;
  if( !fDark ) fDark = new Float_t[fOffset[fNCCD]];
    
  for(i=0;i<fNCCD;i++){
    printf(".");
    nw=di[i].nh*di[i].nv;
    int iw;
    int kw=fread(&iw, 4, 1, fd);
    if( iw != nw ) { 
       Error("LoadDark","Fatal error .. number of pixels is wrong.");
       return ;
    }
    kw=fread(&fDark[fOffset[i]], 4, nw, fd);
  }
  fclose(fd);
  printf(" Completed.\n");
}


//______________________________________________________________________________
void CCDDarkData::Streamer(TBuffer &R__b)
{
   // Stream an object of class CCDDarkData.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      JSFDBS::Streamer(R__b);
      R__b >> fNCCD;
      fOffset=new Int_t[fNCCD+1];
      fNx=new Int_t[fNCCD];
      fNy=new Int_t[fNCCD];
      R__b.ReadArray(fOffset);
      R__b.ReadArray(fNx);
      R__b.ReadArray(fNy);
      fDark=new Float_t[fOffset[fNCCD]];
      R__b.ReadArray(fDark);
   } else {
      R__b.WriteVersion(CCDDarkData::IsA());
      JSFDBS::Streamer(R__b);
      R__b << fNCCD;
      R__b.WriteArray(fOffset, fNCCD+1);
      R__b.WriteArray(fNx, fNCCD);
      R__b.WriteArray(fNy, fNCCD);
      R__b.WriteArray(fDark, fOffset[fNCCD]);
   }
}

