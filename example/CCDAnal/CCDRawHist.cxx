//*LastUpdate:  v0.1.01 12-October-1998 by Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  12-October-1998

//////////////////////////////////////////////////////////////////
//
//  CCDRawHist
//  
//  Make a CCD Raw data histogram
//
//////////////////////////////////////////////////////////////////


#include "JSFSteer.h"
#include "CCDRawData.h"
#include "CCDCorrectDark.h"
#include "CCDRawHist.h"

ClassImp(CCDRawHist)


//___________________________________________________________________________
CCDRawHist::CCDRawHist(const Char_t *name, const Char_t *title)
	: JSFModule(name, title)
{
    sscanf(gJSF->Env()->GetValue("CCDRawHist.hPh0","300.0, -50.0, 250.0"),
	"%g,%g,%g",&hdef1[0].nbin, &hdef1[0].xmin, &hdef1[0].xmax);
    sscanf(gJSF->Env()->GetValue("CCDRawHist.hPh1","300.0, -50.0, 250.0"),
	"%g,%g,%g",&hdef1[1].nbin, &hdef1[1].xmin, &hdef1[1].xmax);
    sscanf(gJSF->Env()->GetValue("CCDRawHist.hPh2","300.0, -50.0, 250.0"),
	"%g,%g,%g",&hdef1[2].nbin, &hdef1[2].xmin, &hdef1[2].xmax);
    sscanf(gJSF->Env()->GetValue("CCDRawHist.hPh3","300.0, -50.0, 250.0"),
	"%g,%g,%g",&hdef1[3].nbin, &hdef1[3].xmin, &hdef1[3].xmax);

}

//___________________________________________________________________________
Bool_t CCDRawHist::Initialize()
{
    TDirectory *last=gDirectory;
    gFile->cd("/");

    hPh0=new TH1F("hPh0","CCD0 pulse height ",
		hdef1[0].nbin,hdef1[0].xmin,hdef1[0].xmax);
    hPh1=new TH1F("hPh1","CCD1 pulse height ",
		hdef1[1].nbin,hdef1[1].xmin,hdef1[1].xmax);
    hPh2=new TH1F("hPh2","CCD2 pulse height ",
		hdef1[2].nbin,hdef1[2].xmin,hdef1[2].xmax);
    hPh3=new TH1F("hPh3","CCD3 pulse height ",
		hdef1[3].nbin,hdef1[3].xmin,hdef1[3].xmax);

    hc1=new TCanvas("hc1","Canvas 1",20, 20, 800, 800);
    hc1->Divide(2,2);
    last->cd();
    return 0;
}


//___________________________________________________________________________
Bool_t CCDRawHist::Process(Int_t ev)
{
// Read one event record and 
// Loop until obtain valid event record.

    TDirectory *last=gDirectory;
    gFile->cd("/");
    CCDCorrectDark *dark=(CCDCorrectDark*)gJSF->FindModule("CCDCorrectDark");
    CCDRawData *raw=(CCDRawData*)gJSF->FindModule("CCDRawData");
    CCDEnvironmentBuf *env=raw->GetEnvironment();
    CCDCorrectedADCBuf *buf=(CCDCorrectedADCBuf*)dark->EventBuf();
    CCDXYMinMax reg;
    Int_t nccd=env->GetNCCD();
	printf(" nccd = %d\n",nccd);
    for(Int_t ic=0;ic<nccd;ic++){
      Int_t nx=env->GetNx(ic);
      Float_t *adc=buf->ADC(ic);
      env->GetSensitiveRegion(ic, reg);

	printf(" .. ic=%d  x=(%d %d), y=(%d %d)\n",
	 ic, reg.x.min, reg.x.max, reg.y.min, reg.y.max);

      for(Int_t iy=reg.y.min;iy<reg.y.max;iy++){
        for(Int_t ix=reg.x.min;ix<reg.x.max;ix++){
	  if( ic == 0 ) hPh0->Fill(adc[nx*iy+ix]);
	  else if( ic == 1 ) hPh1->Fill(adc[nx*iy+ix]);
	  else if( ic == 2 ) hPh2->Fill(adc[nx*iy+ix]);
	  else if( ic == 3 ) hPh3->Fill(adc[nx*iy+ix]);
        }
      }

      hc1->cd(ic+1);
      gPad->SetLogy();
      if( ic == 0 ) hPh0->Draw(); 
      else if( ic == 1 ) hPh1->Draw();
      else if( ic == 2 ) hPh2->Draw();
      else if( ic == 3 ) hPh3->Draw();
      hc1->Update();


    }
    last->cd();
  return kTRUE;
}

//___________________________________________________________________________
void CCDRawHist::Draw()
{
    TDirectory *last=gDirectory;
    gFile->cd("/");
    CCDRawData *ccdraw=(CCDRawData*)gJSF->FindModule("CCDRawData");
    CCDEnvironmentBuf *env=ccdraw->GetEnvironment();

    for(Int_t ic=0;ic<env->GetNCCD();ic++){
      hc1->cd(ic+1);
      if( ic == 0 ) hPh0->Draw();
      else if( ic == 1 ) hPh1->Draw();
      else if( ic == 2 ) hPh2->Draw();
      else if( ic == 3 ) hPh3->Draw();
      hc1->Update();
    }
    last->cd();
}
