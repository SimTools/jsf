//////////////////////////////////////////////////////////////////
//
//  JSFDialogMessage
//
//  Standalone class to define dialog message
//
//$Id$
//
//////////////////////////////////////////////////////////////////

#ifndef __JSFDialogMessage__
#define __JSFDialogMessage__

#include "TObject.h"
#include <string>
using namespace std;

class JSFDialogMessage 
{
 protected:
  int fId;
  int fNline;
  string fVariable;
  string fValue;
  string fComment;
 public:
  JSFDialogMessage(const int id=0, const char* variable="", 
		   const char* value="", const char* comment="");
  virtual ~JSFDialogMessage(){}

  inline const int GetId(){ return fId; }
  inline const int GetNline(){ return fNline; }
  inline const char* GetVariable(){ return fVariable.data() ; }
  inline const char* GetValue(){ return fValue.data() ; }
  inline const char* GetComment(){ return fComment.data(); }

  ClassDef(JSFDialogMessage, 0 ) // Dialog Message

};

#endif

