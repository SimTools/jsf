//*LastUpdate :  0.03/08  29-September-1998  By A.Miyamoto
//*-- Author  : A.Miyamoto  11-September-1998

#include "JSFSteer.h"
#include "JSFLCFULL.h"
#include "JSFGenerator.h"
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
ClassImp(JSFQuickSimData)
ClassImp(JSFQuickSimBuf)

TClonesArray *gTracks;

//_____________________________________________________________________________
JSFQuickSim::JSFQuickSim(const char *name, const char *title)
       : JSFModule(name,title)
{
  fParam   = new JSFQuickSimParam();
  fData    = new JSFQuickSimData("JSFQuickSimData", "JSFQuickSim Data");
  fEventBuf= new JSFQuickSimBuf("JSFQuickSimBuf", 
			       "JSF QuickSim Event data", this);
}

//_____________________________________________________________________________
JSFQuickSimData::JSFQuickSimData(const char *name, const char *title)
       : TNamed(name,title)
{
}

//_____________________________________________________________________________
JSFQuickSimBuf::JSFQuickSimBuf(const char *name, const char *title, JSFQuickSim *sim)
       : JSFEventBuf(name,title, sim)
{
  fNtracks=0;
  if( !gTracks ) gTracks= new TClonesArray("JSFLTKCLTrack", 1000);
  fTracks=gTracks;
}

