{
// **************************************************************** 
//  Example of Event display script. 
// 
//(Author) 
//  10-Mar-1999 A.Miyamoto  Original version 
//  21-Apr-1999 A.Miyamoto  Modified to run both in batch and interactive.
//  25-Jan-2000 A.Miyamoto  Use local GUIMainMacro.C
//
// $Id$
//
// **************************************************************** 

  gROOT->LoadMacro("GUIMainMacro.C");

  if( strncmp(gSystem->HostName(),"ccjlc",5)  != 0 ) {
    if( strncmp(gSystem->Getenv("OSTYPE"),"hpux",4) ==0 ) {
      gSystem->Load("$JSFROOT/example/guiexam1/libJSFGUI.sl");
    }
    else {
      gSystem->Load("$JSFROOT/example/guiexam1/libJSFGUI.so");
    }
  }
  JSFGUIFrame *gui;
  jsf  = new JSFSteer();

  if( gClient == 0 ) {
    gui=0;
    BatchRun();
  }
  else  gui=new JSFGUIFrame(gClient->GetRoot(), 400, 220);

//********************************************
//*  Start execution
//********************************************
 
}



