#include <iostream>
using namespace std;

//
// Example to analize 

Int_t plot2()
{
  JSFSteer *jsf=new JSFSteer();

  TFile *f=new TFile("anal.root");

  TTree *t=(TTree*)f->Get("A");

  TFile *fout=new TFile("plotdata.root","RECREATE");
  TNtuple *ntuple=new TNtuple("ntuple","zh analysis", "mz:mh:chisq:mm:thust:evis");

  JetEvent *event=new JetEvent();
  TBranch *br=t->GetBranch("Event");
  br->SetAddress(&event);

  Int_t nev=t->GetEntries();

  for(Int_t ev=0;ev<nev;ev++){
    br->GetEntry(ev);
    if( ev%100 == 1 ) {
      cout << "Event number is " << ev << endl;
    }
    if( event->NJ() != 4 ) continue;  // Skip, if Number of jets < 4

    // Search pair of best chisq.
    Double_t chimin=1.0E10;
    Int_t idp[6][4]={{0,1,2,3},{0,2,1,3},{0,3,1,2},
		     {2,3,0,1},{1,3,0,2},{1,2,0,3}};
    Int_t igood=0;
    Double_t mz=0.0;
    Double_t mh=0.0;
    Double_t mm=0.0;
    for(Int_t ip=0;ip<6;ip++){
      ANL4DVector j0(event->GetJet(idp[ip][0])->GetANL4D());
      ANL4DVector j1(event->GetJet(idp[ip][1])->GetANL4D());
      ANL4DVector j2(event->GetJet(idp[ip][2])->GetANL4D());
      ANL4DVector j3(event->GetJet(idp[ip][3])->GetANL4D());
      ANL4DVector zpair=j0+j1;
      ANL4DVector hpair=j2+j3;
      Double_t zmass=zpair.GetMass();

      ANL4DVector ecm(300.0, 0.0, 0.0, 0.0);
      ANL4DVector pmis=ecm-hpair;
      Double_t    missingmass=pmis.GetMass();
      
      Double_t    zmd=(zmass-91.1)/3.0;
      Double_t    mmd=(missingmass-91.1)/6.0;
      Double_t chisq=zmd*zmd+mmd*mmd;
      if( chisq < chimin ) {
	chimin=chisq;
	mz=zmass;
	mh=hpair.GetMass();
	mm=missingmass;
	igood=ip;
      }
    }

    ntuple->Fill(mz,mh, chimin, mm, event->Thrust(), event->E());

  }

  ntuple->Write();
  fout->Close();

}
