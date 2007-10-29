// 
// Generate beam spectrum using functions for Bases/Spring
//
//$Id$

//----------------------------------------------------
Int_t genbeam2(Int_t flag=0)
{
  gSystem->Load("libJSFBeamGeneration.so");

  TString paraname;
  TString bsdata;
  if( flag == 0 ) {
    paraname=TString("500_nominal");
    bsdata=TString("500_nominal.root");
  }
  else {     
    std::cerr << "Enter parameter name: ";
    std::cin  >> paraname ;
    std::cerr << "Parameter name is " << paraname << std::endl;
    std::cerr << "Enter parameter file name: ";
    std::cin >> bsdata ;
    std::cerr << "BSGEN data file is " << bsdata << std::endl;
  }

  TFile *fbm=new TFile(bsdata.Data());
  if( !fbm ) {
      printf(" Unable to open a file for beam strahlung\n");
      return 0;
  }

  JSFBeamGenerationCain *bm=(JSFBeamGenerationCain*)fbm->Get(paraname.Data());

  // bm->SetIBParameters(0.005,JSFBeamGenerationCain::kUniform);
  bm->SetIBParameters(0.0);
  bm->MakeBSMap();
  bm->Print();
  Double_t ewidth=bm->GetIBWidth();
  Double_t enominal=bm->GetNominalEnergy();

  printf(" Nominal Energy=%g ",bm->GetNominalEnergy());
  printf(" beam width=%g\n",bm->GetIBWidth());


  Int_t maxevt=200000;

  TFile *fplot=new TFile("genbeam2.root","RECREATE");
  fplot->cd();
  TCanvas *clum=new TCanvas("clum","clum");
  TH1D *hlum=new TH1D("hlum","Luminosity",140,0.90,1.04);
  TH1D *hlumfull=new TH1D("hlumfull","Luminosity",104,0.0,1.04);
  TH2D *he1e2=new TH2D("he1e2","E+ vs E-",104,0.0,1.04,104,0.0,1.04);
  TH1D *hwght=new TH1D("hwght","Weight",110,0.0,1.1);

  Double_t eminus, eplus;
  for(Int_t i=0;i<maxevt;i++){
    Double_t ribm=bm->GetRndm();
    Double_t ribp=bm->GetRndm();
    Double_t rbsm=bm->GetRndm();
    Double_t rbsp=bm->GetRndm();

    Double_t wgt=bm->GetWeight(ribm, ribp, rbsm, rbsp, eminus, eplus);

    hwght->Fill(wgt);
      Double_t x1=eminus;
      Double_t x2=eplus;
      Double_t rs=TMath::Sqrt(x1*x2);
      Double_t pz=x1-x2;
      //    if( wgt > 0.0 ) {
      
      hlum->Fill(rs, wgt);
      hlumfull->Fill(rs, wgt);
      he1e2->Fill(x1,x2, wgt);
      if( i%50000 == 1 ) {
	clum->cd(); hlumfull->Draw(); clum->Update();
      }
      //    }
  }
  //  clum->cd();   hlum->Draw();
  clum->cd();   hlumfull->Draw();
  
  fplot->Write();
}

