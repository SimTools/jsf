//*LastUpdate :  jsf-1-8  17-April-1999  By Akiya Miyamoto
//*-- Author  : Akiya Miyamoto  17-April-1999


///////////////////////////////////////////////////////////////////
//
// JSFReadJIMBank
//
// The JSFReadJIMBank class read a simulation data created by
// JIM and put them into JSFSIMDST class
//
//$Id
//
//////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFLCFULL.h"
#include "JSFGenerator.h"
#include "JSFJIMParam.h"
#include "JSFReadJIMBank.h"
#include <TMath.h>

ClassImp(JSFReadJIMBank)
ClassImp(JSFReadJIMBankBuf)

extern "C" {
extern void readjiminit_(Int_t *unit, Char_t *name, Int_t len);
extern void readjimend_();
extern void readjimread_(Int_t *nret);

extern Float_t ulctau_(Int_t *kf);

};


const Int_t kJGenMax=700;
const Int_t kJCmbMax=700;
const Int_t kJTrkMax=300;
const Int_t kJEClsMax=500;
const Int_t kJHClsMax=400;
const Int_t kJClsSize=3;
const Int_t kJVTXBufSize=2000;

typedef struct {
  Int_t endian;
  Char_t produc[4];
  Int_t ivers, head[2],ierflg;
  Int_t ngen ; Float_t gen[kJGenMax][kGenSize]; 
               Int_t igen[kJGenMax][kIGenSize];
  Int_t ncmb ; Float_t cmbt[kJCmbMax][kCmbtSize];
  Int_t ntrk ; Float_t trkf[kJTrkMax][kTrkfSize]; 
               Float_t trkd[kJTrkMax][kTrkdSize];
               Int_t nvtx[kJTrkMax];
  Int_t nemh ; Int_t emh[kJEClsMax][kJClsSize];
  Int_t nhdh ; Int_t hdh[kJHClsMax][kJClsSize];
  Int_t nsih ; Float_t vtxd[kJVTXBufSize][kVTXHSize] ; 
  Int_t idvtx[kJVTXBufSize][kVTXIDSize];
} COMMON_TRBUFF;

extern COMMON_TRBUFF trbuff_;



//_____________________________________________________________________________
JSFReadJIMBank::JSFReadJIMBank(const char *name, const char *title, Bool_t constbuf)
       : JSFSIMDST(name,title)
{
  if( constbuf )  fEventBuf = new JSFReadJIMBankBuf("JSFReadJIMBankBuf", 
			       "JSFReadJIMBank event buffer",this);
  fParam=new JSFJIMParam();
  fDoesParameterInitialized=kFALSE;
  ReadData();
}


// ---------------------------------------------------------------
Bool_t JSFReadJIMBank::Initialize()
{
  return kTRUE;
}


// ---------------------------------------------------------------
Bool_t JSFReadJIMBank::BeginRun(Int_t nrun)
{
  Int_t lenf=strlen(fDataFileName);
  if( fReadWrite == 2 ) {
    readjiminit_(&fUnit, fDataFileName, lenf);
    printf("%s is opened.\n",fDataFileName);
  }
  return kTRUE;
}

// ---------------------------------------------------------------
Bool_t JSFReadJIMBank::EndRun()
{
  readjimend_();
  return kTRUE;
}


// ---------------------------------------------------------------
Bool_t JSFReadJIMBank::Process(Int_t nev)
{
//

  JSFReadJIMBankBuf *simb=(JSFReadJIMBankBuf*)EventBuf();

  Bool_t rc=simb->UnpackDST(nev);

  if( !fDoesParameterInitialized ) {
    fDoesParameterInitialized=kTRUE;
    fParam->ReadParameter();
  }

  /*
  if( fReadWrite == 2 ) return simb->UnpackDST(nev);

  simb->SetClassData(nev);

  if( fReadWrite == 0 ) return kTRUE;

  return simb->PackDST(nev);
  */
  return rc;
}


