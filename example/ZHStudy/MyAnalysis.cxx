//*LastUpdate :  0.04/01  9-November-1998  By Akiya Miyamoto
//*-- Author  : Akiya Miyamoto  9-November-1998

///////////////////////////////////////////////////////////////////
//
//  MyAnalyais
//
//  User Analyais template
//  
//  
//////////////////////////////////////////////////////////////////

#include "JSFSteer.h"
#include "JSFModule.h"
#include "JSFGenerator.h"
#include "JSFQuickSim.h"
#include "PythiaGenerator.h"
#include "TEnv.h"
#include "TVector.h"

#include "MyAnalysis.h"

ClassImp(MyAnalysis)
ClassImp(MyAnalysisBuf)

extern "C" {
extern void jclust_(Int_t *numprt, Float_t *prtlst, const Int_t *lngdat,
		    const Int_t *ipx0, const Int_t *maxcut, Float_t *ycut,
		    Int_t *numcls, Int_t *numjet, Int_t *jettbl,
		    Float_t *jetlst, Int_t *status);
};

//_____________________________________________________________________________
MyAnalysis::MyAnalysis(const char *name, const char *title)
       : JSFModule(name,title)
{
  fEventBuf = new MyAnalysisBuf("MyAnalysisBuf", 
	 "MyAnalysis event buffer",this);
 
  sscanf(gJSF->Env()->GetValue("MyAnalysis.ycut","0.03"),"%g",&fYcut);

}


// ---------------------------------------------------------------
Bool_t MyAnalysis::Initialize()
{
  // Initialize
  // Put histogram defiition here, if needed.

  return kTRUE;
}


// ---------------------------------------------------------------
Bool_t MyAnalysis::Process(Int_t nev)
{
//  My Event analysis, This function is called in every events

   PythiaGenerator *pythia
     =(PythiaGenerator*)gJSF->FindModule("PythiaGenerator");
   MyAnalysisBuf *buf=(MyAnalysisBuf*)EventBuf();
   buf->SetEcm( pythia->GetEcm() );

   // ***************************************
   // Make histograms of simulator information
   // ***************************************

   JSFQuickSim *sim=(JSFQuickSim*)gJSF->FindModule("JSFQuickSim");
   JSFQuickSimBuf *evt=(JSFQuickSimBuf*)sim->EventBuf();
   Int_t nt=evt->GetNtracks();
   TClonesArray *tracks=evt->GetTracks();

   // Accumulate 4 momentum of the event

   TVector ssum(4);
   TVector st(4);
   static const Int_t maxprt=500;
   static const Int_t lngdat=4;
   Float_t prtlst[maxprt][lngdat];
   Int_t nprt=0;  Int_t ipx0=0;
   Float_t maxEP=0.0;
   Float_t maxMuP=0.0;
   for(Int_t i=0;i<nt;i++){
     JSFLTKCLTrack *t=(JSFLTKCLTrack*)tracks->UncheckedAt(i);
     if( t->GetType() == 11 && t->GetECL() > maxEP) { maxEP= t->GetECL(); } 
     if( t->GetType() == 13 && t->GetE() > maxMuP ){ maxMuP=t->GetE(); }
     st=t->GetPV();
     prtlst[nprt][0]=t->GetPx();      prtlst[nprt][1]=t->GetPy(); 
     prtlst[nprt][2]=t->GetPz();      prtlst[nprt][3]=t->GetE(); 
     nprt++;
     ssum += st;  // Sum 4 momentum
   }

   buf->SetLeptonMaxP((Double_t)maxEP, (Double_t)maxMuP);
   buf->SetNcharged(nt);
   buf->SetEvent4V(ssum(0), ssum(1), ssum(2), ssum(3));

   // Clear other variables
   buf->ClearJetInfo();

   // Does jet analysis
   if( nt > 1 )  {
     static const Int_t maxcut=1;
     Float_t ycut[maxcut] ;    ycut[0]=fYcut;
     Int_t numcls[maxcut] ;   
     Int_t numjet[maxprt]; Int_t jettbl[maxprt] ; 
     Float_t jetlst[maxprt][4]; Int_t status;
     jclust_(&nprt, &prtlst[0][0], &lngdat, &ipx0, &maxcut, ycut, 
	   numcls, numjet, jettbl, &jetlst[0][0], &status);
     buf->CalculateVariables((numcls[0]), jetlst);
   }

  return kTRUE;
 
}

// ---------------------------------------------------------------
void MyAnalysisBuf::CalculateVariables(Int_t njet, Float_t jetlst[][4])
{
//  Calculate costh of jets, missing mass of the event, and 
//  and invariant mass of the event.

  fNumjet=njet; 
  for(Int_t i=0;i<TMath::Min(fNumjet, 5);i++){
     fJet[i][0]=jetlst[i][3] ; fJet[i][1]=jetlst[i][0];
     fJet[i][2]=jetlst[i][1] ; fJet[i][3]=jetlst[i][2];
   }

   fPtmiss=TMath::Sqrt(
		   fEvent4V[1]*fEvent4V[1] + fEvent4V[2]*fEvent4V[2]);
   
   //   printf(" e/mu Pmax=%g %g \n",fMaxEP, fMaxMuP);
   //printf(" ptmiss=%g\n",fPtmiss);
   // printf(" event 4 vector is %g %g %g %g\n",
   //	  fEvent4V[0], fEvent4V[1], fEvent4V[2], fEvent4V[3]);

   fCos1=1.0; 
   fCos2=-1.0;
  if( fNumjet > 0 ) {	
    fCos1 = fJet[0][3]/TMath::Sqrt( fJet[0][1]*fJet[0][1] + 
	    fJet[0][2]*fJet[0][2] + fJet[0][3]*fJet[0][3] );
  }
  if( fNumjet > 1) {  
     fCos2 = fJet[1][3]/TMath::Sqrt( fJet[1][1]*fJet[1][1] + 
	  fJet[1][2]*fJet[1][2] + fJet[1][3]*fJet[1][3] );
  }   

  Double_t missmass2 = (fEcm-fEvent4V[0])*(fEcm-fEvent4V[0]) 
     - fEvent4V[1]*fEvent4V[1] - fEvent4V[2]*fEvent4V[2] 
     - fEvent4V[3]*fEvent4V[3];
  if( missmass2 > 0.0 ) fMissmass=TMath::Sqrt(missmass2);
  else fMissmass=-TMath::Sqrt(-missmass2);


  Double_t hmass2 = fEvent4V[0]*fEvent4V[0] -  fEvent4V[1]*fEvent4V[1] 
     - fEvent4V[2]*fEvent4V[2] - fEvent4V[3]*fEvent4V[3] ;
  if( hmass2 > 0.0 ) fHmass=TMath::Sqrt(hmass2);
  else fHmass=-TMath::Sqrt(hmass2);
    
}




