///////////////////////////////////////////////////////////////////
//
// JSFDialogMessage
//
// Data to display dialog and get value from them are stored.
//
//$Id$
//
///////////////////////////////////////////////////////////////////

#include "JSFDialogMessage.h"

using namespace std;

ClassImp(JSFDialogMessage)

// ---------------------------------------------------------------
JSFDialogMessage::JSFDialogMessage(const int id, const char* variable, 
				   const char* value, const char* comment)
{
  fId=id;
  fVariable=string(variable);
  fValue=string(value);
  fComment=string(comment);

  fNline=1;
  for(UInt_t i=0;i<fComment.length();i++){
    if( fComment[i] == '\n' ) fNline++;
  }

}

