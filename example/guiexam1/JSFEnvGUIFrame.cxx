//*LastUpdate:  jsf-1-12 1-August-1999  Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  1-August-1999 Akiya Miyamoto

//////////////////////////////////////////////////////////////////
//
//  JSFEnvGUIFrame
//
//  GUI Frame to display/set environment parameter.
// 
//  
//  
//$Id$
//////////////////////////////////////////////////////////////////

#include <TSystem.h>
#include <TGMsgBox.h>
#include "JSFSteer.h"
#include "JSFEnv.h"
#include "InputDialog.h"
#include "JSFEnvGUIFrame.h"

ClassImp(JSFEnvGUIFrame)

enum EEnvGUICommandIdentifiers {
  B_DISMISS=1,
  B_SAVE=2,
  B_ABOUT=3,

  B_PARA_SET=11,
  B_PARA_HELP=12
};

//___________________________________________________________
JSFEnvGUIFrame::JSFEnvGUIFrame(const TGWindow *p, UInt_t w, UInt_t h)
      : TGMainFrame(p, w, h)
{
  // Create test main frame. A TGMainFrame is a top level window.

  // ===================================================
  // Popup 
  // ===================================================

  // gJSF->Env()->PrintEnv();
  fListFrame=new TGCompositeFrame(this, 60, 60, kVerticalFrame);
  fListBox = new TGListBox(fListFrame, 89);  // 89 is ID
  fL3 = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 5, 5, 5, 5);
  fListFrame->AddFrame(fListBox, fL3);

  TOrdCollection *tbg=gJSF->Env()->GetObtained();
  TIter next(tbg);
  JSFEnvRec *er;
  Int_t i=0;
  fListBox->AddEntry("*** Parameter name : value ***", i++);
  er=(JSFEnvRec*)next();
  while((er=(JSFEnvRec*) next())) {
      fListBox->AddEntry(er->EntryString(), i++);
  }
   fListBox->Resize(150, 150);
   AddFrame(fListFrame, 
	      new TGLayoutHints(kLHintsTop| kLHintsExpandX, 10, 10, 1, 0));
   

  // ===================================================
  // Built buttons for set parameters
  // ===================================================
   Char_t *sellabel[]={"Set", "Help"};
   Int_t  idsellabel[]={B_PARA_SET, B_PARA_HELP};
   fCFSelect = new TGCompositeFrame(this, 60, 20, kHorizontalFrame);
   for(i=0;i<2;i++){
     fTBSelect[i]=new TGTextButton(fCFSelect, sellabel[i], idsellabel[i]);
     fCFSelect->AddFrame(fTBSelect[i],
			    new TGLayoutHints(kLHintsTop|kLHintsExpandX,4,4,2,2));
     fTBSelect[i]->Associate(this);
   }
   AddFrame(fCFSelect, 
	      new TGLayoutHints(kLHintsTop| kLHintsExpandX, 10, 10, 1, 0));


  // ===================================================
  // Built buttons at the end of pannel.
  // ===================================================
  Char_t *bottomlabel[]={"Dismiss","Save", "About"};
  Int_t  idbottom[]={B_DISMISS, B_SAVE, B_ABOUT};
  fCFBottom    = new TGCompositeFrame(this, 60, 20, kHorizontalFrame);
  for(i=0;i<3;i++){
    fTBBottom[i] = new TGTextButton(fCFBottom, bottomlabel[i], idbottom[i]);
    fCFBottom->AddFrame(fTBBottom[i], 
		new TGLayoutHints(kLHintsTop|kLHintsExpandX,4,4,2,2));
    fTBBottom[i]->Associate(this);
  }
  AddFrame(fCFBottom, 
	      new TGLayoutHints(kLHintsTop| kLHintsExpandX, 10, 10, 1, 0));

  // ===================================================
  // End of constructor
  // ===================================================
  SetWindowName("Environment parameters");
  MapSubwindows();

  // we need to use GetDefault...() to initialize the layout algorithm...
  // Resize(GetDefaultSize());
  Resize(400,220);
  Layout();
  MapWindow();

  gClient->WaitFor(this);

}

//___________________________________________________________________
JSFEnvGUIFrame::~JSFEnvGUIFrame()
{
  delete fTBSelect[0]; delete fTBSelect[1];
  delete fCFSelect; 
  delete fTBBottom[0]; delete fTBBottom[1]; delete fTBBottom[2];
  delete fCFBottom; 
  TGMainFrame::CloseWindow();
}



