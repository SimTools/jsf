//*LastUpdate :  jsf-1-6  30-March-1999  By Akiya Miyamoto
//*LastUpdate :  jsf-1-4  28-January-1999  By Akiya Miyamoto
//*-- Author  : Akiya Miyamoto  28-January-1999

/*
 30-March-1999  A.Miyamoto
   SIMDST Format is changed.  EMH(2,i), HDH(2,i) is increased to 
   EMH(3,i), and HDH(3,i), to store EM and HD responce separately. 
*/


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
// For the data format of SIMDST data, pelase see  begin_html <a href="http://acfahep.kek.jp/subg/sim/simdst/simdst.ps.gz">postscript</a> end_html, or  begin_html <a href="http://acfahep.kek.jp/subg/sim/simdst/simdst/index.html">html</a> end_html document.
// 
// Three void functions are prepared to control write/read fortran data.
//   If WriteData() function is called, fortran data is written.
//   If ReadData() function is called, fortran data is read in.
//   If NoReadWrite() function is called, no read/write of fortran record.
//  When WriteData() or NoReadWrite() is called, JSFQuickSim module must be
//  executed before this class.
//
// (Parameters)
// Following parameters can be specified in the configuration file,
// jsf.conf is default. They can be set by Set functions, too.
//     parameter name       (Default)      Set functions
// JSFSIMDST.DataFile       simdst.dat    SetDataFileName()
// JSFSIMDST.ParameterFile  simdst.parm   SetParamFileName()
// JSFSIMDST.fUnit          21            SetUnut()
// JSFSIMDST.DataReadWrite   1            WriteData()/ReadData()/NoReadWrite()
//
// (Limitation)
//   Maximum size of tracks, etc are limited as follows.
//     GEN  : max 500 tracks
//     TRKF, TRKD, VTX : Max. 500 tracks.
//     EMH, HDH        : Max. 1000 hits.
//   
//$Id$
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
	Float_t head[], Float_t gendat[][kGenSize], 
        Short_t igendat[][kIGenSize],
        Float_t cmbt[][kCmbtSize], Float_t trkf[][kTrkfSize],
	Double_t trkd[][kTrkdSize], 
        Int_t emh[][kClsSize], Int_t hdh[][kClsSize],
	Int_t nvtx[], Int_t *npvtx, Double_t vtxd[][kVTXHSize], 
        Int_t ivtx[][kVTXIDSize], Int_t lenproduc);
extern void simdstread_(Int_t *iunit, 
	Int_t *endian, Char_t *produc,
	Int_t *ivers, Int_t *ngen, Int_t *ncmb, 
        Int_t *ntrk, Int_t *nemh, Int_t *nhdh,
	Float_t head[], Float_t gendat[][kGenSize], 
	Short_t igendat[][kIGenSize],
        Float_t cmbt[][kCmbtSize], Float_t trkf[][kTrkfSize],
	Double_t trkd[][kTrkdSize], 
	Int_t emh[][kClsSize], Int_t hdh[][kClsSize],
	Int_t nvtx[], Int_t *npvtx, Double_t vtxd[][kVTXHSize], 
	Int_t ivtx[][kVTXIDSize], Int_t *nret, Int_t lenproduc);

extern Float_t ulctau_(Int_t *kf);

};

Int_t genidTOtrkid[kGenMax];  // Generator track number to CDC track number
Float_t smrpar[300];

TClonesArray *gsGeneratorParticles;
TClonesArray *gsCombinedTracks;
TClonesArray *gsCDCTracks;
TClonesArray *gsVTXHits;
TClonesArray *gsEMCHits;
TClonesArray *gsHDCHits;

//_____________________________________________________________________________
JSFSIMDST::JSFSIMDST(const char *name, const char *title)
       : JSFModule(name,title)
{
  fEventBuf = new JSFSIMDSTBuf("JSFSIMDSTBuf", 
	       "JSFSIMDST event buffer",this);
 
  sscanf(gJSF->Env()->GetValue("JSFSIMDST.DataFile","simdst.dat"),
	 "%s",fDataFileName);
  sscanf(gJSF->Env()->GetValue("JSFSIMDST.ParameterFile","simdst.parm"),
	 "%s",fParamFileName);
  sscanf(gJSF->Env()->GetValue("JSFSIMDST.fUnit","21"),"%d",&fUnit);
  sscanf(gJSF->Env()->GetValue("JSFSIMDST.DataReadWrite","1"),"%d",&fReadWrite);

  SetMakeBranch(kFALSE);
  fParam=NULL;
}


