//*LastUpdate :  jsf-1-4  28-January-1999  By Akiya Miyamoto
//*-- Author  : Akiya Miyamoto  28-January-1999

///////////////////////////////////////////////////////////////////
//
// JSFSIMDST  
//
// Output SIMDST record as a Fortran binary record.
//
// (Limitation)
//   Maximum size of tracks, etc are limited as follows.
//     GEN  : max 500 tracks
//     TRKF, TRKD, VTX : Max. 500 tracks.
//     EMH, HDH        : Max. 1000 hits.
//   
//////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFLCFULL.h"
#include "JSFGenerator.h"
#include "JSFSIMDST.h"
#include <TMath.h>

ClassImp(JSFSIMDST)
ClassImp(JSFSIMDSTBuf)

#undef JSFDEBUG
#ifdef JSFDEBUG
TH1F *hTh, *hPhi, *hDth, *hDphi, *hNDth, *hNDphi;
TH1F *hKappa, *hTanl, *hNKappa, *hNTanl, *hPKappa, *hPTanl;
TH1F *h20,*h21, *h22, *h23, *h31,*h32, *h33;
#endif

extern "C" {
extern void simdstopen_(Int_t *unit, Char_t *name, Int_t len);
extern void simdstclose_(Int_t *unit);
extern void simdstwrite_(Int_t *iunit, 
	Int_t *endian, Char_t *produc,
	Int_t *ivers, Int_t *ngen, Int_t *ncmb, 
        Int_t *ntrk, Int_t *nemh, Int_t *nhdh,
	Float_t head[], Float_t gendat[][], Short_t igendat[][],
        Float_t cmbt[][], Float_t trkf[][],
	Double_t trkd[][], Int_t emh[][], Int_t hdh[][],
	Int_t nvtx[], Int_t *npvtx, Double_t vtxd[][], Int_t ivtx[][],
	Int_t lenproduc);
extern void utrkmv_(Int_t *lnxhlx, Float_t helxin[], 
		    Float_t xp[], Float_t helxot[]);
};

Int_t isergen[kGenMax];//Generator track's serial number to generator track number
Int_t genidTOtrkid[kGenMax];  // Generator track number to CDC track number
Float_t smrpar[300];



//_____________________________________________________________________________
JSFSIMDST::JSFSIMDST(const char *name, const char *title)
       : JSFModule(name,title)
{
  fEventBuf = new JSFSIMDSTBuf("JSFSIMDSTBuf", 
	       "JSFSIMDST event buffer",this);
 
  sscanf(gJSF->Env()->GetValue("JSFSIMDST.OutputFile","simdst.dat"),
	 "%s",fOutputFileName);
  sscanf(gJSF->Env()->GetValue("JSFSIMDST.OutputFile","simdst.parm"),
	 "%s",fParamFileName);
  sscanf(gJSF->Env()->GetValue("JSFSIMDST.fUnit","21"),"%d",&fUnit);


}


// ---------------------------------------------------------------
Bool_t JSFSIMDST::Initialize()
{
  // Open a file to output simdst record as a Fortran binary file.
  Int_t lenf=strlen(fOutputFileName);
  simdstopen_(&fUnit, fOutputFileName, lenf);

#ifdef JSFDEBUG
  hTh=new TH1F("hTh","Th Distribution",100,1.568, 1.573);
  hPhi=new TH1F("hPhi","Phi Distribution",100,1.543, 1.545);

  hDth=new TH1F("hDth","Dth Distribution",100,0.0, 0.001);
  hDphi=new TH1F("hDphi","Dphi Distribution",100,0.0, 0.0001);

  hNDth=new TH1F("hNDth","Th/Error Distribution",100,-5.0,5.0);
  hNDphi=new TH1F("hNDphi","Phi/Error Distribution",100,-5.0,5.0);

  hKappa=new TH1F("hKappa","Kappa",100,0.033,0.034);
  hTanl =new TH1F("hTanl","Tanlambda",100,-0.002,0.002);
  hNKappa=new TH1F("hNKappa","(kappa-Kappa0)/Error",100,-5.0,5.0);
  hNTanl =new TH1F("hNTanl","(Tanl-tanl0)/Error",100,-5.0, 5.0);
  hPKappa=new TH1F("hPKappa","Kappa-Probability",100,0.0,1.0);
  hPTanl =new TH1F("hPTanl","Tanl-Pobability",100,0.0,1.0);

  h20=new TH1F("h20","dr vefore utrkmv ...",100,-0.01,0.02);
  h21=new TH1F("h21","dr after utrkmv ...",100,0.07,0.10);
  h22=new TH1F("h22","f0 after utrkmv ...",100,-0.048,-0.044);
  h23=new TH1F("h23","dz after utrkmv ...",100,-0.2, 0.2);

  h31=new TH1F("h31","(dr-..)/ddr",100,-5.0,5.0);
  h32=new TH1F("h32","(f0-..)/df0",100,-5.0,5.0);
  h33=new TH1F("h33","(dz-..)/ddz",100,-5.0,5.0);

#endif

  return kTRUE;
}


