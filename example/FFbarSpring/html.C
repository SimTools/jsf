{

  gEnv->SetValue("Root.Html.Root",
               "http://www-jlc.kek.jp/subg/offl/jsf/RootHtmlDoc/html");
  gEnv->SetValue("Root.Html.HomePage",
                 "http://www-jlc.kek.jp/subg/offl/jsf/RootHtmlDoc/html");

  THtml html; 

    Char_t *name=gSystem->DynamicPathName("libFFbarSpring");
    gSystem->Load(name);

  html.SetOutputDir("../../html/"); 
  html.SetSourceDir("./:$(JSFROOT)/src:$(JSFROOT)/include:$(ROOTSYS)/src:$(ROOTSYS)/include"); 

   html.MakeClass("FFbarSpring");
   html.MakeClass("FFbarSpringBuf");
   html.MakeClass("FFbarBases");

   html.MakeIndex();

   Char_t *cmd="cd ../../html ; mv USER_Index.html FFbarSpring_Index.html";
   gSystem->Exec(cmd);
   gSystem->Exec("mkdir -p ../../html/examples/FFbarSpring");
   

   Char_t *outdir="../../html/examples/FFbarSpring";

   //   html.Convert("Readme","Readme",outdir);
   html.Convert("sim.C","Spring example",outdir);
   html.Convert("nsim.C","Run second spring job",outdir);
   html.Convert("bases.C","Bases example",outdir);
   html.Convert("anal.C","Run Spring and analysis",outdir);
   html.Convert("read.C","Example to read simulation data",outdir);


}

