//*LastUpdate : 9-June-2000     Akiya Miyamoto 
//*-- Author  : Akiya Miyamoto  9-June-2000

///////////////////////////////////////////////////////////////////
//
//  Do topological vertex finding.
//
//$Id$  
//  
//////////////////////////////////////////////////////////////////

// #define debug

#include "JSFZVTOP3.h"
#include "JSFSteer.h"
#include "JSFSIMDST.h"

#include "JSFVertexing.h"
#include "JSFGeoCFit.h"
#include "JSFGeneratorParticle.h"

#include <TNtuple.h>

#include "JSFLCFULL.h"

ClassImp(JSFZVTOP3Track)
ClassImp(JSFZVTOP3Vertex)
ClassImp(JSFZVTOP3)


const Int_t kMaxTrks=100;
JSFCDCTrack **JSFZVTOP3::fTrackPointers=(JSFCDCTrack**)(new UInt_t[kMaxTrks]);

extern "C" {
  void zxswim_(Int_t *mode, Float_t *ok,Float_t vpos[], Int_t *id){
    JSFZVTOP3::ZXSWIM(mode, ok, vpos, id);
  };

  Int_t zxfit_(Int_t *mode, Int_t *jj, Int_t vid[], Float_t vpos[], 
      Float_t vpossg[],Float_t *chisq, Float_t xvtx[], Float_t xvtxsg[], 
      Float_t chisqtk[],Float_t pxyz[][3], Int_t *ier){
    return JSFZVTOP3::ZXFIT(mode,jj,vid,vpos,vpossg,chisq,xvtx,xvtxsg,chisqtk,pxyz,ier);
  };    

  extern Int_t zvres3_(Int_t *ntrk, Int_t id[],Float_t *pxj, Float_t *pyj, 
       Float_t *pzj,Float_t *ptotj, Int_t *jetno);

  extern Int_t zvtop3_(Int_t *ntrk, Int_t id[], Int_t *jetno, Int_t *errflg);

  Int_t jsfzvtop3_gufld_(Float_t pos[], Float_t b0[]){
     return JSFZVTOP3::GUFLD(pos, b0);
  }     

  extern Int_t zvptm_(Int_t *mode, Float_t axi[3], Float_t eaxi[6],
		      Float_t axf[3], Float_t eaxf[6], Float_t p[3],
		      Float_t *sigmax, Float_t *p3norm, Float_t *pt,
		      Float_t *ptmin, Float_t *ptmax);

  extern void uconfl_(Float_t *prob, Int_t *ndf, Float_t *chisq);

  Float_t prob_(Float_t *chisq, Int_t *ndf){
    Float_t pr;
    uconfl_(&pr, ndf, chisq);
    return pr;
  }

};

typedef struct {
  Int_t   ibnk[9];
  Float_t rbnk[16];
  Int_t   itrk[kMaxTrks][12];
  Float_t rtrk[kMaxTrks][52];
} COMMON_ZXTRKS;

extern COMMON_ZXTRKS zxtrks_bank_;

typedef struct {
  Int_t   tidy,mc,algo, mode,nvreq;
  Float_t prbv, b3min, disne, mkmin,mkmax,mlmin,mlmax,
          mgmax,dkmin,dlmin,rimin,i3min,icutp,icutn,ripe,
          zipe,pxja,pyja,pzja,rcut,xcut,kang,kipw,pwid,swim,
    gini,gmin,cmax,pcut,momf;
} COMMON_ZVKON3;
extern COMMON_ZVKON3 zvkon3_bank_;

const Int_t kMaxTopl3Jets=20;
const Int_t kMaxTopl3Trks=40;
typedef struct {
  Int_t ibnk[kMaxTopl3Jets][15];
  Int_t ivrt[kMaxTopl3Jets][7][2];
  Int_t itrk[kMaxTopl3Jets][kMaxTopl3Trks][4];
  Float_t rbnk[kMaxTopl3Jets][22];
  Float_t rvrt[kMaxTopl3Jets][7][14];
  Float_t rtrk[kMaxTopl3Jets][kMaxTopl3Trks][17];
} COMMON_ZVTOPL3;
extern COMMON_ZVTOPL3 zvtopl3_;

 
typedef struct {
  Float_t  ecm, decm,beampos[3],dbeamps[6];
  Float_t  pol,dpol,pdtime;
  Float_t  xyzpri[3],dxyzpri[6],rmspri;
  Int_t    rtrnpri;
} COMMON_PHBM ;
extern COMMON_PHBM phbm_bank_;


TNtuple *ntrlodi=0;

