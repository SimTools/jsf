//  Macro to run Pythia generator

int rtest()
{
  gROOT->Reset();
  
//  TFile fout("jsf.root","RECREATE"); //  Output file.

  TFile file("/:/w/30/miyamoto/r1651.test.root");  // Input file
  jsf    = new JSFSteer();
//  dark = new CCDCorrectDark();
//  dark->SetFile(&fout);
//  rawhist = new CCDRawHist();
//  cls = new CCD2x2Cluster();
//  cls->SetFile(&fout);


  Int_t maxevt=jsf->Env()->GetValue("JSFSteer.Nevent",10);  

  TList *list=(file.GetListOfKeys());
  TKey  *key;
  key=(TKey*)list->Last();
    printf(" Key name is %s ",key->GetName());
    printf(" cycle is =%d",key->GetCycle());
    printf("\n");  

  while( key=(TKey*)list->Before(key) ) {
    printf(" Key name is %s ",key->GetName());
    printf(" cycle is =%d",key->GetCycle());
    printf("\n");  

  }

/*
  TIter nextkey(file.GetListOfKeys());

  TKey *key;
  while(key=(TKey*)nextkey()) {
    printf(" Key name is %s ",key->GetName());
    printf(" cycle is =%d",key->GetCycle());
    printf(" size is =%d",key->Sizeof());
    printf("\n");
//    key->Print();
//    key->ls();
  }
*/

//  file->Map();
}









