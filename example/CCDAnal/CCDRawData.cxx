//*LastUpdate:  jsf-1-9 28-May-1999 by Akiya Miyamoto
//*LastUpdate:  v0.1.01 8-October-1998 by Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  8-October-1998

//////////////////////////////////////////////////////////////////
//
//  CCDRawData
//  
//  This class read CCD raw data saved in UNIDAQ format,
//  unpacks, and stores into class data member, when the root file
//  is opened write-enabled.  If the root file is read-only,
//  the data is obtained from the root file.
//
//  Corresponding to the record type of UNIDAQ data, following classes
//  are declared in CCDRawData.h
//     CCDRawDataBuf
//     CCDEnvironmentBuf
//     CCDBeginRunBuf
//     CCDEndRunBuf (*to be implemented)
//     CCDPauseBuf  (*to be implemented)
//     CCDResumeBuf (*to be implemented)
//  In addition,
//     CCDUnidaqHeader
//  defines the universal record header of all record.
//
//  In this program, it is assumed that first data obtained from the 
//  raw data file is begin run record.
//
//  Following parameters can be specified in the configuration file. 
//
//  CCDRawData.RFNFormat:       printf format to construct file name
//  CCDRawData.DoHist:         (TRUE/FASLE)  # Do Ph histogram or not.
//
//$Id$
//
//////////////////////////////////////////////////////////////////


#include "JSFSteer.h"
#include "CCDRawData.h"
#include "TRandom.h"


ClassImp(CCDRawDataBuf)
ClassImp(CCDEnvironmentBuf)
ClassImp(CCDBeginRunBuf)
ClassImp(CCDUnidaqHeader)
ClassImp(CCDRawData)

Int_t *iobuf;
Int_t  iobufsize;

static Int_t kReclength=0;
static Int_t kRectype=1;

static Short_t *lADC=0;
static Int_t *lOffset=0;
static Int_t *lNx=0;
static Int_t *lNy=0;
static Int_t lAstNccd=0;

static Int_t *lEnvdata=0;
static CCDtoADCMap *lMap;

//___________________________________________________________________________
CCDRawData::CCDRawData(const char *name, const char *title)
       : JSFModule(name,title)
{
//  
	printf(" constructor for CCDRawData is created...\n");
    fBeginBuf=new CCDBeginRunBuf();
    fEventBuf=new CCDRawDataBuf(this);

    Int_t len=strlen(gJSF->Env()->GetValue("CCDRawData.RFNFormat",
                               "/:/ea2/vtx/beam03/beam/r%7.7d.dat") );
    fRFNFormat=new Char_t[len+1];
    strcpy(fRFNFormat, gJSF->Env()->GetValue("CCDRawData.RFNFormat",
                               "/:/ea2/vtx/beam03/beam/r%7.7d.dat") );

    fd=NULL;

    SetBufferSize(2000000);  // buffer size for event data.

}

//___________________________________________________________________________
CCDRawData::~CCDRawData()
{
	printf(" CCDRawData will be deleted...fEventBuf is %d\n",(Int_t)fEventBuf);
    if( fEnvBuf ) delete fEnvBuf;
    delete fBeginBuf;
    delete fEventBuf;
    delete fRFNFormat;
    if(!fd) fclose(fd);
}