// ---------------------------------------------------------------
Bool_t JSFSIMDST::Initialize()
{
  return kTRUE;
}


// ---------------------------------------------------------------
Bool_t JSFSIMDST::BeginRun(Int_t nrun)
{
  Bool_t rc=kTRUE;
// Open a file to output simdst record as a Fortran binary file.
  Int_t lenf=strlen(fDataFileName);

  if( fReadWrite == 1 ) {
      simdstopen_(&fUnit, fDataFileName, lenf);
      rc=WriteParameters(nrun);
  }
  else if( fReadWrite == 2 ) { 
      simdstopen_(&fUnit, fDataFileName, lenf);
  }

  return rc;
}


// ---------------------------------------------------------------
Bool_t JSFSIMDST::WriteParameters(Int_t nrun)
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
  Int_t nerrvx=(Int_t)smrpar[69];
  Int_t nsmpvx=(Int_t)smrpar[70];

  if( nerrvx == 1 || nerrvx == 2 ) {
    com[71]="! VTX phi pitch (cm)";
    com[72]="! VTX Z   pitch (cm)";
    for(i=70;i<=73;i++){ fprintf(fd,"%3d   %g %s\n",i,smrpar[i-1],com[i-1]); }

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
  }

  // when nerrvx == 3
  else {
    com[71]="! Number of VTX layer (NUMVTX)";
    com[72]="! r-phi resolution(cm) of VTX";
    com[73]="! z resolution(cm) of VTX";
    com[74]="! r-phi resolution(cm) of VTX";
    com[75]="! z resolution(cm) of VTX";
    for(i=70;i<=76;i++){ fprintf(fd,"%3d   %g %s\n",i,smrpar[i-1],com[i-1]); }

    Int_t lyr=0;
    Int_t ioff=0;
    for(lyr=0;lyr<=nsmpvx;lyr++){
      ioff=4*lyr+76;
      com[ioff]="! VTX radius (cm)";
      com[ioff+1]="! VTX Z-(cm)";
      com[ioff+2]="! VTX Z+(cm)";
      com[ioff+3]="! VTX thickness in radiation length";
      for(i=0;i<4;i++){ 
        fprintf(fd,"%3d   %g %s\n",ioff+i+1,smrpar[ioff+i],com[ioff+i]); }
    }
  }

  fclose(fd);

  return kTRUE;


}

// ---------------------------------------------------------------
Bool_t JSFSIMDST::EndRun()
{
  if( fReadWrite ==1 || fReadWrite == 2 ) simdstclose_(&fUnit);
  return kTRUE;
}


