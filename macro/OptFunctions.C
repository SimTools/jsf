//
// Collection of optional macros to be used in GUIMainMacro
//

void InitOptionalModules()
{
// JSFGUI.S4Sequence: kaldetectors:mctruths:lcio:makers:analysis
  TString inputs(gJSF->Env()->GetValue("JSFGUI.S4Sequence",""));
  TString delim(":");
  TObjArray *tokens=inputs.Tokenize(delim);
  if( tokens->GetEntries() < 1 ) return;
  TIter next(tokens);
  TObjString *objstr=0;
  while( (objstr=(TObjString*)next()) ) {
    TString setname=objstr->GetString();
    if( setname == "kaldetectors" ) {  DefineKalDetectorModules(); }
    else if ( setname == "mctruths" ) {  DefineS4MCTruthModules(); }
    else if ( setname == "makers" ) { DefineS4MakerModules(); }
    else if ( setname == "analysis" ) { DefineS4AnalysisModules(); }
    else if ( setname == "lcio" ) {
	J42LCIO *j42lcio=new J42LCIO();  
    }
    else {
      cerr << "Error .. S4Sequence name " << setname << " is not defined." << endl;
      exit(-1);
    }
  }

}

void DefineS4MCTruthModules()
{

 
  JSFJ4BufManager   *bufman        = new JSFJ4BufManager();
 
  S4VTXMCTruth      *vtxmctruth    = new S4VTXMCTruth();
  S4ITMCTruth       *itmctruth     = new S4ITMCTruth();
  S4TPCMCTruth      *tpcmctruth    = new S4TPCMCTruth();
  S4CALMCTruth      *calmctruth    = new S4CALMCTruth();
  //  S4MUDMCTruth      *mudmctruth    = new S4MUDMCTruth();
  //----------------------------------
  // Suppress output from these modules.
  bufman        -> SetMakeBranch(kFALSE);
  vtxmctruth    -> SetMakeBranch(kFALSE);
  itmctruth     -> SetMakeBranch(kFALSE);
  tpcmctruth    -> SetMakeBranch(kFALSE);
  calmctruth    -> SetMakeBranch(kFALSE);
  //  mudmctruth    -> SetMakeBranch(kFALSE);
                                                                                
}

void DefineKalDetectorModules()
{
//----------------------------------
// Define KalDetector
//----------------------------------
 
   U4KalDetector    *det        = new U4KalDetector();
   U4TPCKalDetector *tpcdet     = new U4TPCKalDetector();
   U4ITKalDetector  *itdet      = new U4ITKalDetector();
   U4VTXKalDetector *vtxdet     = new U4VTXKalDetector();
#if 1
   det->Install(*vtxdet);
   det->Install(*itdet);
   det->Install(*tpcdet);
#endif
}

void DefineS4MakerModules()
{
 
   S4TPCHitMaker     *tpchitmaker   = new S4TPCHitMaker();
   S4TPCTrackMaker   *tpctrackmaker = new S4TPCTrackMaker(tpcdet);
   S4ITHitMaker      *ithitmaker    = new S4ITHitMaker();
   S4ITTrackMaker    *ittrackmaker  = new S4ITTrackMaker(itdet);
   S4VTXHitMaker     *vtxhitmaker   = new S4VTXHitMaker();
   S4VTXTrackMaker   *vtxtrackmaker = new S4VTXTrackMaker(vtxdet);
   S4HYBTrackMaker   *htrackmaker   = new S4HYBTrackMaker(det);
                                                                                
   S4CALHitMaker     *calhitmaker   = new S4CALHitMaker();
   S4CALClusterMaker *calclustmaker = new S4CALClusterMaker();
   S4PFOMaker        *pfomaker      = new S4PFOMaker();
   S4JetMaker        *jetmaker      = new S4JETMaker();
                                                                                
   tpchitmaker   -> SetMakeBranch(kFALSE);
   tpctrackmaker -> SetMakeBranch(kFALSE);
   ithitmaker    -> SetMakeBranch(kFALSE);
   ittrackmaker  -> SetMakeBranch(kFALSE);
   vtxhitmaker    -> SetMakeBranch(kFALSE);
   vtxtrackmaker  -> SetMakeBranch(kFALSE);
   htrackmaker   -> SetMakeBranch(kFALSE);
                                                                                
   calhitmaker   -> SetMakeBranch(kFALSE);
   calclustmaker -> SetMakeBranch(kFALSE);
   pfomaker      -> SetMakeBranch(kFALSE);
   jetmaker      -> SetMakeBranch(kFALSE);
                                                                                

}

void DefineS4AnalysisModules()
{
                                                                                
   S4TPCAnalysis     *tpcanalysis   = new S4TPCAnalysis();
   S4ITAnalysis      *itanalysis    = new S4ITAnalysis();
   S4VTXAnalysis     *vtxanalysis   = new S4VTXAnalysis();
   S4CALAnalysis     *calanalysis   = new S4CALAnalysis();
   S4PFOAnalysis     *pfoanalysis   = new S4PFOAnalysis();
   S4JETAnalysis     *jetanalysis   = new S4JETAnalysis();

   tpcanalysis   -> SetMakeBranch(kFALSE);
   itanalysis    -> SetMakeBranch(kFALSE);
   vtxanalysis   -> SetMakeBranch(kFALSE);
   calanalysis   -> SetMakeBranch(kFALSE);
   pfoanalysis   -> SetMakeBranch(kFALSE);
   jetanalysis   -> SetMakeBranch(kFALSE);

}

