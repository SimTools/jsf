{
  THtml html; 
  html.SetOutputDir("../../html/"); 
  html.SetSourceDir("./"); 

   html.MakeClass("WWSpring");
   html.MakeClass("WWSpringBuf");
   html.MakeClass("WWBases");

   html.MakeIndex();
}

