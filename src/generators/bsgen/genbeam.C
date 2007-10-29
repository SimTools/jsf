// 
// A sample script to generate beam strtulung spectrum
// To run program, do
//   $ jsf genbeam.C
// In this case, the default parameter ( 500 GeV, nominal parameters )
// are generated.  To generate spectrum other than the default, do
//   $ jsf "genbeam.C(1)"
// In this case, enter the BS parameter name ( such as "500_nominal" ) 
// and the BS parameter file name ( such as "500_nominal.root" )
// should be entered in response to the prompt.
// 
//$Id$

//----------------------------------------------------
Int_t genbeam(Int_t flag=0)
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
  TFile *fplot=new TFile("genbeam.root","RECREATE");

  JSFBeamGenerationCain *bm=(JSFBeamGenerationCain*)fbm->Get(paraname.Data());
  if ( !bm ) {
    std::cerr << "BSGEN data for " << paraname << " does not exist in " << bsdata << std::endl;
    return ;
  }

  // bm->SetIBParameters(0.005,JSFBeamGenerationCain::kUniform);
  bm->SetIBParameters(0.0);
  bm->MakeBSMap();
  bm->Print();
  Double_t ewidth=bm->GetIBWidth();
  Double_t enominal=bm->GetNominalEnergy();

  printf(" Nominal Energy=%g ",bm->GetNominalEnergy());
  printf(" beam width=%g\n",bm->GetIBWidth());


  Int_t maxevt=200000;
  
  fplot->cd();
  TCanvas *clum=new TCanvas("clum","clum");
  TH1D *hlum=new TH1D("hlum","Luminosity",140,0.90,1.04);
  TH1D *hlumfull=new TH1D("hlumfull","Luminosity",104,0.0,1.04);
  TH2D *he1e2=new TH2D("he1e2","E+ vs E-",104,0.0,1.04,104,0.0,1.04);

  Double_t eminus, eplus;
  for(Int_t i=0;i<maxevt;i++){
    bm->Generate(eminus, eplus);

    Double_t x1=eminus/bm->GetNominalEnergy();
    Double_t x2=eplus/bm->GetNominalEnergy();
    Double_t rs=TMath::Sqrt(x1*x2);
    Double_t pz=x1-x2;

    if( x1 <= 1.0E-5 || x2 <= 1.0E-5 ) { continue ; }

    hlum->Fill(rs);
    hlumfull->Fill(rs);
    he1e2->Fill(x1,x2);
    if( i%50000 == 1 ) {
      //	clum->cd(); hlum->Draw(); clum->Update();
	clum->cd(); hlumfull->Draw(); clum->Update();
    }
  }

  //  clum->cd();   hlum->Draw();
  clum->cd();   hlumfull->Draw();
  
  fplot->Write();
}