//_____________________________________________________________________________
JSFZVTOP3::JSFZVTOP3()
{

  if( ntrlodi == 0 ) {
  ntrlodi=new TNtuple("ntrlodi","Ntuple","trdi:lodi:disv:lodidisv");
  }

  zvkon3_bank_.tidy=0 ;    // set TIDY=0 to use all tracks 
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.TIDY","0"),"%d",&zvkon3_bank_.tidy);

  zvkon3_bank_.prbv=0.01 ; // consider V if fit prob. > PRBV
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.PRBV","0.01"),"%g",&zvkon3_bank_.prbv);

  zvkon3_bank_.b3min=3.0 ; // both V tracks must be B3MIN sigma from IP
                     // (for Kshort and lambda only)
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.B3MIN","3.0"),"%g",&zvkon3_bank_.b3min);

  zvkon3_bank_.disne=250.0; // min DIStance to NEarest other track from V
                      // in microns (for Kshort, lambda and gamma)
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.DISNE","250.0"),"%g",&zvkon3_bank_.disne);

  zvkon3_bank_.mkmin=0.485; // lower Mass range for Kshort identification
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.MKMIN","0.485"),"%g",&zvkon3_bank_.mkmin);

  zvkon3_bank_.mkmax=0.515; // upper Mass...
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.MKMAX","0.515"),"%g",&zvkon3_bank_.mkmax);

  zvkon3_bank_.mlmin=1.112; // lower Mass range for Lambda identification
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.MLMIN","1.112"),"%g",&zvkon3_bank_.mlmin);

  zvkon3_bank_.mlmax=1.120; // upper Mass (..both pion-proton combinations)
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.MLMAX","1.120"),"%g",&zvkon3_bank_.mlmax);

  zvkon3_bank_.mgmax=0.025; // MAX Mass for e+e- conversion
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.MGMAX","0.025"),"%g",&zvkon3_bank_.mgmax);

  zvkon3_bank_.dkmin=0.5;   // MIN distance/cm from IP to V for Kshort
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.DKMIN","0.5"),"%g",&zvkon3_bank_.dkmin);

  zvkon3_bank_.dlmin=1.0;   // MIN distance/cm from IP to V for Lambda
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.DLMIN","1.0"),"%g",&zvkon3_bank_.dlmin);

  zvkon3_bank_.rimin=2.2;   // MIN Radius/cm for Interaction vertex
                      // (for gamma conversion, detector interaction)
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.RIMIN","2.2"),"%g",&zvkon3_bank_.rimin);

  zvkon3_bank_.i3min=0.15;  // remove if both tracks 3D Imp. para. > I3MIN/cm 
                      // (detector interaction selection only)
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.I3MIN","0.15"),"%g",&zvkon3_bank_.i3min);

  zvkon3_bank_.icutp=0.3;   // remove ANY track 3D Imp. para > ICUTP/cm 
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.ICUTP","0.3"),"%g",&zvkon3_bank_.icutp);

  zvkon3_bank_.icutn=-0.1;  // remove ANY track 3D Imp. para < ICUTN/cm 
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.ICUTN","-0.1"),"%g",&zvkon3_bank_.icutn);

  //
  //! general ZVTOP input
  // 
  zvkon3_bank_.ripe=7.0;    // xy error on IP ellipsoid in ZVTOP, microns.
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.RIPE","7.0"),"%g",&zvkon3_bank_.ripe);

  zvkon3_bank_.zipe=30.0;   // z error on IP ellipsoid in ZVTOP, microns.
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.ZIPE","30.0"),"%g",&zvkon3_bank_.zipe);

  zvkon3_bank_.pxja=0.0;    // Jet Axis direction used to calculate angle
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.PXJA","0.0"),"%g",&zvkon3_bank_.pxja);

  zvkon3_bank_.pyja=0.0;    // A for KANG weight for ALGO=1 and for initial
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.PYJA","0.0"),"%g",&zvkon3_bank_.pyja);

  zvkon3_bank_.pzja=0.0;    // ghost axis in ALGO=2. By default (all P*JA = 0) 
                      // the sum of the input track momenta is used.
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.PZJA","0.0"),"%g",&zvkon3_bank_.pzja);

  zvkon3_bank_.mc=0;        // MC=1 : include Monte Carlo information in
                      //              the output. 
  zvkon3_bank_.mc=gJSF->Env()->GetValue("JSFZVTOP3.MC",0);
 
  //! select ZVTOP algorithm
  zvkon3_bank_.algo=1;      // 1 = ZVRES  original pure vertex RESolubility
                      // 2 = ZVKIN  KINematic info and ghost track
  zvkon3_bank_.algo=gJSF->Env()->GetValue("JSFZVTOP3.ALGO",1);

  //! tunable inputs for ZVRES
  zvkon3_bank_.rcut=0.6;    // resolubility cut, can vary as 0.0 < VCUT < 1.0,
                      // larger values produce more vertices.
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.RCUT","0.6"),"%g",&zvkon3_bank_.rcut);

  zvkon3_bank_.xcut=10.0;   // cut on the chi**2 contribution of a track to 
                      // a vertex.  
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.XCUT","10.0"),"%g",&zvkon3_bank_.xcut);

  zvkon3_bank_.kang=5.0;    // Weights vertex finding probability by the
                      // factor exp(-KANG*A**2) where A is the angle
                      // in radians between the line joining the IP
                      // to the 3D spatial co-ordinate and the jet
                      // axis. (+ve values ~10 increase vertex 
                      // finding efficiency in core of jet).
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.KANG","5.0"),"%g",&zvkon3_bank_.kang);

  zvkon3_bank_.kipw=1.0;    // i.p track weight, weights the significance
                      // of the IP, large values will tend to absorb
                      // nearby vertices/fakes into the primary vertex.
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.KIPW","1.0"),"%g",&zvkon3_bank_.kipw);

  zvkon3_bank_.mode=0;      // 0 -usual ZVTOP/ZVRES
                      // 1 -lepton or high impact track mode - Note 2
                      // 2 -like 1, but remove track for vertex finding 
                      // 3 -...and remove space on IP side of lepton 
  zvkon3_bank_.mode=gJSF->Env()->GetValue("JSFZVTOP3.MODE",0);

  zvkon3_bank_.pwid=100.0;  // Gaussian WIDth of Plane for MODE>=1 in microns
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.PWID","100.0"),"%g",&zvkon3_bank_.pwid);

  zvkon3_bank_.swim=0.0;    // 1.0 - use Aaron's track swim errors in ZVRES
                      // SWIM>0.0 multiply swam track by SWIM
                      // SWIM<0.0 multiply non-swam track by |SWIM| 
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.SWIM","0.0"),"%g",&zvkon3_bank_.swim);

  // 
  //! tunable inputs for ZVKIN
  //
  zvkon3_bank_.gini=25.0;   // Initial ghost track width/um
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.GINI","25.0"),"%g",&zvkon3_bank_.gini);

  zvkon3_bank_.gmin=25.0;   // Minimum ghost track width/um
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.GMIN","25.0"),"%g",&zvkon3_bank_.gmin);

  zvkon3_bank_.cmax=1.0;    // Max chi**2 of +ve LODI track to ghost
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.CMAX","1.0"),"%g",&zvkon3_bank_.cmax);

  zvkon3_bank_.pcut=0.01;   // Min prob. to cluster track vertices
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.PCUT","0.01"),"%g",&zvkon3_bank_.pcut);

  zvkon3_bank_.nvreq=0;     // Number of reconstructed Vertices REQuested
                      // NVREQ = 0, no request, PCUT is used
                      // NVREQ > 0, vertex clustering stops when
                      // vertex candidates reduced to NVREQ.
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.NVREQ","0"),"%d",&zvkon3_bank_.nvreq);

  zvkon3_bank_.momf=0.0;    // MOMentum Factor IP/BD discrimination
                      // MOMF=1.0 is fairly large momentum weight
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.MOMF","0.0"),"%g",&zvkon3_bank_.momf);
 
  //  Note 1
  //     TIDY > 0     initially loops over all track pairs and find the
  //                  furthest V vertex from IP with fit probability > PRBV.
  //                  If the 2 satisfies the above cuts for Kshort, lambda
  //                  or gamma (and is 0 charge vertex), or interaction
  //                  with detector material, the two tracks are flagged
  //                  and the next furthest 2-prong is then considered
  //                  iteratively until no more tracks are flagged. Flagged
  //                  tracks are not used further by ZVTOP, but they are
  //                  output with their flag in ZVTOPL.
  //
  //     TIDY = 2     if N >= 2, (where N is the number of secondary tracks 
  //     (ALGO=2)     found by ZVKIN) they are fit to a common vertex distance
  //                  D along vertex axis. Tracks that initially failed ICUTP
  //                  but pass close to the vertex (in TRDI and LODI - D) are
  //                  'unflagged' and the whole algorithm is called again.
  //                  Similarly, if N < 2, all tracks initially failing ICUTP
  //                  are put back into the jet and the algorithm rerun.
  //                 (ZVTOPL3.GWID is set -ve to flag such a second iteration).
  //
  //  Note 2
  //     MODE > 0     the first track passed to ZVTOP, id(1), together
  //     (ALGO=1)     with the IP location, is used to define a plane.
  //                  Locations not in the plane are suppressed as
  //                  a function of distance from the plane by weighing 
  //                  V(r) with Gaussian factor tunable via PWID, smaller
  //                  values to PWID confine vertex search closer to plane.
  //                - For track id(1) a reasonable 3D impact distance 
  //                  to the IP, > 2 or 3 sigma, is recommended.


  phbm_bank_.beampos[0]=0;
  phbm_bank_.beampos[1]=0;
  phbm_bank_.beampos[2]=0;
  for(Int_t i=0;i<6;i++){ phbm_bank_.dbeamps[i]=0.0; }
  phbm_bank_.dbeamps[0]=1.0e-4;
  phbm_bank_.dbeamps[2]=1.0e-4;
  phbm_bank_.dbeamps[3]=1.0e-4;

  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.BEAMSIGX","0.0001"),"%g",&phbm_bank_.dbeamps[0]);
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.BEAMSIGY","0.0001"),"%g",&phbm_bank_.dbeamps[1]);
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.BEAMSIGZ","0.0001"),"%g",&phbm_bank_.dbeamps[2]);


}

