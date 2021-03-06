class JSFGUIFrame;
class JSFSteer;
JSFGUIFrame *gui=0;
JSFSteer *jsf=0;
JSFSteer *gJSF=0;

Int_t jsf()
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

  gSystem->Load("libJSF.so");
  gROOT->Macro("LoadLibraries.C");

  gSystem->Load("libJSFGUI.so");
//	  jsf  = new JSFSteer();
	  jsf  = JSFSteer::Instance();
	  gJSF = jsf;


  gROOT->LoadMacro(jsf->Env()->GetValue("JSFGUI.GUIMainMacro","GUIMainMacro.C"));

	  if( gClient == 0 ) {
	    BatchRun();
	  }
	  else  {
	    if( gROOT->GetClass("JSFJIMEventDisplay") ) {
	      gui=new JSFGUIFrame(gClient->GetRoot(), 400, 220,
				  kTRUE,new JSFJIMEventDisplay() );
	    }
    else {
      gui=new JSFGUIFrame(gClient->GetRoot(), 400, 220);
    }
  }

//********************************************
//*  Start execution
//********************************************
 
}