// ---------------------------------------------------------------
Bool_t JSFReadJIMBankBuf::UnpackDST(Int_t nev)
{
// Read SIMDST data from a file.

  //  readjimread_(&unit, &fEndian, fProduc, &ivers, &ngen, &fNCombinedTracks,
  //     &fNCDCTracks, &fNEMCHits, &fNHDCHits, fHead, gendat, igendat, 
  //     cmbt, trkf, trkd, emh, hdh, nvtxh, &fNVTXHits, vtxd, idvtx, 
  //     &nret, lenproduc);


  Int_t nret;
  readjimread_(&nret);


  Int_t ivers;
  for(ivers=0;ivers<4;ivers++){ fProduc[ivers]=trbuff_.produc[ivers]; }
  fProduc[4]=0;
  ivers=trbuff_.ivers;
  printf(" ivers=%d\n",ivers);
  if( ivers != fVersion ) {
    printf("SIMDST version of the file is %d ",ivers);
    printf("while the program is for the version %d\n",fVersion);
    printf("Use proper program to read this file.\n");
    return kFALSE;
  }
  else if( nret == -1 ) { 
    printf("Read end-of-file of input file\n");
    return kFALSE;
  }
  else if( nret == -2 ) {
    printf("Read error of input file.\n");
    return kFALSE;
  }


  SetClonesArray();

  // ***************************************
  // Fill GeneratorParticle Array
  // *************************************** 
  fNGeneratorParticles=trbuff_.ngen;
  fNGeneratorParticles=0;
  Int_t i;  
  /*
  TClonesArray &gt = *(fGeneratorParticles);
  for(i=0;i<fNGeneratorParticles;i++){
    Int_t *igendat=&(trbuff_.igen[i][0]); 
    Float_t *gendat=&(trbuff_.gen[i][0]);

    // Old format data
    TVector pv(4); pv(0) = gendat[3] ; pv(1) = gendat[0]; 
                   pv(2) = gendat[1] ; pv(3) = gendat[2];
    TVector xv(4); xv(0) = gendat[7] ; xv(1) = gendat[4];
                   xv(2) = gendat[5] ; xv(3) = gendat[6];
    Int_t iser=i+1;
    if ( ivers < 200 ) { 
      Int_t kf=(Int_t)gendat[8];
      Float_t mass=gendat[9];
      Float_t charge=gendat[10];

      printf(" iser=%d kf=%d mass, charge=%g %g \n",iser,kf,mass,charge);   printf(" iser=%d kf=%d \n",iser,kf);



      new(gt[i]) JSFGeneratorParticle(iser, kf, mass,
	      charge, pv, xv);
    }
    else {
      Int_t kf=igendat[0];
      Double_t xctau=((Double_t)ulctau_(&kf));
      Double_t life=0.0;
      Int_t igen1=igendat[1]; Int_t igen2=igendat[2] ; Int_t igen3=igendat[3];

      new(gt[i]) JSFGeneratorParticle(iser, kf, (Double_t)gendat[8],
	      (Double_t)gendat[9], pv, xv, igen1, igen2, igen3, 
	      xctau, life);
    }
  }
  */

  // ***************************************
  // Put EMC/HDC CALHit class
  // ***************************************

  fNEMCHits = trbuff_.nemh;
  TClonesArray &emha = *(fEMCHits);
  for(i=0;i<fNEMCHits;i++){
    //     new(emha[i])  JSFEMCHit(trbuff_.emh[i][2], trbuff_.emh[i][0], trbuff_.emh[i][1]);
     new(emha[i])  JSFEMCHit(trbuff_.emh[i][1], trbuff_.emh[i][0], 0);
  }
  fNHDCHits = trbuff_.nhdh;
  TClonesArray &hdha = *(fHDCHits);
  for(i=0;i<fNHDCHits;i++){
    //     new(hdha[i])  JSFHDCHit(hdh[i][2], hdh[i][0], hdh[i][1]);
     new(hdha[i])  JSFHDCHit(trbuff_.hdh[i][1], trbuff_.hdh[i][0], 0);
  }

  // ***************************************
  // Put CDCTrack class
  // ***************************************
  fNCDCTracks = trbuff_.ntrk;
  JSFLTKCLTrack *lt=NULL;
  JSFCDCTrack   *ct=NULL;
  JSFVTXHit     *vh=NULL;
  TClonesArray &cdc = *(fCDCTracks);
  Double_t trkd[kTrkdSize];
  for(i=0;i<fNCDCTracks;i++){
    Int_t j ; for(j=0;j<kTrkdSize;j++){ trkd[j]=trbuff_.trkd[i][j]; }
    new(cdc[i]) JSFCDCTrack(&trbuff_.trkf[i][0], trkd);
  }

  // ***************************************
  // Put VTXHits class
  // ***************************************
  fNVTXHits = trbuff_.nsih;
  TClonesArray &vhits = *(fVTXHits);
  for(i=0;i<fNVTXHits;i++){
    Int_t it=trbuff_.idvtx[i][0]-1;
    Int_t igen=(Int_t)trbuff_.trkf[it][8];
    new(vhits[i]) JSFVTXHit(trbuff_.vtxd[i][0], trbuff_.vtxd[i][1], trbuff_.vtxd[i][2], 
		   trbuff_.vtxd[i][3], trbuff_.vtxd[i][4], trbuff_.idvtx[i][1], it, igen);
    if( trbuff_.idvtx[i][0] > 0 ) {
      vh=(JSFVTXHit*)fVTXHits->UncheckedAt(i);
      ct=(JSFCDCTrack*)fCDCTracks->UncheckedAt(it);
      if( vh && ct ) {
        ct->AddVTXHit(vh);
      }

      // else {
      //	printf("Warning in JSFReadJIMBank::UnpackDST\n");
      //	printf(" address of JSFVTXHit class(%x) for index %d or JSFCDCTrack class(%x) for index %d is wrong.",(Int_t)vh,(Int_t)ct, i, it);
      //	printf(" idvtx[%d][0]=%d\n",i,trbuff_.idvtx[i][0]);
      // }
    }
  }

  // ***************************************
  // Put LTKCLTrack class
  // ***************************************
  fNCombinedTracks=trbuff_.ncmb;
  TClonesArray &cta = *(fCombinedTracks);
  for(i=0;i<fNCombinedTracks;i++){
    new(cta[i]) JSFLTKCLTrack(&trbuff_.cmbt[i][0]);
    lt=(JSFLTKCLTrack*)cta.UncheckedAt(i);
    Int_t icdc=(Int_t)trbuff_.cmbt[i][7]-1;
    if( icdc >= 0 ){
      ct=(JSFCDCTrack*)fCDCTracks->UncheckedAt(icdc);
      lt->SetCDC(icdc, ct);
    }
  }

  return kTRUE;

}

// ---------------------------------------------------------------
JSFReadJIMBankBuf::JSFReadJIMBankBuf(const char *name, const char *title,
		 JSFSIMDST *module)
    : JSFSIMDSTBuf(name, title, module)
{  
  strcpy(fProduc,"JIM ");
}



