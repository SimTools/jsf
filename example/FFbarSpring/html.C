{
  THtml html; 
  html.SetOutputDir("../../html/"); 
  html.SetSourceDir("./"); 

   html.MakeClass("FFbarSpring");
   html.MakeClass("FFbarSpringBuf");
   html.MakeClass("FFbarBases");

   html.MakeIndex();

   Char_t *cmd="cd ../../html ; mv USER_Index.html FFbarSpring_Index.html";
   gSystem->Exec(cmd);
   

   Char_t *outdir="../../html/example/FFbarSpring";

   //   html.Convert("Readme","Readme",outdir);
   html.Convert("sim.C","Spring example",outdir);
   html.Convert("nsim.C","Run second spring job",outdir);
   html.Convert("bases.C","Bases example",outdir);
   html.Convert("anal.C","Bases example",outdir);
   html.Convert("read.C","Bases example",outdir);


}

