{
// Print ROOT version number
  Int_t ver, mver,plevel;
  sscanf(gROOT->GetVersion(),"%d.%d/%d",&ver,&mver,&plevel);
  printf("#define __ROOT_VERSION__ %d\n",ver);
  printf("#define __ROOT_MINORVERSION__ %d\n",mver);
  printf("#define __ROOT_PATCHLEVEL__ %d\n",plevel);
}
