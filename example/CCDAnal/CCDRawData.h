#ifndef __CCDRawData__
#define __CCDRawData__

//////////////////////////////////////////////////////////////////////////
//
// CCDRawData
//
// A class to read CCD raw data
//
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TDatime.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif

static const Int_t kEventRecord=0;
static const Int_t kBeginRunRecord=1;
static const Int_t kPauseRunRecord=2;
static const Int_t kResumeRunRecord=3;
static const Int_t kEndRunRecord=4;
static const Int_t kCalibrationRunRecord=5;
static const Int_t kEnvironmentRecord=6;
static const Int_t kDarkRun=0;
static const Int_t kBeamRun=1;
static const Int_t kMoreDarkRun=2;

static const Int_t kCCDhpk10m=1;
static const Int_t kCCDhpk50m=2;
static const Int_t kCCDeev=11;

// ** Local structures.
struct CCDtoADCMap {
  Int_t adcid;  // ADC ID   0,1,....
  Int_t loc;    // CCDorder
};

struct CCDIXY {
  Int_t x;
  Int_t y;
};

struct CCDMinMax {
public:
  Int_t min;
  Int_t max;
 public:
  CCDMinMax(Int_t mn=0, Int_t mx=0){ min=mn ; max=mx;}
};
struct CCDXYMinMax {
 public:
  CCDMinMax x;
  CCDMinMax y;
 public:
  CCDXYMinMax(CCDMinMax ix=CCDMinMax(0,0), CCDMinMax iy=CCDMinMax(0,0))
    { x=ix ; y=iy; }
};

//* Structures to unpack CCD data in 3rd beam test

struct BEAM03_ENVRECHEAD {
  int formatid;
  int numadc;
  int numccd;
  int numccdtype;
  int nwclock;
  int nwccd;
};
struct BEAM03_ENVADCHEAD {
  int adcid;
  int nwadc;
  int numccd;
};
struct BEAM03_ENVCCD {
  int ccdpos;
  int ccdorder;
  int xdir;
  int ydir;
  int ccdtype;
};
struct BEAM03_ADCFORMAT {
  int sumh;
  int sumv;
  struct FBspecF { 
      int inact;
      int brackref;
      int isolation;
  }hf ; 
  int heffect ;
  struct FBspecB { 
    int isolation;
    int brackref;
    int inact;
    int overclock;
  } hb ;
  int dummy;
  int verticalshift;
  FBspecF vf;
  int veffect;
  FBspecB vb;
};


// ** CCD Unidaq header 

class CCDUnidaqHeader : public TObject {
protected:
  Int_t fBufferLength ; // Buffer length
  Int_t fRecordType;    // Record type
  Int_t fRunNumber;     // Run Number
  Int_t fEventNumber;   // Event Number
  Int_t fRunMode;       // Run mode 
  Int_t fReserved;      // Reserved word
  Int_t fHeadersize; // header word length.
public:
  CCDUnidaqHeader(){
    fBufferLength=0 ; fRecordType=0 ; fRunNumber=0 ;
    fEventNumber=0; fRunMode=0; fReserved=0; fHeadersize=6 ; }
  virtual ~CCDUnidaqHeader(){}

  void UnPack(Int_t hd[]);
  void Print();
  Int_t GetBufferLength(){return fBufferLength ;}
  Int_t GetRecordType(){return fRecordType;}
  Int_t GetRunNumber() {return fRunNumber;}
  Int_t GetEventNumber(){ return fEventNumber; }
  Int_t GetRunMode(){ return fRunMode; }
  Int_t GetReserved(){ return fReserved; }
  Int_t GetHeadersize(){ return fHeadersize; }

  ClassDef(CCDUnidaqHeader,1) // CCD Unidaq header
};

class CCDRawData;

// ** CCD Event Header
class CCDRawDataBuf : public JSFEventBuf {
protected:
  CCDUnidaqHeader fHeader; //! Unidaq header
  Int_t fNCCD;             //! Number of CCD in this data.
  Int_t *fNx;              //! Number of pixel in X for i-th CCD          
  Int_t *fNy;              //! Number of pixel in Y for i-th CCD         
  Int_t *fOffset;          //! Data location offset for i-th CCD
  Short_t *fADC;           //! ADC raw data without any correction
public:
  CCDRawDataBuf(const char *name="CCDRawDataBuf",
		const char *title="CCD raw data", CCDRawData *mod=NULL);
  CCDRawDataBuf(CCDRawData *mod, const char *name="CCDRawDataBuf",
		const char *title="CCD raw data");
  virtual ~CCDRawDataBuf();

  inline Int_t fP(Int_t iccd, Int_t ix, Int_t iy){
    return fOffset[iccd]+fNx[iccd]*iy+ix ;
  } // Pointer to the location of ADC data. iccd,ic,iy ranges are 0 to n-1 
  
