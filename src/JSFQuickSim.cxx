///////////////////////////////////////////////////////////////////
//
// JSRSFQuickSim 
//
// Quick simulator for JLC detector.
// This version calls Quick simulator written by Fortran, which is 
// those in lclib, then detector informations are outpu a la C++ class.
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
// 
//  Environment parameter and its default value  are as follows
//
// # If 0, track parameter in JSFCDCTrack is parameter by CDCTrack only.
// # If 1, JSFCDCTrack is a result of CDC-VTX combined track parameter.
//  JSFQuickSim.CDCTrackIsCDCVTX   1 
//
//$Id$
//
//
//////////////////////////////////////////////////////////////////


#include <TMath.h>
#include "JSFConfig.h"
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

// ******* Common /PRJUNK/  ****************
// to access temporary information of LTKCL package.
//const int MXxZET=43;
//const int MXxPHI=77;
const int MXxZET=90;
const int MXxPHI=100;
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

// TClonesArray *JSFQuickSimBuf::fTracks=0;


//_____________________________________________________________________________
JSFQuickSim::JSFQuickSim(const Char_t *name, const Char_t *title, const Char_t *opt)
       : JSFModule(name,title,opt)
{
  // JLC QuickSimulator module.
  // 
  fParam   = new JSFQuickSimParam();
  fEventBuf= new JSFQuickSimBuf("JSFQuickSimBuf", 
			       "JSF QuickSim Event data", this);

}

//_____________________________________________________________________________
JSFQuickSimBuf::JSFQuickSimBuf()
{
  fNTracks=0;
  fTracks = NULL;
  fNCDCTracks=0;
  fCDCTracks= NULL;
  fNVTXHits=0 ;
  fVTXHits=NULL;

  fNEMCHits=0 ;
  fEMCHits=NULL;

  fNHDCHits=0 ;
  fHDCHits=NULL;

  fCDCTrackIsCDCVTX = gJSF->Env()->GetValue("JSFQuickSim.CDCTrackIsCDCVTX",1);

}

//_____________________________________________________________________________
JSFQuickSimBuf::JSFQuickSimBuf(const char *name, const char *title, JSFQuickSim *sim)
       : JSFEventBuf(name,title, sim)
{

  fNTracks=0;
  fTracks = new TObjArray(1000);

  fNCDCTracks=0;
  fCDCTracks= new TClonesArray("JSFCDCTrack", kMaxCDCTracks);

  fNVTXHits=0 ;
  fVTXHits= new TClonesArray("JSFVTXHit", kMaxVTXHits);

  fNEMCHits=0 ;
  fEMCHits= new TClonesArray("JSFEMCHit", kMaxCalHits);

  fNHDCHits=0 ;
  fHDCHits= new TClonesArray("JSFHDCHit", kMaxCalHits);

  fCDCTrackIsCDCVTX = gJSF->Env()->GetValue("JSFQuickSim.CDCTrackIsCDCVTX",1);

}

//_____________________________________________________________________________
JSFQuickSim::~JSFQuickSim()
{
  if( fParam ) delete fParam;
  if( fEventBuf  ) delete fEventBuf;

}

//_____________________________________________________________________________
JSFQuickSimBuf::~JSFQuickSimBuf()
{

  Delete();
  if(fCDCTracks) delete fCDCTracks;
  if(fVTXHits)   delete fVTXHits;
  if(fEMCHits)   delete fEMCHits;
  if(fHDCHits)   delete fHDCHits;

  if(fTracks)    { 
     fTracks->Delete();
     delete fTracks;
  }

}

//_____________________________________________________________________________
void JSFQuickSimBuf::Clear(const Option_t *option)
{
  if(fTracks)    fTracks->Clear(option);
  if(fCDCTracks) fCDCTracks->Clear(option);
  if(fVTXHits)   fVTXHits->Clear(option);
  if(fEMCHits)   fEMCHits->Clear(option);
  if(fHDCHits)   fHDCHits->Clear(option);

  fNTracks=0;
  fNCDCTracks=0;
  fNVTXHits=0;
  fNEMCHits=0;
  fNHDCHits=0;

}