//_____________________________________________________________________________
JSFZVTOP3::~JSFZVTOP3()
{
  Delete();
}


// ---------------------------------------------------------------
void JSFZVTOP3::ZXSWIM(Int_t *mode, Float_t *ok, Float_t vpos[], Int_t *id)
{
// 
  printf(" ZXSWIM is called. .. mode=%d",*mode);
  printf(" id=%d",*id);
  printf(" vpos=%g %g %g",vpos[0],vpos[1],vpos[2]);

  *ok=0.5;
  return ;

}

// ---------------------------------------------------------------
Int_t JSFZVTOP3::ZXFIT(Int_t *mode, Int_t *jj, Int_t vid[], Float_t vpos[], 
	 Float_t vpossg[], Float_t *chisq, Float_t xvtx[], Float_t xvtxsg[], 
	 Float_t chisqtk[],Float_t pxyz[][3], Int_t *ier)
{
  //  Does vertex fitting...
  //(Inputs)
  //  mode ; 
  //  jj   ; Number of tracks
  //  vid[]; An array of track id whose vertex is fitted.
  //  vpos[3] : Initial vertex position
  //  vpossg[3] : error of vertex position.
  //(Output)
  //  chisq   : Chisq of vertex fit
  //  xvtx(3) : Fitted vertex (x,y,z)
  //  xvtxsq(6) : Error matrix of vertex position
  //  chisqtk() : Chi-square of each track.
  //  pxyz[][3] : three momentum at the vertex

  Float_t b0[3];
  JSFZVTOP3::GUFLD(vpos,b0);
  Float_t bf=b0[2];


  TClonesArray fitbuf("JSFHelicalTrack",*jj);
  JSFVertexing vtx(2);

  for(Int_t i=0;i<*jj;i++){
    JSFCDCTrack *t=JSFZVTOP3::fTrackPointers[vid[i]-1];
    JSFHelicalTrack ht=t->GetHelicalTrack();
    
    ht.SetBfield(bf);
    if( *jj == 2 ) {  vtx.SetTrack(i,ht); }
    new( fitbuf[i] ) JSFHelicalTrack(ht);
  }

  TVector3 v(vpos[0], vpos[1], vpos[2]);

  JSFGeoCFit fit(*jj, &fitbuf, v);
  Bool_t err=fit.Fit();
  //  printf(" Fit err =%d ntry=%d\n",err,fit.GetNtry());

#if __ROOT_FULLVERSION__ < 30000
  JSFDMatrix ans(fit.GetParameter());
  JSFDMatrix errmatrix(fit.GetErrorMatrix());
#else
  TMatrixD ans(fit.GetParameter());
  TMatrixD errmatrix(fit.GetErrorMatrix());
#endif

  *chisq=fit.GetChisq();
  xvtx[0]=ans(0,0);
  xvtx[1]=ans(1,0);
  xvtx[2]=ans(2,0);
  fit.GetTrackChisq(chisqtk);
  Int_t ip=0;

  for(Int_t i=0;i<3;i++){ for(Int_t j=0;j<i+1;j++){
    xvtxsg[ip++]=errmatrix(i,j);
  }}

  for(Int_t i=0;i<*jj;i++){
    Int_t ip=3*(i+1);
    Float_t pt=1.0/TMath::Abs(ans(ip+1,0));
    Float_t phi0=ans(ip,0);
    Float_t tanl=ans(ip+2,0);
    pxyz[i][0]=-pt*TMath::Cos(phi0);
    pxyz[i][1]= pt*TMath::Sin(phi0);
    pxyz[i][2]= pt*tanl;
  }

  fitbuf.Delete();

  if( err ) return 0;

  return 4;  
  
}

