#ifndef __CCD2x2Cluster__
#define __CCD2x2Cluster__

//////////////////////////////////////////////////////////////////////////
//
// CCD2x2Cluster
//
// 2x2 Clustering
//
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"
#include "TClonesArray.h"
#include "TH1.h"

#ifndef __JSFModule__
#include "JSFModule.h"
#endif
#ifndef __CCDRawData__
#include "CCDRawData.h"
#endif


class CCD2x2Cluster;
class CCD2x2ClusterBuf;

//** Single Cluster format 

static const Int_t kCCDMaxClusterSize = 5;
static const Int_t kCCDMaxNumber = 4;
static const Int_t kCCDMaxNumberOfCluster = 500;

class CCDCluster : public TObject {
 protected:
   Int_t fI[2];     // X and Y address of peak pixel.
   Int_t fComb;     // 0 - 4 which gives max 2x2 combination
   Float_t fX[2];   // X and Y coordinates of the cluster.
   Float_t fR[2];   // Peak/neighbour Ph ratio
   Float_t fPhsum;  // Cluster pulse height
   Float_t fPh[kCCDMaxClusterSize][kCCDMaxClusterSize]; // ADC value of each pixel.
 public:
   CCDCluster(){}
   virtual ~CCDCluster(){}

   friend class CCD2x2Cluster;
   friend class CCD2x2ClusterBuf;

  ClassDef(CCDCluster,1) // CCD Cluster
};


//*** Dark data.

class CCD2x2ClusterBuf : public JSFEventBuf {
  protected:
    Int_t fNCCD;  // Actual number of CCD
    Int_t fNcluster[kCCDMaxNumber]; // Number of Cluster in each CCD.
    CCDCluster fCluster[kCCDMaxNumber][kCCDMaxNumberOfCluster]; //! Cluster data

  public:
    CCD2x2ClusterBuf(const char *name="CCD2x2ClusterBuf",
	const char *title="CCD 2x2 cluster data", CCD2x2Cluster *mod=NULL);
    CCD2x2ClusterBuf(CCD2x2Cluster *mod, 
        const char *name="CCD2x2ClusterBuf", 
	const char *title="CCD 2x2 cluster data");
    virtual ~CCD2x2ClusterBuf(){};

    virtual void MakeCluster(CCDCorrectedADCBuf *buf, CCDEnvironmentBuf *env){}
    
    Int_t GetNcluster(Int_t nccd){ return fNcluster[nccd];}

    friend class CCD2x2Cluster;

    ClassDef(CCD2x2ClusterBuf, 1) // Cluster data by 2x2 clustering

};

//*** Basic Analysis module for 2x2 clustering

class CCD2x2Cluster : public JSFModule {
 protected:
   Float_t fThreshold[kCCDMaxNumber]; // Threshold for pulse height
   Bool_t fDoHist; // kTRUE to do histograming.
   Bool_t fShowHist; // kTRUE to show histogram
   TH1F *fHncls[kCCDMaxNumber];     //! Histogram of number of cluster
   TH1F *fHcph[kCCDMaxNumber];       //! Histogram of cluster Ph
//   TH1F **fHncls;     //! Histogram of number of cluster
//   TH1F **fHcph;       //! Histogram of cluster Ph
 public:
   CCD2x2Cluster(const Char_t *name="CCD2x2Cluster",
	      const Char_t *title="2x2 clustering");
   virtual ~CCD2x2Cluster();

   Bool_t Initialize();
   Bool_t Process(Int_t ev);

   ClassDef(CCD2x2Cluster, 1) // Do 2x2 clustering

};

#endif