//___________________________________________________________________________
Bool_t CCDRawData::BeginRun(Int_t nrun)
{
//  
//  

	printf(" BeginRun is called. for run %d\n",nrun);

  if( ! fFile->IsWritable() ) { 
      TDirectory *last=gDirectory;
      Char_t dir[100];
      sprintf(dir,"/conf/CCDRawData/begin%5.5i",nrun);
      fFile->cd(dir);
	printf(" current file name is %s\n",fFile->GetPath());
	gDirectory->pwd();
      if( nrun > 1440 ) fEnvBuf=new CCDEnvironmentBuf3();
      else Error("BeginRun","UnPacker for this run %d is not installed.",nrun);
      fEnvBuf->Read("CCDEnvironmentBuf");  // Save beginrun record.
      printf(" Obtained Environment record for run %d\n",nrun);
	printf(" nccd in CCDRawData is %d\n",fEnvBuf->GetNCCD());
      last->cd();
      return kTRUE;
   }

  Int_t ln=strlen(fRFNFormat);
#if defined(R__ACC) || defined(AIX)
  Char_t infile[256];
#else 
  Char_t infile[ln+10];
#endif
  sprintf(infile,fRFNFormat,nrun);
	
  printf(" Going to open a file %s \n",infile);

  fd=fopen(infile,"rb");
  if( fd==NULL ) {
    Error("BeginRun","Unable to open a file %s.",infile);
    return kFALSE;
  }
  Int_t hd[6];
  Int_t lw=fread(hd, 4, 6, fd) ;
  if ( lw <= 0 || hd[kRectype] != kBeginRunRecord ) {
    Error("BeginRun","Can not find record or first record is not begin run");
    return kFALSE;
  }

#if defined(R__ACC) || defined(AIX)
  Int_t data[720000];
  if( hd[kReclength] > 720000 ) {
    printf(" Data size(%d) exceeds predefined buffer size(720000)\n",hd[kReclength]);
  }
#else
  Int_t ilen=hd[kReclength];
  Int_t data[ilen];
#endif
  Int_t kr=hd[kReclength]-6;
  lw=fread(data,4,kr,fd);
  fBeginBuf->UnPack(hd, (Char_t*)&data[0]);

// Prepare Unpacker run-dependantly.

  Int_t irun=fBeginBuf->GetHeader()->GetRunNumber();
  
  if( irun > 1440 ) fEnvBuf=new CCDEnvironmentBuf3();
  else Error("BeginRun","UnPacker for this run %d is not installed.",irun);

// output standard information
  SetModuleStatus(kBeginRun);
  SetRunNumber(irun);
  ConfDirectory();

  return kTRUE;
}

//___________________________________________________________________________
Bool_t CCDRawData::Process(Int_t ev)
{
// Read one event record and 
// Loop until obtain valid event record.

REPEAT:

  if( !iobuf ) { iobufsize=797162; iobuf=new Int_t[iobufsize]; }


  Int_t hd[6];
  Int_t lw=fread(hd, 4, 6, fd) ;
  if ( lw <= 0 ) {
	printf(" Reached end of file in CCDRawData::Process\n");
	return kFALSE;
  }
  if( hd[kReclength] > iobufsize ) {
     printf(" Reallocatting iobuf to save %d word data\n",hd[kReclength]);
     delete iobuf;
     iobufsize=hd[kReclength];
     iobuf=new Int_t[iobufsize];
  }

  Int_t kr=hd[kReclength]-6;
  lw=fread(iobuf,4,kr,fd);

  switch ( hd[kRectype] ) {
     case kEnvironmentRecord: {
	fEnvBuf->UnPack(hd, iobuf);
        TDirectory *last=gDirectory;
	Char_t dir[100];
	sprintf(dir,"/conf/CCDRawData/begin%5.5i",gJSF->GetRunNumber());
	fFile->cd(dir);
	fEnvBuf->Write("CCDEnvironmentBuf");  // Save beginrun record.
	last->cd();
	goto REPEAT;
     }
     case kEventRecord: {
	CCDRawDataBuf *buf=(CCDRawDataBuf*)fEventBuf;
	buf->UnPack(hd, (Short_t*) iobuf);
	break;
     }
     default: {
	printf(" Obtained record type is %d\n",hd[kRectype]);
	goto REPEAT;
     }
  }

  ((CCDRawDataBuf*)fEventBuf)->GetHeader()->Print();


  return kTRUE;

}

//___________________________________________________________________________
void CCDUnidaqHeader::UnPack(Int_t hd[])
{
// Unpack Unidaq header and set JSF Run Number and event number

   fBufferLength=hd[0];
   fRecordType=hd[1];
   fRunNumber=hd[2];
   fEventNumber=hd[3];
   fRunMode=hd[4];
   fReserved=hd[5];

   gJSF->SetRunNumber(fRunNumber);
   gJSF->SetEventNumber(fEventNumber);
}


//___________________________________________________________________________
void CCDUnidaqHeader::Print()
{
// Print Header data.

  printf(" Buffer Length=%d",fBufferLength);
  printf(" Record Type=%d",fRecordType);
  printf(" Run Number=%d",fRunNumber);
  printf(" Event Number=%d",fEventNumber);
  printf(" Run Mode=%d",fRunMode);
  printf(" Reserved%d",fReserved);
  printf("\n");
}

