//***************************************************************
//*  read.C
//*  An example script to read data created by sim.C script.
//*
//*  31-July-1999 Akiya Miyamoto Use dynamic loading on system other than 
//*                              ccjlc.         
//*$Id$
//***************************************************************

TFile *file;

Int_t read()
{
  gSystem->Load("libJSF.so");
  jsf = new JSFSteer();

  file=new TFile("jsf.root","READ");
  jsf->SetIOFiles();

  jsf->Initialize();

  Int_t maxevt=30;

  for(Int_t i=1;i<=maxevt;i++){
    if( !jsf->GetEvent(i) ) break;  // GetEvent method is to read i-th event.

    JSFQuickSim *sim=(JSFQuickSim*)jsf->FindModule("JSFQuickSim");
    JSFQuickSimBuf *sbuf=(JSFQuickSimBuf*)sim->EventBuf();
  
    JSFHadronizer *had=(JSFHadronizer*)jsf->FindModule("JSFHadronizer");
    FFbarSpring *spr=(FFbarSpring*)jsf->FindModule("FFbarSpring");

    printf("\n\n");
    printf("read Run %d Event %d\n",jsf->GetRunNumber(), jsf->GetEventNumber());
    printf(" Number of Combined tracks =%d\n",sbuf->GetNTracks());
    printf(" Number of CDC Tracks      =%d\n",sbuf->GetNCDCTracks());
    printf(" Number of VTX Hits        =%d\n",sbuf->GetNVTXHits());

    TClonesArray *cmbt=sbuf->GetTracks();
    for(Int_t j=0;j<sbuf->GetNTracks();j++){
      JSFLTKCLTrack *lt=(JSFLTKCLTrack*)cmbt->UncheckedAt(j);
      JSFCDCTrack *t=lt->GetCDC();
//
//  In some case, two entries in the JSFLTKCLClass are connected to 
//  the same JSFCDCTrack class.  Because EMC cluster energy may still
//  remain after CDC track energy subtraction and such clusters are 
//  registered as gamma in mixed EMC cluster.  The JSFCDCTrack class 
//  connected by such JSFLTKCLclass may have completely different 
//  track momentum than those in JSFLTKCLclass
//        Akiya Miyamoto 13-Feburary-1999
//        
      printf(" Track number %d  Track type=%d ",j,lt->GetType());
      printf(" %s ",lt->GetTypeName());
      printf(" Charge=%d\n",t->GetCharge());
      printf(" px,py,pz=%g %g %g\n",lt->GetPx(),lt->GetPy(),lt->GetPz());
      printf(" px,py,pz=%g %g %g\n",t->GetPx(),t->GetPy(),t->GetPz());
      printf(" nVTX=%d\n",t->GetNVTX());
      for(Int_t k=0;k<t->GetNVTX();k++){
	JSFVTXHit *vh=(JSFVTXHit*)t->GetVTXHit(k);
	printf(" k=%d r=%g phi,z=%g,%glayer=%d \n",k,vh->GetR(),
	       vh->GetPhi(), vh->GetZ(), vh->GetLayerNumber());
      }
    }
    
  }

  jsf->Terminate();


}