//_____________________________________________________________________________
JSFQuickSim::~JSFQuickSim()
{
  if( fParam ) delete fParam;
  if( fData  ) delete fData;
  if( fEventBuf  ) delete fEventBuf;
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
Bool_t JSFQuickSim::TBPUTGeneratorParticles()
{
  // Copy Generator Particle information in the GeneratorParticles class
  // into the TBS bank.

  Int_t iret;
  gJSFLCFULL->TBCRTE(1,"Generator:Particle_List",0,0, iret);
  gJSFLCFULL->TBCRTE(1,"Generator:Header",0,0, iret);


  // First search Generator class

  Float_t data[20];
  for(Int_t i=0;i<20;i++){ data[i]=0.0;}
  JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *gevt=(JSFGeneratorBuf*)gen->EventBuf();
  TClonesArray *pa=gevt->GetParticles();
  for(Int_t j=0;j<gevt->GetNparticles();j++){
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
 
    gJSFLCFULL->TBPUT(1,"Generator:Particle_List",ielm,20,(Int_t*)data,iret);
  }

  for(Int_t i=0;i<20;i++){ data[i]=0.0;}
  data[0]=gevt->GetEventNumber();
  data[1]=gevt->GetDate();
  data[2]=gevt->GetTime();
  data[4]=gevt->GetStartSeed();
  data[6]=gevt->GetEcm();
  gJSFLCFULL->TBPUT(1,"Generator:Header",1,20,(Int_t*)data,iret);  

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFQuickSim::MakeJSFLTKCLTracks()
{
  // Copy Production:Combined track banks into JSFLTKCLTrack class

  JSFQuickSimBuf *buf=(JSFQuickSimBuf*)EventBuf();
  TClonesArray &tracks=*(buf->GetTracks());
  Int_t nt=0;

  Char_t *bankname[64]={ "Production:Combined_Gamma_Track",
			 "Production:Combined_Lepton_Track",
			 "Production:Combined_Hadron_Track"};
  EJSFLTKCLTrackBank bank[3]={kCombinedGammaTrack, kCombinedLeptonTrack, 
			      kCombinedHadronTrack};

  Float_t data[300];  
  Int_t nw, nelm, neary[1000], iret;
  for(Int_t ib=0;ib<3;ib++){
     gJSFLCFULL->TBNOEL(1,bankname[ib], nelm, neary);
     for(Int_t i=0;i<nelm;i++){
       gJSFLCFULL->TBGET(1,bankname[ib],neary[i], nw, (Int_t*)data, iret);
       if( nw > 50 ) { 
	Warning("MakeJSFLTKCLTrack",
       " Too many CDC track associated to the Combined track. nw=%d",  nw);
       }
        new(tracks[nt++])  JSFLTKCLTrack(bank[ib], data);
     }
  }
  buf->SetNtracks(nt);

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t JSFQuickSim::Process(Int_t ev)
{

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

   swmevt_(&recid, &level, &idebug, &iret);
   
   smrevt_(&recid, &level, &idebug, &iret);

   cmbtrk_(&iret);

   if( !MakeJSFLTKCLTracks() ) return kFALSE;

   return kTRUE;
}


//_____________________________________________________________________________
JSFQuickSimParam::JSFQuickSimParam()
{
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
  fNSMPVX   =     3    ; // # sampling layers + 1 = NSMPVX        
  fDPHIVX   =   25.E-4 ; // phi pitch (cm)                        
  fDZEEVX   =   25.E-4 ; // Z   pitch (cm)                        
  
  //  Beam pipe
  fVTXLayer[0][0]  =     1.   ; // radius (cm)                   
  fVTXLayer[0][1]  =  -100.   ; // Z-(cm)                        
  fVTXLayer[0][2]  =   100.   ; // Z+(cm)                        
  fVTXLayer[0][3]  =   0.003  ; // thickness in radiation length 
  //  Layer #1
  fVTXLayer[1][0]  =    2.5  ; // radius (cm)                   s
  fVTXLayer[1][1]  =   -7.5  ; // Z-(cm)                        
  fVTXLayer[1][2]  =    7.5  ; // Z+(cm)                        
  fVTXLayer[1][3]  =  0.003  ; // thickness in radiation length 
  //  Layer #2
  fVTXLayer[2][0]  =    7.5  ; // radius (cm)                   
  fVTXLayer[2][1]  =  -22.5  ; // Z-(cm)                        
  fVTXLayer[2][2]  =   22.5  ; // Z+(cm)                        
  fVTXLayer[2][3]  =  0.003  ; // thickness in radiation length 
  // layer #NSMPVX (CDC inner cylinder)
  fVTXLayer[3][0]  =    25.  ; // radius (cm)                   
  fVTXLayer[3][1]  =  -230.  ; // Z-(cm)                        
  fVTXLayer[3][2]  =   230.  ; // Z+(cm)                        
  fVTXLayer[3][3]  =   0.01  ; // thickness in radiation length 


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

  fCMBCUT[0] = 20.0 ; // ADXECT for CMBCUT
  fCMBCUT[1] = 2.0 ; //ENSGEM for EM-clusters
  fCMBCUT[2] = 2.0 ; // ENSGHD for HD-clusters

}

//_____________________________________________________________________________
JSFQuickSimParam::~JSFQuickSimParam()
{
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
  smrvgo_.nsmpvx   =  fNSMPVX   ; // # sampling layers + 1 = NSMPVX        
  smrvgo_.dphivx   =  fDPHIVX   ; // phi pitch (cm)                        
  smrvgo_.dzeevx   =  fDZEEVX   ; // Z   pitch (cm)                        
  
  //  fVTX Layer info
  for(Int_t i=0;i<=fNSMPVX;i++){
    smrvgo_.rcyvx[i]    = fVTXLayer[i][0] ; // radius (cm)                   
    smrvgo_.zcyvx[0][i] = fVTXLayer[i][1] ; // Z-(cm)                        
    smrvgo_.zcyvx[1][i] = fVTXLayer[i][2] ; // Z+(cm)                        
    smrvgo_.rdlvx[i]    = fVTXLayer[i][3] ; // thickness in radiation length 
  }

  for(Int_t i=0;i<4;i++){
    for(Int_t j=0;j<5;j++){
      smrclp_.clspar[i][j]=fCLSPAR[i][j];
    }
  }

  cmbcut_.adxect = fCMBCUT[0] ;
  cmbcut_.ensgem = fCMBCUT[1] ;
  cmbcut_.ensghd = fCMBCUT[2] ;

}


//____________________________________________________________________________
void JSFQuickSim::MakeBranch(TTree *tree)
{
  //  JSFModule::MakeBranch(tree);

   fTree=tree;
   Int_t split=0;
   Int_t bsize=10000;
   tree->Branch(fData->GetName(), fData->ClassName() ,&fData, bsize, split);
   tree->Branch(fEventBuf->GetName(), fEventBuf->ClassName() ,&fEventBuf, bsize, split);
}


//_____________________________________________________________________________
void JSFQuickSim::SetBranch(TTree *tree)
{
//  Set Branch address for this module

   fTree=tree;
   if( fData ) {
     Char_t name[50];
     sprintf(name,"%s",fData->GetName());
     TBranch *branch=tree->GetBranch(name);
     branch->SetAddress(&fData);
   }
   if( fEventBuf ) {
     Char_t name[50];
     sprintf(name,"%s",fEventBuf->GetName());
     TBranch *branch=tree->GetBranch(name);
     branch->SetAddress(&fEventBuf);
   }
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
void JSFQuickSimData::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFQuickSimData.
   Int_t i, j, iret, kw, nw, nelm[500];
   Float_t   rbuf[10000];

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TNamed::Streamer(R__b);

   // Create banks
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


  // Production:VTX;Space_Point
      R__b >> nw;
      for(i=0;i<nw;i++){
	R__b >> j;
	R__b >> kw;
	R__b.ReadStaticArray((float*)rbuf);
        gJSFLCFULL->TBPUT(1,"Production:VTX;Space_Point",j,
			  kw,(Int_t*)rbuf, iret);
      }

  // Production:CDC;Track_Parameter
      R__b >> nw;
      for(i=0;i<nw;i++){
        R__b >> j;
	R__b >> kw;
	R__b.ReadStaticArray((float*)rbuf);
	R__b.ReadStaticArray((int*)(rbuf+8));
	R__b.ReadStaticArray((float*)(rbuf+10));
	R__b.ReadStaticArray((double*)(rbuf+20));
	R__b.ReadStaticArray((Short_t*)(rbuf+50));
	R__b.ReadStaticArray((float*)(rbuf+53));
	R__b.ReadStaticArray((int*)(rbuf+55));
        gJSFLCFULL->TBPUT(1,"Production:CDC;Track_Parameter",j,
			  kw, (Int_t*)rbuf , iret);
      }

  // Production:CDC_VTX;Track_Parameter
      R__b >> nw;
      for(i=0;i<nw;i++){
        R__b >> j;
	R__b >> kw;
	R__b.ReadStaticArray((float*)rbuf);
	R__b.ReadStaticArray((int*)(rbuf+8));
	R__b.ReadStaticArray((float*)(rbuf+10));
	R__b.ReadStaticArray((double*)(rbuf+20));
	R__b.ReadStaticArray((Short_t*)(rbuf+50));
	R__b.ReadStaticArray((int*)(rbuf+52));
        gJSFLCFULL->TBPUT(1,"Production:CDC_VTX;Track_Parameter",j,
			  kw, (Int_t*)rbuf , iret);
      }

  // Production:EMC;Hit_Cell
      R__b >> nw;   // number of elements
      for(i=0;i<nw;i++){
        R__b >> j;  // element number
	R__b >> kw; // number of words
	R__b.ReadStaticArray((float*)rbuf);
	R__b.ReadStaticArray((int*)(rbuf+3));
        gJSFLCFULL->TBPUT(1,"Production:EMC;Hit_Cell",j,
			  kw, (Int_t*)rbuf , iret);
      }

  // Production:EMC;Cluster
      R__b >> nw;
      for(i=0;i<nw;i++){
        R__b >> j;
	R__b >> kw;
	R__b.ReadStaticArray((float*)rbuf);
	R__b.ReadStaticArray((int*)(rbuf+13));
        gJSFLCFULL->TBPUT(1,"Production:EMC;Cluster",j,
			  kw, (Int_t*)rbuf , iret);
      }

  // Production:EMC;Cluster_to_Cell
      R__b >> nw;
      for(i=0;i<nw;i++){
        R__b >> j;
	R__b >> kw;
        R__b.ReadStaticArray((int*)rbuf);
        gJSFLCFULL->TBGET(1,"Production:EMC;Cluster_to_Cell",j,
			  kw, (Int_t*)rbuf , iret);
      }

  // Production:EMC;Hit_Pad
      R__b >> nw;
      for(i=0;i<nw;i++){
        R__b >> j;
        gJSFLCFULL->TBGET(1,"Production:EMC;Hit_Pad",j,
			  kw, (Int_t*)rbuf , iret);
	R__b >> kw;
	R__b.ReadStaticArray((int*)(rbuf));
      }


  // Production:HDC;Hit_Cell
      R__b >> nw;   // number of elements
      for(i=0;i<nw;i++){
        R__b >> j;  // element number
	R__b >> kw; // number of words
	R__b.ReadStaticArray((float*)rbuf);
	R__b.ReadStaticArray((int*)(rbuf+3));
        gJSFLCFULL->TBPUT(1,"Production:HDC;Hit_Cell",j,
			  kw, (Int_t*)rbuf , iret);
      }

  // Production:HDC;Cluster
      R__b >> nw;
      for(i=0;i<nw;i++){
        R__b >> j;
	R__b >> kw;
	R__b.ReadStaticArray((float*)rbuf);
	R__b.ReadStaticArray((int*)(rbuf+13));
        gJSFLCFULL->TBPUT(1,"Production:HDC;Cluster",j,
			  kw, (Int_t*)rbuf , iret);
      }

  // Production:HDC;Cluster_to_Cell
      R__b >> nw;
      for(i=0;i<nw;i++){
        R__b >> j;
	R__b >> kw;
        R__b.ReadStaticArray((int*)rbuf);
        gJSFLCFULL->TBGET(1,"Production:HDC;Cluster_to_Cell",j,
			  kw, (Int_t*)rbuf , iret);
      }


  // Production:Combined_Gamma_Track
      R__b >> nw;
      for(i=0;i<nw;i++){
        R__b >> j;
	R__b >> kw;
        R__b.ReadStaticArray((int*)rbuf);
        gJSFLCFULL->TBGET(1,"Production:Combined_Gamma_Track",j,
			  kw, (Int_t*)rbuf , iret);
      }


  // Production:Combined_Lepton_Track
      R__b >> nw;
      for(i=0;i<nw;i++){
        R__b >> j;
	R__b >> kw;
        R__b.ReadStaticArray((int*)rbuf);
        gJSFLCFULL->TBGET(1,"Production:Combined_Lepton_Track",j,
			  kw, (Int_t*)rbuf , iret);
      }


  // Production:Combined_Hadron_Track
      R__b >> nw;
      for(i=0;i<nw;i++){
        R__b >> j;
	R__b >> kw;
        R__b.ReadStaticArray((int*)rbuf);
        gJSFLCFULL->TBGET(1,"Production:Combined_Hadron_Track",j,
			  kw, (Int_t*)rbuf , iret);
      }



//================  Output stream
   } else {
      R__b.WriteVersion(JSFQuickSimData::IsA());
      TNamed::Streamer(R__b);
  // Production:VTX;Space_Point
      gJSFLCFULL->TBNOEL(1,"Production:VTX;Space_Point",nw,nelm);
      R__b << nw;
      for(i=0;i<nw;i++){
        R__b << nelm[i];
        gJSFLCFULL->TBGET(1,"Production:VTX;Space_Point",nelm[i],
			  kw, (Int_t*)rbuf , iret);
	R__b << kw;
	R__b.WriteArray((float*)rbuf, kw);
      }

  // Production:CDC;Track_Parameter
      gJSFLCFULL->TBNOEL(1,"Production:CDC;Track_Parameter",nw,nelm);
      R__b << nw;
      for(i=0;i<nw;i++){
        R__b << nelm[i];
        gJSFLCFULL->TBGET(1,"Production:CDC;Track_Parameter",nelm[i],
			  kw, (Int_t*)rbuf , iret);
	R__b << kw;
	R__b.WriteArray((float*)rbuf, 8);
	R__b.WriteArray((int*)(rbuf+8), 1);
	R__b.WriteArray((float*)(rbuf+10), 8);
	R__b.WriteArray((double*)(rbuf+20), 15);
	R__b.WriteArray((Short_t*)(rbuf+50), 4);
	R__b.WriteArray((float*)(rbuf+53), 2);
	Int_t lw=kw-55;
	R__b.WriteArray((int*)(rbuf+55), lw);
      }

  // Production:CDC_VTX;Track_Parameter
      gJSFLCFULL->TBNOEL(1,"Production:CDC_VTX;Track_Parameter",nw,nelm);
      R__b << nw;
      for(i=0;i<nw;i++){
        R__b << nelm[i];
        gJSFLCFULL->TBGET(1,"Production:CDC_VTX;Track_Parameter",nelm[i],
			  kw, (Int_t*)rbuf , iret);
	R__b << kw;
	R__b.WriteArray((float*)rbuf, 8);
	R__b.WriteArray((int*)(rbuf+8), 1);
	R__b.WriteArray((float*)(rbuf+10), 8);
	R__b.WriteArray((double*)(rbuf+20), 15);
	R__b.WriteArray((Short_t*)(rbuf+50), 4);
	R__b.WriteArray((int*)(rbuf+52), 3);
      }

  // Production:EMC;Hit_Cell
      gJSFLCFULL->TBNOEL(1,"Production:EMC;Hit_Cell",nw,nelm);
      R__b << nw;
      for(i=0;i<nw;i++){
        R__b << nelm[i];
        gJSFLCFULL->TBGET(1,"Production:EMC;Hit_Cell",nelm[i],
			  kw, (Int_t*)rbuf , iret);
	R__b << kw;
	Int_t lw=kw-3;
	R__b.WriteArray((float*)rbuf, 3);
	R__b.WriteArray((int*)(rbuf+3), lw);
      }

  // Production:EMC;Cluster
      gJSFLCFULL->TBNOEL(1,"Production:EMC;Cluster",nw,nelm);
      R__b << nw;
      for(i=0;i<nw;i++){
        R__b << nelm[i];
        gJSFLCFULL->TBGET(1,"Production:EMC;Cluster",nelm[i],
			  kw, (Int_t*)rbuf , iret);
	R__b << kw;
	Int_t lw=kw-13;
	R__b.WriteArray((float*)rbuf, 7);
	R__b.WriteArray((int*)(rbuf+13), lw);
      }

  // Production:EMC;Cluster_to_Cell
      gJSFLCFULL->TBNOEL(1,"Production:EMC;Cluster_to_Cell",nw,nelm);
      R__b << nw;
      for(i=0;i<nw;i++){
        R__b << nelm[i];
        gJSFLCFULL->TBGET(1,"Production:EMC;Cluster_to_Cell",nelm[i],
			  kw, (Int_t*)rbuf , iret);
	R__b << kw;
	R__b.WriteArray((int*)(rbuf), kw);
      }

  // Production:EMC;Hit_Pad
      gJSFLCFULL->TBNOEL(1,"Production:EMC;Hit_Pad",nw,nelm);
      R__b << nw;
      for(i=0;i<nw;i++){
        R__b << nelm[i];
        gJSFLCFULL->TBGET(1,"Production:EMC;Hit_Pad",nelm[i],
			  kw, (Int_t*)rbuf , iret);
	R__b << kw;
	R__b.WriteArray((int*)(rbuf), kw);
      }

  // Production:HDC;Hit_Cell
      gJSFLCFULL->TBNOEL(1,"Production:HDC;Hit_Cell",nw,nelm);
      R__b << nw;
      for(i=0;i<nw;i++){
        R__b << nelm[i];
        gJSFLCFULL->TBGET(1,"Production:HDC;Hit_Cell",nelm[i],
			  kw, (Int_t*)rbuf , iret);
	R__b << kw;
	Int_t lw=kw-3;
	R__b.WriteArray((float*)rbuf, 3);
	R__b.WriteArray((int*)(rbuf+3), lw);
      }


  // Production:HDC;Cluster
      gJSFLCFULL->TBNOEL(1,"Production:HDC;Cluster",nw,nelm);
      R__b << nw;
      for(i=0;i<nw;i++){
        R__b << nelm[i];
        gJSFLCFULL->TBGET(1,"Production:HDC;Cluster",nelm[i],
			  kw, (Int_t*)rbuf , iret);
	R__b << kw;
	Int_t lw=kw-13;
	R__b.WriteArray((float*)rbuf, 7);
	R__b.WriteArray((int*)(rbuf+13), lw);
      }

  // Production:HDC;Cluster_to_Cell
      gJSFLCFULL->TBNOEL(1,"Production:HDC;Cluster_to_Cell",nw,nelm);
      R__b << nw;
      for(i=0;i<nw;i++){
        R__b << nelm[i];
        gJSFLCFULL->TBGET(1,"Production:HDC;Cluster_to_Cell",nelm[i],
			  kw, (Int_t*)rbuf , iret);
	R__b << kw;
	R__b.WriteArray((int*)(rbuf), kw);
      }


  // Production:Combined_Gamma_Track
      gJSFLCFULL->TBNOEL(1,"Production:Combined_Gamma_Track",nw,nelm);
      R__b << nw;
      for(i=0;i<nw;i++){
        R__b << nelm[i];
        gJSFLCFULL->TBGET(1,"Production:HDC;Combined_Gamma_Track",nelm[i],
			  kw, (Int_t*)rbuf , iret);
	R__b << kw;
	R__b.WriteArray((int*)(rbuf), kw);
      }

  // Production:Combined_Lepton_Track
      gJSFLCFULL->TBNOEL(1,"Production:Combined_Lepton_Track",nw,nelm);
      R__b << nw;
      for(i=0;i<nw;i++){
        R__b << nelm[i];
        gJSFLCFULL->TBGET(1,"Production:HDC;Combined_Lepton_Track",nelm[i],
			  kw, (Int_t*)rbuf , iret);
	R__b << kw;
	R__b.WriteArray((int*)(rbuf), kw);
      }

  // Production:Combined_Hadron_Track
      gJSFLCFULL->TBNOEL(1,"Production:Combined_Hadron_Track",nw,nelm);
      R__b << nw;
      for(i=0;i<nw;i++){
        R__b << nelm[i];
        gJSFLCFULL->TBGET(1,"Production:HDC;Combined_Hadron_Track",nelm[i],
			  kw, (Int_t*)rbuf , iret);
	R__b << kw;
	R__b.WriteArray((int*)(rbuf), kw);
      }

     

   } // End of WriteStreamer
}

