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

class JSFJ4;

TCanvas *cHist;

TH1F *hNCDC, *hNVTX, *hNIT, hNCAL;

//_________________________________________________________
void UserInitialize()
{
  //  This function is called at the begining of the job or when
  //  "reset hist" action is selected in the gui menu.
  //  This is used to define/reset histograms.

  hNCDC=new TH1F("hNCDC", "No. of CDC Hits", 100, 0, 200);
  hNVTX=new TH1F("hNVTX", "No. of VTX Hits", 100, 0, 100);
  hNIT=new TH1F("hNIT", "No. of IT Hits", 100, 0, 100);
  hNCAL=new TH1F("hNCAL", "No. of CAL Hits", 100, 0, 200);

}

//_________________________________________________________
void UserAnalysis()
{
  // This function is called when the processing of one event is completed.
  // Any data processing of the event can be performed in this function.
  // 

  cout << "Event Number is " << jsf->GetEventNumber() << endl;

  JSFJ4    *jsfj4=(JSFJ4*)gJSF->FindModule("JSFJ4");
  JSFJ4Buf *jsfj4buf=(JSFJ4Buf*)jsfj4->EventBuf();

  TObjArray *vtxhits=jsfj4buf->GetComponent("VTXHits");
  if ( vtxhits ) { 
    Int_t  nvtx= vtxhits->GetEntries() ;
    //    cout << " Number of entries in VTX is " << nvtx << endl;
    hNVTX->Fill(nvtx);
    /*
    for(Int_t ih=0;ih<vtxhits->GetEntries();ih++){
      JSFJ4VTXHit *vh=(JSFJ4VTXHit*)vtxhits->At(ih);
      vh->Print();
    }
    */
  }


  TObjArray *ithits=jsfj4buf->GetComponent("ITHits");
  if( ithits ) {
    //    cout << " Number of entries in IT is " << ithits->GetEntries() << endl;
    Int_t nit= ithits->GetEntries() ;
    hNIT->Fill(nit);
    /*
    cout << "nit =" << nit << endl;
    for(Int_t ih=0;ih<ithits->GetEntries();ih++){
      JSFJ4ITHit *ith=(JSFJ4ITHit*)ithits->At(ih);
      ith->Print();
    }
    */

  }

  TObjArray *cdchits=jsfj4buf->GetComponent("CDCHits");
  if ( cdchits ) { 
    Int_t  ncdc= cdchits->GetEntries() ;
    hNCDC->Fill(ncdc);
    /*
    cout << " Number of entries in CDC is " << ncdc << endl;
    for(Int_t ih=0;ih<cdchits->GetEntries();ih++){
      JSFJ4CDCHit *ch=(JSFJ4CDCHit*)cdchits->At(ih);
      ch->Print();
    }
    */
  }


  TObjArray *calhits=jsfj4buf->GetComponent("CALHits");
  if ( calhits ) { 
    Int_t  ncal= calhits->GetEntries() ;
    hNCAL->Fill(ncal);
    /*
    cout << " Number of entries in CAL is " << ncal << endl;
    for(Int_t ih=0;ih<calhits->GetEntries();ih++){
      JSFJ4CALHit *calh=(JSFJ4CALHit*)calhits->At(ih);
      calh->Print();
    }
    */
  }

  //
  TObjArray *vertices=jsfj4buf->GetComponent("PrimaryVertices");
  if( vertices ) {
    /*
    cout << " Number of Vertex is " << vertices->GetEntries() << endl;
    for(Int_t iv=0;iv<vertices->GetEntries();iv++){
      JSFJ4PrimaryVertex *v=(JSFJ4PrimaryVertex*)vertices->UncheckedAt(iv);
      v->Print("all");
      //      v->Print();
      //      cout << " number of particles is " << v->GetParticles()->GetEntries() << endl;
    }
    */
  }
  
  
}

//_________________________________________________________
void DrawHist()
{
  //  This function is called to draw histograms during the interactive 
  //  session.  Thus you can see the accumulation of the histogram
  //  interactively.  
  TDirectory *old=gDirectory;
  if( !cHist ) {
    cHist=new TCanvas("cHist","Canvas 1",100, 100, 400, 400);
    cHist->Divide(2,2);
  }
  else {
  }
  cHist->cd(1); hNCDC->Draw();
  cHist->cd(2); hNVTX->Draw();

  cHist->Update();

  old->cd();


}





