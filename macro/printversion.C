{
// Print ROOT version number
  Int_t ver, mver,plevel;
  sscanf(gROOT->GetVersion(),"%d.%d/%d",&ver,&mver,&plevel);
  printf("ROOT_VERSIONSTRING = %s \n",gROOT->GetVersion());
  printf("ROOT_VERSION = %d\n",ver);
  printf("ROOT_MINARVERSION = %d\n",mver);
  printf("ROOT_PATCHLEVEL= %d\n",plevel);
}
