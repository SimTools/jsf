{
  THtml html; 
  html.SetOutputDir("../../html/"); 
  html.SetSourceDir("./"); 

   html.MakeClass("WWSpring");
   html.MakeClass("WWSpringBuf");
   html.MakeClass("WWBases");

   html.MakeIndex();

   Char_t *cmd="cd ../../html ; mv USER_Index.html ww_Index.html";
   gSystem->Exec(cmd);


}

