//*LastUpdate :  jsf-1-6  30-March-1999  By Akiya Miyamoto
//*LastUpdate :  jsf-1-5  26-Feburary-1999  By Akiya Miyamoto
//*LastUpdate :  jsf-1-4  6-Feburary-1999  By Akiya Miyamoto
//*LastUpdate :  jsf-0-3-8  29-September-1998  By A.Miyamoto
//*-- Author  : A.Miyamoto  11-September-1998

/*  Change Log

  30-March-1999  A.Miyamoto  Put modification to run with lclib-98a-4

*/


///////////////////////////////////////////////////////////////////
//
// JSRSFQuickSim 
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
//
//////////////////////////////////////////////////////////////////


#include <TMath.h>
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
ClassImp(JSFQuickSimBuf)

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
   gJSFLCFULL->TBCRTE(1,"Production:VTX;Track_Parameter",0,0,iret);

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
    //     new(emc[nemc++])  JSFEMCHit(iwrk[0], iwrk[1]);
    new(emc[nemc++])  JSFEMCHit(iwrk[1], iwrk[0], 0);
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
    //    new(hdc[nhdc++])  JSFHDCHit(iwrk[0], iwrk[1]);
    new(hdc[nhdc++])  JSFHDCHit(iwrk[1], 0, iwrk[0]);
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
  // Create JSFCDCTracks class.
  // The track parameters in the TBS bank,
  // Production:CDC_VTX;Track_Parameter is saved as data member. 
  //(Warning)
  // Only tracks which has entry in JSFLTKCLClass is saved in this class.
  // 


  Int_t ncmb = fNTracks;
  TClonesArray &cdc=*(GetCDCTracks());

  Int_t ncdc=0;
  Int_t i,nw,iret;
  Int_t itrkp[100]; 
  Char_t *bankname="";
  Int_t nerrvx=((JSFQuickSim*)Module())->Param()->GetVTXNERRVX() ;
  if( nerrvx == 3 ) bankname="Production:CDC_VTX;Track_Parameter";
  else  bankname="Production:CDC;Track_Parameter";

  for(i=0;i<ncmb;i++){
     JSFLTKCLTrack *ct=(JSFLTKCLTrack*)fTracks->UncheckedAt(i);
     if( ct->GetNCDC() < 0 ) continue;
     if( ct->GetType() == 1 || ct->GetType() ==3  ) continue;
     Int_t icdc=ct->Get1stCDC();

     gJSFLCFULL->TBGET(1,bankname,icdc, nw, itrkp,iret);
     if( iret < 0 ) {
       printf("Error to TBGET %s ..",bankname);
       printf("Element# %d  IRET=%d",icdc,iret);
       printf("\n");
       return kFALSE;
     }
     if( nerrvx == 3 ) {
       Char_t *bnkcdc="Production:CDC;Track_Parameter";
       Int_t nwt;  Int_t itrkpp[200];
       gJSFLCFULL->TBGET(1,bnkcdc,icdc,nwt,itrkpp,iret);
       itrkp[55]=itrkpp[55];
       itrkp[56]=itrkpp[56];
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


//____________________________________________________________________________
void JSFQuickSim::MakeBranch(TTree *tree)
{
  //  JSFModule::MakeBranch(tree);

   fTree=tree;
   if( fEventBuf && fMakeBranch ) {
     Int_t split=0;
     Int_t bsize=50000;
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

