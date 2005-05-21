//*LastUpdate : <<Version>> <<Date>> by <<Author>>
//*-- Author  : <<Author>> <<Date>>

///////////////////////////////////////////////////////////////////
//
//  <<Analysis>>
//
//  <<Comment>>
//
<<+S4Anal>>
//  Sample analysis program 
//  To single-particle generator data.
//  Make ntuples, containig, 
//    (a) Event Basis
//      number of EMCAL/HDCAL hits
//      Total energies of EMCAL/HDCAL hits
//      number of EMCAL/HDCAL clusters
//      Total energies of EMCAL/HDCAL clusters
//      Number of PFO objects
//      Energies of PFO objects
//      phi and costheta of PFO objects
//
//    (b) Hit bases
//      Energy depoits in each EMCAL/HDCAL cells
//      
<<-S4Anal>>
//
//$Id: 
//  
//////////////////////////////////////////////////////////////////

#include <iostream>

#include "JSFSteer.h"
#include "<<Analysis>>.h"

<<+S4Anal>>
#include "TNtuple.h"
#include "S4CALHitMaker.h"
#include "S4CALHit.h"
#include "S4CALTypeDef.h"
#include "S4CALAddress.h"
#include "S4CALExactHit.h"
<<-S4Anal>>

ClassImp(<<Analysis>>)

//_____________________________________________________________________________
<<Analysis>>::<<Analysis>>(const char *name, const char *title)
       : JSFModule(name,title)
{
}

//_____________________________________________________________________________
<<Analysis>>::~<<Analysis>>()
{
}

//_____________________________________________________________________________
Bool_t <<Analysis>>::Initialize()
{
  // Initialize

<<+S4Anal>>
  fNtevent=new TNtuple("evnt","Event basis ntuple","nem:nhd:eem:ehd:nemc:nhdc:npfo:epfo:pfophi:pfocs");

  fNthits=new TNtuple("hitnt","Hit basis ntuple","eme:emphi:emcs:emtof:hde:hdphi:hdcs:hdtof");

<<-S4Anal>>

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t <<Analysis>>::Process(Int_t nev)
{
// Process event data

<<+S4Anal>>

  S4CALHitMaker *chm=(S4CALHitMaker*)gJSF->FindModule("S4CALHitMaker");
  S4CALHitBuf   *chb=(S4CALHitBuf*)chm->EventBuf();
  TObjArray     *chts=chb->GetHits();
  TIter next(chts);
  S4CALHit *ch=0;
  Float_t daev[20];
  Float_t dahit[20];
  for(Int_t i=0;i<20;i++){ daev[i]=0.0;  dahit[i]=0.0; }
// "nem:nhd:eem:ehd:nemc:nhdc:npfo:epfo:pfophi:pfocs"
//    0  1   2   3   4    5    6    7      8     9
// "eme:emphi:emcs:emtof:hde:hdphi:hdcs:hdtof"
//    0   1    2     3    4    5    6    7    
  Int_t nem=0;
  Int_t nhd=0;

  Int_t nprint=0;
  Bool_t  isEM=kFALSE;
  while( (ch=(S4CALHit*)next()) ) {
    if( ch->GetCALType() & ( kgBaEm | kgEcEm )) {
       nem++;
       daev[2]+=ch->GetEnergyDeposit();
       dahit[0]=ch->GetEnergyDeposit();
       TVector3 pos=ch->GetHitPosition();
       dahit[1]=pos.Phi();
       dahit[2]=pos.CosTheta();
       dahit[3]=ch->GetTOF();
       isEM=kTRUE;
    }
    else {
       nhd++;
       daev[3]+=ch->GetEnergyDeposit();
       dahit[4]=ch->GetEnergyDeposit();
       TVector3 pos=ch->GetHitPosition();
       dahit[5]=pos.Phi();
       dahit[6]=pos.CosTheta();
       dahit[7]=ch->GetTOF();
       isEM=kFALSE;
    }

// Get Pointers to the exact hits
// Print cell information of first 20 hits 
    TIter nexh(ch->GetExactHitsPtr());
    S4CALExactHit *ceh=0;
    while( (ceh=(S4CALExactHit*)nexh()) ) {
//       if( nprint < 20 ) { 
         Int_t cellid=ceh->GetCellID();
         std::cerr << " CellID=" << cellid ;
         std::cerr << " ConeID=" << S4CALAddress::GetConeID(cellid);
         std::cerr << " TowerID=" << S4CALAddress::GetTowerID(cellid);
         std::cerr << " MiniConeID=" << S4CALAddress::GetMiniConeID(cellid,isEM);
         std::cerr << " MiniTowerID=" << S4CALAddress::GetMiniTowerID(cellid,isEM);
         std::cerr << " LayerID=" << S4CALAddress::GetLayerID(cellid,isEM);
         std::cerr << std::endl;
	 std::cerr << " CellPosition X=" << ceh->GetCellPosition().X(); 
	 std::cerr << " CellPosition Y=" << ceh->GetCellPosition().Y();
	 std::cerr << " CellPosition Z=" << ceh->GetCellPosition().Z() << std::endl;
         nprint ++ ;
//       }
    }
  }    
  daev[0]=nem;
  daev[1]=nhd;
     
  fNtevent->Fill(daev);
  fNthits->Fill(dahit);

<<-S4Anal>>

  return kTRUE;
}

//_____________________________________________________________________________
Bool_t <<Analysis>>::BeginRun(Int_t nrun)
{
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t <<Analysis>>::EndRun()
{
  return kTRUE;
}

//_____________________________________________________________________________
Bool_t <<Analysis>>::Terminate()
{

<<+S4Anal>>
  fNtevent->Write();
  fNthits->Write();
<<-S4Anal>>
  return kTRUE;
}






