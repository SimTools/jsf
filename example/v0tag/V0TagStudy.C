//****************************************************
//*
//*  This file is in V0TagStudy.C
//*  To run this program, do
//*     $ jsf gui.C
//*  Input data of this program is k0evt.dat, which is
//*  created by the program, k0evt.F
//*  Other settings will be found in jsf.conf.
//*  See also Readme file in this directory.
//*
//*  results of this script will be found in 
//*  http://www-jlc.kek.jp/subg/offl/jsf/doc/vertexing/index.html

//*(Require)
//*  PythiaUtilities.C is reauired, which is in 
//*  $JSFROOT/macro dirctory.
//*
//*(Author)
//*  29-Feburary-2000 Akiya Miyamoto
//*
//*$ID$
//*
//****************************************************



TH1F *hMomB, *hMomC, *hMomS;
TH1F *hDecayLB, *hDecayLC, *hDecayLS;
TH1F *hConfl, *hNtry, *hV0Qual, *hDist, *hK0mass;

//_________________________________________________________
void UserInitialize()
{
   GenInfoInitialize();
}

//_________________________________________________________
void UserAnalysis()
{

  GenInfoAnalysis();
  V0Tagger();
}

//_________________________________________________________
void GenInfoInitialize()
{
  //  This function is called at the begining of the job or when
  //  "reset hist" action is selected in the gui menu.
  //  This is used to define/reset histograms.

  gROOT->LoadMacro("PythiaUtilities.C");

  hMomB=new TH1F("hMomB","Momentum of B particle(GeV)",150,0.0,150.0);
  hDecayLB=new TH1F("hDecayLB","Decay Length of B particle(cm)",100,0.0,1.0);
  hMomC=new TH1F("hMomC","Momentum of C particle(GeV)",150,0.0,150.0);
  hDecayLC=new TH1F("hDecayLC","Decay Length of C particle(cm)",100,0.0,1.0);
  hMomS=new TH1F("hMomS","Momentum of S particle(GeV)",150,0.0,150.0);
  hDecayLS=new TH1F("hDecayLS","Decay Length of S particle(cm)",100,0.0,50.0);

  hConfl=new TH1F("hConfl","Confidence level of Geo Fit",100,0.0,1.0);
  hNtry=new TH1F("hNtry","Number of try before conversion",50,0.0,50.0);
  hV0Qual=new TH1F("hV0Qual","V0 Quality",100,0.0,5.0);
  hDist=new TH1F("hDist","Distance between generated VTX - fitted vtx",
		 100, 0.0, 10.0);
  hK0mass=new TH1F("hK0mass","K0s mass",100,0.45,0.55);

}

//_________________________________________________________
TVector3 GetGeneratedVertex()
{
  // Returns vertex point of generated particle,
  // Using the first non-zero vertex particle.


  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();
  Int_t ngen=sdb->GetNGeneratorParticles();
  TClonesArray *gen=(TClonesArray*)sdb->GetGeneratorParticles();
  Int_t i;
  for(i=0;i<ngen;i++){
    JSFGeneratorParticle *pt=(JSFGeneratorParticle*)gen->UncheckedAt(i);
    Int_t im=pt->GetMother();
    if( im <= 0 ) continue;
    JSFGeneratorParticle *ptm=(JSFGeneratorParticle*)gen->UncheckedAt(im-1);
    if( ptm->GetDecayLength() == 0.0 ) continue;
    return pt->GetXV3();
  }

}

//_________________________________________________________
void GenInfoAnalysis()
{
  // This function is called when the processing of one event is completed.
  // Any data processing of the event can be performed in this function.
  //

  
  if( jsf->GetEventNumber()%100 == 0 ) {
    printf(" Event #%d\n",jsf->GetEventNumber());
  }

  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();
  Int_t ngen=sdb->GetNGeneratorParticles();
  TClonesArray *gen=(TClonesArray*)sdb->GetGeneratorParticles();

  const Int_t maxp=20;
  JSFGeneratorParticle *p[maxp];
  // Save pointers to documentation line particles ( Mother <= 0 )
  Int_t n=0;
  Int_t i;
  Int_t ndau[maxp]={0};

  for(i=0;i<ngen;i++){
    JSFGeneratorParticle *pt=(JSFGeneratorParticle*)gen->UncheckedAt(i);
    if(pt->GetMother()<0) continue;
    if( pt->GetDecayLength() == 0.0 ) continue;

    Int_t ida=TMath::Abs(pt->GetID());
    Int_t ityp=0;
    if( ida > 1000 ) ityp=ida/1000;
    else ityp=ida/100;
    switch(ityp) {
      case 5: hMomB->Fill(pt->GetE());
	       hDecayLB->Fill(pt->GetDecayLength());  
	       break;
      case 4: hMomC->Fill(pt->GetE());
	       hDecayLC->Fill(pt->GetDecayLength());  
	       break;
      case 3: hMomS->Fill(pt->GetE());
	       hDecayLS->Fill(pt->GetDecayLength());  
	       break;
    }
  }
}

