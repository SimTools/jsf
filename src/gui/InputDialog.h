#ifndef __InputDialog__
#define __InputDialog__

///////////////////////////////////////////////////////////////////////////
//                                                                       //
// Input Dialog Widget                                                   //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include <TGFrame.h>
#include <TGTextEntry.h>
#include <TGTextView.h>
#include <TList.h>

class InputDialog {

private:
   TGTransientFrame *fDialog;  // transient frame, main dialog window
   TGTextEntry      *fTE;      // text entry widget containing
   TList            *fWidgets; // keep track of widgets to be deleted in dtor
   char             *fRetStr;  // address to store return string

public:
   InputDialog(const char *prompt, const char *defval, char *retstr, Int_t line=1,
	       UInt_t w=400);
   ~InputDialog();
   void ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   void CloseWindow();
};

#endif

