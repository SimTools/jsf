{
  THtml html; 
  html.SetOutputDir("../../html/"); 
  html.SetSourceDir("./"); 

   html.MakeClass("FFbarSpring");
   html.MakeClass("FFbarSpringBuf");
   html.MakeClass("FFbarBases");

   html.MakeIndex();
}

