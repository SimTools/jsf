Int_t readj4()
{

  JSFSteer *jsf = new JSFSteer();

  TFile *file=new TFile("jsfj4.root");
  file->ls();

  jsf->SetIOFiles();

  cout << "IOFiles are set" << endl;

  jsf->Initialize();

  cout << " End of jsf-Inititliae" << endl;

  JSFJ4 *jsfj4=0;
  JSFJ4Buf *jsfj4buf;
  for(Int_t ev=1;ev<1000;ev++){
    if( !jsf->GetEvent(ev) ) break;

    cout << "Event number is " << jsf->GetEventNumber() << endl;


    jsfj4=(JSFJ4*)jsf->FindModule("JSFJ4");
    cout << " jsfj4=" << jsfj4 << endl;
    jsfj4buf=(JSFJ4Buf*)jsfj4->EventBuf();
    cout << " jsfj4buf=" << jsfj4buf << endl;
    TObjArray *vtxhits=jsfj4buf->GetComponent("VTXHits");
    Int_t nhits= vtxhits->GetEntries() ;
    cout << " Number of vtxhits is " << nhits << endl;
    /*
    for(Int_t ih=0;ih<nhits;ih++){
      JSFJ4VTXHit *ph=(JSFJ4VTXHit*)vtxhits->At(ih);
      
      ph->Print();
      //      cout << " Track ID is " << ph->GetTrackID() << endl;
      //      cout << " Paritlce ID is " << ph->GetParticleID() << endl;
      //      cout << " Layer ID is " << ph->GetLayerID() << endl;
    }
    */

    TObjArray *cdchits=jsfj4buf->GetComponent("CDCHits");
    cout << " cdchits=" << cdchits << endl;
    cout << "Number of CDC hits is " << cdchits->GetEntries() << endl;
    /*
    for(Int_t ih=0;ih<cdchits->GetEntries();ih++){
      JSFJ4CDCHit *ch=(JSFJ4CDCHit*)cdchits->At(ih);
      ch->Print();
    }
    */


  }

  jsf->Terminate();


}
