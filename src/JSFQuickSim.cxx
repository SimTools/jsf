//*LastUpdate :  jsf-1-4  6-Feburary-1999  By Akiya Miyamoto
//*LastUpdate :  jsf-0-3-8  29-September-1998  By A.Miyamoto
//*-- Author  : A.Miyamoto  11-September-1998


///////////////////////////////////////////////////////////////////
//
// JSFQuickSim 
//
// Quick simulator for JLC detector.
// This version calls Quick simulator written by Fortran, which is 
// those in lclib, then detector informations are output a la C++ class.
//   
// When Quick Simulator is executed, detector information are saved in
// JSFQuickSimBuf class, where pointers to detector signals are prepared.
// The JSFQuickSimBuf includes following information:
//    CDC track information, VTX/EMC/HDC hit information, 
//    LTKCL information ( created by combining CDC and EMC/HDC information)
// 
// There are several link information among classes.  Thoese are
//    JSFLTKCLTrack.fCDC -> JSFCDCTrack  ( only for charged track.)
//    JSFCDCTrack.f1stCDC -> index of JSFCDCTrack in JSFQuickSimBuf.fCDCTracks
//    JSFCDCTrack.fVTXHits[i]  -> JSFVTXHit
//    JSFCDCTrack.fGenID  -> (index of JSFGeneratorParticle + 1 )
//    JSFVTXHit.fLinkedTrack ->index of JSFCDCTrack in JSFQuickSimBuf.fCDCTracks
//    JSFVTXHit.fGenTrack ->(index of JSFGeneratorParticle + 1 )
// 
// For simple minded usage, the use of LTKCL information is recomended,
// as it includes both Track and Calorimeter information.  For example to 
// calculate total energy from LTKCL is as follows.
//
//   JSFQuickSim *sim=(JSFQuickSim*)gJSF->FindModule("JSFQuickSim");
//   JSFQuickSimBuf *sbuf=(JSFQuickSimBuf*)sim->EventBuf();
//   TClonesArray *cmbt=sbuf->GetTracks();
//   TVector psum(4);
//   for(Int_t i=0;i<sbuf->GetNTracks();i++){
//     JSSFLTKCLTrack *lt=(JSFLTKCLTrack*)sbuf->UncheckedAt(i);
//     TVecor pv(4);
//     pv=lt->GetPv();
//     psum=psum+pv;
//   }
//   printf(" Total energy is %g\n",psum(0));    
// 
//$Id$
//
//////////////////////////////////////////////////////////////////


#include "JSFSteer.h"
#include "JSFLCFULL.h"
#include "JSFGenerator.h"

#include "JSFQuickSimCommon.h"
#include "JSFQuickSim.h"

extern "C" {
extern void smrjin_();
extern void dscswm_();
extern void dscsmr_();
extern void swmprd_();
extern void smrprd_();
extern void swmrin_(int *level, int *debug);
extern void smrrin_(int *level, int *debug);
extern void swmevt_(int *recid, int *level, int *debug, int *iret);
extern void smrevt_(int *recid, int *level, int *debug, int *iret);
extern void cmbtrk_(int *iret);
};

ClassImp(JSFQuickSim)
ClassImp(JSFQuickSimParam)
ClassImp(JSFQuickSimBuf)
ClassImp(JSFCALGeoParam)

TClonesArray *gTracks;
TClonesArray *gCDCTracks;
TClonesArray *gVTXHits;
TClonesArray *gEMCHits;
TClonesArray *gHDCHits;


// ******* Common /PRJUNK/  ****************
// to access temporary information of LTKCL package.
const int MXxZET=43;
const int MXxPHI=77;
const int MXxBD=(MXxZET+1)*(MXxPHI+1);
const int MXxCLS=5000;
const int MXxWRK=1000;
const int LNxTRK = 100;
const int MXxTRK = 500;
typedef struct {
  Int_t NBNON, NCELLS;
  Int_t JBNPNT[MXxPHI+1][MXxZET+1];
  Int_t IBNPNT[MXxBD][2], NCLIN[MXxBD], ICLPNT[MXxCLS][5];
  Float_t  WORK[MXxWRK];
  Int_t NCDCTK;
  Float_t RTKBNK[MXxTRK][LNxTRK];
  Int_t   KBNPNT[MXxPHI+1][MXxZET+1], NTKIN[MXxBD];
  Int_t   ITKPNT[MXxBD][MXxTRK], KCNTRK[MXxTRK];
} COMMON_PRJUNK;

extern COMMON_PRJUNK prjunk_;
// *****************************************************


//_____________________________________________________________________________
JSFQuickSim::JSFQuickSim(const char *name, const char *title)
       : JSFModule(name,title)
{
  // JLC QuickSimulator module.
  // 
  fParam   = new JSFQuickSimParam();
  fEventBuf= new JSFQuickSimBuf("JSFQuickSimBuf", 
			       "JSF QuickSim Event data", this);
}