//___________________________________________________________________________
CCDEnvironmentBuf::CCDEnvironmentBuf()
{
// Buffer to save CCD environment record.
   fEnvdata=lEnvdata;
   fTestID=0;
   fLenEnv=0;
   fMap=lMap;
   fNCCD=0;
   return ;
}

//___________________________________________________________________________
CCDEnvironmentBuf::~CCDEnvironmentBuf()
{
}

//___________________________________________________________________________
void CCDEnvironmentBuf::UnPack(Int_t hd[], Int_t data[])
{
   fHeader.UnPack(hd);
   fHeader.Print();
   if( !fEnvdata ) {
	fHeader.Print();
	fLenEnv=fHeader.GetBufferLength()-fHeader.GetHeadersize();
	fEnvdata=new Int_t[fLenEnv];
   }
   memcpy(fEnvdata, data, 4*fLenEnv);
   
}

//___________________________________________________________________________
void CCDEnvironmentBuf::UnPackADC(Short_t data[], Int_t &nccd, 
  Int_t& inx, Int_t& iny, Int_t& ioffset, Int_t& locadc)
{
// UnPack Event ADC data, using the parameter stored in the enviromnent record.

   static Int_t lastrun=0;
   static Int_t *nx=0;
   static Int_t *ny=0;
   static Int_t *offset=0;
   static Short_t *adc=0;
   if ( lastrun != fHeader.GetRunNumber() ) {
       printf(" Got new run %d, unpacking parameters are reset\n",
	 	fHeader.GetRunNumber());
       if( !(nccd == fNCCD) ) { // Number of CCD is changed
	  if( nccd != 0 ) delete nx ;delete ny; delete offset; delete adc;
	  nccd = fNCCD;
	  nx = new Int_t[fNCCD];
	  ny = new Int_t[fNCCD];
	  offset = new Int_t[fNCCD+1];
	  adc = NULL ;
       }
       offset[0]=0;
       for(Int_t i=0;i<fNCCD;i++){
	 nx[i]=GetNx(i);
	 ny[i]=GetNy(i);
	 offset[i+1]=offset[i]+nx[i]*ny[i];
       }
       if( !(adc) ) adc = new Short_t[offset[fNCCD]];
       lastrun = fHeader.GetRunNumber();
   }

   inx=(Int_t)nx;          iny=(Int_t)ny;
   ioffset=(Int_t)offset;  locadc=(Int_t)adc;
}

//___________________________________________________________________________
void CCDEnvironmentBuf::GetDarkReference(Int_t nccd, CCDMinMax& minmax)
{
// returns the location of dark signal.
// meaning of the return value is ,    min <= ix < max,
// where ix is the pixel number ranges from 0 to n-1.
//
  Warning("GetDarkReference","Default function was called.\n");
  minmax = CCDMinMax(0,0);
}

//___________________________________________________________________________
void CCDEnvironmentBuf::GetSensitiveRegion(Int_t nccd, CCDXYMinMax& minmax)
{
// Returns senseitive region for x and y
// CCDXYMinMax contains the value,  
//          xmin <= ix < xmax       and          ymin <= iy < ymax
// where ix and iy are pixel address ranges from 0 to n-1.
//
  Warning("GetSensitiveRegion","Default function was called.\n");
  CCDMinMax x = CCDMinMax(0,0);
  CCDMinMax y = CCDMinMax(0,0);
  minmax=CCDXYMinMax(x,y);
}

//___________________________________________________________________________
CCDBeginRunBuf::CCDBeginRunBuf()
{
   fDateTime=NULL;
   fComment=NULL;
}

//___________________________________________________________________________
CCDBeginRunBuf::~CCDBeginRunBuf()
{
   if( fDateTime ) delete fDateTime;
   if( fComment ) delete fComment;
}


//___________________________________________________________________________
void CCDBeginRunBuf::UnPack(Int_t hd[], Char_t *data)
{
   fHeader.UnPack(hd);

   Int_t ldt=strlen(data);
   if( fDateTime ) {
	fDateTime=new Char_t[ldt+1];
   } else {
	if( (Int_t)(4*strlen(fDateTime)) <= ldt ) {
	   delete fDateTime;
  	   fDateTime=new Char_t[ldt+1];
        }
   }
   strcpy(fDateTime, data );

   printf(" Got begin run DateTime is %s\n",fDateTime);

}