// ---------------------------------------------------------------
Int_t JSFZVTOP3::GUFLD(Float_t pos[3], Float_t b0[3])
{
  // Return magnetic field.

  static JSFQuickSimParam *fpar=0;

  if( fpar == 0 ) {
    JSFSIMDST *sds=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
    fpar=sds->Param(); 
  }
  b0[0]=0;
  b0[1]=0;
  b0[2]=fpar->GetBField();
  return 0;
}

// ---------------------------------------------------------------
void JSFZVTOP3::MakeVertices(TObjArray *tracks)
{
  // Do vertexing using ZVTOP Algorithm
  //(Input)
  //  tracks : Pointers to JSFCDCTrack class which belongs to this track
  //(Output)
  //  Results are saved as data member of the class.

  // Loop over jets: select charged track parameter and
  // move its pivot to IP.

  Double_t err[15];   
  Float_t  trk[30]; 

  // Clear array for vertex finding
  Int_t nt=0;
  for(Int_t i=0;i<9;i++){ zxtrks_bank_.ibnk[i]=0; }
  for(Int_t i=0;i<16;i++){ zxtrks_bank_.rbnk[i]=0; }
  for(Int_t ic=0;ic<kMaxTrks;ic++){
    for(Int_t i=0;i<12;i++){ zxtrks_bank_.itrk[ic][i]=0; }
    for(Int_t i=0;i<52;i++){ zxtrks_bank_.rtrk[ic][i]=0; }
  }

  JSFSIMDST *sds=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
  JSFQuickSimParam *fpar=sds->Param(); 

  // Loop over particles in jet to prepare zvtrks common.
  Int_t trkid[kMaxTrks];
  Int_t numtrk=0;

  Float_t pntip[3]={0.0, 0.0, 0.0};
  Float_t bf=fpar->GetBField();

  TIter next(tracks);
  JSFCDCTrack *ct;
  while( (ct=(JSFCDCTrack*)next())) {
     ct->GetTrackParameter(trk);      

     Float_t rclosest=TMath::Sqrt(trk[4]*trk[4]+trk[5]*trk[5]); 

     //     if( rclosest > 1.0 ) continue; 
     if( rclosest > 2.0 ) {
       ct->MovePivot(pntip,bf);
     }
     else {
       Float_t pv[3];
       ct->GetPivot(pv);
       // If current pivot position is more than 1 cm away from IP,
       // move it to IP.
       if( pv[0]*pv[0]+pv[1]*pv[1]+pv[2]*pv[2] > 1.0 ) {
	 ct->MovePivotToIP(fpar);
       }
     }

     ct->GetErrorMatrix(err);
     ct->GetTrackParameter(trk);
     
     zxtrks_bank_.rtrk[nt][0]=ct->GetCharge();
     zxtrks_bank_.rtrk[nt][1]=1;
     trkid[numtrk]=nt+1;
     numtrk++;
     JSFZVTOP3::fTrackPointers[nt]=ct;

     Float_t hlx[5];
     Float_t pivot[3];
     ct->GetHelix(hlx);
     ct->GetPivot(pivot);

     zxtrks_bank_.rtrk[nt][23+1]=hlx[1]+TMath::Pi()/2;
     zxtrks_bank_.rtrk[nt][23+2]=TMath::Abs(hlx[2]);
     zxtrks_bank_.rtrk[nt][23+3]=hlx[4];

#ifdef debug
     Float_t cosct=ct->GetPz()/TMath::Sqrt(ct->GetPx()*ct->GetPx()
			   + ct->GetPy()*ct->GetPy()+ct->GetPz()*ct->GetPz());
     printf(" GenID=%d",ct->GetGenID());
     printf(" nt=%d px,py,pz=%g,%g,%g costh=%g\n",
	    numtrk,ct->GetPx(),ct->GetPy(),ct->GetPz(),cosct);
#endif
     Double_t xip=pivot[0]+hlx[0]*TMath::Cos(hlx[1]);
     Double_t yip=pivot[1]+hlx[0]*TMath::Sin(hlx[1]);
     Double_t zip=pivot[2]+hlx[3];
     zxtrks_bank_.rtrk[nt][23+4]=xip;
     zxtrks_bank_.rtrk[nt][23+5]=yip;
     zxtrks_bank_.rtrk[nt][23+6]=zip;

#ifdef debug
     printf(" track %d ip %g %g %g ",nt,xip,yip,zip);
#endif

     zxtrks_bank_.rtrk[nt][23]=TMath::Sqrt(xip*xip+yip*yip);
     zxtrks_bank_.rtrk[nt][22]=zxtrks_bank_.rtrk[nt][23]/
                               TMath::Sqrt(err[0]);
     zxtrks_bank_.rtrk[nt][48]=TMath::Abs(zip)*TMath::Sqrt(xip*xip+yip*yip)/
                               TMath::Sqrt(xip*xip+yip*yip+zip*zip);
     zxtrks_bank_.rtrk[nt][49]=
       TMath::Sqrt( (xip*xip+yip*yip)/err[0]+zip*zip/err[9]);

#ifdef debug
     printf(" 2d drp=%g drp/s=%g ",zxtrks_bank_.rtrk[nt][23],zxtrks_bank_.rtrk[nt][22]);
     printf(" 3d drp=%g drp/s=%g ",zxtrks_bank_.rtrk[nt][48],zxtrks_bank_.rtrk[nt][49]);
     
     printf("\n");
#endif

     zxtrks_bank_.rtrk[nt][29+10]=err[2];
     zxtrks_bank_.rtrk[nt][29+15]=err[9];
     
     nt++;
     zxtrks_bank_.ibnk[8]=nt;
  }


  // Call ZVRES3
  Int_t errflg;
  Int_t jetno=1;
#ifdef debug
  printf(" \n*********************************\n");
  printf(" start ZVTOP3 .. jetno=%d no of trackis %d\n",jetno,numtrk);
#endif
  Int_t iret=zvtop3_(&numtrk, &trkid[0], &jetno, &errflg);
  if( iret != 1 ) {
    printf(" error return from ZVTOP3 .. iret=%d\n",iret);
  }  

  //  Save results in Class members
  
  fNTRK=zvtopl3_.ibnk[0][0]; 
  fNVRT=zvtopl3_.ibnk[0][1]; 
  fNISO=zvtopl3_.ibnk[0][2]; 
#ifdef debug
  printf(" fNTRK=%d  fNVRT=%d  fNISO=%d\n",fNTRK, fNVRT, fNISO);
#endif
  for(Int_t i=0;i<4;i++){  fPTKS[i]=zvtopl3_.ibnk[0][i+3]; }
  for(Int_t i=0;i<3;i++){  fIPX[i] =zvtopl3_.ibnk[0][i+7]; }
  for(Int_t i=0;i<3;i++){  fAXI[i] =zvtopl3_.ibnk[0][i+10]; }
  fGWID=zvtopl3_.ibnk[0][13];

  // Save vertex information
   
  for(Int_t iv=0;iv<fNVRT;iv++){
    Add(new JSFZVTOP3Vertex(iv));
  }

  CalculateMSPTM();

  return ;
 
}


