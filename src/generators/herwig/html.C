{
//
//$Id$
//
  gEnv->SetValue("Root.Html.Root",
	       "http://www-jlc.kek.jp/subg/offl/jsf/RootHtmlDoc/html");
  gEnv->SetValue("Root.Html.HomePage",
		 "http://www-jlc.kek.jp/subg/offl/jsf/RootHtmlDoc/html");

   THtml html; 
   gSystem->Load("libTHerwig.so");

   html.SetOutputDir("../../html/"); 
   html.SetSourceDir("./:$(JSFROOT)/src:$(JSFROOT)/include:$(ROOTSYS)/src:$(ROOTSYS)/include"); 


   html.MakeClass("THerwig");
   html.MakeClass("THerwigParticle");
   html.MakeClass("HerwigGenerator");

   html.MakeIndex();

   Char_t *cmd="cd ../../html ; mv USER_Index.html herwigx_Index.html";
   gSystem->Exec(cmd);

}

