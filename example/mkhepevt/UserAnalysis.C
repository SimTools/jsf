//****************************************************
//*
//* A sample program to create HEPEVT format file 
//* which can be used as an input to Geant4 generator
//*  
//*$Id$
//*
//****************************************************

#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

struct hepevt {
  Int_t status;
  Int_t pid;
  Int_t firstdaughter;
  Int_t lastdaughter;
  Float_t px;
  Float_t py;
  Float_t pz;
  Float_t mass;
};


ofstream ofs("pythia.data");

//_________________________________________________________
void UserInitialize()
{

}

//_________________________________________________________
void UserAnalysis()
{
  // This function is called when the processing of one event is completed.
  // Any data processing of the event can be performed in this function.
  // 

  vector<hepevt> gentrack;

  JSFSIMDSTBuf *sdb=(JSFSIMDSTBuf*)simdst->EventBuf();

  
  // **  If these comments are removed, generator particle information
  //  **  are printed.
  TClonesArray *gen=sdb->GetGeneratorParticles();
  Int_t nskip=0;
  for(Int_t i=0;i<sdb->GetNGeneratorParticles();i++){
    JSFGeneratorParticle *g=gen->UncheckedAt(i);
    
    Int_t mother=g->GetMother();
    if( mother <= 0 ) {
      nskip++;
      continue;
    }

    Int_t ndau=g->GetNDaughter();

    hepevt atrack;
    if( ndau > 0 ) atrack.status=2 ;
    else           atrack.status=1;
    atrack.pid=g->GetID();
    if( ndau < 1 ) {
      atrack.firstdaughter=0;
      atrack.lastdaughter=0;
    }
    else {
      atrack.firstdaughter=g->GetFirstDaughter()-nskip;
      atrack.lastdaughter=atrack.firstdaughter + g->GetNDaughter() - 1;
    }

    atrack.px=g->GetPx();
    atrack.py=g->GetPy();
    atrack.pz=g->GetPz();
    atrack.mass=g->GetMass();

    gentrack.push_back(atrack);

  }

  ofs << gentrack.size() << endl;

  vector<hepevt>::iterator pv=gentrack.begin();
  while( pv != gentrack.end() ) {
    hepevt atrack=*pv++;
    ofs << setw(10) << atrack.status ;
    ofs << setw(10) << atrack.pid ;
    ofs << setw(10) << atrack.firstdaughter ;
    ofs << setw(10) << atrack.lastdaughter ;
    ofs << setw(16) << setprecision(8) << atrack.px;
    ofs << setw(16) << setprecision(8) << atrack.py;
    ofs << setw(16) << setprecision(8) << atrack.pz;
    ofs << setw(16) << setprecision(8) << atrack.mass;
    ofs  << endl;
  }


}

//_________________________________________________________
void DrawHist()
{
}