//_________________________________________________________
void JSFZVTOP3::DebugPrint()
{
  // Print a vertexing result.

  Int_t ij=0;

  printf(" jet %d\n",ij);
  printf(" #Tracks %d # Vertex %d #iso=%d\n",
	 zvtopl3_.ibnk[ij][0],zvtopl3_.ibnk[ij][1],zvtopl3_.ibnk[ij][2]);

  printf(" RBNK_ZVTOPL3 information\n");
  printf(" PTKS(XYZ magnitude of summed track momentum):%g %g %g %g\n",
	 zvtopl3_.rbnk[ij][0], zvtopl3_.rbnk[ij][1],
	 zvtopl3_.rbnk[ij][2], zvtopl3_.rbnk[ij][3]);
  printf(" IPX(XYZ location of IP from PHBM:%g %g %g\n",
	 zvtopl3_.rbnk[ij][4], zvtopl3_.rbnk[ij][5],zvtopl3_.rbnk[ij][6]);

  printf(" MSPTM=%g",GetMSPTM());
  printf(" MSEC=%g ",GetSecondaryMass());
  printf(" DecayLength=%g",GetDecayLength());
  printf("\n");

  Int_t nvrt=zvtopl3_.ibnk[ij][1];
  printf(" nvrt=%d\n",nvrt);
  for(Int_t iv=0;iv<nvrt;iv++){
    printf(" Vertex Number %d\n",iv);
    printf("   Number of tracks in this vertex : %d\n",
	   zvtopl3_.ivrt[ij][iv][0]);
    printf("   vertex charge : %d\n" ,zvtopl3_.ivrt[ij][iv][1]);
    printf("   Vertex significance func. value:%g\n",zvtopl3_.rvrt[ij][iv][0]);
    printf("   Vertex position:%g %g %g\n",zvtopl3_.rvrt[ij][iv][1],
	   zvtopl3_.rvrt[ij][iv][2],zvtopl3_.rvrt[ij][iv][3]);
    printf("   Chisq of vertex fit:%g\n",zvtopl3_.rvrt[ij][iv][10]);
    printf("   Probability of vertex fit:%g\n",zvtopl3_.rvrt[ij][iv][11]);
    printf("   vertex mass:%g\n",zvtopl3_.rvrt[ij][iv][12]);

  }

  printf(" Track id and Vertex number in ths jet\n");
  for(Int_t k=0;k< zvtopl3_.ibnk[ij][0];k++){
    printf(" (Id,Vno)=(%d,%d)",zvtopl3_.itrk[ij][k][0],zvtopl3_.itrk[ij][k][1]);
    printf(" chisq=%g",zvtopl3_.rtrk[ij][k][0]);
    printf(" trdi,lodi=%g,%g",zvtopl3_.rtrk[ij][k][15],zvtopl3_.rtrk[ij][k][16]);
    printf("\n");
  }

}


