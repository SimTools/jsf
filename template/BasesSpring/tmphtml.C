{
  THtml html; 
  html.SetOutputDir("../html/"); 
  html.SetSourceDir("./"); 

   html.MakeClass("<<Spring>>");
   html.MakeClass("<<Spring>>Buf");
   html.MakeClass("<<Bases>>");

   html.MakeIndex();
}

