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

  if( strncmp(gSystem->HostName(),"ccjlc",5)  != 0 ) {
    if( strcmp(gSystem->Getenv("OSTYPE"),"hpux") ==0 ) {
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