//___________________________________________________________________________
CCDRawDataBuf::CCDRawDataBuf(const char *name, const char *title, CCDRawData *module) 
	: JSFEventBuf(name, title, (JSFModule*)module)
{
   fNCCD=lAstNccd;
   fNx=lNx;
   fNy=lNy;
   fOffset=lOffset;
   fADC=lADC;
}

//___________________________________________________________________________
CCDRawDataBuf::CCDRawDataBuf(CCDRawData *module, const char *name, const char *title) 
	: JSFEventBuf(name, title, (JSFModule*)module)
{
   fNCCD=lAstNccd;
   fNx=lNx;
   fNy=lNy;
   fOffset=lOffset;
   fADC=lADC;
}

//___________________________________________________________________________
CCDRawDataBuf::~CCDRawDataBuf()
{
}


//___________________________________________________________________________
void CCDRawDataBuf::UnPack(Int_t hd[], Short_t data[])
{
   fHeader.UnPack(hd);


   //  Reset unpack information whennever run number is changed.

   CCDEnvironmentBuf *env=((CCDRawData*)fModule)->GetEnvironment();

   Int_t lnx;  Int_t lny ; Int_t loffset ; Int_t ladc;

   env->UnPackADC(data, fNCCD, lnx, lny, loffset, ladc);

   fNx=(Int_t*)lnx;  fNy=(Int_t*)lny;  
   fOffset=(Int_t*)loffset;   fADC=(Short_t*)ladc;

   lNx=fNx;  lNy=fNy ; lOffset=fOffset ; lADC =fADC; lAstNccd=fNCCD ;

}



//______________________________________________________________________________
void CCDRawDataBuf::Streamer(TBuffer &R__b)
{
   // Stream an object of class CCDRawDataBuf.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      JSFEventBuf::Streamer(R__b);
      fHeader.Streamer(R__b);

      Int_t nccd;
      R__b >> nccd;
      if( nccd != fNCCD ) {
	if( nccd != 0 ) { 
	   	delete fNx ; delete fNy; delete fOffset ; delete fADC ; }
	fNCCD=nccd;
	fNx = new Int_t[fNCCD];
	fNy = new Int_t[fNCCD];
	fOffset = new Int_t[fNCCD+1];
	lNx=fNx;  lNy=fNy ; lOffset=fOffset ; lAstNccd=fNCCD ;
        fADC=NULL;
      }
      R__b.ReadArray(fNx);
      R__b.ReadArray(fNy);
      R__b.ReadArray(fOffset);
      if( !fADC ) { 
	fADC=new Short_t[fOffset[fNCCD]];  lADC=fADC;
      }
      R__b.ReadArray(fADC);
   } else {
      R__b.WriteVersion(CCDRawDataBuf::IsA());
      JSFEventBuf::Streamer(R__b);
      fHeader.Streamer(R__b);
      R__b << fNCCD;
      R__b.WriteArray(fNx, fNCCD);
      R__b.WriteArray(fNy, fNCCD);
      R__b.WriteArray(fOffset, fNCCD+1);
      R__b.WriteArray(fADC,fOffset[fNCCD]);
   }
}


//______________________________________________________________________________
void CCDEnvironmentBuf::Streamer(TBuffer &R__b)
{
   // Stream an object of class CCDEnvironmentBuf.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TObject::Streamer(R__b);
      fHeader.Streamer(R__b);
      R__b >> fTestID;
      R__b >> fNCCD;
      R__b >> fLenEnv;
      if( ! fEnvdata ) {
	 fEnvdata = new Int_t[fLenEnv];
         fMap = new CCDtoADCMap[fNCCD];
         lEnvdata =fEnvdata ;
	 lMap = fMap;
      }
      R__b.ReadArray(fEnvdata);
      for(Int_t i=0;i<fNCCD;i++) {
	R__b >> fMap[i].adcid;
	R__b >> fMap[i].loc;
      }

   } else {
      R__b.WriteVersion(CCDEnvironmentBuf::IsA());
      TObject::Streamer(R__b);
      fHeader.Streamer(R__b);
      R__b << fTestID;
      R__b << fNCCD;
      R__b << fLenEnv;
      R__b.WriteArray(fEnvdata, fLenEnv);
      for(Int_t i=0;i<fNCCD;i++) {
	R__b << fMap[i].adcid;
	R__b << fMap[i].loc;
      }
   }
}
