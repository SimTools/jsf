//*LastUpdate:  jsf-1-12 31-July-1999  Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  31-July-1999 Akiya Miyamoto

//////////////////////////////////////////////////////////////////
//
//  JSFEnv
//
//  JSF Environment parameter
//  
//$Id$
//////////////////////////////////////////////////////////////////

#ifndef __JSFEnv__
#define __JSFEnv__

#include <TEnv.h>
#include <TOrdCollection.h>
#include <TString.h>

// ______________________________________________________________
class JSFEnvRec: public TObject {
friend class JSFEnv;
friend class JSFEnvGUIFrame;
protected:
  TString  fName;
  TString  fType;
  TString  fValue;
  EEnvLevel fLevel;
  TString  fHelp;
  TString  fArgument;

public:
  JSFEnvRec(){}
  JSFEnvRec(const char *n, const char *t, const char *v, EEnvLevel l,
            const char *help=NULL, const char *arg=NULL);
  void ChangeValue(const char *value, EEnvLevel lbl);
  void ChangeValue(const char *t, const char *v, EEnvLevel l,
            const char *help=NULL, const char *arg=NULL);

  const Char_t *EntryString();

  // Int_t Compare(TObject *er){ 
  //   return strcmp(fName.Data(),((JSFEnvRec*)er)->fName.Data()); }
  // Bool_t IsSortable(){ return kTRUE; }

  //  ClassDef(JSFEnvRec,1) // JSF Environment parameter

  ClassDef(JSFEnvRec,1) // JSF Environment Record parameter

};


// ______________________________________________________________
class JSFEnv : public TEnv {
private:
  TOrdCollection *fDefined;  //! Env parameters used by the program.
  TOrdCollection *fObtained; //! Env parameters obtained from a file.
  Char_t *fEnvFileName; //! File name of environment parameter.
public:
  JSFEnv(char *name);
  virtual ~JSFEnv();

  TOrdCollection *GetObtained(){ return fObtained;}
  TOrdCollection *GetDefined(){ return fDefined;}
  const char *GetValue(const char *name, const char *dflt);
  int GetValue(const char *name, int dflt);
  void SetValue(const char *name, const char *value, 
		EEnvLevel lbl=kEnvChange, const char *t=0);
  JSFEnvRec *Lookup(const char *name);
  Char_t *GetEnvFileName(){ return fEnvFileName; }
  void GetArguments();
  void ReadFile(const char *fname, EEnvLevel l);
  Bool_t WriteFile(char* fn=NULL);

  ClassDef(JSFEnv,1) // JSF Environment parameter

};

#endif
