{
  THtml html; 
  html.SetOutputDir("../../html/"); 
  html.SetSourceDir("./:$(JSFROOT)/src:$(JSFROOT)/include:$(ROOTSYS)/src:$(ROOTSYS)/include"); 


//
   html.MakeClass("CCDRawData");
   html.MakeClass("CCDUnidaqHeader");
   html.MakeClass("CCDRawDataBuf");
   html.MakeClass("CCDBeginRunBuf");
   html.MakeClass("CCDEnvironmentBuf");
   html.MakeClass("CCDEnvironmentBuf3");
   html.MakeClass("CCDRawHist");

   html.MakeClass("CCDCorrectDark");
   html.MakeClass("CCDCorrectedADCBuf");

   html.MakeClass("CCD2x2Cluster");
   html.MakeClass("CCD2x2ClusterBuf");
   html.MakeClass("CCDCluster");

   html.MakeClass("CCDRunIndex");
   html.MakeClass("CCDPedestals");
   html.MakeClass("CCDPedestal");
   html.MakeClass("CCDDarkData");

   html.MakeIndex();

   Char_t *cmd="cd ../../html ; mv USER_Index.html CCDAnal_Index.html";
   gSystem->Exec(cmd);

}

