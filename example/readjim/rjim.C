// ************************************************************
// rjim.C
//  
// Read a JIM's bnk
// 
// ************************************************************

//*************************************************
//* Main part to generate qqbar event
//*************************************************
int rjim()
{
  //  gROOT->Reset();
  file = new TFile("jsfjim.root","RECREATE");  // Output file

  jsf   = new JSFSteer();
  rjim  = new JSFReadJIMBank();
  rjim->SetDataFileName("jetset_qq_fast.bnk");  
  jsf->Initialize();

  Int_t maxevt=200;

  jsf->BeginRun(1);              // Set run number to 1.
  for(Int_t ev=1;ev<=maxevt;ev++){
    if( !jsf->Process(ev)) break ;

    //***********
    rjim->Dump();
    JSFQuickSimParam *par=(JSFQuickSimParam*)rjim->Param();
    printf(" rjim=%x par=%x\n",(Int_t)rjim, (Int_t)par);
    printf("# VTX layer is %d\n",par->GetVTXNLayer());

    jsf->FillTree();
    jsf->Clear();
  }

  jsf->Terminate();
  file->Write();

}