// ---------------------------------------------------------------
Bool_t JSFSIMDST::Process(Int_t nev)
{
//

  JSFSIMDSTBuf *simb=(JSFSIMDSTBuf*)EventBuf();
  if( fReadWrite == 2 ) return simb->UnpackDST(nev);

  simb->SetClassData(nev);

  if( fReadWrite == 0 ) return kTRUE;

  return simb->PackDST(nev);

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
#ifdef R__ACC
  Float_t gendat[kGenMax][kGenSize];
  Short_t igendat[kGenMax][kIGenSize];
#else
  Float_t gendat[nobuf][kGenSize];
  Short_t igendat[nobuf][kIGenSize];
#endif
  Int_t j;
  for(j=0;j<nobuf;j++){
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
  
#ifdef R__ACC
  Float_t cmbt[kTrkMax][kCmbtSize];
  Float_t trkf[kTrkMax][kTrkfSize];
  Double_t trkd[kTrkMax][kTrkdSize];
  Int_t    nvtxh[kTrkMax];
  Double_t vtxd[kVTXBufSize][kVTXHSize];
  Int_t idvtx[kVTXBufSize][kVTXIDSize];
#else
  Float_t cmbt[fNCombinedTracks][kCmbtSize];
  Float_t trkf[fNCDCTracks][kTrkfSize];
  Double_t trkd[fNCDCTracks][kTrkdSize];
  Int_t    nvtxh[fNCDCTracks];
  Double_t vtxd[fNVTXHits][kVTXHSize];
  Int_t idvtx[fNVTXHits][kVTXIDSize];
#endif

  Int_t i;
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

#ifdef R__ACC
  Int_t emh[kClsMax][kClsSize];
  Int_t hdh[kClsMax][kClsSize];
#else
  Int_t emh[fNEMCHits][kClsSize];
  Int_t hdh[fNHDCHits][kClsSize];
#endif

  for(i=0;i<fNEMCHits;i++){
     JSFEMCHit *h=(JSFEMCHit*)fEMCHits->UncheckedAt(i);
     emh[i][0]=(Int_t)(h->GetEMEnergy()*1000.0);
     emh[i][1]=(Int_t)(h->GetHDEnergy()*1000.0);
     emh[i][2]=h->GetCellID();
  }

  for(i=0;i<fNHDCHits;i++){
     JSFHDCHit *h=(JSFHDCHit*)fHDCHits->UncheckedAt(i);
     hdh[i][0]=(Int_t)(h->GetEMEnergy()*1000.0);
     hdh[i][1]=(Int_t)(h->GetHDEnergy()*1000.0);
     hdh[i][2]=h->GetCellID();
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
void JSFSIMDSTBuf::SetClonesArray()
{
  // Set pointers for TClonesArray;

      if( !gsGeneratorParticles ) 
         gsGeneratorParticles= new TClonesArray("JSFGeneratorParticle", kGenMax);
      fGeneratorParticles=gsGeneratorParticles;
      if( !gsCombinedTracks ) 
         gsCombinedTracks = new TClonesArray("JSFLTKCLTrack", kTrkMax);
      fCombinedTracks = gsCombinedTracks ; 
      if( !gsCDCTracks )gsCDCTracks= new TClonesArray("JSFCDCTrack", kTrkMax);
      fCDCTracks=gsCDCTracks;
      if( !gsVTXHits ) gsVTXHits= new TClonesArray("JSFVTXHit", kVTXBufSize);
      fVTXHits=gsVTXHits;
      if( !gsEMCHits ) gsEMCHits= new TClonesArray("JSFEMCHit", kClsMax);
      fEMCHits=gsEMCHits;
      if( !gsHDCHits ) gsHDCHits= new TClonesArray("JSFHDCHit", kClsMax);
      fHDCHits=gsHDCHits;

}


// ---------------------------------------------------------------
Bool_t JSFSIMDSTBuf::UnpackDST(Int_t nev)
{
// Read SIMDST data from a file.

  Float_t  gendat[kGenMax][kGenSize];
  Short_t  igendat[kGenMax][kIGenSize];
  Float_t  cmbt[kTrkMax][kCmbtSize];
  Float_t  trkf[kTrkMax][kTrkfSize];
  Double_t trkd[kTrkMax][kTrkdSize];
  Int_t    emh[kClsMax][kClsSize];
  Int_t    hdh[kClsMax][kClsSize];
  Double_t vtxd[kVTXBufSize][kVTXHSize];
  Int_t    idvtx[kVTXBufSize][kVTXIDSize];
  Int_t    nret;
  Int_t    nvtxh[kTrkMax];
  Int_t    ngen;

  Int_t lenproduc=4;
  Int_t unit=((JSFSIMDST*)Module())->GetUnit();
  Int_t ivers;
  simdstread_(&unit, &fEndian, fProduc, &ivers, &ngen, &fNCombinedTracks,
       &fNCDCTracks, &fNEMCHits, &fNHDCHits, fHead, gendat, igendat, 
       cmbt, trkf, trkd, emh, hdh, nvtxh, &fNVTXHits, vtxd, idvtx, 
       &nret, lenproduc);

  if( ivers != fVersion ) {
    printf("SIMDST version of the file is %d ",ivers);
    printf("while the program is for the version %d\n",fVersion);
    printf("Use proper program to read this file.\n");
    return kFALSE;
  }

  SetClonesArray();

  // ***************************************
  // Fill GeneratorParticle Array
  // ***************************************
  fNGeneratorParticles=ngen;
  TClonesArray &gt = *(fGeneratorParticles);
  Int_t i;  
  for(i=0;i<fNGeneratorParticles;i++){
   Int_t kf=igendat[i][0];
   Double_t xctau=((Double_t)ulctau_(&kf));
   Double_t life=0.0;
   TVector pv(4); pv(0) = gendat[i][3] ; pv(1) = gendat[i][0]; 
                  pv(2) = gendat[i][1] ; pv(3) = gendat[i][2];
   TVector xv(4); xv(0) = gendat[i][7] ; xv(1) = gendat[i][4];
                  xv(2) = gendat[i][5] ; xv(3) = gendat[i][6];
   Int_t iser=i+1;
   Int_t igen1=igendat[i][1]; Int_t igen2=igendat[i][2] ; Int_t igen3=igendat[i][3];
   new(gt[i]) JSFGeneratorParticle(iser, kf, (Double_t)gendat[i][8],
	      (Double_t)gendat[i][9], pv, xv, igen1, igen2, igen3, 
	      xctau, life);
  }

  // ***************************************
  // Put EMC/HDC CALHit class
  // ***************************************

  TClonesArray &emha = *(fEMCHits);
  for(i=0;i<fNEMCHits;i++){
     new(emha[i])  JSFEMCHit(emh[i][2], emh[i][0], emh[i][1]);
  }
  TClonesArray &hdha = *(fHDCHits);
  for(i=0;i<fNHDCHits;i++){
     new(hdha[i])  JSFHDCHit(hdh[i][2], hdh[i][0], hdh[i][1]);
  }

  // ***************************************
  // Put CDCTrack class
  // ***************************************
  JSFLTKCLTrack *lt=NULL;
  JSFCDCTrack   *ct=NULL;
  JSFVTXHit     *vh=NULL;
  TClonesArray &cdc = *(fCDCTracks);
  for(i=0;i<fNCDCTracks;i++){
    new(cdc[i]) JSFCDCTrack(&trkf[i][0], &trkd[i][0]);
  }

  // ***************************************
  // Put VTXHits class
  // ***************************************
  TClonesArray &vhits = *(fVTXHits);
  for(i=0;i<fNVTXHits;i++){
    Int_t it=idvtx[i][0]-1;
    Int_t igen=(Int_t)trkf[it][8];
    new(vhits[i]) JSFVTXHit(vtxd[i][0], vtxd[i][1], vtxd[i][2], 
			   vtxd[i][3], vtxd[i][4], idvtx[i][1], it, igen);
    if( idvtx[i][0] > 0 ) {
      vh=(JSFVTXHit*)fVTXHits->UncheckedAt(i);
      ct=(JSFCDCTrack*)fCDCTracks->UncheckedAt(it);
      ct->AddVTXHit(vh);
    }
  }

  // ***************************************
  // Put LTKCLTrack class
  // ***************************************
  TClonesArray &cta = *(fCombinedTracks);
  for(i=0;i<fNCombinedTracks;i++){
    new(cta[i]) JSFLTKCLTrack(&cmbt[i][0]);
    lt=(JSFLTKCLTrack*)cta.UncheckedAt(i);
    Int_t icdc=(Int_t)cmbt[i][7]-1;
    if( icdc >= 0 ){
      ct=(JSFCDCTrack*)fCDCTracks->UncheckedAt(icdc);
      lt->SetCDC(icdc, ct);
    }
  }

  return kTRUE;

}

// ---------------------------------------------------------------
JSFSIMDSTBuf::JSFSIMDSTBuf(const char *name, const char *title,	JSFModule *module)
    : JSFEventBuf(name, title, module)
{  
  fEndian = 1296651082;
  //  fVersion = 201;
  fVersion = 202;     // Since jsf-1-6
  strcpy(fProduc,"QIK ");

}



// ---------------------------------------------------------------
void JSFSIMDSTBuf::AddVTXHit(Double_t r, Double_t phi, Double_t z, 
      Double_t dphi, Double_t dz, Int_t layer, Int_t trackid, Int_t gentrack)
{
  TClonesArray &vhits = *(fVTXHits);
  new(vhits[fNVTXHits]) JSFVTXHit(r, phi, z, dphi, dz, 
				    layer, trackid, gentrack);
  if( trackid > 0 ) {
    JSFVTXHit *vh=(JSFVTXHit*)fVTXHits->UncheckedAt(fNVTXHits);
    JSFCDCTrack *ct=(JSFCDCTrack*)fCDCTracks->UncheckedAt(trackid);
    ct->AddVTXHit(vh);
  }

  fNVTXHits++;

}