//_____________________________________________________________________________
void JSFQuickSimBuf::Delete(const Option_t *option)
{
  if(fTracks)    fTracks->Delete(option);
  if(fCDCTracks) fCDCTracks->Delete(option);
  if(fVTXHits)   fVTXHits->Delete(option);
  if(fEMCHits)   fEMCHits->Delete(option);
  if(fHDCHits)   fHDCHits->Delete(option);
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
  if( fFile->IsWritable() ) { 
    fParam->Write();
    Write();
  }
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

   fSMRRND=smrrnd_.iseed;
   fSWMRND=swmrnd_.iseed;

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


//____________________________________________________________________________
void JSFQuickSim::MakeBranch(TTree *tree)
{
  //  JSFModule::MakeBranch(tree);

   fTree=tree;
   if( fEventBuf && fMakeBranch ) {
#if ROOT_VERSION_CODE >= ROOT_VERSION(3,1,3)
     Int_t split=-1;
#else
     Int_t split=0;
#endif
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


// ---------------------------------------------------------------
Bool_t JSFQuickSim::EndRun()
{

   // Save random seed
   fSMRRND=smrrnd_.iseed;
   fSWMRND=swmrnd_.iseed;

  if( fFile->IsWritable() ) {  Write();  }

   return kTRUE;
}


// ---------------------------------------------------------------
Bool_t JSFQuickSim::GetLastRunInfo()
{

  Read(GetName());
  
  smrrnd_.iseed=fSMRRND;
  swmrnd_.iseed=fSWMRND;

  printf("Random seeds for JSFQuickSim were reset by ");
  printf("values from a file.\n");
  
  return kTRUE;
}


//_____________________________________________________________________________
Bool_t JSFQuickSimBuf::MakeEventBuf()
{
  
  //  Clear();
  Delete();

   if( !MakeJSFLTKCLTracks() ) return kFALSE;
   if( !MakeCALHits() ) return kFALSE;
   if( !MakeCDCTracks() ) return kFALSE;
   if( !MakeVTXHits() ) return kFALSE;

   MakeJSFLTKCLTrackPointers();

   return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFQuickSimBuf::MakeJSFLTKCLTrackPointers()
{
  // Set link information of JSFLTKCLTracks

  Int_t ncmb = fNTracks;
  Int_t isrc, iemc, emcls[50];
  JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *gevt=(JSFGeneratorBuf*)gen->EventBuf();
  TClonesArray *pa=gevt->GetParticles();

  //
  // Set Pointers to access GeneratorParticle contributing EM Cluster
  //
  for(Int_t icmb=0;icmb<ncmb;icmb++){
     JSFLTKCLTrack *ct=(JSFLTKCLTrack*)fTracks->UncheckedAt(icmb);
     //
     // Set pointers for generator tracks contributing to the
     // Generator tracks.
     isrc=ct->GetSource();
     iemc=(isrc/10)%10;
     

     Int_t nemc=ct->GetNEMC();
     Int_t nw, iret;
     Int_t itype=ct->GetType();
     if( itype==1 || itype==2 ) {
       gJSFLCFULL->TBGET(1,"Production:EMC;Cluster",nemc,nw,emcls,iret);
       if( nw > 50 || iret < 0 ) { 
	 printf("Error in MakeJSFLTKCLTrackPointers .. ");
	 printf("Production:EMC;Cluster  nw=%d  iret=%d\n",nw,iret);
	 printf(" icmb=%d itype=%d nemc=%d",icmb,itype,nemc);
	 printf(" isrc=%d iemc=%d\n",isrc,iemc);
       }
       else{ 
	 Int_t nemgen=emcls[14];
	 ct->SetNEMGen(nemgen);
	 if( nemgen > 0 ) {
	   ct->fIDEMGen=new Int_t[nemgen];
	   for(Int_t ipg=0;ipg<nemgen;ipg++){
	     Int_t ig=emcls[ipg+15];
	     if( ig < 1 || ig > pa->GetEntries() || ig > gevt->GetNparticles()) {
	       printf(" Warning in JSFQuickSim::MakeJSFLTKCLTrackPointers \n");
	       printf(" invalid pointer to EMC cluster to GeneratorParticleList\n");
	       printf(" ig=%d ",ig);
	       printf(" GenEntries=%d",pa->GetEntries());
	       printf(" GeneratorParticles=%d ",gevt->GetNparticles());
	       printf(" ipg=%d nemgen=%d",ipg,nemgen);
	       printf(" \n");
	     }

	     JSFGeneratorParticle *pgen=(JSFGeneratorParticle*)pa->UncheckedAt(ig-1);
	     ct->SetEMGen(pgen);
	     ct->fIDEMGen[ipg]=ig;
	     
	     if( pgen->GetSerial() != ig ) {
	       printf(" Warning at JSFQuickSim::MakeJSFLTKCLTrackPointers\n");
	       printf(" Generator Serial Number(%d)",pgen->GetSerial());
	       printf(" and pointer in elmcs (%d) is different\n",ig);
	     }


	   }
	 }
       }
     }
  }  

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFQuickSimBuf::MakeJSFLTKCLTracks()
{
  // Copy Production:Combined track banks into JSFLTKCLTrack class

  // TClonesArray &tracks=*(GetTracks());
  TObjArray &tracks=*(GetTracks());
  if( tracks.GetEntries() > 0 ) {  tracks.Delete(); }
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
       tracks.Add(new JSFLTKCLTrack(bank[ib], data));
       nt++;
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
  if( nelm >= kMaxCalHits ) {
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
  if( nerrvx == 3 && fCDCTrackIsCDCVTX == 1 ) 
        bankname="Production:CDC_VTX;Track_Parameter";
  else  bankname="Production:CDC;Track_Parameter";

  //
  Int_t nelm, neary[500];
  gJSFLCFULL->TBNOEL(1, bankname, nelm,neary);
  for(i=0;i<nelm;i++){
    Int_t id=i+1;
    gJSFLCFULL->TBGET(1,bankname,id, nw, itrkp,iret);
    if( iret < 0 ) {
      printf("Error to TBGET %s ..",bankname);
      printf("Element# %d  IRET=%d",id,iret);
      printf("\n");
      return kFALSE;
    }
    if( nerrvx == 3  && fCDCTrackIsCDCVTX == 1 ) {
      Char_t *bnkcdc="Production:CDC;Track_Parameter";
      Int_t nwt;  Int_t itrkpp[200];
      gJSFLCFULL->TBGET(1,bnkcdc,id,nwt,itrkpp,iret);
      itrkp[55]=itrkpp[55];
      itrkp[56]=itrkpp[56];
    }

    new(cdc[i]) JSFCDCTrack( itrkp );
    JSFCDCTrack *t=(JSFCDCTrack*)cdc.UncheckedAt(i);
    Int_t *ncel=(Int_t*)&prjunk_.RTKBNK[i][59];
    if( *ncel > 0 ) {
      t->SetPositionAtEMC( &prjunk_.RTKBNK[i][60] );
    }
  }
  ncdc=nelm;
  SetNCDCTracks(ncdc);

  for(i=0;i<ncmb;i++){
     JSFLTKCLTrack *ct=(JSFLTKCLTrack*)fTracks->UncheckedAt(i);
     if( ct->GetNCDC() <= 0 ) continue;
     if( ct->GetType() == 1 || ct->GetType() ==3  ) continue;

     for(Int_t j=0;j<ct->GetNCDC();j++){
       
       Int_t icdc=ct->GetIDCDC(j);
       JSFCDCTrack *t=(JSFCDCTrack*)cdc.UncheckedAt(icdc-1);
       ct->SetCDC(icdc, t);
     }
  }

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


//______________________________________________________________________________
void JSFQuickSimBuf::SetGeneratorPointers(JSFGeneratorBuf *ingen)
{
  // After reading the class data for JSFQuickSimBuf, pointers 
  // for JSFCDCTracks and JSFVTXHits are reset.

  JSFGenerator *gen=NULL;
  JSFGeneratorBuf *genb=NULL;
  if( ingen ) {
    genb=ingen;
  }
  else {
    gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator"); 
    genb=(JSFGeneratorBuf*)gen->EventBuf();
  }
  TClonesArray    *gps= (genb==NULL ? NULL : genb->GetParticles());

  if( gps == NULL ) {
    printf("Warning in JSFQuickSim: No Generator information is available ");
    printf("to set EM cluster information in LTKCLTrack\n");
    return ;
  }

  for(Int_t i=0;i<fNTracks;i++){
    JSFLTKCLTrack *lt=(JSFLTKCLTrack*)fTracks->UncheckedAt(i);
    if( !lt->fEMGen ) { lt->fEMGen=new TObjArray(); }
    else{ lt->fEMGen->Clear() ; }
    for(Int_t j=0;j<lt->fNEMGen;j++){
      JSFGeneratorParticle *gp=(JSFGeneratorParticle*)gps->UncheckedAt(lt->fIDEMGen[j]-1);
      lt->SetEMGen(gp);
    }
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
    if (icdc < 0) {
      lt->SetCDCR(icdc,0);
    }
    else {
      JSFCDCTrack *t=(JSFCDCTrack*)fCDCTracks->UncheckedAt(icdc-1);
      lt->SetCDCR(icdc, t);
      if( !lt->fCDCs ) { 
	lt->fCDCs=new TObjArray(); 
      }
      for(Int_t j=0;j<lt->fNCDC;j++){
	JSFCDCTrack *tt=(JSFCDCTrack*)fCDCTracks->UncheckedAt(lt->fIDCDC[j]-1);
	lt->fCDCs->Add(tt);
      }
    }
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
void JSFQuickSimBuf::Append(JSFQuickSimBuf *src, Int_t numgp, JSFGeneratorBuf *gbuf)
{
  // Append JSFQuickSimBuf object, src, to current object
  // numgp is a number of generator particles in the src.  It is used to increment
  // pointer information to JSFGenerator objects 
  // gbuf: is a pointer to newly created JSFGeneratorBuf class.

  AppendCALHits(src);

  AppendLTKCLTracks(src, numgp, gbuf);

}

//______________________________________________________________________________
void JSFQuickSimBuf::AppendCALHits(JSFQuickSimBuf *src)
{
  // Append JSFQuickSimBuf object, src, to current object
  // numgp is a number of generator particles in the src.  It is used to increment
  // pointer information 

  // Append JSFEMCHits

  Int_t indcel[kMaxCalHits];  Int_t sind[kMaxCalHits]; 
  Int_t scelid[kMaxCalHits];  Int_t scelpnt[kMaxCalHits];

  if( src->GetNEMCHits() > 0 ) {
    TClonesArray *emc=src->GetEMCHits(); // Make a cellid index of current hits.
    JSFEMCHit *hemc;
    Int_t i;
    for(i=0;i<fNEMCHits;i++){
      hemc=(JSFEMCHit*)fEMCHits->UncheckedAt(i);
      indcel[i]=hemc->GetCellID();
    }                                               
    TMath::Sort(fNEMCHits, indcel, sind );
    for(i=0;i<fNEMCHits;i++){
      scelid[i]=indcel[sind[i]];
      scelpnt[i]=sind[i];                // Sort CellID data.
    }

    Int_t nemc=fNEMCHits;
    for(i=0;i<src->GetNEMCHits();i++){
      JSFEMCHit *h=(JSFEMCHit*)emc->UncheckedAt(i);
      Int_t cellid=h->GetCellID();
      Int_t ifnd=TMath::BinarySearch(fNEMCHits, scelid, cellid);
      if( scelid[ifnd]==cellid ) {
        hemc=(JSFEMCHit*)fEMCHits->UncheckedAt(scelpnt[ifnd]);
	hemc->AddEnergy(h->GetEMEnergy(), h->GetHDEnergy());
      }
      else {
	new((*fEMCHits)[nemc]) JSFEMCHit(h->GetCellID(), h->GetEMEnergy(), h->GetHDEnergy());
	nemc++;
      }
    }
    fNEMCHits=nemc;
  }

  // ****************************
  //  Merging HDC hits
  // ****************************

  if( src->GetNHDCHits() > 0 ) {
    TClonesArray *hdc=src->GetHDCHits(); // Make a cellid index of current hits.
    JSFHDCHit *hhdc;
    Int_t i;
    for(i=0;i<fNHDCHits;i++){
      hhdc=(JSFHDCHit*)fHDCHits->UncheckedAt(i);
      indcel[i]=hhdc->GetCellID();
    }
    TMath::Sort(fNHDCHits, indcel, sind );
    
    for(i=0;i<fNHDCHits;i++){
      scelid[i]=indcel[sind[i]];
      scelpnt[i]=sind[i];             // Sort CellID data.
    }

    Int_t nhdc=fNHDCHits;
    for(i=0;i<src->GetNHDCHits();i++){
      JSFHDCHit *h=(JSFHDCHit*)hdc->UncheckedAt(i);
      Int_t cellid=h->GetCellID();
      Int_t ifnd=TMath::BinarySearch(fNHDCHits, scelid, cellid);
      if( scelid[ifnd]==cellid ) {
        hhdc=(JSFHDCHit*)fHDCHits->UncheckedAt(scelpnt[ifnd]);
        hhdc->AddEnergy(h->GetEMEnergy(), h->GetHDEnergy());
      }
      else {
	new((*fHDCHits)[nhdc]) JSFHDCHit(h->GetCellID(), h->GetEMEnergy(), h->GetHDEnergy());
	nhdc++;
      }
    }
    fNHDCHits=nhdc;
  }

}


//______________________________________________________________________________
void JSFQuickSimBuf::AppendLTKCLTracks(JSFQuickSimBuf *src, Int_t numgp, JSFGeneratorBuf *gbuf)
{
  // Append JSFQuickSimBuf object, src, to current object
  // src  : A pointer to source JSFQuickSimBuf object.
  // numgp: Number of generator particles added.
  // gbuf : A pointer to JSFGeneratorBuf where generator information is added.

  Int_t oldvtx=fNVTXHits;
  Int_t oldcdc=fNCDCTracks;

  //(1) Copy all VTX Hits
  //  fLinkedTrack is updated later/
  Int_t nvtxsrc=src->GetNVTXHits();
  TClonesArray *vtx=src->GetVTXHits();
  for(Int_t i=0;i<nvtxsrc;i++){
    JSFVTXHit *v=(JSFVTXHit*)vtx->UncheckedAt(i);
    v->fGenTrack +=numgp;
    v->fLinkedTrack +=fNCDCTracks;
    new((*fVTXHits)[fNVTXHits]) JSFVTXHit(*v);
    fNVTXHits++;
  }

  //(2) Copy CDCTracks object
  Int_t ncdcsrc=src->GetNCDCTracks();
  TClonesArray *cdc=src->GetCDCTracks();
  for(Int_t i=0;i<ncdcsrc;i++){
    JSFCDCTrack *t=(JSFCDCTrack*)cdc->UncheckedAt(i);
    new((*fCDCTracks)[fNCDCTracks]) JSFCDCTrack(*t);
    JSFCDCTrack *nt=(JSFCDCTrack*)fCDCTracks->UncheckedAt(fNCDCTracks);
    for(Int_t j=0;j<t->fNVTX;j++){
      Int_t ind=vtx->IndexOf(t->GetVTXHit(j));
      nt->fVTXHits[j]=(JSFVTXHit*)fVTXHits->UncheckedAt(oldvtx+ind);
    }
    fNCDCTracks++;
  }

  //(3) Copy LTKCLTrack object
  Int_t nltsrc=src->GetNTracks();
  TObjArray *lt=src->GetTracks();
  TClonesArray *genp= ( gbuf != NULL ? gbuf->GetParticles() : NULL );
  Int_t ngenp=( genp != NULL ? genp->GetEntries() : 0 );

  for(Int_t i=0;i<nltsrc;i++){
    JSFLTKCLTrack *tsrc=(JSFLTKCLTrack*)lt->UncheckedAt(i);
    JSFLTKCLTrack *t=new JSFLTKCLTrack(*tsrc);
    fTracks->Add(t);
    fNTracks++;
    
    Int_t k=cdc->IndexOf(tsrc->fCDC);
    t->fCDC=(JSFCDCTrack*)fCDCTracks->UncheckedAt(oldcdc+k);
    t->f1stCDC+=oldcdc;

    if( t->fCDCs ) {
      t->fCDCs->Clear();
      for(Int_t j=0;j<tsrc->fCDCs->GetEntries();j++){
	JSFCDCTrack *ct=(JSFCDCTrack*)tsrc->fCDCs->UncheckedAt(j);
	k=cdc->IndexOf(ct);
	t->fCDCs->Add(fCDCTracks->UncheckedAt(oldcdc+k));
      }
    }

    if( t->fEMGen ) {
      t->fEMGen->Clear();
      for(Int_t j=0;j<tsrc->fEMGen->GetEntries();j++){
	Int_t iser=tsrc->fIDEMGen[j];

	if( genp ) {
	  JSFGeneratorParticle *ngp=(JSFGeneratorParticle*)genp->UncheckedAt(ngenp-numgp+iser-2);
	  t->fEMGen->Add(ngp);
	}
      }
    }  // End of if( t->fEMGen ) 

  }

}

      
#if __ROOT_FULLVERSION__ >= 30000
//______________________________________________________________________________
void JSFQuickSimBuf::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFQuickSimBuf.
   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
     if( R__v > 1 ) {
       JSFQuickSimBuf::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
     }
     else {
       //=== process old versions before automatic schema evolution
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
       R__b.CheckByteCount(R__s, R__c, JSFQuickSimBuf::IsA());
       //=== end of old versions
     }
     SetPointers();
     SetGeneratorPointers();
   } else {
     JSFQuickSimBuf::Class()->WriteBuffer(R__b, this);
   }
}

#else
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

#endif
