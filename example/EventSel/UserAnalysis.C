// ---------------------------------------------------------
//
// A sample script of event selection 
//  
// Modules for the analysis is defined in the function,
// UserModuleDefine().  It includes a module, EventSelection,
// which can be used to select events and bypass QuickSimulator
// if it is not a desired event.
//
// The JSF Module, EventSelection must be prepared as C++ program.
//
//$Id$
//
//----------------------------------------------------------

JSFSteer *jsf;

//--------------------------------------------------
void UserModuleDefine()
{
  Char_t *inputFileName=jsf->Env()->GetValue("JSFGUI.InputFileName","");
  Char_t *outputFileName=jsf->Env()->GetValue("JSFGUI.OutputFileName",
					      "jsf.root");
  Int_t irunmode=jsf->Env()->GetValue("JSFGUI.RunMode",1);

    ofile= new TFile(outputFileName,"RECREATE");
    jsf->SetIOFiles();

    full  = new JSFLCFULL();
    py  = new PythiaGenerator();
    gen = py;

    evsel = new EventSelection();

    sim    = new JSFQuickSim();

    simdst = new JSFSIMDST();
    simdst->SetQuickSimParam(sim->Param());

    if( jsf->Env()->GetValue("JSFGUI.SIMDST.Output",0) == 0 && simdst ) 
    simdst->NoReadWrite();


    if( jsf->Env()->GetValue("JSFGUI.OutputEventData",0) == 0  ) {
      full->SetMakeBranch(kFALSE);  
      if(sim) sim->SetMakeBranch(kFALSE);   
      if(simdst) simdst->SetMakeBranch(kFALSE);
      gen->SetMakeBranch(kFALSE); 
      if( hdr ) hdr->SetMakeBranch(kFALSE);
    }
 
    gROOT->LoadMacro(jsf->Env()->GetValue("JSFGUI.InitPythiaMacro","InitPythia.C"));
    py->SetEcm(jsf->Env()->GetValue("JSFGUI.ECM",300.0));
    InitPythia();         // Set Pythia parameters.

}

//_________________________________________________________
void UserInitialize()
{
  //  This function is called at the begining of the job or when
  //  "reset hist" action is selected in the gui menu.
  //  This is used to define/reset histograms.
}

//_________________________________________________________
void UserAnalysis()
{
  // This function is called when the processing of one event is completed.
  // Any data processing of the event can be performed in this function.
  // 


}

//_________________________________________________________
void DrawHist()
{
  //  This function is called to draw histograms during the interactive 
  //  session.  Thus you can see the accumulation of the histogram
  //  interactively.  

}





