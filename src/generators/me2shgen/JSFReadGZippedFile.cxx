
#include "JSFReadGZippedFile.h"


// -----------------------------------------------------------
JSFReadGZippedFile::JSFReadGZippedFile(const char *file)
{
   fStatus=kClose;
   fLen=0;
   fRp=0;

   if( file ) { Open(file); }

}

// -----------------------------------------------------------
JSFReadGZippedFile::~JSFReadGZippedFile()
{
  if( fStatus == kOpen ) { Close(); }

}

// -----------------------------------------------------------
bool JSFReadGZippedFile::Open(const char *file)
{
  fIn = gzopen(file, "rb");
  if( fIn == NULL ) {
    cerr << "JSFReadGZippedFile::Open  .. Can't gzopen " << file << endl;
    fStatus=kError;
    return false;
  }
  fLen=0;
  fRp=0;
  fStatus=kOpen;
  return true;
}

// -----------------------------------------------------------
bool JSFReadGZippedFile::Close()
{
  if (gzclose(fIn) != Z_OK) {
    cerr << "JSFReadGZippedFile::Close  Failed to close " << fIn << endl;
    fStatus=kError;
    return false;
  }
  fStatus=kClose;
  return true;
}

// -----------------------------------------------------------
int JSFReadGZippedFile::GetLine(char *out, int maxlen)
{
  int i=0;
  for(;;){
    if( fLen > 0 ) {
       while(fRp<fLen) {
	if( fBuf[fRp] == '\n' ) {
	  out[i]='\0';
	  fRp++;
	  return i;
	}
	out[i]=fBuf[fRp];
	i++;
	fRp++;
	if( i >= maxlen ) {
	  fStatus=kError;
	  return i;
	}
      }
    }
    fLen = gzread( fIn, fBuf, sizeof(fBuf));
     if( fLen <= 0 ) {
      fStatus=kEOF;
      out[i]='\0';
      return fLen;
    }
     fRp=0;
  }
  return -1;
}


  
