{
   THtml html; 
   gSystem->Load("libJSFGUI.so");

   html.SetOutputDir("../../html/"); 
   html.SetSourceDir("./:$(JSFROOT)/src:$(JSFROOT)/include:$(ROOTSYS)/src:$(ROOTSYS)/include"); 


//   html.MakeClass("JSFGUIFrame");
   html.MakeClass("JSFEDProperty");
   html.MakeClass("JSFEDHelix");
   html.MakeClass("JSFEnvGUIFrame");
   html.MakeClass("JSFEventDisplay");
   html.MakeClass("JSFDemoDisplay");

//   html.MakeClass("JSFEDSetOptions");
//   html.MakeClass("JSFGUIAnalysis");
//   html.MakeClass("InputDialog");

   html.MakeIndex();

   Char_t *cmd="cd ../../html ; mv USER_Index.html guiexam1_Index.html";
   gSystem->Exec(cmd);

}