// ---------------------------------------------------------------
Bool_t JSFSIMDST::BeginRun(Int_t nrun)
{
//  Save detector parameters into a ascii file.
//  Parameter file name is obtained by fParamFileName.
//  Format of the parameter file is similar to the detector parameter
//  file used by the Quick Simulator, such as
//  in /proj/phys/lclib/pro/simjlc/param/detect5.com
//

  Int_t nw, iret;
  gJSFLCFULL->TBGET(2,"Smearing",2,nw,(Int_t*)smrpar,iret);
  if( iret < 0 ) {
    printf("In JSFSIMDST::BeginRun .. Unable to get smearing parameter");
    printf(" for Run%d\n",nrun);
    return kFALSE;
  }

  Char_t *com[200];
  FILE *fd=fopen(fParamFileName,"w");
  
  Int_t i;
  Int_t &iseed=(Int_t&)smrpar[0];
  com[0]="! seed for smearing";
  com[1]="! B field (kG) ( dummy if Swimmer specifies B )";
  i=1;  fprintf(fd,"%3d   %d %s\n",i,iseed,com[i-1]);
  i=2;  fprintf(fd,"%3d   %g %s\n",i,smrpar[i-1],com[i-1]);

  fprintf(fd,"@! \n@! Tracking detector \n@!\n");
  com[10]="! CDC inner radius(cm)";
  com[11]="! CDC outer radius(cm)";
  com[12]="! CDC Z-(cm)";
  com[13]="! CDC Z+(cm)";
  com[14]="! CDC # sampling points";
  com[15]="! CDC cut on # sampling points";
  com[16]="! CDC sigma_(r-phi)";
  com[17]="! CDC sigma_Z";
  for(i=11;i<=18;i++){ fprintf(fd,"%3d   %g %s\n",i,smrpar[i-1],com[i-1]); }

  fprintf(fd,"@! \n@! EMC Calorimeter \n@!\n");
  com[30]="! EMC # phi segments";
  com[31]="! EMC # theta segments of barrel";
  com[32]="! EMC # radial segments of endcap";
  com[33]="! EMC inner radius(cm)";
  com[34]="! EMC outer radius(cm)";
  com[35]="! EMC Z-(cm)";
  com[36]="! EMC Z+(cm)";
  com[37]="! EMC sigma_E/E pedestal for barrel";
  com[38]="! EMC sigma_E/E at 1 GeV";
  com[39]="! EMC sigma_E/E pedestal for endcap";
  com[40]="! EMC sigma_E/E at 1 GeV";
  for(i=31;i<=41;i++){ fprintf(fd,"%3d   %g %s\n",i,smrpar[i-1],com[i-1]); }

  fprintf(fd,"@! \n@! HDC Calorimeter \n@!\n");
  com[50]="! HDC # phi segments";
  com[51]="! HDC # theta segments of barrel";
  com[52]="! HDC # radial segments of endcap";
  com[53]="! HDC inner radius(cm)";
  com[54]="! HDC outer radius(cm)";
  com[55]="! HDC Z-(cm)";
  com[56]="! HDC Z+(CM)";
  com[57]="! HDC sigma_E/E pedestal for barrel";
  com[58]="! HDC sigma_E/E at 1 GeV";
  com[59]="! HDC sigma_E/E pedestal for endcap";
  com[60]="! HDC sigma_E/E at 1 GeV";
  for(i=51;i<=61;i++){ fprintf(fd,"%3d   %g %s\n",i,smrpar[i-1],com[i-1]); }

  fprintf(fd,"@! \n@! VTX detector \n@!\n");
  com[69]="! VTX NERRVX Type of parameter format";
  com[70]="! VTX # sampling layers + 1 = NSMPVX";
  com[71]="! VTX phi pitch (cm)";
  com[72]="! VTX Z   pitch (cm)";
  for(i=70;i<=73;i++){ fprintf(fd,"%3d   %g %s\n",i,smrpar[i-1],com[i-1]); }

  Int_t nerrvx=(Int_t)smrpar[69];
  Int_t nsmpvx=(Int_t)smrpar[70];

  Int_t lyr=0;
  Int_t ioff=0;
  for(lyr=0;lyr<=nsmpvx;lyr++){
    ioff=4*lyr+73;
    com[ioff]="! VTX radius (cm)";
    com[ioff+1]="! VTX Z-(cm)";
    com[ioff+2]="! VTX Z+(cm)";
    com[ioff+3]="! VTX thickness in radiation length";
    for(i=0;i<4;i++){  
      fprintf(fd,"%3d   %g %s\n",ioff+i+1,smrpar[ioff+i],com[ioff+i]); }
  }
  ioff +=4;
  if( nerrvx > 1 ) {
    com[ioff]="! VTX r-phi and Z resolution (sigma)";
    com[ioff+1]="! VTX not used";
    com[ioff+2]="! VTX not used";
    com[ioff+3]="! VTX not used";
    com[ioff+4]="! VTX not used";
    for(i=0;i<5;i++){ 
      if( strcmp(com[ioff+i],"! VTX not used") != 0 ) 
      fprintf(fd,"%3d   %g %s\n",ioff+i+1,smrpar[ioff+i],com[ioff+i]); 
    }
  } 
  fclose(fd);

  return kTRUE;


}

