//
//  To run this program, do
//  $ root bases.C
//  
//  This macro will calculate the integral according to the function 
//  given by FFbaeBases.
// 

Int_t bases()
{ 
  //  Load library 

  // Allocate a file to output bases result.
  file = new TFile("bases.root","RECREATE");
  jsf = new JSFSteer();

  bs  = new FFbarBases();
  bs->Bases();
  bs->Bh_plot();
  printf(" Scalls is %d\n",bs->GetScalls());
  bs->Write();
//  file->Write();
 
}

