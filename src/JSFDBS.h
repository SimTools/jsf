#ifndef __JSFDBS__
#define __JSFDBS__

//////////////////////////////////////////////////////////////////
//
//  JSFDBS
//
//  General class to manage data base information
//  
//////////////////////////////////////////////////////////////////


#include <TNamed.h>
#include <TFile.h>

class JSFDBS  : public TNamed {
protected:
  static TFile  *fFile; //! JSF common data base file.

  void OpenDBS();
public:
  JSFDBS(Int_t run=0);
  virtual   ~JSFDBS();
  
  Bool_t IsOpen();
  static void SetFile(TFile *file){ fFile=file; }
  virtual void CloseDBS(){ fFile->Close(); fFile=NULL; }
  virtual Char_t *SetKeyName(Int_t run);
  virtual void ReadDBS(Int_t run);
  virtual void WriteDBS(Int_t run);

  virtual void Print(){};

  ClassDef(JSFDBS,1) // JSF Common database class
};


#endif
