///////////////////////////////////////////////////////////////////
//
//  Taken from tutorial 
//
// This file contains the class InputDialog.
// An InputDialog object prompts for an input string using a simple
// dialog box. The InputDialog class is also a good example of how
// to use the ROOT GUI classes via the interpreter. Since interpreted
// classes can not call virtual functions via base class pointers, all
// GUI objects are used by composition instead of by inheritance.
//
// This file contains also some utility functions that use
// the InputDialog class to either get a string, integer or
// floating point number. There are also two functions showing
// how to use the file open and save dialogs. The utility functions are:
//
// const char *OpenFileDialog()
// const char *SaveFileDialog()
// const char *GetStringDialog(const char *prompt, const char *defval)
// Int_t GetIntegerDialog(const char *prompt, Int_t defval)
// Float_t GetFloatDialog(const char *prompt, Float_t defval)
//
// To use the InputDialog class and the utility functions you just
// have to load the dialogs.C file as follows:
// .L dialogs.C
//
// Now you can use them like:
// {
//    const char *file = OpenFileDialog();
//    Int_t run   = GetIntegerDialog("Give run number:", 0);
//    Int_t event = GetIntegerDialog("Give event number:", 0);
//    printf("analyse run %d, event %d from file %s\n", run ,event, file);
// }
//
///////////////////////////////////////////////////////////////////
#include <iostream>

#include "JSFConfig.h"

#include "TGWindow.h"
#include "TGLabel.h"
#include "TGButton.h"
#include "InputDialog.h"


enum EInputDialog {
  B_OK=1,
  B_CANCEL=2,
  B_TEXT_ENTRY
};


//_______________________________________________________________________________
InputDialog::~InputDialog()
{
   // Cleanup dialog.

//   fWidgets->Delete();
   delete fWidgets;

   delete fTE;
   delete fDialog;
}

//__________________
void InputDialog::CloseWindow()
{
   delete this;
}

//_______________________________________________________________________________
InputDialog::InputDialog(const char *prompt, const char *defval, char *retstr,
			 Int_t line, UInt_t w)
{
  // Create simple input dialog.
  // (Input Argument)
  //   prompt: prompt string shown on top
  //   defval: initial value shown in entry box.
  //   line  : Number of lines of prompt string. ( default=1) 
  //           if =0, it is counted from prompt string.
  //   w     : Width of input dialog ( default = 400 (pixels) )
  //           if line=0 and w=0, it is decided from the maximum number of 
  //           characters in line  of prompt string.
  // (Output argument):
  //   retstr : input string.
  //

   fWidgets = new TList;

   const TGWindow *main = gClient->GetRoot();
   fDialog = new TGTransientFrame(main, main, 100, 100);
   fDialog->Connect("CloseWindow()","InputDialog",this,"CloseWindow()");

   TGLayoutHints *l1 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 0);
   TGLayoutHints *l2 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
   fWidgets->Add(l1);
   fWidgets->Add(l2);

   // command to be executed by buttons and text entry widget
   char cmd[128];
   sprintf(cmd, "{long r__ptr=0x%x; ((InputDialog*)r__ptr)->ProcessMessage($MSG,$PARM1,$PARM2);}", (UInt_t)this);

   // Set width and height of widget automatically.
   Int_t tline=line;
   if( tline == 0 ) {
     Int_t lprompt=strlen(prompt);
     Int_t i;
     for( i=0;i<lprompt;i++) { if(prompt[i]==0x0a) tline++ ; }
   }

   UInt_t wuse=w;
   if( wuse == 0 ) {
     Int_t maxcol=0;
     Int_t lprompt=strlen(prompt);
     Int_t i;
     Int_t col=0;
     for( i=0;i<lprompt;i++) { 
       col++;
       if(prompt[i]==0x0a) {
         if( col > maxcol ) maxcol=col; 
         col=0; 
       }
     }
     if( maxcol < 20 ) maxcol=20;
     wuse=8*maxcol;
   }

   // create prompt label and textentry widget
   if( tline > 1  ) {
#if __ROOT_FULLVERSION__ > 22500
     TGTextView *label=new TGTextView(fDialog, wuse, 15*(tline+2));
#else
     TGTextFrame *label=new TGTextFrame(fDialog, wuse, 15*tline, kDoubleBorder);
#endif
     label->LoadBuffer(prompt);
     fWidgets->Add(label);
     fDialog->AddFrame(label, l1);
   }
   else {
     TGLabel *label = new TGLabel(fDialog, prompt);
     fWidgets->Add(label);
     fDialog->AddFrame(label, l1);
   }

   TGTextBuffer *tbuf = new TGTextBuffer(256);  //will be deleted by TGtextEntry
   tbuf->AddText(0, defval);

   fTE = new TGTextEntry(fDialog, tbuf);
   fTE->Resize(wuse, fTE->GetDefaultHeight());
   // fTE->Associate(fDialog);
   fTE->SetCommand(cmd);

   fDialog->AddFrame(fTE, l2);

   // create frame and layout hints for Ok and Cancel buttons
   TGHorizontalFrame *hf = new TGHorizontalFrame(fDialog, 60, 20, kFixedWidth);
   TGLayoutHints     *l3 = new TGLayoutHints(kLHintsCenterY | kLHintsExpandX, 5, 5, 0, 0);

   // put hf as last in list to be deleted
   fWidgets->Add(l3);

   // create OK and Cancel buttons in their own frame (hf)
   UInt_t  nb = 0, width = 0, height = 0;
   TGTextButton *b;

   b = new TGTextButton(hf, "&Ok", cmd, B_OK);
   fWidgets->Add(b);
   //   b->Associate(fDialog);
   hf->AddFrame(b, l3);
   height = b->GetDefaultHeight();
   width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;

   b = new TGTextButton(hf, "&Cancel", cmd, B_CANCEL);
   fWidgets->Add(b);
   //   b->Associate(fDialog);
   hf->AddFrame(b, l3);
   height = b->GetDefaultHeight();
   width  = TMath::Max(width, b->GetDefaultWidth()); ++nb;

   // place button frame (hf) at the bottom
   TGLayoutHints *l4 = new TGLayoutHints(kLHintsBottom | kLHintsCenterX, 0, 0, 5, 5);
   fWidgets->Add(l4);
   fWidgets->Add(hf);

   fDialog->AddFrame(hf, l4);

   // keep buttons centered and with the same width
   hf->Resize((width + 20) * nb, height);

   // set dialog title
   fDialog->SetWindowName("Get Input");

   // map all widgets and calculate size of dialog
   fDialog->MapSubwindows();

   width  = fDialog->GetDefaultWidth();
   height = fDialog->GetDefaultHeight();

   fDialog->Resize(width, height);

   // position relative to the parent window (which is the root window)
   Window_t wdum;
   int      ax, ay;

