
class JSFGUIFrame;
JSFGUIFrame *gui=0;
JSFSteer *jsf=0;
JSFSteer *gJSF=0;
 
Int_t demo()
{
// **************************************************************** 
//  Example of Event display script. 
// 
//(Author) 
//  10-Mar-1999 A.Miyamoto  Original version 
//  21-Apr-1999 A.Miyamoto  Modified to run both in batch and interactive.
//
// $Id$
//
// **************************************************************** 

  gROOT->LoadMacro("$JSFROOT/macro/GUIMainMacro.C");

  gSystem->Load("$JSFROOT/example/guiexam1/libJSFGUI.so");

  JSFGUIFrame *gui;
  jsf  = new JSFSteer();

  if( gClient == 0 ) {
    gui=0;
    BatchRun();
  }
  else  gui=new JSFGUIFrame(gClient->GetRoot(), 400, 220, kTRUE);

  gui->RunDemo();
//********************************************
//*  Start execution
//********************************************
 
}



