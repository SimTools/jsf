{
// 
//  Example of Event display script. 
// 
// $Id$
//

  gROOT->LoadMacro("MainMacro.C");

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
  gui=new JSFGUIFrame(gClient->GetRoot(), 400, 220);

//********************************************
//*  Start execution
//********************************************
 
}



