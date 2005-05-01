#include <iostream>
using namespace std;

//
// Simple script to plot histogram

Int_t plot()
{
  JSFSteer *jsf=new JSFSteer();

  TFile *f=new TFile("anal.root");

  //   A->Print();

  // We don need to do 
  // TTree *A=(TTree*)f->Get("A");

  TCanvas *c=new TCanvas("c","Histogram Canvas");
  c->Divide(2,2);

  c->cd(1) ; A->Draw("Thrust()");
  c->cd(2) ; A->Draw("MM()");
  c->cd(3) ; A->Draw("E()");

}