  void UnPack(Int_t hd[], Short_t data[]);

  CCDUnidaqHeader *GetHeader(){ return &fHeader; }
  Short_t GetValue(Int_t ccd, Int_t nx, Int_t ny){ 
    return fADC[fP(ccd, nx, ny)]; }
  Short_t *ADC(Int_t ccd){return &fADC[fOffset[ccd]] ; }

  Int_t GetNx(Int_t ccd){ return fNx[ccd];}
  Int_t GetNy(Int_t ccd){ return fNy[ccd];}

  ClassDef(CCDRawDataBuf, 1) // CCD raw data buffer
};


// ** CCD Environment record
class CCDEnvironmentBuf : public TObject {
 protected:
  CCDUnidaqHeader fHeader; // Record header 
  Int_t  fTestID;      // Beam test ID , 1 to 3
  Int_t *fEnvdata;     //! Where to save Environment record.
  Int_t  fLenEnv;      //! Word length of Environment record.
  CCDtoADCMap  *fMap;  //! ccd -> (adc#, loc#) map.
  Int_t  fNCCD;        //! Number of CCD
 public:
  CCDEnvironmentBuf();
  virtual ~CCDEnvironmentBuf();
  virtual void UnPack(Int_t hd[], Int_t data[]);
  virtual void UnPackADC(Short_t data[], Int_t &nccd, Int_t& nx, Int_t& ny,
			 Int_t& offset, Int_t& adc);
  CCDUnidaqHeader *GetHeader(){ return &fHeader; }

  virtual Int_t GetTestID(){return fTestID;}
  virtual Int_t GetNCCD(){return fNCCD;}
  virtual Int_t GetNx(Int_t ccd){return 0;}
  virtual Int_t GetNy(Int_t ccd){return 0;}
  virtual Int_t GetType(Int_t ccd){return 0;}
  virtual void  GetDarkReference(Int_t nccd, CCDMinMax& minmax);
  virtual void  GetSensitiveRegion(Int_t nccd, CCDXYMinMax& minmax);


  ClassDef(CCDEnvironmentBuf, 1) // CCD Unidaq ENvironment record.
};


// ** CCD Environment unpacker for BeamTest-03
class CCDEnvironmentBuf3 : public CCDEnvironmentBuf {
 public:
  CCDEnvironmentBuf3();
  virtual ~CCDEnvironmentBuf3();

  virtual void UnPack(Int_t hd[], Int_t data[]);
  virtual Int_t GetNx(Int_t ccd);
  virtual Int_t GetNy(Int_t ccd);
  virtual Int_t GetType(Int_t ccd);
  virtual void UnPackADC(Short_t data[], Int_t &nccd, Int_t& nx, Int_t& ny,
			 Int_t& offset, Int_t& adc);
  virtual void  GetDarkReference(Int_t nccd, CCDMinMax& minmax);
  virtual void  GetSensitiveRegion(Int_t nccd, CCDXYMinMax& minmax);

  ClassDef(CCDEnvironmentBuf3, 1) // CCD Unidaq ENvironment record.
};




// ** CCD BeginRun record
class CCDBeginRunBuf : public TObject {
 protected:
  CCDUnidaqHeader fHeader; // Record header 
  Char_t *fDateTime ; //! Date and time
  Char_t *fComment;   //! Comment string
 public:
  CCDBeginRunBuf();
  virtual ~CCDBeginRunBuf();

  void UnPack(Int_t hd[], Char_t *str);
  CCDUnidaqHeader *GetHeader(){ return &fHeader; }
  
  Char_t *GetDateTime(){ return fDateTime; }
  Char_t *GetComment(){ return fComment;}

  ClassDef(CCDBeginRunBuf, 1) // CCD Unidaq BeginRun record
};

//*** Basic Analysis module of CCDRawData

class CCDRawData : public JSFModule {
 protected:
   CCDEnvironmentBuf *fEnvBuf;   //! Pointer to environment record
   CCDBeginRunBuf    *fBeginBuf; //! BeginRun Record

   Char_t   *fRFNFormat ; //! Pointer to the raw data file.
   FILE     *fd;          //! File descriptor for the raw data.

 public:
   CCDRawData(const Char_t *name="CCDRawData",
	      const Char_t *title="Get CCD Raw data");
   virtual ~CCDRawData();

   Bool_t BeginRun(Int_t nrun);
   Bool_t Process(Int_t ev);

   CCDEnvironmentBuf *GetEnvironment(){ return fEnvBuf; }
   CCDBeginRunBuf *GetBeginRunBuf(){ return fBeginBuf; }

   ClassDef(CCDRawData, 1) // Read CCD raw data file 
};

#endif