//_____________________________________________________________________________
JSFQuickSimBuf::JSFQuickSimBuf(const char *name, const char *title, JSFQuickSim *sim)
       : JSFEventBuf(name,title, sim)
{
  fNTracks=0;
  if( !gTracks ) gTracks= new TClonesArray("JSFLTKCLTrack", 1000);
  fTracks=gTracks;

  fNCDCTracks=0;
  if( !gCDCTracks )gCDCTracks= new TClonesArray("JSFCDCTrack", kMaxCDCTracks);
  fCDCTracks=gCDCTracks;

  fNVTXHits=0 ;
  if( !gVTXHits ) gVTXHits= new TClonesArray("JSFVTXHit", kMaxVTXHits);
  fVTXHits=gVTXHits;

  fNEMCHits=0 ;
  if( !gEMCHits ) gEMCHits= new TClonesArray("JSFEMCHit", kMaxCalHits);
  fEMCHits=gEMCHits;

  fNHDCHits=0 ;
  if( !gHDCHits ) gHDCHits= new TClonesArray("JSFHDCHit", kMaxCalHits);
  fHDCHits=gHDCHits;

}

//_____________________________________________________________________________
JSFQuickSim::~JSFQuickSim()
{
  //  if( fParam ) delete fParam;
  //  if( fEventBuf  ) delete fEventBuf;
}

//_____________________________________________________________________________
JSFQuickSimBuf::~JSFQuickSimBuf()
{
}

//_____________________________________________________________________________
Bool_t JSFQuickSim::Initialize()
{
  dscswm_();
  dscsmr_();
  return kTRUE;

}

//_____________________________________________________________________________
Bool_t JSFQuickSim::BeginRun(Int_t nrun)
{

  fParam->SetSwimParam();
  fParam->SetSmearParam();
  

  smrjin_();
  if( fFile->IsWritable() ) fParam->Write();
  
  Int_t level=1;
  Int_t idebug=0;
  swmrin_(&level, &idebug);
  smrrin_(&level, &idebug);

  return kTRUE ;
}

//_____________________________________________________________________________
Bool_t JSFQuickSim::Process(Int_t ev)
{
  // Simulate one event.

   Int_t recid=1;
   Int_t level=1;
   Int_t idebug=0;
   Int_t iret;

//  Move load Generator data in the class into TBS buffer


   if( !TBPUTGeneratorParticles() ) return kFALSE;

   gJSFLCFULL->TBCRTE(1,"Production:EMC;Hit_Pad",0,0,iret);

   gJSFLCFULL->TBCRTE(1,"Production:EMC;Hit_Cell",0,0,iret);
   gJSFLCFULL->TBCRTE(1,"Production:EMC;Cluster",0,0,iret);
   gJSFLCFULL->TBCRTE(1,"Production:EMC;Cluster_to_Cell",0,0,iret);
   gJSFLCFULL->TBCRTE(1,"Production:HDC;Hit_Cell",0,0,iret);
   gJSFLCFULL->TBCRTE(1,"Production:HDC;Cluster",0,0,iret);
   gJSFLCFULL->TBCRTE(1,"Production:HDC;Cluster_to_Cell",0,0,iret);

   gJSFLCFULL->TBCRTE(1,"Production:CDC;Track_Parameter",0,0,iret);
   gJSFLCFULL->TBCRTE(1,"Production:CDC_VTX;Track_Parameter",0,0,iret);
   gJSFLCFULL->TBCRTE(1,"Production:VTX;Space_Point",0,0,iret);
   gJSFLCFULL->TBCRTE(1,"Production:VTX;Space_Point_Error",0,0,iret);

   swmevt_(&recid, &level, &idebug, &iret);
   
   smrevt_(&recid, &level, &idebug, &iret);

   cmbtrk_(&iret);

   if( !ReviseGeneratorInfo() ) return kFALSE;

   if( !(((JSFQuickSimBuf*)EventBuf())->MakeEventBuf()) ) return kFALSE;

   return kTRUE;
}


