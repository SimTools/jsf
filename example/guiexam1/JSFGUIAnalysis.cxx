//*LastUpdate :  jsf-1-5  23-Feburary-1999  By Akiya Miyamoto
//*-- Author  : A.Miyamoto  23-September-1998


///////////////////////////////////////////////////////////////////
//
// JSFGUIAnalysis
//
// For the control of analysis sequence.
// This is a preliminary version of event display for experiment
// to know how to use ROOT GUI and 3D Graphics.
//
//$Id$
//
///////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFGUIAnalysis.h"


enum EJSFGUIAnalysisID {
   GUIA_B_DISMISS
};


//---------------------------------------------------------------------------
JSFGUIAnalysis::JSFGUIAnalysis(const TGWindow *p, 
		 const TGWindow *main, UInt_t w,  UInt_t h, UInt_t options)
    : TGTransientFrame(p, main, w, h, options)
{

   //
   fBottomFrame   = new TGHorizontalFrame(this, 60, 20);
   fButtonDismiss = new TGTextButton(fBottomFrame, "&Dismiss",GUIA_B_DISMISS);
   fBottomFrame->AddFrame(fButtonDismiss,  new TGLayoutHints(kLHintsTop|kLHintsCenterX,
   				   0, 0, 1, 0) );
   fButtonDismiss->Associate(this);

   fLHBDismiss = new TGLayoutHints(kLHintsBottom | kLHintsExpandX , 0, 0, 1, 0);
   AddFrame(fBottomFrame, fLHBDismiss);

   //
   SetWindowName("User Analysis Options");
   MapSubwindows();

   // we need to use GetDefault...() to initialize the layout algorithm...
   // Resize(GetDefaultSize());
   Resize(400,200);
   Layout();

   MapWindow();

}


//_______________________________________________________________________
Bool_t JSFGUIAnalysis::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle messages send to the JSFGUIAnalysis object. E.g. all menu button
   // messages.

   switch (GET_MSG(msg)) {
     
     case kC_COMMAND:
         switch (GET_SUBMSG(msg)) {

            case kCM_BUTTON:
	      switch (parm1) {
                case GUIA_B_DISMISS:
                  CloseWindow();
                  break;
              }

         } // End of GET_SUBMSG switch
   } // End of 
   return kTRUE;
}



