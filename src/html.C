{
//$id$

  THtml html; 
  html.SetOutputDir("../html/"); 
//  html.SetSourceDir("./"); 

  html.MakeClass("JSFSteer");

//  html.MakeClass("JSF2DV");
//  html.MakeClass("JSF3DV");
//  html.MakeClass("JSF3DV_f");
//  html.MakeClass("JSFRThPhi");
//  html.MakeClass("JSFRThPhi_f");
//  html.MakeClass("JSFRPhiZ");
//  html.MakeClass("JSFRPhiZ_f");

   html.MakeClass("JSFModule");
   html.MakeClass("JSFSteer");
   html.MakeClass("JSFSteerConf");
   html.MakeClass("JSFEventBuf");
   html.MakeClass("JSFDBS");
   html.MakeClass("JSFUtil");
   html.MakeClass("JSFEnv");
   html.MakeClass("JSFEnvRec");

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


   html.MakeClass("PythiaGenerator");
   html.MakeClass("JSFReadGenerator");
   html.MakeClass("JSFReadGeneratorBuf");
   html.MakeClass("DebugGenerator");
   html.MakeClass("DebugWGenerator");
   html.MakeClass("JSFReadParton");
   html.MakeClass("JSFMergeEvent");

   html.MakeClass("JSFQuickSim");
   html.MakeClass("JSFQuickSimParam");
   html.MakeClass("JSFQuickSimBuf");
   html.MakeClass("JSFSIMDST");
   html.MakeClass("JSFSIMDSTBuf");
   html.MakeClass("JSFLTKCLTrack");
   html.MakeClass("JSFCDCTrack");
   html.MakeClass("JSFVTXHit");
   html.MakeClass("JSFCALHit");
   html.MakeClass("JSFEMCHit");
   html.MakeClass("JSFHDCHit");
   html.MakeClass("JSFCALGeoParam");

   html.MakeClass("JSFHelicalTrack");
//   html.MakeClass("JSFHitCylinder");
//   html.MakeClass("JSFHelixParameter");
//   html.MakeClass("JSFHelixParameter_f");
//   html.MakeClass("JSFHelixErrorMatrix");
//   html.MakeClass("JSFHelixErrorMatrix_u");

   html.MakeIndex();

   Char_t *cmd="cd ../../html ; mv USER_Index.html JSFMain_Index.html";
   gSystem->Exec(cmd);


}


