{
//$Id$

  gEnv->SetValue("Root.Html.Root",
		 "http://www-jlc.kek.jp/subg/offl/jsf/RootHtmlDoc/html");
  gEnv->SetValue("Root.Html.HomePage",
		 "http://www-jlc.kek.jp/subg/offl/jsf/RootHtmlDoc/html");

  THtml html; 
  html.SetOutputDir("../../../html/"); 
  html.SetSourceDir("./:$(JSFROOT)/src:$(JSFROOT)/include:$(ROOTSYS)/src:$(ROOTSYS)/include"); 

  if ( gSystem->Load("libAnlib.so") == 0 ) { 
    gSystem->Load("libJSFZVTOP3.so");


    html.MakeClass("JSFZVTOP3");
    html.MakeClass("JSFZVTOP3Track");
    html.MakeClass("JSFZVTOP3Vertex");
    html.MakeClass("JSFVirtualVertex");
    html.MakeClass("JSFVirtualVertexing");
    html.MakeClass("JSFGeneratorVertex");
    html.MakeClass("JSFGeneratorVertexing");
    
    html.MakeIndex();

    Char_t *cmd="cd ../../../html ; mv USER_Index.html JSFZVTOP3_Index.html";
    gSystem->Exec(cmd);
  }

}


