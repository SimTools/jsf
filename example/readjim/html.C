{
   THtml html; 
   html.SetOutputDir("../../html/"); 
   html.SetSourceDir("./"); 

   html.MakeClass("JSFReadJIMBank");
   html.MakeClass("JSFReadJIMBankBuf");
   html.MakeClass("JSFJIMParam");
   html.MakeClass("JSFJIMEventDisplay");
   html.MakeClass("JSFJLCSIM");

   html.MakeIndex();

   Char_t *cmd="cd ../../html ; mv USER_Index.html ReadJIM_Index.html";
   gSystem->Exec(cmd);

}
