#ifndef __JSFReadGZippedFile__
#define __JSFReadGZippedFile__

#include <iostream>

#include "zlib.h"

using namespace std;

#define BUFLEN 16384

class JSFReadGZippedFile 
{
public:
  enum EStatus {kClose, kOpen, kEOF, kError};
protected:
  gzFile fIn;
  char  fBuf[BUFLEN];
  char  fOut[BUFLEN];
  int   fLen;      // length of last buffer read
  EStatus fStatus;
  int   fRp;       // position of last read character in fBuf 
public:
  JSFReadGZippedFile(const char *file=0);
  virtual ~JSFReadGZippedFile();

  bool Open(const char *file);
  bool Close();

  int GetLine(char *str, int maxlen);
  EStatus GetStatus(){ return fStatus; }
  bool IsEOF(){ 
    if ( fStatus == kEOF ) { return true; }
    else { return false; }
  }
  
};

#endif
