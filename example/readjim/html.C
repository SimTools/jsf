{
   THtml html; 
   html.SetOutputDir("../../html/"); 
   html.SetSourceDir("./"); 

   html.MakeClass("JSFReadJIMBank");
   html.MakeClass("JSFReadJIMBankBuf");
   html.MakeClass("JSFJIMParam");

   html.MakeIndex();
}
