// ================================================================-
//
// Class OutputStdhepEvents
// (Function)
// To be used to output events with a StdHep format.
// (Usage)
//  See UserAnalysis.C
// 
// ================================================================-

class OutputStdhepEvents
{
private:
  Int_t fNout;     // Number of output(selected) events
  Int_t fNoutFile; // Number of output files
  Int_t fOutStream;  // output file stream
  Int_t fOutputNEventsPerFile; // Number of output events per file.
  // file is closed, if exceeds this value.  
  // It can be set by the JSF parameter, JSFEventSelection.OutputUnitSize
  std::string fFileNamePrefix; // Output File name prefix
  // It can be set by the JSF parameter, JSFEventSelection.OutputFileNamePrefix
  // Actual file name is 
  //  ${gFileNamePrefix}_${gNoutFile}.stdhep
  // If gutputFileSize <=0, file name is ${gFileNamePrefix}.stdhep
  std::string fStdhepTitle;  // Generator name to be put in the StdHepFile.

  Bool_t fOpen; // True when output file is open
  Int_t fPrintLevel; // If >0, print messages
  Int_t fCurrentNout; // Number of events written in this file.
  std::string fCurrentFileName;  // File name of output file.

  static OutputStdhepEvents *fgInstance;

public:
  OutputStdhepEvents(){ fgInstance = this; Initialize(); }
  static OutputStdhepEvents *GetInstance(){ 
    if( !fgInstance ) { fgInstance = new OutputStdhepEvents(); }
    return fgInstance; 
  }
  virtual ~OutputStdhepEvents(){ 
    if( fOpen ) { CloseFile(); }
  }

  void Initialize();
  Bool_t WriteAnEvent();
  Bool_t OpenFile();
  Bool_t CloseFile();

};

OutputStdhepEvents::fgInstance=0;


// ---------------------------------------------------------------------------
void OutputStdhepEvents::Initialize()
{
  fNout=0;     // Number of output(selected) events
  fNoutFile=0; // Number of output files
  fOutStream=0;  // output file stream
  fOutputNEventsPerFile=gJSF->Env()->GetValue("OutputStdhepEvents.OutputNEventsPerFile",-1);
  fFileNamePrefix=std::string(gJSF->Env()->GetValue("OutputStdhepEvents.FileNamePrefix","SelectedEvents"));
  fStdhepTitle=std::string(gJSF->Env()->GetValue("OutputStdhepEvents.StdhepTitle","JSFEventSelection"));
  
  fOpen=kFALSE;
  fPrintLevel=gJSF->Env()->GetValue("OutputStdhepEvents.PrintLevel",1);
}

// ---------------------------------------------------------------------------
Bool_t OutputStdhepEvents::OpenFile()
{
  if( fOpen ) return kTRUE;

  std::strstream fn;
  fNoutFile++;
  Int_t iok=0;
  Int_t ntry=fOutputNEventsPerFile;

  if( fOutputNEventsPerFile < 0 ) {  
    fn << fFileNamePrefix << ".stdhep" << std::ends; }
    ntry=jsf->Env()->GetValue("JSFGUI.NEventsAnalize",0);
  else { 
    fn << fFileNamePrefix << "_" << fNoutFile << ".stdhep" << std::ends; 
  }
  fCurrentFileName=fn.str();
  if( fNoutFile == 1 && !JSFReadStdHep::IsInitialized() ) {
    JSFWriteStdHep::StdXWINIT(fCurrentFileName.data(), fStdhepTitle.data(), ntry, fOutStream, iok);
  }
  else {
    JSFWriteStdHep::StdXWOPEN(fCurrentFileName.data(), fStdhepTitle.data(), ntry, fOutStream, iok);
  }
  if( iok != 0 ) {
    std::cerr << "Fatal error to open" << fCurrentFileName << " in OutputStdhepEvents::OpenFile" << std::endl;
    return kFalse;
  }
  if( fPrintLevel > 0 ) {
      std::cerr << " File " << fCurrentFileName << " was open to output StdHep file." << std::endl;
  }
  fOpen=kTRUE;
  fCurrentNout=0;
  JSFWriteStdHep::StdFLPYXSEC(ntry);
  Int_t ilbl=100;
  JSFWriteStdHep::StdXWRT(ilbl, fOutStream, iok);
  if( fPrintLevel > 0 ) { std::cerr << "Write Begin Run ... iok=" << iok << std::endl; }
  return kTRUE;
}


// ---------------------------------------------------------------------------
Bool_t OutputStdhepEvents::WriteAnEvent()
{
  if( !fOpen ) {
    if( ! OpenFile() ) { return kFALSE; }
  }
  // Output Event data
  Int_t ilbl=1;
  Int_t iok=0;



  JSFWriteStdHep::StdXWRT(ilbl, fOutStream, iok);
  fNout++;
  fCurrentNout++;

  if( fPrintLevel > 10 ) {
    std::cerr << " OutputStdhepEvents::WriteAnEvent.. fNout=" << fNout 
	      << " fCurrentNout=" << fCurrentNout << std::endl;
  }

  if( fOutputNEventsPerFile < 0 || fNout < fNoutFile*fOutputNEventsPerFile ) return kTRUE;

  if( !CloseFile() ) { return kFALSE; }
  return kTRUE;
}


// ---------------------------------------------------------------------------
Bool_t OutputStdhepEvents::CloseFile()
{
  if( !fOpen ) reuturn kTRUE;
  JSFWriteStdHep::StdXEND(fOutStream);
  if( fPrintLevel > 0 ) {
    std::cerr << "File " << fCurrentFileName << " was cloased " << std::endl;
    std::cerr << "It is " << fNoutFile << "-th file and " << fCurrentNout << " events were written" << std::endl;
    std::cerr << "Total Number of output events is " << fNout << std::endl;
  }
  fOpen=kFALSE;

  return kTRUE;
}

