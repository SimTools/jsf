// Read CCD Database file.


int readdbs()
{
  gROOT->Reset();

  jsf = new JSFSteer();

  printf(" going to get run1651 data\n");
  run1651 = new CCDRunIndex(1651);

  printf(" Reference run number is %d\n",run1651->GetRunNumber());

}