//_________________________________________________________
void V0Tagger()
{
  // Find V0, then make a geometrically constrained fit.

  JSFQuickSimParam *parm=(JSFQuickSimParam*)simdst->Param();
  Float_t bf=parm->GetBField();
  
  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();
  Int_t ncdc=sdb->GetNCDCTracks();
  if( ncdc < 2 ) return;
  TClonesArray *cdc=(TClonesArray*)sdb->GetCDCTracks();

  // Initialize vertexing class for two tracks vertexing.
  JSFVertexing vtx(2); 

  TVector3 gvtx=GetGeneratedVertex();


  // List generated particle information

#if debug
  Int_t ngen=sdb->GetNGeneratorParticles();
  TClonesArray *gen=(TClonesArray*)sdb->GetGeneratorParticles();
  Int_t i;
  for(i=0;i<ngen;i++){
    JSFGeneratorParticle *pt=(JSFGeneratorParticle*)gen->UncheckedAt(i);
    pt->ls();
  }
#endif

  Int_t i,j;
  
  TClonesArray fitbuf("JSFHelicalTrack",10); 
  
  for(i=0;i<ncdc;i++){
    JSFCDCTrack *ct0=(JSFCDCTrack*)cdc->UncheckedAt(i);
    JSFHelicalTrack ht0=ct0->GetHelicalTrack();
    ht0.SetBfield(bf);
    vtx.SetTrack(i, ht0);
    for(j=i+1;j<ncdc;j++){
      JSFCDCTrack *ct1=(JSFCDCTrack*)cdc->UncheckedAt(j);
      JSFHelicalTrack ht1=ct1->GetHelicalTrack();
      ht1.SetBfield(bf);
      vtx.SetTrack(j,ht1);
      Double_t qual=vtx.FindV0();
      if( vtx.GetQuality() > 10.0 ) continue;

      hV0Qual->Fill(vtx.GetQuality());
      TVector3 v(0.0,0.0,0.0);
      vtx.GetVertex(v);

      // We put candidate helical track parameter into
      // buffer for Geometrical Constraint fit.
      new( fitbuf[0] ) JSFHelicalTrack(ht0);
      new( fitbuf[1] ) JSFHelicalTrack(ht1);

      JSFGeoCFit fit(2, &fitbuf, v);

      fit.Fit();

      //      JSFDMatrix ans(fit.GetParameter()); ( For Root Version < 3.0 )
      TMatrixD  ans(fit.GetParameter());

#if debug
      printf(" Chisq=%g",fit.GetChisq());
      printf(" ConfidenceLevel=%g\n",fit.GetConfidenceLevel());
      printf(" Number of Trial is %d\n",fit.GetNtry());
      printf(" fitted vertex is (%g,%g,%g)\n",ans(0,0), ans(1,0),ans(2,0));
#endif

      hConfl->Fill((Float_t)fit.GetConfidenceLevel());
      hNtry->Fill((Float_t)fit.GetNtry());
      TVector3 dist(ans(0,0)-gvtx.X(), ans(1,0)-gvtx.Y(), ans(2,0)-gvtx.Z());
      hDist->Fill(dist.Mag());

      Double_t pt1=1.0/TMath::Abs(ans(4,0));
      Double_t phi01=ans(3,0);
      Double_t tnl1 =ans(5,0);
      Double_t psq1 = pt1*pt1*(1.0+tnl1*tnl1);
      ANL4DVector pi1(TMath::Sqrt(psq1+0.13957*0.13957),
		      -pt1*TMath::Sin(phi01), pt1*TMath::Cos(phi01),
		      pt1*tnl1);
      Double_t pt2=1.0/TMath::Abs(ans(7,0));
      Double_t phi02=ans(6,0);
      Double_t tnl2 =ans(8,0);
      Double_t psq2 = pt2*pt2*(1.0+tnl2*tnl2);
      Double_t e2   = TMath::Sqrt(psq2+0.13957*0.13957);
      ANL4DVector pi2(e2,-pt2*TMath::Sin(phi02), pt2*TMath::Cos(phi02),
		      pt2*tnl2);
      ANL4DVector pk0=pi1+pi2;
#if debug
      printf(" CL=%g\n",fit.GetConfidenceLevel());
      printf(" Gen Vertex=%g %g %g\n",gvtx.X(),gvtx.Y(),gvtx.Z());
      printf(" Fitted Vertex is %g %g %g\n",ans(0,0),ans(1,0),ans(2,0));
      printf(" dist=%g\n",dist.Mag());
      pi1.DebugPrint();
      pi2.DebugPrint();
      pk0.DebugPrint();
      printf(" Mass of K0 is %g\n",pk0.GetMass());
#endif      
      hK0mass->Fill(pk0.GetMass());

    }
  }


}


//_________________________________________________________
void DrawHist()
{
  //  This function is called to draw histograms during the interactive 
  //  session.  Thus you can see the accumulation of the histogram
  //  interactively.  
}




