//*LastUpdate:  v0.1.01 14-October-1998 by Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  14-October-1998

//////////////////////////////////////////////////////////////////
//
//  CCD2x2Clustering
//  
//  Does 2x2 Clustering 
//
//  CCD2x2Clustering.Threshold0:   Threshould for 0-th CCD
//  CCD2x2Clustering.Threshold1:   Threshould for 1-th CCD
//  CCD2x2Clustering.Threshold2:   Threshould for 2-th CCD
//  CCD2x2Clustering.Threshold3:   Threshould for 3-th CCD
//  CCD2x2Clustering.DoHist:       (TRUE/FALSE) Do histograming or not
//  CCD2x2Clustering.ShowHist:     (TRUE/FALSE) Show histograming or not
//
//////////////////////////////////////////////////////////////////


#include "JSFSteer.h"
#include "CCDRawData.h"
#include "CCDCorrectDark.h"
#include "CCD2x2Cluster.h"
#include "CCDDBS.h"

ClassImp(CCDCluster)
ClassImp(CCD2x2ClusterBuf)
ClassImp(CCD2x2Cluster)

//___________________________________________________________________________
CCD2x2Cluster::CCD2x2Cluster(const char *name, const char *title)
       : JSFModule(name,title)
{
//  
   fEventBuf=new CCD2x2ClusterBuf(this);

   fThreshold[0]=gJSF->Env()->GetValue("CCD2x2Cluster.Threshold0",5.0);
   fThreshold[1]=gJSF->Env()->GetValue("CCD2x2Cluster.Threshold1",5.0);
   fThreshold[2]=gJSF->Env()->GetValue("CCD2x2Cluster.Threshold2",5.0);
   fThreshold[3]=gJSF->Env()->GetValue("CCD2x2Cluster.Threshold3",5.0);

   fDoHist=gJSF->Env()->GetValue("CCD2x2Cluster.DoHist",kFALSE);
   fShowHist=gJSF->Env()->GetValue("CCD2x2Cluster.ShowHist",kFALSE);

   SetBufferSize(64000);

}

//___________________________________________________________________________
CCD2x2Cluster::~CCD2x2Cluster()
{
//   if( fHncls[0] ) delete [] fHncls ;
//   if( fHcph[0] ) delete [] fHcph ;
}

//___________________________________________________________________________
Bool_t CCD2x2Cluster::Initialize()
{
//  
  if( fDoHist ) { 
     TDirectory *last=gDirectory;
     fFile->cd();
     Int_t nccd=kCCDMaxNumber;
     for(Int_t i=0;i<nccd;i++){
        Char_t idstr[10], title[40];
	sprintf(idstr,"fHncls%d",i);
	sprintf(title,"Number of Cluster for CCD#%d",i);
   	fHncls[i]=new TH1F(idstr,title,100,0.0,200.0);
	sprintf(idstr,"fHcph%d",i);
	sprintf(title,"2x2 Cluster pulse height CCD#%d",i);
   	fHcph[i]=new TH1F(idstr,title,100,-50.0, 450.0);
     }
     last->cd();
  }

  return kTRUE;
}

