TFile **f=0;
TH1F **hsum=0;

Int_t sumplot()
{
  //  gROOT->Reset();
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
   gStyle->SetFillColor(10);
   gStyle->SetFillStyle(0);
   gStyle->SetFrameFillColor(10);
   gStyle->SetFrameFillStyle(0);

  f=new void[6];
  hsum=new void[6];

  Float_t ymax=30.0;
  Float_t ymax2=20.0;
  Int_t lastflv=1;
  Char_t *pref="test.";


  Char_t *flvn[6]={"xx.","uu.","dd.","ss.","cc.","bb."};
  for(Int_t i=5;i>=lastflv;i--){
    Char_t fn[64];
    sprintf(fn,"%s%sroot",pref,flvn[i]);
    f[i]=new TFile(fn);
  }

  TCanvas *c2=new TCanvas();
  c2->SetFillColor(10);
  c2->SetFillStyle(0);
  c2->SetFrameFillColor(10);
  c2->SetFrameFillStyle(0);


  TH1F *href=new TH1F("href","reference",100,0,6.0);
  href->GetXaxis()->SetTitle("msptm(GeV)");
  href->SetMaximum(ymax); href->SetMinimum(0.0);  href->Draw();

  
  Char_t *flvnam[6]={"00","dd","uu","ss","cc","bb"};

  Float_t evnts[6][3];
  Char_t fulcut[256];

  Char_t ntcmd[50];
  Char_t hname[50];
  Int_t ic=5;
  for(Int_t nf=5;nf>=lastflv;nf--){
    printf(" Reading file %s\n",f[nf]->GetName());
    f[nf]->cd();
    
    sprintf(hname,"h%s%d",flvnam[nf],ic);
    //    sprintf(ntcmd,"msptm>>%s",hname);
    sprintf(ntcmd,"msptm>>%s",hname);
    sprintf(fulcut,"dl0>0.03&&(!(msec<0.515&&nsec==2))");
    //    sprintf(fulcut,"dl0>0.03&&(!((msec>0.485&&msec<0.515)&&nsec==2))");
    //    sprintf(fulcut,"dl0>0.03&&msec>0.555&&nsec==2");
    
    Int_t nread=ntuple->GetEntries();
    if( nf==4 || nf==2 ) nread*=0.790;

    ntuple->Draw(ntcmd,fulcut,"same");
    TH1F *ht=(TH1F*)gROOT->FindObject(hname);
    hsum[nf]=new TH1F(*ht);
    SetProperties(nf, ht,2);
    //    c2->Update();
    
  } // End of flavour loop


  //  Draw sum up plot
  //  c2->cd(2);
  TH1F *href2=new TH1F(*href);
  href2->SetMaximum(ymax2);
  href2->SetFillColor(10);
  href2->SetFillStyle(0);
  href2->Draw();
  
  Char_t hname[64];
  for(Int_t nf=lastflv+1;nf<=5;nf++){
    hsum[nf]->Add(hsum[nf-1]);
  }

  for(Int_t nf=5;nf>=lastflv;nf--){
    SetProperties(nf, hsum[nf]);
    hsum[nf]->Draw("same");
  }
  
  Int_t ico[6]={3, 8, 9, 38, 30, 50};

  Char_t *com[30]={" ","u#bar{u}","d#bar{d}",
		       "s#bar{s}","c#bar{c}","b#bar{b}"};

  for(Int_t i=5;i>0;i--){
    Int_t ypos=500+80*i;
    TMarker *marker = new TMarker(3.3,ypos,21);
     marker->SetMarkerColor(ico[i]);
     marker->SetMarkerStyle(21);
     marker->SetMarkerSize(1.7);
     marker->Draw();
     TLatex *tex = new TLatex(3.8,ypos,&com[i][0]);
     tex->SetTextColor(ico[i]);
     tex->SetTextSize(0.05);
     tex->SetLineWidth(2);
     tex->SetTextAlign(22);
     tex->Draw();
  }
  c2->Modified();
  c2->cd();

      
}

//___________________________________________
void SetProperties(Int_t nf, TH1 *ht, Int_t itype=1)
{

  //  Int_t ic[6]={3, 24, 40, 31, 47,33};
  Int_t ic[6]={3, 8, 9, 38, 30, 50};

  switch (itype) {
    case 1:  ht->SetFillColor( ic[nf] ); break;
    case 2:  
      ht->SetLineWidth(5);
      ht->SetLineColor( ic[nf] ); break;
  }

}