// ---------------------------------------------------------------
Bool_t JSFSIMDST::EndRun()
{
  simdstclose_(&fUnit);
  return kTRUE;
}


// ---------------------------------------------------------------
Bool_t JSFSIMDST::Process(Int_t nev)
{
//
  return ((JSFSIMDSTBuf*)EventBuf())->PackDST(nev);
}

// ---------------------------------------------------------------
Bool_t JSFSIMDSTBuf::PackDST(Int_t nev)
{
// Output Quick Simulator results 

  // ***************************************
  // Save JSFGenerator Bank information into the array, gendat.
  // ***************************************


  JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *gevt=(JSFGeneratorBuf*)gen->EventBuf();

  fHead[0]=nev;
  fHead[1]=gevt->GetEcm();

  fGeneratorParticles=gevt->GetParticles();
  fNGeneratorParticles = gevt->GetNparticles();

  Int_t nobuf=fNGeneratorParticles;
  if( nobuf >= kGenMax ) { 
    printf(" Too many generator Track at event%d. Process abandand.\n",nev);
    return kFALSE;
  }
  Int_t ngen=0 ;
  Float_t gendat[nobuf][kGenSize];
  Short_t   igendat[nobuf][kIGenSize];
  for(Int_t j=0;j<nobuf;j++){
    JSFGeneratorParticle *p=(JSFGeneratorParticle*)fGeneratorParticles->At(j);
    isergen[p->fSer]=-1;
    genidTOtrkid[p->fSer]=-1;
    //    if( p->fNdaughter != 0 ) continue;
    isergen[p->fSer]=ngen;
    gendat[ngen][0]=p->fP[1];
    gendat[ngen][1]=p->fP[2]; 
    gendat[ngen][2]=p->fP[3];
    gendat[ngen][3]=p->fP[0];
    gendat[ngen][4]=p->fX[1];
    gendat[ngen][5]=p->fX[2];
    gendat[ngen][6]=p->fX[3];
    gendat[ngen][7]=p->fX[0];

    gendat[ngen][8]=p->fMass;
    gendat[ngen][9]=p->fCharge;

    //  gendat[ngen][8]=p->fID;
    //  gendat[ngen][9]=p->fMass;
    //  gendat[ngen][10]=p->fCharge;
    
    igendat[ngen][0]=p->fID;
    igendat[ngen][1]=p->fNdaughter;
    igendat[ngen][2]=p->fFirstDaughter;
    igendat[ngen][3]=p->fMother;

    ngen++;
  }

  // ************************************************************
  // Make a SIMDST information based on Combined Track Information(JSFLTKCLTrack class).
  // ************************************************************

  JSFQuickSim *sim=(JSFQuickSim*)gJSF->FindModule("JSFQuickSim");
  JSFQuickSimBuf *sevt=(JSFQuickSimBuf*)sim->EventBuf();
  TClonesArray *fCombinedTracks=sevt->GetTracks();
  TClonesArray *fCDCTracks=sevt->GetCDCTracks();
  fNCombinedTracks = sevt->GetNtracks();
  
  Float_t cmbt[fNCombinedTracks][kCmbtSize];
  Float_t trkf[fNCombinedTracks][kTrkfSize];
  Double_t trkd[fNCombinedTracks][kTrkdSize];
  Int_t    nvtxh[fNCombinedTracks];

  Int_t i;
  Bool_t rc=kTRUE;
  fNCDCTracks=0;
  for(i=0;i<fNCombinedTracks;i++){
     JSFLTKCLTrack *t=(JSFLTKCLTrack*)fCombinedTracks->UncheckedAt(i);
     cmbt[i][0]=t->GetPx();  cmbt[i][1]=t->GetPy();  
     cmbt[i][2]=t->GetPz(); cmbt[i][3]=t->GetE(); 
     cmbt[i][4]=t->GetCharge(); cmbt[i][5]=t->GetType(); cmbt[i][6]=0;
     cmbt[i][7]=0; if( t->GetNCDC() > 0 ) cmbt[i][7]=t->Get1stCDC();

     if( t->GetType() != 1 && t->GetType() !=3 ) {
       Int_t icdc=t->Get1stCDC();
       JSFCDCTrack *ht=(JSFCDCTrack*)fCDCTracks->UncheckedAt(icdc-1);
       ht->GetTrackParameter(&trkf[fNCDCTracks][0]);
       ht->GetErrorMatrix(&trkd[fNCDCTracks][0]);
       nvtxh[fNCDCTracks]=ht->GetNVTX();
       fNCDCTracks++ ;
     }
     if( !rc ) return kFALSE;
  }
//  printf(" ncdc=%d ncmb=%d\n",fNCDCTracks,fNCombinedTracks);
 
  // ***************************************
  // Make EMC/HDC Hit Cell info.
  // ***************************************

  fNEMCHits=sevt->GetNEMCHits();
  fEMCHits=sevt->GetEMCHits();
  Int_t emh[fNEMCHits][kClsSize];
  for(i=0;i<fNEMCHits;i++){
     JSFEMCHit *h=(JSFEMCHit*)fEMCHits->UncheckedAt(i);
     emh[i][0]=h->GetIEnergy();
     emh[i][1]=h->GetCellID();
     //     printf(" i=%d  energy=%d cellid=%d\n",i,emh[i][0],emh[i][1]);
  }

  fNHDCHits=sevt->GetNHDCHits();
  fHDCHits=sevt->GetHDCHits();
  Int_t hdh[fNHDCHits][kClsSize];
  for(i=0;i<fNHDCHits;i++){
     JSFHDCHit *h=(JSFHDCHit*)fHDCHits->UncheckedAt(i);
     hdh[i][0]=h->GetIEnergy();
     hdh[i][1]=h->GetCellID();
     // printf(" i=%d  HDC energy=%d cellid=%d\n",i,hdh[i][0],hdh[i][1]);
  }

  // ***************************************
  // Make an array for vertex hit
  // ***************************************

  fNVTXHits=sevt->GetNVTXHits();
  fVTXHits=sevt->GetVTXHits();
  const Int_t kVTXHSize=5;
  const Int_t kVTXIDSize=2;
  Double_t vtxd[fNVTXHits][kVTXHSize];
  Int_t idvtx[fNHDCHits][kVTXIDSize];
  for(i=0;i<fNVTXHits;i++){
     JSFVTXHit *h=(JSFVTXHit*)fVTXHits->UncheckedAt(i);
     vtxd[i][0]=h->GetR();
     vtxd[i][1]=h->GetPhi();
     vtxd[i][2]=h->GetZ();
     vtxd[i][3]=h->GetDphi();
     vtxd[i][4]=h->GetDz();
     idvtx[i][0]=h->GetLinkedTrack();
     idvtx[i][1]=h->GetLayerNumber();
  }

  Int_t  lenproduc=4;

  Int_t unit=((JSFSIMDST*)Module())->GetUnit();
  simdstwrite_(&unit, &fEndian, fProduc, &fVersion, &ngen, &fNCombinedTracks,
        &fNCDCTracks, &fNEMCHits, &fNHDCHits, fHead, gendat, igendat, 
        cmbt, trkf,
	trkd, emh, hdh, nvtxh, &fNVTXHits, vtxd, idvtx, lenproduc);

  return kTRUE;

}

// ---------------------------------------------------------------
JSFSIMDSTBuf::JSFSIMDSTBuf(const char *name, const char *title,	JSFModule *module)
    : JSFEventBuf(name, title, module)
{  
  fEndian = 1296651082;
  fVersion = 201;
  strcpy(fProduc,"QIK ");

}
