//****************************************************
//* UserAnalysis.C
//* This is an example to read stdhep file,
//* apply event selection cut, then output events which 
//* satisfies the criteria.
//*
//****************************************************

class OutputStdhepEvents;

OutputStdhepEvents *gOut;

//_________________________________________________________
void UserInitialize()
{
  //  This function is called at the begining of the job or when
  //  "reset hist" action is selected in the gui menu.
  //  This is used to define/reset histograms.

  gROOT->LoadMacro("OutputStdhepEvents.C");
  gOut=new OutputStdhepEvents();

}

//_________________________________________________________
void UserAnalysis()
{
  // This function is called when the processing of one event is completed.
  // Any data processing of the event can be performed in this function.
  // 

  if ( gJSF->GetEventNumber()%100 == 0 ) {
    cerr << "Passed " << gJSF->GetEventNumber() << endl;
  }
  //
  if( gJSF->GetEventNumber() < 10 ) {
     std::cout << "Analizing event " << gJSF->GetEventNumber() << std::endl;
  }

  JSFGenerator *generator=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *genbuf=(JSFGeneratorBuf*)generator->EventBuf();
  
//
  TClonesArray *gen=genbuf->GetParticles();
  TIter nextgp(gen);

  JSFGeneratorParticle *gp=0;
  ANL4DVector pee(0.0,0.0,0.0,0.0);

  Int_t nel=0;
  Double_t acol=0.0;
  Double_t cs1=-100.0;
  Double_t cs2=-100.0;
  Double_t e1=0.0;
  Double_t e2=0.0;

  while( (gp=(JSFGeneratorParticle*)nextgp() ) ) {
    if( gp->GetStatus() != 1 ) continue;
    Int_t apid=TMath::Abs(gp->GetID());

    ANL4DVector p(gp->GetE(), gp->GetPx(), gp->GetPy(), gp->GetPz());
    if( apid == 11 ) {
      nel++;
      if( nel == 1 ) { cs1=p.CosTheta(); e1=p.E();  }
      if( nel == 2 ) { cs2=p.CosTheta(); e2=p.E(); }

      if( nel == 2 ) { acol=pee.Acol(p); }
      pee+=p;      
    }
  }

  Double_t csmax=0.92;

// Output events if it satisfies the condition.

  if( ( TMath::Abs(cs1) < 0.92 || TMath::Abs(cs2) < 0.92 ) 
      && acol < 15.0 ) {
    if( ! gOut->WriteAnEvent() ) {
     gJSF->SetReturnCode(JSFSteer::JSFTerminate);
    }
  }
  else { 
    gJSF->SetReturnCode(JSFSteer::kJSFNoOutput);
  }
  return;
}


//_________________________________________________________
void UserTerminate()
{
  if( gOut ) { delete gOut; }

}

//_________________________________________________________
void DrawHist()
{
  //  This function is called to draw histograms during the interactive 
  //  session.  Thus you can see the accumulation of the histogram
  //  interactively.  


}



