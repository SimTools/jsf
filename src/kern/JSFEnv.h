//*LastUpdate:  jsf-1-14 29-January-2000  Akiya Miyamoto
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
#include <vector>

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
  JSFEnvRec():fName(""),fType(""),fValue(""),fLevel(kEnvGlobal),fHelp(""),fArgument(""){}
  JSFEnvRec(const char *n, const char *t, const char *v, EEnvLevel l,
            const char *help=NULL, const char *arg=NULL);
  void ChangeValue(const char *value, EEnvLevel lbl);
  void ChangeValue(const char *t, const char *v, EEnvLevel l,
            const char *help=NULL, const char *arg=NULL);

  const Char_t *EntryString();
  Char_t *GetHelpMessage();

  ClassDef(JSFEnvRec,1) // JSF Environment Record parameter

};


// ______________________________________________________________
class JSFEnv : public TEnv {
private:
  TOrdCollection *fDefined;  //! Env parameters used by the program.
  TOrdCollection *fObtained; // Env parameters obtained from a file.
  Char_t *fEnvFileName; // File name of environment parameter.
  Bool_t  fRecordDefault; //! Record default value when true.
public:
  JSFEnv():fDefined(0),fObtained(0),fEnvFileName(0){}
  JSFEnv(char *name);
  virtual ~JSFEnv();
  JSFEnv(const JSFEnv& env, bool localonly=kFALSE );

  void SetRecordDefault(bool flag){ fRecordDefault=flag; }
  inline Bool_t GetRecordDefault(){ return fRecordDefault; }
  void PrintDefined();

  TOrdCollection *GetObtained(){ return fObtained;}
  TOrdCollection *GetDefined(){ return fDefined;}
  void Add(const JSFEnv *env, bool replace=kFALSE);
  const char *GetValue(const char *name, const char *dflt);
  int GetValue(const char *name, int dflt);
  Double_t GetValue(const char *name, Double_t dflt);
  std::vector<int> GetIValue(const char *name, const char *dflt, const int n);
  std::vector<Double_t> GetDValue(const char *name, const char *dflt, const int n);
  void SetValue(const char *name, const char *value, 
		EEnvLevel lbl=kEnvChange, const char *t=0);
  JSFEnvRec *LookUp(const char *name);
  Char_t *GetEnvFileName(){ return fEnvFileName; }
  void GetArguments();
  void ReadFile(const char *fname, EEnvLevel l);
  Bool_t WriteFile(char* fn=NULL);

  ClassDef(JSFEnv,1) // JSF Environment parameter

};

#endif
