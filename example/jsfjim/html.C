{
   THtml html; 
   html.SetOutputDir("../../html/"); 
   html.SetSourceDir("./"); 

   html.MakeClass("JSFJIM");

   html.MakeIndex();

   Char_t *cmd="cd ../../html ; mv USER_Index.html JSFJIM_Index.html";
   gSystem->Exec(cmd);

}
