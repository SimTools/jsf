//*LastUpdate :  jsf-1-4  28-January-1999  By Akiya Miyamoto
//*-- Author  : Akiya Miyamoto  28-January-1999

///////////////////////////////////////////////////////////////////
//
// JSFSIMDST  
//
// Purpose of this class is 
// (1) Output SIMDST record as a Fortran binary record.
// (2) Read SIMDST format fortran record.
// (3) For user program which uses SIMDST format data, 
//     provide interface with JSFQuickSim class
// Thus this class does not output ROOT event tree.
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
  gJSFLCFULL->TBGET(2,"Smearing",2,nw,smrpar,iret);
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
  JSFSIMDSTBuf *simb=(JSFSIMDSTBuf*)EventBuf();

  simb->SetClassData(nev);

  return (simb->PackDST(nev));
}

// ---------------------------------------------------------------
void JSFSIMDSTBuf::SetClassData(Int_t nev)
{
  JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *gevt=(JSFGeneratorBuf*)gen->EventBuf();
  fHead[0]=nev;
  fHead[1]=gevt->GetEcm();

  fGeneratorParticles=gevt->GetParticles();
  fNGeneratorParticles = gevt->GetNparticles();

  JSFQuickSim *sim=(JSFQuickSim*)gJSF->FindModule("JSFQuickSim");
  JSFQuickSimBuf *sevt=(JSFQuickSimBuf*)sim->EventBuf();
  fCombinedTracks=sevt->GetTracks();
  fNCDCTracks=sevt->GetNCDCTracks();
  fCDCTracks=sevt->GetCDCTracks();
  fNCombinedTracks = sevt->GetNTracks();

  fNEMCHits=sevt->GetNEMCHits();
  fEMCHits=sevt->GetEMCHits();

  fNHDCHits=sevt->GetNHDCHits();
  fHDCHits=sevt->GetHDCHits();

  fNVTXHits=sevt->GetNVTXHits();
  fVTXHits=sevt->GetVTXHits();

}


// ---------------------------------------------------------------
Bool_t JSFSIMDSTBuf::PackDST(Int_t nev)
{
// Output Quick Simulator results 

  // ***************************************
  // Save JSFGenerator Bank information into the array, gendat.
  // ***************************************

  Int_t nobuf=fNGeneratorParticles;
  if( nobuf >= kGenMax ) { 
    printf(" Too many generator Track at event%d. Process abandand.\n",nev);
    return kFALSE;
  }
  Int_t ngen=0 ;
  Float_t gendat[nobuf][kGenSize];
  Short_t igendat[nobuf][kIGenSize];
  for(Int_t j=0;j<nobuf;j++){
    JSFGeneratorParticle *p=(JSFGeneratorParticle*)fGeneratorParticles->At(j);
    genidTOtrkid[p->fSer]=-1;
    //    if( p->fNdaughter != 0 ) continue;
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

    igendat[ngen][0]=p->fID;
    igendat[ngen][1]=p->fNdaughter;
    igendat[ngen][2]=p->fFirstDaughter;
    igendat[ngen][3]=p->fMother;

    ngen++;
  }

  // ************************************************************
  // Make a SIMDST information based on Combined Track 
  // Information(JSFLTKCLTrack class).
  // ************************************************************
  
  Float_t cmbt[fNCombinedTracks][kCmbtSize];
  Float_t trkf[fNCDCTracks][kTrkfSize];
  Double_t trkd[fNCDCTracks][kTrkdSize];
  Int_t    nvtxh[fNCDCTracks];
  const Int_t kVTXHSize=5;
  const Int_t kVTXIDSize=2;
  Double_t vtxd[fNVTXHits][kVTXHSize];
  Int_t idvtx[fNHDCHits][kVTXIDSize];

  Int_t i,j;
  Int_t ncdc=0;
  Int_t nvtx=0;
  for(i=0;i<fNCombinedTracks;i++){
     JSFLTKCLTrack *t=(JSFLTKCLTrack*)fCombinedTracks->UncheckedAt(i);
     cmbt[i][0]=t->GetPx();  cmbt[i][1]=t->GetPy();  
     cmbt[i][2]=t->GetPz(); cmbt[i][3]=t->GetE(); 
     cmbt[i][4]=t->GetCharge(); cmbt[i][5]=t->GetType(); cmbt[i][6]=0;
     cmbt[i][7]=0; 
     if( t->GetNCDC() <= 0 ) continue ;
     if( t->GetType() == 1 || t->GetType() == 3 ) continue;

       cmbt[i][7]=ncdc+1;
       JSFCDCTrack *ct=t->GetCDC();
       ct->GetTrackParameter(&trkf[ncdc][0]);
       ct->GetErrorMatrix(&trkd[ncdc][0]);
       nvtxh[ncdc]=ct->GetNVTX();

       // ***************************************
       // Make an array for vertex 
       // ***************************************
       JSFVTXHit *v;
       for(j=0;j<nvtxh[ncdc];j++){
	 v=ct->GetVTXHit(j);
	 vtxd[nvtx][0]=v->GetR();
	 vtxd[nvtx][1]=v->GetPhi();
	 vtxd[nvtx][2]=v->GetZ();
	 vtxd[nvtx][3]=v->GetDphi();
	 vtxd[nvtx][4]=v->GetDz();
	 idvtx[nvtx][0]=ncdc+1;
	 idvtx[nvtx][1]=v->GetLayerNumber();
	 nvtx++;
       }

       ncdc++ ;
  }
  if( ncdc != fNCDCTracks ) {
    printf("In JSFSIMDST::PackDST  ncdc(%d)",ncdc);
    printf(" and fNCDCTracks (%d) is inconsistent.\n",fNCDCTracks);
  }
  if( nvtx != fNVTXHits ) {
    printf("In JSFSIMDST::PackDST nvtx(%d)",nvtx);
    printf(" and fNVTXHits (%d) is inconsistent.\n",fNVTXHits);
  }

  // ***************************************
  // Make EMC/HDC Hit Cell info.
  // ***************************************

  Int_t emh[fNEMCHits][kClsSize];
  for(i=0;i<fNEMCHits;i++){
     JSFEMCHit *h=(JSFEMCHit*)fEMCHits->UncheckedAt(i);
     emh[i][0]=h->GetIEnergy();
     emh[i][1]=h->GetCellID();
  }

  Int_t hdh[fNHDCHits][kClsSize];
  for(i=0;i<fNHDCHits;i++){
     JSFHDCHit *h=(JSFHDCHit*)fHDCHits->UncheckedAt(i);
     hdh[i][0]=h->GetIEnergy();
     hdh[i][1]=h->GetCellID();
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
