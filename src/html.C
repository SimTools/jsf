{
  THtml html; 
  html.SetOutputDir("../html/"); 
  html.SetSourceDir("./"); 

  html.MakeClass("JSFSteer");

   html.MakeClass("JSFModule");
   html.MakeClass("JSFSteer");
   html.MakeClass("JSFSteerConf");
   html.MakeClass("JSFEventBuf");
   html.MakeClass("JSFDBS");

   html.MakeClass("JSFSpring");
   html.MakeClass("JSFSpringBuf");
   html.MakeClass("JSFSpringParton");
   html.MakeClass("JSFBases");

   html.MakeClass("JSFGenerator");
   html.MakeClass("JSFGeneratorBuf");
   html.MakeClass("JSFGeneratorParticle");

   html.MakeClass("JSFLCFULL");
   html.MakeClass("JSFFULLGenerator");
   html.MakeClass("JSFHadronizer");

   html.MakeClass("JSFQuickSim");
   html.MakeClass("JSFQuickSimParam");
   html.MakeClass("JSFQuickSimData");
   html.MakeClass("JSFQuickSimBuf");

   html.MakeClass("PythiaGenerator");
   html.MakeClass("DebugGenerator");
   html.MakeClass("DebugWGenerator");
   html.MakeClass("JSFLTKCLTrack");

   html.MakeIndex();
}

