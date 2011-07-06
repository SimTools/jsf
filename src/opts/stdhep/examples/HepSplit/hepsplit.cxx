#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdlib.h>

#include "stdlun.h"
#include "stdhd.h"

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
  extern void stdflpyxsec_(int *ntries);
  extern void stdxend_(int *istream);
  extern void heplst_(int *mlst);
};

// =================================================================================
class HepSplit
{
  std::string  fInfile;
  std::string  fOutfilePrefix;
  int  fMaxRead;
  int  fNWperFile;
  int  fWriteBeginRecord;

  int  fNFile;
  int  fNReadTotal;
  int  fNWrite;
  int  fNWriteTotal;
  int  fInStream;
  int  fOutStream;

public:
  HepSplit();
  int  Arguments(int argc, char* argv[]);
  void PrintHelp();
  int  Run();

};

// =====================================================
HepSplit::HepSplit()
{
  heplun_.lnhwrt=0;
  heplun_.lnhrd=23;
  heplun_.lnhout=6;
  heplun_.lnhdcy=0;
  heplun_.lnhpdf=0;
  heplun_.lnhdmp=0;
  mcfio_init_();

  fInfile=std::string("input.stdhep");
  fOutfilePrefix=std::string("output");
  fMaxRead=0;
  fNWperFile=300;
  fWriteBeginRecord=1;

  fNFile=0;
  fNReadTotal=0;
  fNWrite=0;
  fNWriteTotal=0;
  fInStream=0;
  fOutStream=0;
}

//======================================================
int HepSplit::Arguments(int argc, char *argv[])
{
//  std::cout << " # of arguments=" << argc << std::endl;
  int ip=1;
  while( ip < argc ) {
    std::string a(argv[ip]);
    if( a == std::string("--infile") ) {
      fInfile=std::string(argv[ip+1]);
      ip++;
    }
    else if ( a == std::string("--outpref") ) {
      fOutfilePrefix=std::string(argv[ip+1]);
      ip++;
    }
    else if ( a == std::string("--maxread") ) {
      fMaxRead=atoi(argv[ip+1]);
      ip++;
    }
    else if ( a == std::string("--nw_per_file")) {
      fNWperFile=atoi(argv[ip+1]);
      ip++;
    }
    else if ( a == std::string("--write_begin_rec")) {
      fWriteBeginRecord=atoi(argv[ip+1]);
      ip++;
    }
    else if ( a == std::string("--help") ) {
      PrintHelp();
      return -1;
    }
    else {
      std::cout << "Invarid arguments: last arguments is " << argv[ip] << std::endl;
      PrintHelp();
      return -1 ;
    }
    ip++;
  }
  std::cout << "==== hepsplit input parameters ==================================" << std::endl;
  std::cout << "  Input file             : " << fInfile << std::endl;
  std::cout << "  Output File prefix     : " << fOutfilePrefix << std::endl;
  std::cout << "  Max readin events (0=to EOF)  : " << fMaxRead << std::endl;
  std::cout << "  Number of events per file     : " << fNWperFile << std::endl;
  std::cout << "  Output Begin Run Record to each file ? (0,1)=(No,Yes) : " << fWriteBeginRecord << std::endl;
  return 0;
}

// ==========================================================================
void HepSplit::PrintHelp()
{  
  std::cout << "HepSplit" << std::endl
            << "  Split an input stdhep file to more than one files. " << std::endl
            << "  Command format " << std::endl
            << "     hepsplit [options] " << std::endl
            << "  Options " << std::endl
            << "     --infile [infile]    : (D=input.stdhep) Input stdhep file name  " << std::endl
            << "     --outprefix [ofile]  : (D=output )    Prefix of output file. File names will be " <<std::endl
            << "                  output_NNN.stdhep, where NNN=0~999" << std::endl
            << "     --maxread   [N]      : (D=0) Number of read recors, <= 0 to the end " << std::endl
            << "     --nw_per_file [N]    : (D=300) Number of events in each output file." << std::endl
            << "     --write_begin_record : (D=1) Output begin record in each file or not. (0,1)=(No, Yes)" << std::endl;

}

//======================================================
int main(int argc, char* argv[])
{

  HepSplit hs;

  int iret=hs.Arguments(argc, argv);
  if( iret < 0 ) { return iret; }

  return hs.Run();

}

//======================================================
int HepSplit::Run()
{
  
  int ntries=fMaxRead;
  int iok=0;
  stdxrinit_(fInfile.c_str(), &ntries, &fInStream, &iok, fInfile.size());
  if( iok != 0 ) {
    std::cout << "Fatal error : failed to initialize read file." << fInfile << std::endl;
    return -1;
  }

  int flag=0;
  std::string outfile;
  while ( iok == 0 && ( fMaxRead ==0 || fNReadTotal < fMaxRead )) {
    stdxrd_(&flag, &fInStream, &iok);
    if( iok != 0 ) {
      std::cout << "Fatal error: read error. iok=" << iok << std::endl;
      return -1;
    }
    fNReadTotal++;
    fNWrite++;
    int lastflag=flag;
    if( fNWrite > fNWperFile || fOutStream == 0 ) { 
      if( fOutStream != 0 ) { 
        stdxend_(&fOutStream);
        fOutStream=0;
      }
      fNFile++;
      std::stringstream cbuff;
      cbuff << "_" << std::setw(3) << std::setfill('0') << std::right << fNFile << ".stdhep" << std::ends;
      outfile=fOutfilePrefix+cbuff.str();
      std::string  outtitle=std::string(stdhd1_.title);
      stdxwopen_(outfile.c_str(), outtitle.c_str(),&ntries, &fOutStream, &iok, outfile.size(), outtitle.size());
      if( iok != 0 ) {
        std::cout << "Fatal error: Unable to open output file " << outfile << std::endl;
        return -1;
      }
      std::cout << "Open " << outfile << std::endl;
      if( fWriteBeginRecord != 0 ) {
        int thisflag=100;
        lastflag=-100;
        std::cout << "Write header title: " << stdhd1_.title << std::endl;
        stdxwrt_(&thisflag, &fOutStream, &iok);
        if( iok != 0 ) {
          std::cout << "Fatal error: Failed to write begin run record to " << outfile << std::endl;
          return -1;
        }
      }
      fNWrite=0;
    }
    int outok=0;
    if( lastflag > 0 ) { stdxwrt_(&flag, &fOutStream, &outok); } // Avoid write two BRR
//    std::cout << " write  flag=" << flag << " outok=" << outok << " nwrite=" << fNWrite << std::endl;
    fNWrite++;
    fNWriteTotal++;
  }
  stdxend_(&fInStream);
  if( fOutStream != 0 ) { stdxend_(&fOutStream); }

  return 0;
}  