#if __ROOT_FULLVERSION__ >= 22300
   gVirtualX->TranslateCoordinates(main->GetId(), main->GetId(),
#else
   gGXW->TranslateCoordinates(main->GetId(), main->GetId(),
#endif
                          (((TGFrame *) main)->GetWidth() - width) >> 1,
                          (((TGFrame *) main)->GetHeight() - height) >> 1,
                          ax, ay, wdum);
   fDialog->Move(ax, ay);
   fDialog->SetWMPosition(ax, ay);

   // make the message box non-resizable
   fDialog->SetWMSize(width, height);
   fDialog->SetWMSizeHints(width, height, width, height, 0, 0);

   fDialog->SetMWMHints(kMWMDecorAll | kMWMDecorResizeH  | kMWMDecorMaximize |
                                       kMWMDecorMinimize | kMWMDecorMenu,
                        kMWMFuncAll  | kMWMFuncResize    | kMWMFuncMaximize |
                                       kMWMFuncMinimize,
                        kMWMInputModeless);

   // popup dialog and wait till user replies
   fDialog->MapWindow();

   fRetStr = retstr;

   gClient->WaitFor(fDialog);
}

//______________________________________________________________________
void InputDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Handle button and text enter events
  
   switch (GET_MSG(msg)) {
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {
             case kCM_BUTTON:
                switch (parm1) {
                   case B_OK:
                      // here copy the string from text buffer to return variable
                      strcpy(fRetStr, fTE->GetBuffer()->GetString());
		      CloseWindow();
//                      delete this;
                      break;

                   case B_CANCEL:
                      fRetStr[0] = 0;
		      CloseWindow();
//                      delete this;
                      break;
                 }
              default:
                 break;
          }
          break;

       case kC_TEXTENTRY:
         switch (GET_SUBMSG(msg)) {
             case kTE_ENTER:
                // here copy the string from text buffer to return variable
                strcpy(fRetStr, fTE->GetBuffer()->GetString());
		  CloseWindow();
//                delete this;
                break;
             default:
                break;
          }
          break;

       default:
          break;
   }
}