//_________________________________________________________
void JSFZVTOP3::CalculateMSPTM()
{
  Int_t jetno=0;
  Int_t idsec[50];
  fMSPTM=0.0;
  fProbo=0.0;
  fNseco=0.0;
  fProb2=0.0;
  fDecayLength=0.0;
  fDecayLength0=0.0;
  fNsec =0;
  fChisq=0.0;
  fSecondaryMomentum=0.0;
  fSecondaryMass=0.0;
  fMSPT = 0.0;
  fPTM2 = 0.0;

  Int_t nvrt=fNVRT;
#ifdef debug
  printf("************************************\n");
  printf(" Start MSPTM... nvrt=%d\n",nvrt);
#endif

  if ( nvrt < 2 ) return ;

  //  Vertex axis
  Int_t ivrt=1;
  Float_t vax = zvtopl3_.rvrt[jetno][ivrt][1];
  Float_t vay = zvtopl3_.rvrt[jetno][ivrt][2];
  Float_t vaz = zvtopl3_.rvrt[jetno][ivrt][3];

  Float_t chisq=zvtopl3_.rvrt[jetno][ivrt][10];
  Int_t nseco=zvtopl3_.ivrt[jetno][ivrt][0];
  Float_t probo;
  Int_t ndfo=5*nseco -3*(nseco+1);
  uconfl_(&probo, &ndfo, &chisq); 
  fProbo=probo;
  fNseco=nseco;

  fProb2  =zvtopl3_.rvrt[jetno][ivrt][11];

  // take furthest rec. vertex as seed to add isolated tracks.
  Float_t disv = TMath::Sqrt(vax*vax + vay*vay + vaz*vaz);

#ifdef debug
  printf(" Vertex position is %g %g %g disv=%g \n",vax,vay,vaz,disv);
#endif

#if debug
  Double_t verr[3][3];
  verr[0][0]=zvtopl3_.rvrt[jetno][ivrt][4];
  verr[1][0]=zvtopl3_.rvrt[jetno][ivrt][5];
  verr[1][1]=zvtopl3_.rvrt[jetno][ivrt][6];
  verr[2][0]=zvtopl3_.rvrt[jetno][ivrt][7];
  verr[2][1]=zvtopl3_.rvrt[jetno][ivrt][8];
  verr[2][2]=zvtopl3_.rvrt[jetno][ivrt][9];
  verr[0][1]=verr[1][0];
  verr[0][2]=verr[2][0];
  verr[1][2]=verr[2][1];
  Double_t xyz[3]={vax,vay,vaz};
  Double_t errw=0.0;
  Int_t i,j;
  for(i=0;i<3;i++){ for(j=0;j<3;j++) {
    errw+= xyz[i]*verr[i][j]*xyz[j];
  }}
  Float_t disverr=disv*disv/TMath::Sqrt(errw);
  printf(" disv=%g sqrt(errw)=%g ",disv,TMath::Sqrt(errw)/disv);
  printf(" disverr = %g \n",disverr);
#endif


  Int_t nsec=0;
  Float_t ptot=0.0;
  Float_t pxt =0.0;
  Float_t pyt =0.0;
  Float_t pzt =0.0;
  Float_t etot=0.0;

#if debug
  Int_t ievt=gJSF->GetEventNumber();
#endif

  TVector3 vtx1(vax, vay, vaz);
  TVector3 vtx2;
  Float_t vdis=100.0;
  if( nvrt > 2 ) { 
    vtx2=TVector3( zvtopl3_.rvrt[jetno][2][1], 
		 zvtopl3_.rvrt[jetno][2][2], zvtopl3_.rvrt[jetno][2][3]); 
    vdis=(vtx1-vtx2).Mag();
  }
#ifdef debug

  printf("\n nvrt=%d disv=%g\n",nvrt,disv);
  printf(" dl1=%g ",vtx1.Mag());
  if( nvrt > 2 ) {   printf(" dl2=%g ",vtx2.Mag()) ; }
  printf("\n");
  
#endif

  Float_t lodimin;
  Float_t trdimax;
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.MSPTM.LODIMIN","0.3"),"%g",&lodimin);
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.MSPTM.TRDIMAX","0.1"),"%g",&trdimax);
  Int_t mtvers=gJSF->Env()->GetValue("JSFZVTOP3.MSPTM.MTVERS",0);


  for(Int_t it=0;it<GetNTRK();it++){
    Float_t trdi = zvtopl3_.rtrk[jetno][it][15];
    Float_t lodi = zvtopl3_.rtrk[jetno][it][16];

    // trdi : distance in cm from the track to the IP ( no secondary)
    //        or closest approach to the track from the IP pasing through ...
    // lodi : distrance from IP along 2.5cm line to TRDI

#ifdef debug
     printf(" it =%d trdi=%g lodi=%g lodi/disv=%g \n",it,trdi,lodi,lodi/disv); 
#endif


     Float_t array[3];
     array[0]=trdi;
     array[1]=lodi;
     array[2]=disv;
     array[3]=lodi/disv;

     ntrlodi->Fill(array);

     if( (mtvers == 0 && (zvtopl3_.itrk[jetno][it][1] == 2 || 
     	                ( zvtopl3_.itrk[jetno][it][1] > 2  &&
           	          lodi/disv > lodimin && trdi < trdimax ))) ||
	 (mtvers == 3 && ( zvtopl3_.itrk[jetno][it][1] == 2 || 
			 ( zvtopl3_.itrk[jetno][it][1] < 2  &&
			   lodi/disv > lodimin && trdi < trdimax ))) ||
	 (mtvers == 4 && ( zvtopl3_.itrk[jetno][it][1] == nvrt || 
			 ( zvtopl3_.itrk[jetno][it][1] < nvrt  &&
			   lodi/disv > lodimin && trdi < trdimax ))) ) {

      Int_t ids=zvtopl3_.itrk[jetno][it][0]-1; 
      Float_t cosl=TMath::Cos(TMath::ATan(zxtrks_bank_.rtrk[ids][23+3]));
      Float_t sinl=TMath::Sin(TMath::ATan(zxtrks_bank_.rtrk[ids][23+3]));
      //      ptot=1.0/(zxtrks_bank_.rtrk[ids][23+2]*cosl);
      Float_t tanl=zxtrks_bank_.rtrk[ids][23+3];
      ptot=TMath::Sqrt(1.0+tanl*tanl)/zxtrks_bank_.rtrk[ids][23+2];
      Float_t px=ptot*cosl*TMath::Cos(zxtrks_bank_.rtrk[ids][23+1]);
      Float_t py=ptot*cosl*TMath::Sin(zxtrks_bank_.rtrk[ids][23+1]);
      Float_t pz=ptot*sinl;

#ifdef debug
      printf(" px,py,pz=%g %g %g\n",px,py,pz);
#endif

      pxt += px ;
      pyt += py ;
      pzt += pz ;
      etot += TMath::Sqrt(ptot*ptot + 0.01948);
      idsec[nsec++]=zvtopl3_.itrk[jetno][it][0]; 

    }  // Track is in secondary
  }    // Loop over quality tracks in hemi

  Float_t vpos[3]={0.0, 0.0, 0.0};
  Float_t vpossg[3]={10.0, 10.0, 10.0};

  Int_t mode=1;
  
  Float_t xvtx[3], xvtxsg[6];
  Float_t chisqtk[50], pxyz[50][3];
  Int_t ierr;
  ZXFIT(&mode, &nsec, idsec, vpos , vpossg, 
	&chisq, xvtx, xvtxsg, chisqtk, pxyz, &ierr);

  Float_t dsec=TMath::Sqrt( 
	   TMath::Power((xvtx[0]-zvtopl3_.rbnk[jetno][4]),2) +
	   TMath::Power((xvtx[1]-zvtopl3_.rbnk[jetno][5]),2) +
	   TMath::Power((xvtx[2]-zvtopl3_.rbnk[jetno][6]),2));

  Int_t ndf=5*nsec -3*(nsec+1);
  uconfl_(&fProb, &ndf, &chisq); 

  fDecayLength=dsec;
  fDecayLength0=disv;
  fNsec = nsec;
  fChisq= chisq;

  Float_t msec = TMath::Sqrt(etot*etot-pxt*pxt - pyt*pyt - pzt*pzt);
  Float_t moms = TMath::Sqrt(pxt*pxt+pyt*pyt+pzt*pzt);
  Float_t plse = (pxt*vax + pyt*vay + pzt*vaz)/disv;
  Float_t pt3d = TMath::Sqrt( TMath::Abs((moms-plse)*(moms+plse)));
  Float_t mspt = TMath::Sqrt(msec*msec + pt3d*pt3d) + pt3d;

  fSecondaryMomentum=moms;
  fSecondaryMass=msec;

  Float_t axi[3];
  axi[0]=zvtopl3_.rbnk[jetno][4];
  axi[1]=zvtopl3_.rbnk[jetno][5];
  axi[2]=zvtopl3_.rbnk[jetno][6];
  Float_t eaxi[6];
  eaxi[0]=zvkon3_bank_.ripe*zvkon3_bank_.ripe*1.0E-8;
  eaxi[2]=zvkon3_bank_.ripe*zvkon3_bank_.ripe*1.0E-8;
  eaxi[5]=zvkon3_bank_.zipe*zvkon3_bank_.zipe*1.0E-8;
  //  eaxi[0]=0.0007*0.0007 ; eaxi[2]=0.0007*0.0007; eaxi[5]=0.007*0.007;
  // eaxi[0]=0.0007*0.0007 ; eaxi[2]=0.0007*0.0007; eaxi[5]=0.003*0.003;
  eaxi[0]=0.0001*0.0001 ; eaxi[2]=0.0001*0.0001; eaxi[5]=0.0004*0.0004;
  eaxi[1]=0.0;  eaxi[3]=0; eaxi[4]=0.0;
  Float_t p[3];
  p[0]=pxt ; p[1]=pyt ; p[2]=pzt;

  mode=3;
  Float_t sigmax=1.0;
  sscanf(gJSF->Env()->GetValue("JSFZVTOP3.MSPTM.SIGMAX","1.0"),"%g",&sigmax);
  if( gJSF->Env()->GetValue("JSFZVTOP3.MSPT.VTXSRC",0) != 0 ) {
    for(Int_t i=0;i<3;i++){ xvtx[i]=zvtopl3_.rvrt[jetno][1][i+1]; }
    for(Int_t i=0;i<6;i++){ xvtxsg[i]=zvtopl3_.rvrt[jetno][1][i+4]; }
  }
  Float_t p3norm, ptm0, ptm2, ptmax;
  zvptm_(&mode, axi, eaxi, xvtx, xvtxsg, p, &sigmax, &p3norm, 
	 &ptm0, &ptm2, &ptmax);
  Float_t msptm = TMath::Sqrt(msec*msec + ptm2*ptm2) + TMath::Abs(ptm2);

#ifdef debug
  printf(" refitted vertex position is %g %g %g\n",xvtx[0],xvtx[1],xvtx[2]);
#endif

 
#ifdef debug
  if( msptm > 2.0 && dsec > 0.03 && disv > 0.03 ) {
    //  if( msptm > 2.0 && disverr > 3.0 ) {
    printf(" prob=%g ndf=%d chisq=%g\n",fProb,ndf,chisq);
    printf(" msec=%g dsec=%g ",msec,dsec);
    //    printf(" disverr=%g",disverr);
    printf(" msptm=%g\n",msptm);
   }
#endif

  fMSPTM=msptm;
  fMSPT = mspt;
  fPTM2 = ptm2;

  return ;

}