//___________________________________________________________________________
Bool_t CCD2x2Cluster::Process(Int_t ev)
{
// Does 2x2 clustering of CCD data

  CCDRawData *raw=(CCDRawData*)gJSF->FindModule("CCDRawData");
  CCDEnvironmentBuf *env=(CCDEnvironmentBuf*)raw->GetEnvironment();

  CCDCorrectDark *cor=(CCDCorrectDark*)gJSF->FindModule("CCDCorrectDark");
  CCDCorrectedADCBuf *buf=(CCDCorrectedADCBuf*)cor->EventBuf();
  CCD2x2ClusterBuf *cbuf=(CCD2x2ClusterBuf*)fEventBuf;


  CCDXYMinMax reg;
  Int_t ic,ix,iy,nx,ny,i,j,nc;
  Float_t ph;
  Float_t *adc;
  CCDIXY iadr[4][3]={ {{-1,0},{0,-1},{-1,-1}},  {{-1,0},{0,1},{-1,1}},
	{{1,0},{0,-1},{1,-1}}, {{1,0},{0,1},{1,1}} } ;

  cbuf->fNCCD=env->GetNCCD();
  if( cbuf->fNCCD > 4 ) {
     Fatal("Process","Number of CCD (%d) > 4 is not supported",cbuf->fNCCD);
  }

  static CCDPedestals peds;
  static lastrun=0;
  if( lastrun != gJSF->GetRunNumber() ) peds.ReadDBS(gJSF->GetRunNumber());
  lastrun=gJSF->GetRunNumber();

  Float_t thr[cbuf->fNCCD];
  for( i=0;i<cbuf->fNCCD;i++){ thr[i]=fThreshold[i]*peds.Get(i)->fRms; }

  for(ic=0;ic<env->GetNCCD();ic++){    // CCD loop
    env->GetSensitiveRegion(ic,reg);
    cbuf->fNcluster[ic]=0;
    nx=buf->GetNx(ic);
    ny=buf->GetNy(ic);
    adc=buf->ADC(ic);
    Short_t map[ny][nx];
    memset(map,0,sizeof(map));         // map != 0 when selected as cluster
    nc=0;

     for(iy=reg.y.min+2;iy<reg.y.max-2;iy++){
      for(ix=reg.x.min+2;ix<reg.x.max-2;ix++){
         if( adc[nx*iy+ix] < thr[ic] ) continue;
         if( map[iy][ix] != 0 ) continue;
         ph=adc[nx*iy+ix];
         if( adc[nx*iy+ix+1] > ph ) continue;
         if( adc[nx*(iy+1)+ix] > ph ) continue;
	 if( adc[nx*(iy+1)+ix+1] > ph ) continue;

         // Found peak.  Max. Ph combination
	 Float_t phmax=0.0;
         Int_t ipeak=0;
	 for(i=0;i<4;i++){
	   Float_t sum=ph;
	   for(j=0;j<3;j++){ 
		sum += adc[nx*(iy+iadr[i][j].y)+ix+iadr[i][j].x] ;
	   }
           if( sum > phmax ) { phmax=sum; ipeak=i ; }
         }
	 map[iy][ix]=1;
	 for(j=0;j<3;j++){ map[iy+iadr[ipeak][j].y][ix+iadr[ipeak][j].x]=1; }

	 if( nc >= kCCDMaxNumberOfCluster ) {
	   Warning("Process","Too many Cluster found for CCD%d ",ic);
           break;
         }
         // Decided 2x2 combination.
	 nc++;
	 
	 cbuf->fCluster[ic][nc].fI[0]=ix;
	 cbuf->fCluster[ic][nc].fI[1]=iy;
	 cbuf->fCluster[ic][nc].fComb=ipeak;
	 cbuf->fCluster[ic][nc].fPhsum=phmax;
	 cbuf->fCluster[ic][nc].fR[0]= ((Float_t)iadr[ipeak][0].x)*
		adc[nx*(iy+iadr[ipeak][0].y)+ix+iadr[ipeak][0].x]/ph  ;
	 cbuf->fCluster[ic][nc].fR[1]= ((Float_t)iadr[ipeak][1].y)*
		adc[nx*(iy+iadr[ipeak][1].y)+ix+iadr[ipeak][1].x]/ph  ;

	 for(j=-2;j<3;j++){
	   for(i=-2;i<3;i++){
	     cbuf->fCluster[ic][nc].fPh[j+2][i+2]=adc[nx*(iy+j)+ix+i];
         }}

 	 if( fDoHist ) fHcph[ic]->Fill(phmax);

      }
      if( nc >= kCCDMaxNumberOfCluster ) break;
    }  // End of loop on a single loop
    cbuf->fNcluster[ic]=nc;
    if( fDoHist ) fHncls[ic]->Fill((Float_t)nc);

  } // end of CCD loop
	 
  return kTRUE;

}



//___________________________________________________________________________
CCD2x2ClusterBuf::CCD2x2ClusterBuf(const char *name, 
	const char *title, CCD2x2Cluster *module) 
	: JSFEventBuf(name, title, (JSFModule*)module)
{
   fNCCD=0;
}

//___________________________________________________________________________
CCD2x2ClusterBuf::CCD2x2ClusterBuf(CCD2x2Cluster *module, 
	const char *name, const char *title) 
	: JSFEventBuf(name, title, (JSFModule*)module)
{
   fNCCD=0;
}


//______________________________________________________________________________
void CCD2x2ClusterBuf::Streamer(TBuffer &R__b)
{
   // Stream an object of class CCD2x2ClusterBuf.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      JSFEventBuf::Streamer(R__b);
      R__b >> fNCCD;
      R__b.ReadStaticArray(fNcluster);
      for(Int_t ic=0;ic<fNCCD;ic++){
	for(Int_t j=0;j<fNcluster[ic];j++){
	  fCluster[ic][j].Streamer(R__b);
        }
      }
   } else {
      R__b.WriteVersion(CCD2x2ClusterBuf::IsA());
      JSFEventBuf::Streamer(R__b);
      R__b << fNCCD;
      R__b.WriteArray(fNcluster, 4);
      for(Int_t ic=0;ic<fNCCD;ic++){
	for(Int_t j=0;j<fNcluster[ic];j++){
	  fCluster[ic][j].Streamer(R__b);
        }
      }
   }
}

