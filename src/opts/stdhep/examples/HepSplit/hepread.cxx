#include <iostream>
#include <string>

#include "stdlun.h"

extern "C" {
  extern void mcfio_init_();
  extern void stdxropen_(const char *fname, int *nentries, 
                        int *istream,int *iok, int lfname);
  extern void stdxrinit_(const char *fname, int *nentries, 
                        int *istream,int *iok, int lfname);
  extern void stdxrd_(int *iflag, int *istream, int *iok);
  extern void stdxend_(int *istream);


  extern void stdxwinit_(const char *filename, const char *title, 
                         int *ntries, int *istream, int *iok,
                         int lfilename, int ltitle);
  extern void stdxwopen_(const char *filename, const char *title, 
                         int *ntries, int *istream, int *iok,
                         int lfilename, int ltitle);
  extern void stdxwrt_(int *ilbl, int *istream, int *lok);
  extern void jsf_mcfio_init_();
  extern void stdflpyxsec_(int *ntries);
  extern void stdxend_(int *istream);
  extern void heplst_(int *mlst);
};



int main()
{
  std::string infile("/home/ilc/miyamoto/soft/physsim2/genprod/data/p106013_01.stdhep");
  
  heplun_.lnhwrt=0;
  heplun_.lnhrd=23;
  heplun_.lnhout=6;
  heplun_.lnhdcy=0;
  heplun_.lnhpdf=0;
  heplun_.lnhdmp=0;

  mcfio_init_();

  int ntries=100;
  int instream=0;
  int iok=0;
  stdxrinit_(infile.c_str(), &ntries, &instream, &iok, infile.size());
  if( iok != 0 ) {
    std::cout << "Fatal error : failed to initialize read file." << std::endl;
    return -1;
  }

  int maxread=100;
  int nread=0;
  int flag=0;
  while( iok ==0 && nread < maxread ) {
    stdxrd_(&flag, &instream, &iok);
    nread++;
    std::cout << " nread=" << nread << " iok=2 " << iok << " flag=" << flag << std::endl;
  }
  stdxend_(&instream);
  
  return 0;
}  