//_____________________________________________________________________
JSFZVTOP3Track::JSFZVTOP3Track(Int_t id, Int_t jetno)
{
  //(Function)
  //  Copy contents of ZVTOP3 common to data member
  //(Input)
  //  id   : Track number ( 0,1,2,....)
  //  jetno: jet number (0,1,2,....) 

  fZID=zvtopl3_.itrk[jetno][id][0];
  fVNO=zvtopl3_.itrk[jetno][id][1];

  fXIST=zvtopl3_.rtrk[jetno][id][0];
  for(Int_t i=0;i<7;i++){
    fXISG[i]=zvtopl3_.rtrk[jetno][id][i+1];
    fPRBG[i]=zvtopl3_.rtrk[jetno][id][i+8];
  }
  fTRDI = zvtopl3_.rtrk[jetno][id][15];
  fLODI = zvtopl3_.rtrk[jetno][id][16];

}

//_____________________________________________________________________
JSFZVTOP3Vertex::~JSFZVTOP3Vertex()
{
  fTracks->Delete();
}

//_____________________________________________________________________
JSFZVTOP3Vertex::JSFZVTOP3Vertex(Int_t vno, Int_t jetno)
{
  //(Function)
  //  Copy contents of ZVTOP3 common to data member
  //(Input)
  //  vno  : vertex number (0,1,2,....) 
  //  jetno: jet number (0,1,2,....) 

  fNcharged=zvtopl3_.ivrt[jetno][vno][0];
  fCharge=zvtopl3_.ivrt[jetno][vno][1];

  fVSIG  =zvtopl3_.rvrt[jetno][vno][0];
  fPos[0]=zvtopl3_.rvrt[jetno][vno][1];
  fPos[1]=zvtopl3_.rvrt[jetno][vno][2];
  fPos[2]=zvtopl3_.rvrt[jetno][vno][3];
  for(Int_t i=0;i<6;i++){
    fDPOS[i]=zvtopl3_.rvrt[jetno][vno][4+i];
  }

  fXISQ  =zvtopl3_.rvrt[jetno][vno][10];
  fPRBV  =zvtopl3_.rvrt[jetno][vno][11];
  fMass  =zvtopl3_.rvrt[jetno][vno][12];
  fALPHA =zvtopl3_.rvrt[jetno][vno][13];

  if( zvkon3_bank_.algo == 1 ) {
    Int_t ndf=(2*fNcharged-3);
    fPRBV=prob_(&fXISQ,&ndf);
    zvtopl3_.rvrt[jetno][vno][11]=fPRBV;
  }

  fTracks = new TObjArray();
  for(Int_t i=0;i<zvtopl3_.ibnk[jetno][0];i++){
    if( zvtopl3_.itrk[jetno][i][1] == vno+1 ) {
    fTracks->Add(new JSFZVTOP3Track(i,jetno));
    }
  }      

}

#undef debug
