{
// 
//  Example of Event display script. 
// 

  gROOT->LoadMacro("MainMacro.C");

  if( strncmp(gSystem->HostName(),"ccjlc",5)  != 0 ) {
    gSystem->Load("libJSFGUI.so");
  }
  JSFGUIFrame *gui;

  jsf  = new JSFSteer();
  gui=new JSFGUIFrame(gClient->GetRoot(), 400, 220);

//********************************************
//*  Start execution
//********************************************
 
}