//__________________________________________________________________
void JSFEnvGUIFrame::CloseWindow()
{
   // Got close message for this MainFrame. Calls parent CloseWindow()
   // (which destroys the window) and terminate the application.
   // The close message is generated by the window manager when its close
   // window menu item is selected.

   TGMainFrame::CloseWindow();
}

//_________________________________________________________________
Bool_t JSFEnvGUIFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t id0)
{
  // Handle messages send to the TestMainFrame object. E.g. all menu button
  // messages.

  Int_t retval=0;
  Int_t buttons=0;
  Char_t retstr[128];
  Char_t newstr[256];
  switch (GET_MSG(msg)) {

    case kC_COMMAND:
      switch (GET_SUBMSG(msg)) {

        case kCM_BUTTON:
          switch (parm1) {
            case B_DISMISS: 
	      CloseWindow();
	      break;

	    case B_SAVE: 
	      gJSF->Env()->WriteFile();
	      sprintf(newstr,"New parameters are saved in a file %s.\nPrevious file is renamed in %s.bak.",
      	      gJSF->Env()->GetEnvFileName(),gJSF->Env()->GetEnvFileName());
	      new TGMsgBox(fClient->GetRoot(), this, "JSF Message",
			  newstr, kMBIconExclamation, buttons, &retval);

	      break;
		  
 	    case B_ABOUT:
	      new TGMsgBox(fClient->GetRoot(), this, "JSF Message",
"This panel is to set environment parameters.To change them, \n\
select item and press [SET] botton.[HELP] will display help \n\
of parameters. In the parameter list, the prefix [L] is local \n\
(those defined in the working directory), [G] is global(those \n\
defined in JSFROOT directory) and [C] is modified parameter.",
		kMBIconExclamation, buttons, &retval);
	      break;

  	    case B_PARA_SET:
  	    case B_PARA_HELP:
	      Int_t id=fListBox->GetSelected();
	      if( id < 0 ) {
		new TGMsgBox(fClient->GetRoot(), this, "JSF Message",
			   "No entry is selected.\n  Select parameters first.",
			      kMBIconExclamation, buttons, &retval);
		break;
	      }
	      else if( id==0 ) {
		JSFEnvRec *er=gJSF->Env()->Lookup("HEADER");
	 	Char_t *msg=er->GetHelpMessage();
		new TGMsgBox(fClient->GetRoot(), this, "JSF Message", msg,
			      kMBIconExclamation, buttons, &retval);
                if( msg ) delete msg;
		break;
	      }
	      
              TOrdCollection *tbl=gJSF->Env()->GetObtained();
	      JSFEnvRec  *sel=(JSFEnvRec*)tbl->At(id);
	      if( parm1 == B_PARA_HELP ) {
  	        Char_t *msg=sel->GetHelpMessage();
 	 	new TGMsgBox(fClient->GetRoot(), this, "JSF Message", msg,
			      kMBIconExclamation, buttons, &retval);
                if( msg ) delete msg;
		break;
	      }

              Char_t *msg=sel->GetHelpMessage();
              Int_t lmsg=strlen(msg);
              if( lmsg == 0 )  new InputDialog(sel->fName.Data(),sel->fValue.Data(),retstr);
              else {
                // Add comment in InputDialog
                Char_t *dlmsg=new Char_t[lmsg+sel->fName.Length()+4];
                strcpy(dlmsg,sel->fName.Data());
                *(dlmsg+sel->fName.Length())=':';
                *(dlmsg+sel->fName.Length()+1)=0x0a;
                *(dlmsg+sel->fName.Length()+2)=0x0a;
                strcpy(dlmsg+sel->fName.Length()+3,msg);
	        new InputDialog(dlmsg,sel->fValue.Data(),retstr,0,0);
                delete msg;  delete dlmsg;
              }
	      if( retstr[0] != 0  ) {
 	        sel->fValue=retstr;
	        gJSF->Env()->SetValue(sel->fName.Data(), sel->fValue.Data());
	        fListBox->RemoveEntry(id);
  	        if( id > 0 ) { 
		  fListBox->InsertEntry(sel->EntryString(),id,id-1); }
	        fListBox->MapSubwindows();
	        fListBox->Layout();
	      }
	      //	      delete dialog;
	      break;
 
	  } // End of parm1 switch for kCM_BOTTON action
       }
   }
   return kTRUE;
}