//_____________________________________________________________________________
Bool_t JSFQuickSim::TBPUTGeneratorParticles()
{
  // Copy Generator Particle information in the GeneratorParticles class
  // into the TBS bank.
  // All generator particle information stored in JSFGenerator class is
  // saved in TBS's bank, Generator:Particle_List.  All objects in the
  // TClonesArray are copied according to the order save there.


  Int_t iret;
  gJSFLCFULL->TBCRTE(1,"Generator:Particle_List",0,0, iret);
  gJSFLCFULL->TBCRTE(1,"Generator:Header",0,0, iret);


  // First search Generator class

  Float_t data[20];
  Int_t i;
  for(i=0;i<20;i++){ data[i]=0.0;}
  JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *gevt=(JSFGeneratorBuf*)gen->EventBuf();
  TClonesArray *pa=gevt->GetParticles();
  Int_t j;
  for(j=0;j<gevt->GetNparticles();j++){
    JSFGeneratorParticle *p=(JSFGeneratorParticle*)pa->At(j);
    data[0]=p->fSer;
    data[1]=p->fID;
    data[2]=p->fMass;
    data[3]=p->fCharge;
    data[4]=p->fP[1];
    data[5]=p->fP[2];
    data[6]=p->fP[3];
    data[7]=p->fP[0];
    data[8]=p->fX[1];
    data[9]=p->fX[2];
    data[10]=p->fX[3];
    data[11]=p->fNdaughter;
    data[12]=p->fFirstDaughter;
    data[13]=p->fMother;
    data[14]=p->fX[0];
    data[15]=p->fLifeTime;
    data[16]=p->fDecayLength;
    Int_t ielm=j+1;
    if( ielm != p->fSer ){
      printf("Warning JSFQuickSim::TBPUTGeneratorParticles");
      printf("  Particle Serial number(%d) and ",p->fSer);
      printf(" element number(%d) is inconsistent.\n",ielm);
    }
    gJSFLCFULL->TBPUT(1,"Generator:Particle_List",ielm,20,(Int_t*)data,iret);
  }

  for(i=0;i<20;i++){ data[i]=0.0;}
  data[0]=gevt->GetEventNumber();
  data[1]=gevt->GetDate();
  data[2]=gevt->GetTime();
  data[4]=gevt->GetStartSeed();
  data[6]=gevt->GetEcm();
  gJSFLCFULL->TBPUT(1,"Generator:Header",1,20,(Int_t*)data,iret);  

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFQuickSimBuf::MakeEventBuf()
{
   if( !MakeJSFLTKCLTracks() ) return kFALSE;
   if( !MakeCALHits() ) return kFALSE;
   if( !MakeCDCTracks() ) return kFALSE;
   if( !MakeVTXHits() ) return kFALSE;

   return kTRUE;
}


//_____________________________________________________________________________
Bool_t JSFQuickSimBuf::MakeJSFLTKCLTracks()
{
  // Copy Production:Combined track banks into JSFLTKCLTrack class

  TClonesArray &tracks=*(GetTracks());
  Int_t nt=0;

  Char_t *bankname[64]={ "Production:Combined_Gamma_Track",
			 "Production:Combined_Lepton_Track",
			 "Production:Combined_Hadron_Track"};
  EJSFLTKCLTrackBank bank[3]={kCombinedGammaTrack, kCombinedLeptonTrack, 
			      kCombinedHadronTrack};

  Float_t data[100];  
  Int_t nw, nelm, neary[500], iret;
  for(Int_t ib=0;ib<3;ib++){
     gJSFLCFULL->TBNOEL(1,bankname[ib], nelm, neary);
     for(Int_t i=0;i<nelm;i++){
       gJSFLCFULL->TBGET(1,bankname[ib],neary[i], nw, data, iret);
       if( nw > 50 ) { 
	Warning("MakeJSFLTKCLTrack",
       " Too many CDC track associated to the Combined track. nw=%d",  nw);
       }
        new(tracks[nt++])  JSFLTKCLTrack(bank[ib], data);
     }
  }
  SetNTracks(nt);

  return kTRUE;
}


//_____________________________________________________________________________
Bool_t JSFQuickSimBuf::MakeCALHits()
{
  // Create JSFEMCHits and JSFHDCHits from 
  // the TBS bank, Production:EMC;Hit_Cell and Production:HDC;Hit_Cell

  TClonesArray &emc=*(GetEMCHits());
  Int_t nemc=0;
  Int_t nelm, neary[kMaxCalHits];
  Int_t iwrk[50]; 
  Int_t i,nw,iret;

  gJSFLCFULL->TBNOEL(1,"Production:EMC;Hit_Cell", nelm, neary);
  if( nelm >= kMaxCalHits ) {
      printf("Error in JSFQuickSimBuf::MakeCALHits\n");
      printf("Number of EMC Calorimeter hits =%d, exceeded buffer size\n",nelm);
      return kFALSE;
  }
  for(i=0;i<nelm;i++){
    gJSFLCFULL->TBGET(1,"Production:EMC;Hit_Cell",neary[i],nw,iwrk,iret);
    new(emc[nemc++])  JSFEMCHit(iwrk[0], iwrk[1]);
  }
  SetNEMCHits(nemc);

  Int_t nhdc=0;
  TClonesArray &hdc=*(GetHDCHits());
  gJSFLCFULL->TBNOEL(1,"Production:HDC;Hit_Cell", nelm, neary);
  if( nhdc >= kMaxCalHits ) {
    printf("Error in JSFQuickSimBuf::MakeCALHits\n");
    printf("Number of HDC Calorimeter hits =%d, exceeded buffer size\n",nelm);
    return kFALSE;
  }
  for(i=0;i<nelm;i++){
    gJSFLCFULL->TBGET(1,"Production:HDC;Hit_Cell",neary[i],nw,iwrk,iret);
    new(hdc[nhdc++])  JSFHDCHit(iwrk[0], iwrk[1]);
  }
  SetNHDCHits(nhdc);

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFQuickSimBuf::MakeVTXHits()
{
  // Create JSFVTXHits class 
  // the TBS bank, Production:VTX;Hit and Production:VTX;Hit_Errors
  // Using the tracks saved in JSFCDCTrack class, VTX hits are obtained
  // from Production:VTX;Hit and Production:VTX;Hit_Errors bank,
  // and saved in JSFVTXHit class.

  Int_t ncdc = fNCDCTracks;
  TClonesArray &vtx=*(GetVTXHits());
  Int_t icdc,nwh,nwe,iret;
  Int_t nvtx=0;
  const Int_t kMaxBuff=200;
  Float_t hits[kMaxBuff][3], errs[kMaxBuff][2]; 

  for(icdc=0;icdc<ncdc;icdc++){
    JSFCDCTrack *t=(JSFCDCTrack*)fCDCTracks->UncheckedAt(icdc);
    Int_t igen=t->GetGenID();
    gJSFLCFULL->TBGET(1,"Production:VTX;Space_Point",igen,
		      nwh, (Float_t*)hits,iret);
    if( iret < 0 ) continue;  // No VTX hits for this track.
    if( nwh > 3*kMaxBuff ) {
      printf("Error to TBGET Production:VTX;Space_Point ..");
      printf("Element# %d  IRET=%d",igen,iret);
      printf("nwh=%d buffer size=%d",nwh,3*kMaxBuff);
      printf("\n");
      return kFALSE;
    }
    Int_t nhit=nwh/3;

    gJSFLCFULL->TBGET(1,"Production:VTX;Space_Point_Error",igen,
		      nwe, (Float_t*)errs,iret);
    if( iret < 0 || nwe > 2*kMaxBuff ) {
      printf("Error to TBGET Production:VTX;Space_Point_Error ..");
      printf("Element# %d  IRET=%d",igen,iret);
      printf("nwe=%d buffer size=%d",nwe,2*kMaxBuff);
      return kFALSE;
    }
    
    Int_t j;
    for(j=0;j<nhit;j++){
      Int_t layer=((JSFQuickSim*)Module())->Param()->GetVTXLayerNumber(hits[j][0]);
      new(vtx[nvtx])  JSFVTXHit(hits[j][0], hits[j][1], hits[j][2],
				  errs[j][0], errs[j][1], layer, icdc, igen);
      JSFVTXHit *vh=(JSFVTXHit*)vtx.UncheckedAt(nvtx);
      t->AddVTXHit(vh);
      nvtx++;
    }  
  }
  SetNVTXHits(nvtx);

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFQuickSimBuf::MakeCDCTracks()
{
  // Create JSFCDCTracks class 
  // the TBS bank, Production:CDC;Tracks and Production:CDC;Tracks
  //(Warning)
  // Only tracks which has entry in JSFLTKCLClass is saved in this class.
  // 

  Int_t ncmb = fNTracks;
  TClonesArray &cdc=*(GetCDCTracks());

  Int_t ncdc=0;
  Int_t i,nw,iret;
  Int_t itrkp[100]; 
  for(i=0;i<ncmb;i++){
     JSFLTKCLTrack *ct=(JSFLTKCLTrack*)fTracks->UncheckedAt(i);
     if( ct->GetNCDC() < 0 ) continue;
     if( ct->GetType() == 1 || ct->GetType() ==3  ) continue;
     Int_t icdc=ct->Get1stCDC();

     gJSFLCFULL->TBGET(1,"Production:CDC;Track_Parameter",icdc,
	 	       nw, itrkp,iret);
     if( iret < 0 ) {
       printf("Error to TBGET Production:CDC;Track_Parameter ..");
       printf("Element# %d  IRET=%d",icdc,iret);
       printf("\n");
       return kFALSE;
     }
     new(cdc[ncdc]) JSFCDCTrack( itrkp );
     JSFCDCTrack *t=(JSFCDCTrack*)cdc.UncheckedAt(ncdc);
     Int_t *ncel=(Int_t*)&prjunk_.RTKBNK[icdc-1][59];
     if( *ncel > 0 ) {
       t->SetPositionAtEMC( &prjunk_.RTKBNK[icdc-1][60] );
     }
     ct->SetCDC(ncdc, t);
     ncdc++;
  }
  SetNCDCTracks(ncdc);

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFQuickSim::ReviseGeneratorInfo()
{
  //  Generator information of particles with finite decay length is revised
  //  by swim routine.  JSFGenerator class information is updated according to the
  //  updated Generator:Particle_List informations.

  JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *gevt=(JSFGeneratorBuf*)gen->EventBuf();
  TClonesArray *pa=gevt->GetParticles();

  Int_t i, nw,iret;
  Float_t wrk[20];
  Int_t nelm=0;
  for(i=0;i<gevt->GetNparticles();i++){
    JSFGeneratorParticle *p=(JSFGeneratorParticle*)pa->At(i);
    nelm=i+1;
    gJSFLCFULL->TBGET(1,"Generator:Particle_List",nelm,nw,wrk,iret);
    if( iret < 0 ) {
      printf("Error to get Generator:Particle_List bank in JSFQuickSim::ReviseGeneratorInfo\n");
      printf("TBGET's IRET=%d\n",iret);
      return kFALSE;
    }
    p->fX[1]=wrk[8];
    p->fX[2]=wrk[9];
    p->fX[3]=wrk[10];
    p->fLifeTime=wrk[15];
    p->fDecayLength=wrk[16];
  }

  return kTRUE;
}

//_____________________________________________________________________________
JSFQuickSimParam::JSFQuickSimParam()
{
  //  Parameters of JLC QUick Simulator.
  //  

  fBfield=20.0;
  fSeed=314159 ;

  smrfld_.imodbf[0]=0 ; 
  smrfld_.imodbf[1]=0 ; 
  smrfld_.bfdata[0]=0 ; 
  smrfld_.bfdata[1]=0 ; 

  prfeld_.bfield=10.1085  ;  // kgauss
  prfeld_.ptor=329.9837713  ;
  prfeld_.ropt=3.03045206e-3 ;

  //  Tracker
  fTrack[0]  = 30.0   ; //inner radius(cm)  
  fTrack[1]  = 230.   ; // outer radius(cm)
  fTrack[2]  = -230.  ; // Z-(cm) 
  fTrack[3]  = 230.   ; // Z+(cm)
  fTrack[4]  = 100    ; // # sampling points   
  fTrack[5]  = 10     ; // cut on # sampling points   
  fTrack[6]  = 1.E-2  ; // sigma_(r-phi)
  fTrack[7]  = 3.E-1  ; // sigma_Z 

  // EM calorimeter
  fEMCal[0]  =    156   ; // # phi segments       
  fEMCal[1]  =     50   ; // # theta segments of barrel      
  fEMCal[2]  =     18   ; // # radial segments of endcap     
  fEMCal[3]  =     45.  ; // inner radius(cm)                
  fEMCal[4]  =    250.  ; // outer radius(cm)                
  fEMCal[5]  =   -300.  ; // Z-(cm)                          
  fEMCal[6]  =    300.  ; // Z+(cm)                          
  fEMCal[7]  =    1.E-2 ; // sigma_E/E pedestal for barrel   
  fEMCal[8]  =   1.5E-1 ; // sigma_E/E at 1 GeV              
  fEMCal[9]  =    1.E-2 ; // sigma_E/E pedestal for endcap   
  fEMCal[10] =   1.5E-1 ; // sigma_E/E at 1 GeV              

   // HD calorimeter
  fHDCal[0]  =    78    ; // # phi segments                 
  fHDCal[1]  =    25    ; // # theta segments of barrel     
  fHDCal[2]  =     9    ; // # radial segments of endcap    
  fHDCal[3]  =    48.   ; // inner radius(cm)               
  fHDCal[4]  =   270.   ; // outer radius(cm)               
  fHDCal[5]  =  -324.   ; // Z-(cm)                         
  fHDCal[6]  =   324.   ; // Z+(CM)                         
  fHDCal[7]  =    1.E-2 ; // sigma_E/E pedestal for barrel  
  fHDCal[8]  =    4.E-1 ; // sigma_E/E at 1 GeV             
  fHDCal[9]  =    1.E-2 ; // sigma_E/E pedestal for endcap  
  fHDCal[10] =    4.E-1 ; // sigma_E/E at 1 GeV             


   // VTX 
  fNERRVX   =     2    ; // VTX Space point error flag
  fNSMPVX   =     5    ; // # sampling layers + 1 = NSMPVX        
  fDPHIVX   =   25.E-4 ; // phi pitch (cm)                        
  fDZEEVX   =   25.E-4 ; // Z   pitch (cm)                        
  
  //  Beam pipe
  fVTXLayer[0][0]  =     1.   ; // radius (cm)                   
  fVTXLayer[0][1]  =  -100.   ; // Z-(cm)                        
  fVTXLayer[0][2]  =   100.   ; // Z+(cm)                        
  fVTXLayer[0][3]  =   0.003  ; // thickness in radiation length 
  //  Layer #1
  fVTXLayer[1][0]  =    2.4  ; // radius (cm)                   s
  fVTXLayer[1][1]  =   -5.0  ; // Z-(cm)                        
  fVTXLayer[1][2]  =    5.0  ; // Z+(cm)                        
  fVTXLayer[1][3]  =  0.003  ; // thickness in radiation length 
  //  Layer #2
  fVTXLayer[2][0]  =    3.6  ; // radius (cm)                   
  fVTXLayer[2][1]  =   -7.5  ; // -Z-(cm)                        
  fVTXLayer[2][2]  =    7.5  ; // Z+(cm)                        
  fVTXLayer[2][3]  =  0.003  ; // thickness in radiation length 
  //  Layer #3
  fVTXLayer[3][0]  =    4.8  ; // radius (cm)                   
  fVTXLayer[3][1]  =  -10.0  ; // -Z-(cm)                        
  fVTXLayer[3][2]  =   10.0  ; // Z+(cm)                        
  fVTXLayer[3][3]  =  0.003  ; // thickness in radiation length 
  //  Layer #4
  fVTXLayer[4][0]  =    6.0  ; // radius (cm)                   
  fVTXLayer[4][1]  =  -12.5  ; // -Z-(cm)                        
  fVTXLayer[4][2]  =   12.5  ; // Z+(cm)                        
  fVTXLayer[4][3]  =  0.003  ; // thickness in radiation length 
  // layer #NSMPVX (CDC inner cylinder)
  fVTXLayer[5][0]  =    25  ; // radius (cm)                   
  fVTXLayer[5][1]  =  -230.  ; // Z-(cm)                        
  fVTXLayer[5][2]  =   230.  ; // Z+(cm)                        
  fVTXLayer[5][3]  =   0.01  ; // thickness in radiation length 

  fVTXError[0] = 4.E-4 ;
  fVTXError[1] = 4.E-4 ;
  fVTXError[2] = 4.E-4 ;
  fVTXError[3] = 4.E-4 ;
  fVTXError[4] = 4.E-4 ;

  //  Parameters for Clustering.
  Float_t clp[4][5] ={ { 100., 0.40, 0.20, 1.00, 0.3 },
		      { 100., 0.40, 0.20, 1.00, 0.3 },
		      { 100., 0.40, 0.20, 1.00, 0.25},
		      { 100., 0.40, 0.20, 1.00, 0.25} };

  for(Int_t i=0;i<4;i++){
    for(Int_t j=0;j<5;j++){
      fCLSPAR[i][j]=clp[i][j];
    }
  }

  // Load parameters

  sscanf(gJSF->Env()->GetValue("JSFQuickSim.ParameterFile","Undefined"),
	 "%s",fParamFile);
  if( strcmp("Undefined",fParamFile) != 0 ) {
    printf(" Detector parameter is obtained from %s\n",fParamFile);
    ReadParamDetector(fParamFile);
  }
  else {
    printf("Detector parameter file is undefined.\n");
    printf(" Will use parameters those defined in the source file, JSFQuickSim.cxx\n");
  }

  sscanf(gJSF->Env()->GetValue("JSFQuickSim.CMBCUT.ADXECT","20.0"),
	 "%g",&fCMBCUT[0]);
  sscanf(gJSF->Env()->GetValue("JSFQuickSim.CMBCUT.ENSGEM","2.0"),
	 "%g",&fCMBCUT[1]);
  sscanf(gJSF->Env()->GetValue("JSFQuickSim.CMBCUT.ENSGHD","2.0"),
	 "%g",&fCMBCUT[2]);

  // Set Calorimeter Geometry info.

  fEMCGeom = new JSFCALGeoParam(kEMC, 
	    (Int_t)fEMCal[0], (Int_t)fEMCal[1], (Int_t)fEMCal[2],
	           fEMCal[3], fEMCal[4], fEMCal[5], fEMCal[6] );		    

  fHDCGeom = new JSFCALGeoParam(kHDC, 
	    (Int_t)fHDCal[0], (Int_t)fHDCal[1], (Int_t)fHDCal[2],
	           fHDCal[3], fHDCal[4], fHDCal[5], fHDCal[6] );		    

}

//_____________________________________________________________________________
JSFQuickSimParam::~JSFQuickSimParam()
{
  delete fEMCGeom;
  delete fHDCGeom;
}

//_____________________________________________________________________________
void JSFQuickSimParam::ReadParamDetector(Char_t *file)
{
  // Read QuickSimulator parameter from a file.
  // In lclib.97a, there are two parameter files for Quick Simulator, but
  // information prepared for swimmer is included in the parameter file
  // for smearing.  Therefore, in this function, reads the smear parameter
  // file a la lclib.97a format, and saved in the JSFQuickSim class for the
  // latter use by Quick Simulator.

  FILE *fd;
  fd=fopen(file,"r");
  if( !fd ) Fatal("ReadParamFile","Unable to open file %s",file);
 
  printf("JSFQuickSimParam::ReadParamDetector ");
  printf(" --  Read detector parameter from the file %s\n",file);
  Char_t input[256];
  while(fgets(input, 256, fd) ){
    if( !strncmp("@!",input,2) ) continue;
    
    Int_t id;
    Float_t val;
    sscanf(input,"%d %g",&id, &val);
    
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
    else if( id == 72 ) fDPHIVX = val ; // phi pitch (cm)
    else if( id == 73 ) fDZEEVX = val ; // Z pitch (cm)
    else if( 73 < id && id < 4*(fNSMPVX+1)+74 ) { // VTX layer info.
      Int_t ilay=(id-74)/4 ;
      fVTXLayer[ilay][id-(74+4*ilay)]=val;
    }
    else if( 4*(fNSMPVX+1)+73 < id && id < 4*(fNSMPVX+1)+79 ) { // VTX error info.
      fVTXError[id-(4*(fNSMPVX+1)+74 )]=val;
    }
    else if( 1000 < id && id < 1021 ) clspar[id-1001]=val;
    else {
      Fatal("ReadParamDetector",
	      "Invalid Parameter ID %d, Error line is \n%s",id,input);


    }
  }
  fclose(fd);
  

}
//_____________________________________________________________________________
void JSFQuickSimParam::SetSwimParam()
{
  // Set Swimmer parameter

  swmrnd_.iseed=fSeed ;

  prfeld_.ptor=prfeld_.ptor*prfeld_.bfield/fBfield ;
  prfeld_.ropt=1.0/prfeld_.ptor;
  prfeld_.bfield=fBfield;

}

//_____________________________________________________________________________
void JSFQuickSimParam::SetSmearParam()
{
  // Set Swimmer parameter  ( corresponds to detect4.com )

  //  Tracker
  smrtgo_.rcytk[0] = fTrack[0] ; //inner radius(cm)  
  smrtgo_.rcytk[1] = fTrack[1] ; // outer radius(cm)
  smrtgo_.zcytk[0] = fTrack[2] ; // Z-(cm) 
  smrtgo_.zcytk[1] = fTrack[3] ; // Z+(cm)
  smrtgo_.nsmptk   = (Int_t)fTrack[4] ; // # sampling points   
  smrtgo_.nsmpct   = (Int_t)fTrack[5] ; // cut on # sampling points   
  smrres_.sigrf    = fTrack[6] ; // sigma_(r-phi)
  smrres_.sigze    = fTrack[7] ; // sigma_Z 

  // EM calorimeter
  smrego_.nphiem   =  (Int_t)fEMCal[0]  ; // # phi segments       
  smrego_.nthtem   =  (Int_t)fEMCal[1]  ; // # theta segments of barrel      
  smrego_.nradem   =  (Int_t)fEMCal[2]  ; // # radial segments of endcap     
  smrego_.rcyem[0] =  fEMCal[3]  ; // inner radius(cm)                
  smrego_.rcyem[1] =  fEMCal[4]  ; // outer radius(cm)                
  smrego_.zcyem[0] =  fEMCal[5]  ; // Z-(cm)                          
  smrego_.zcyem[1] =  fEMCal[6]  ; // Z+(cm)                          
  smrres_.sgemb0   =  fEMCal[7]  ; // sigma_E/E pedestal for barrel   
  smrres_.sgemb    =  fEMCal[8]  ; // sigma_E/E at 1 GeV              
  smrres_.sgeme0   =  fEMCal[9]  ; // sigma_E/E pedestal for endcap   
  smrres_.sgeme    =  fEMCal[10] ; // sigma_E/E at 1 GeV              

   // HD calorimeter
  smrhgo_.nphihd   =  (Int_t)fHDCal[0]  ; // # phi segments                 
  smrhgo_.nththd   =  (Int_t)fHDCal[1]  ; // # theta segments of barrel     
  smrhgo_.nradhd   =  (Int_t)fHDCal[2]  ; // # radial segments of endcap    
  smrhgo_.rcyhd[0] =  fHDCal[3]  ; // inner radius(cm)               
  smrhgo_.rcyhd[1] =  fHDCal[4]  ; // outer radius(cm)               
  smrhgo_.zcyhd[0] =  fHDCal[5]  ; // Z-(cm)                         
  smrhgo_.zcyhd[1] =  fHDCal[6]  ; // Z+(CM)                         
  smrres_.sghdb0   =  fHDCal[7]  ; // sigma_E/E pedestal for barrel  
  smrres_.sghdb    =  fHDCal[8]  ; // sigma_E/E at 1 GeV             
  smrres_.sghde0   =  fHDCal[9]  ; // sigma_E/E pedestal for endcap  
  smrres_.sghde    =  fHDCal[10] ; // sigma_E/E at 1 GeV             

   // VTX 
  smrvgo_.nerrvx   =  fNERRVX   ; // VTX Error flag
  smrvgo_.nsmpvx   =  fNSMPVX   ; // # sampling layers + 1 = NSMPVX        
  smrvgo_.dphivx   =  fDPHIVX   ; // phi pitch (cm)                        
  smrvgo_.dzeevx   =  fDZEEVX   ; // Z   pitch (cm)                        
  
  //  fVTX Layer info
  Int_t i;
  for(i=0;i<=fNSMPVX;i++){
    smrvgo_.rcyvx[i]    = fVTXLayer[i][0] ; // radius (cm)                   
    smrvgo_.zcyvx[i][0] = fVTXLayer[i][1] ; // Z-(cm)                        
    smrvgo_.zcyvx[i][1] = fVTXLayer[i][2] ; // Z+(cm)                        
    smrvgo_.rdlvx[i]    = fVTXLayer[i][3] ; // thickness in radiation length 
  }

  for(i=0;i<5;i++){
    smrvgo_.errvx[i] = fVTXError[i];
  }

  for(i=0;i<4;i++){
    for(Int_t j=0;j<5;j++){
      smrclp_.clspar[i][j]=fCLSPAR[i][j];
    }
  }

  cmbcut_.adxect = fCMBCUT[0] ;
  cmbcut_.ensgem = fCMBCUT[1] ;
  cmbcut_.ensghd = fCMBCUT[2] ;

  // Set Calorimeter Geometry info.

  fEMCGeom->SetGeoParam(kEMC, (Int_t)fEMCal[0], (Int_t)fEMCal[1], 
	(Int_t)fEMCal[2], fEMCal[3], fEMCal[4], fEMCal[5], fEMCal[6] );
  fHDCGeom->SetGeoParam(kHDC, (Int_t)fHDCal[0], (Int_t)fHDCal[1], 
	(Int_t)fHDCal[2], fHDCal[3], fHDCal[4], fHDCal[5], fHDCal[6] );
}

//____________________________________________________________________________
Int_t JSFQuickSimParam::GetVTXLayerNumber(Float_t radius)
{
//  Convert Radius of VTX hit into VTX layer ID,
//  Using the simulation parameter in smrpar[].
//  Layer number of inner most detector is 1.
//
   Float_t wid=0.01;
   Int_t i;
   for(i=1;i<GetVTXNLayer();i++){
     if( radius > GetVTXRadius(i)-wid && radius < GetVTXRadius(i)+wid ) 
       return i;
   }
   return 0;
}


//____________________________________________________________________________
void JSFQuickSim::MakeBranch(TTree *tree)
{
  //  JSFModule::MakeBranch(tree);

   fTree=tree;
   if( fEventBuf && fMakeBranch ) {
     Int_t split=0;
     Int_t bsize=4000;
     tree->Branch(fEventBuf->GetName(), fEventBuf->ClassName() ,
		  &fEventBuf, bsize, split);
   }
}

//_____________________________________________________________________________
void JSFQuickSim::SetBranch(TTree *tree)
{
//  Set Branch address for this module

   fTree=tree;
   if( fEventBuf ) {
     Char_t name[50];
     sprintf(name,"%s",fEventBuf->GetName());
     fBranch=tree->GetBranch(name);
     fBranch->SetAddress(&fEventBuf);
   }
}

//_____________________________________________________________________________
JSFCALGeoParam::JSFCALGeoParam(EJSFCALType type, 
                    Int_t nphi, Int_t ntheta, Int_t nrad,
		    Float_t rmin, Float_t rmax,  Float_t zminus, Float_t zplus )
{
   SetGeoParam(type, nphi, ntheta, nrad,
		    rmin,  rmax,  zminus, zplus );
}

//_____________________________________________________________________________
void JSFCALGeoParam::SetGeoParam(EJSFCALType type, 
		 Int_t nphi, Int_t ntheta, Int_t nrad,
	         Float_t rmin, Float_t rmax,  Float_t zminus, Float_t zplus )
{
  // Initialize geometry parameters of the calorimeter.  Inputs are,
  //     type   : kEMC or kHDC
  //     nphi   : Number of phi segmentation.
  //     ntheta : Number of theta segmentation of barrel calorimeter
  //     nrad   : Number of radial segmentation of endcap calorimeter
  //     rmin   : Inner radius of endcap calorimeter (cm)
  //     rmax   : maximum radius of endcap calorimeter (cm)
  //     zminus : Z coordinate of -Z endcap calorimeter (cm)
  //     zplus  : Z coordinate of +Z endcap calorimeter (cm)
  //              Barrel calorimeter is from zmin to zmax, 
  //              and its surface radius is rmax.
  //

  fType = type;
  fRmin = rmin;
  fRmax = rmax;
  
  fPhiStep = 2.0*TMath::Pi()/(Float_t)nphi;

  Float_t csmn = zplus/TMath::Sqrt(rmin*rmin+zplus*zplus);
  Float_t csmx = zminus/TMath::Sqrt(rmin*rmin+zminus*zminus);
  fBZetaMin    = TMath::Log( (1-csmn)/(1+csmn) )/2 ;
  Float_t thmx = TMath::Log( (1-csmx)/(1+csmx) )/2 ;
  fBZetaStep   = ( thmx - fBZetaMin )/(Float_t)nphi ;

  Float_t snmn = rmin/TMath::Sqrt(rmin*rmin+zminus*zminus);
  Float_t snmx = rmax/TMath::Sqrt(rmax*rmax+zminus*zminus);
     fMZetaMin = TMath::Log( (1+snmn)/(1-snmn) )/2 ;
  Float_t  rmx = TMath::Log( (1+snmx)/(1-snmx) )/2 ;
    fMZetaStep = ( rmx - fMZetaMin )/(Float_t)nrad ;

          snmn = rmin/TMath::Sqrt(rmin*rmin+zplus*zplus);
          snmx = rmax/TMath::Sqrt(rmax*rmax+zplus*zplus);
     fPZetaMin = TMath::Log( (1+snmn)/(1-snmn) )/2 ;
           rmx = TMath::Log( (1+snmx)/(1-snmx) )/2 ;
    fPZetaStep = ( rmx - fMZetaMin )/(Float_t)nrad ;
}



//______________________________________________________________________________
void JSFQuickSimParam::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFQuickSimParam.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TNamed::Streamer(R__b);
      R__b >> fSeed;
      R__b >> fBfield;
      R__b.ReadStaticArray(fTrack);
      R__b.ReadStaticArray(fEMCal);
      R__b.ReadStaticArray(fHDCal);
      R__b >> fNSMPVX;
      R__b >> fDPHIVX;
      R__b >> fDZEEVX;
      R__b.ReadStaticArray((float*)fVTXLayer);
      R__b.ReadStaticArray((float*)fCLSPAR);
   } else {
      R__b.WriteVersion(JSFQuickSimParam::IsA());
      TNamed::Streamer(R__b);
      R__b << fSeed;
      R__b << fBfield;
      R__b.WriteArray(fTrack, 8);
      R__b.WriteArray(fEMCal, 11);
      R__b.WriteArray(fHDCal, 11);
      R__b << fNSMPVX;
      R__b << fDPHIVX;
      R__b << fDZEEVX;
      Int_t nout=4*(fNSMPVX+1);
      R__b.WriteArray((float*)fVTXLayer, nout);
      R__b.WriteArray((float*)fCLSPAR, 20);
   }
}


//______________________________________________________________________________
void JSFQuickSimBuf::SetPointers()
{
  // After reading the class data for JSFQuickSimBuf, pointers 
  // for JSFCDCTracks and JSFVTXHits are reset.

  Int_t i;
  for(i=0;i<fNTracks;i++){
    JSFLTKCLTrack *lt=(JSFLTKCLTrack*)fTracks->UncheckedAt(i);
    Int_t icdc=lt->Get1stCDC();
    if( icdc < 0 ) continue;
    JSFCDCTrack *t=(JSFCDCTrack*)fCDCTracks->UncheckedAt(icdc);
    lt->SetCDC(icdc, t);
  }

  for(i=0;i<fNVTXHits;i++){
    JSFVTXHit *vh=(JSFVTXHit*)fVTXHits->UncheckedAt(i);
    Int_t icdc=vh->GetLinkedTrack();
    if( icdc < 0 ) continue;
    JSFCDCTrack *t=(JSFCDCTrack*)fCDCTracks->UncheckedAt(icdc);
    t->AddVTXHit(vh);
  }


}

//______________________________________________________________________________
void JSFQuickSimBuf::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFQuickSimBuf.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      JSFEventBuf::Streamer(R__b);
      R__b >> fNTracks;
      fTracks->Streamer(R__b);
      R__b >> fNCDCTracks;
      fCDCTracks->Streamer(R__b);
      R__b >> fNVTXHits;
      fVTXHits->Streamer(R__b);
      R__b >> fNEMCHits;
      fEMCHits->Streamer(R__b);
      R__b >> fNHDCHits;
      fHDCHits->Streamer(R__b);
      SetPointers();
   } else {
      R__b.WriteVersion(JSFQuickSimBuf::IsA());
      JSFEventBuf::Streamer(R__b);
      R__b << fNTracks;
      fTracks->Streamer(R__b);
      R__b << fNCDCTracks;
      fCDCTracks->Streamer(R__b);
      R__b << fNVTXHits;
      fVTXHits->Streamer(R__b);
      R__b << fNEMCHits;
      fEMCHits->Streamer(R__b);
      R__b << fNHDCHits;
      fHDCHits->Streamer(R__b);
   }
}

