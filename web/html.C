{
//$id$

//( "JSFMain_Index.html",  "FFbarSpring_Index.html","ww_Index.html", "guiexam1_Index.html", "ReadJIM_Index.html", "JSFJIM_Index.html", "JSFZVTOP3_Index.html", "CCDAnal_Index.html","bsgen_Index.html","herwigx_Index.html");

  gEnv->SetValue("Root.Html.Root",
                 "http://www-jlc.kek.jp/subg/offl/jsf/RootHtmlDoc/html");
  gEnv->SetValue("Root.Html.HomePage",
                 "http://www-jlc.kek.jp/subg/offl/jsf/RootHtmlDoc/html");

  THtml html; 
  html.SetOutputDir("htmldoc"); 
  html.SetSourceDir("./:$(JSFROOT)/src:$(JSFROOT)/include:$(ROOTSYS)/src:$(ROOTSYS)/include:$(JSFROOT)/share/me2shgen:$(JSFROOT)/share/herwig:$(JSFROOT)/share/jetnet/src:$(JSFROOT)/share/zvtop/src:$(JSFROOT)/share/bsgen:$(KFLIBROOT)/Anlib/src:$(JSFROOT)/example/guiexam1:$(JSFROOT)/example/FFbarSpring");

   gSystem->Load("libBasesSpring.so");
   gSystem->Load("libJSFGUI.so");
   gSystem->Load("libAnlib.so");
   gSystem->Load("libJSFBeamGeneration.so");
   gSystem->Load("libJSFME2SHGenerator.so");
   gSystem->Load("libJSFZVTOP3.so");
   gSystem->Load("libJSFJETNET.so");
   gSystem->Load("libTHerwig.so");


   html.MakeClass("JSFSteer");

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
   html.MakeClass("JSFBasesTempHist");

   html.MakeClass("JSFGenerator");
   html.MakeClass("JSFGeneratorBuf");
   html.MakeClass("JSFGeneratorParticle");
   html.MakeClass("JSFGeneratedProcessType");

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

   html.MakeClass("JSFVirtualFit");
   html.MakeClass("JSFGeoCFit");
   html.MakeClass("JSFVertexing");

   html.MakeClass("JSFEDProperty");
   html.MakeClass("JSFEDHelix");
   html.MakeClass("JSFEnvGUIFrame");
   html.MakeClass("JSFEventDisplay");
   html.MakeClass("JSFDemoDisplay");

 
   html.MakeClass("JSFZVTOP3");
   html.MakeClass("JSFZVTOP3Track");
   html.MakeClass("JSFZVTOP3Vertex");
   html.MakeClass("JSFVirtualVertex");
   html.MakeClass("JSFVirtualVertexing");
   html.MakeClass("JSFGeneratorVertex");
   html.MakeClass("JSFGeneratorVertexing");

   html.MakeClass("JSFBeamGeneration");
   html.MakeClass("JSFBeamGenerationCain");
 
   html.MakeClass("JSFJETNET");

   html.MakeClass("JSFHEPRUP");
   html.MakeClass("JSFHEPEUP");
   html.MakeClass("JSFMEGenerator");
   html.MakeClass("JSFMEGeneratorBuf");
   html.MakeClass("JSFSHGenerator");
   html.MakeClass("JSFSHGeneratorBuf");
   html.MakeClass("JSFReadMEGenerator");
   html.MakeClass("JSFReadMEGeneratorBuf");
   html.MakeClass("JSFPythiaHadronizer");
   html.MakeClass("JSFPythiaHadronizerBuf");

   html.MakeClass("ANL2DSpline");
   html.MakeClass("ANL2DVector");
   html.MakeClass("ANL3DVector");
   html.MakeClass("ANL4DVector");

   html.MakeClass("ANLCheatedJetFinder");
   html.MakeClass("ANLCheatedDurhamJetFinder");
   html.MakeClass("ANLCheatedJadeJetFinder");
   html.MakeClass("ANLCheatedJadeEJetFinder");
   html.MakeClass("ANLCheatedJetFinder");
   html.MakeClass("ANLDurhamJetFinder");
   html.MakeClass("ANLEventShape");
   html.MakeClass("ANLGVTXTagger");
   html.MakeClass("ANLJadeEJetFinder");
   html.MakeClass("ANLJadeJetFinder");
   html.MakeClass("ANLJet");
   html.MakeClass("ANLJetFinder");
   html.MakeClass("ANLPair");
   html.MakeClass("ANLPairCombiner");
   html.MakeClass("ANLTaggedJet");
   html.MakeClass("ANLVTXTagger");
   html.MakeClass("FlavourGetter");
   html.MakeClass("Lockable");

   html.MakeClass("THerwigParticle");
   html.MakeClass("THerwig");
   html.MakeClass("HerwigGenerator");
   html.MakeClass("HerwigGenerator");


   gSystem->Load("../example/FFbarSpring/libFFbarSpring.so");
   html.MakeClass("FFbarSpring");
   html.MakeClass("FFbarSpringBuf");
   html.MakeClass("FFbarBases");

   html.MakeIndex();

   Char_t *cmd="cd htmldoc ; mv USER_Index.html JSFMain_Index.html";
   gSystem->Exec(cmd);
//
   html.Convert("../macro/gui.C","Main script for JSF","htmldoc");
   html.Convert("../macro/GUIMainMacro.C","Main script for JSF","htmldoc");
   html.Convert("../macro/InitPythia.C","Set Parameter of Pythia Generator","htmldoc");

   

}


