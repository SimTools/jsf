{
   THtml html; 
   html.SetOutputDir("../../html/"); 
   html.SetSourceDir("./"); 

   gSystem->Load("libJSFGUI.so");

   html.MakeClass("JSFEventDisplay");
//   html.MakeClass("JSFGUIFrame");
   html.MakeClass("JSFEDProperty");
   html.MakeClass("JSFGUIAnalysis");

   html.MakeIndex();
}

