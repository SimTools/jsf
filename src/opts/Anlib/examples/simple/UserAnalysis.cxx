#include "TApplication.h"
#include "Anlib.h"

//_____________________________________________________________________
//*------------------------*//
//* User Analysis          *//
//*------------------------*//
void UserAnalysis()
{
   // Generate fake events

   Double_t e, px, py, pz;
   Int_t ntrk = 30;
   TObjArray tracks;
   for (Int_t i = 0; i < ntrk; i++) {
      switch (i/10) {
      	 case 0:
      	 	px = 1. + 0.02*(i%10);
      	 	py = pz = 0.;
      	 	break;
      	 case 1: 
      	 	py = 1. + 0.02*(i%10);
      	 	px = pz = 0.;
      	 	break;
      	 case 2:
      	 	pz = 1. + 0.02*(i%10);
      	 	px = py = 0.;
      	 	break;
      	 default:
      	 	px = 1. + 0.02*(i%10);
      	 	py = pz = 0.;
      	 	break;
      }
      e = px + py + pz;
      cerr << "gonna create ANL4DVector" << endl;
      tracks.Add(new ANL4DVector(e,px,py,pz));
   }
   
   // Test ANL4DVector Class

   cerr << endl << "-----------------------------------------------";
   cerr << endl << "---------------------";
   cerr << endl << "Lockable LVector Test";
   cerr << endl << "---------------------" << endl;

   TIter next(&tracks);
   ANL4DVector *t;
   while ( (t = (ANL4DVector *)next()) ) {
   	if (t->IsOnHeap()) cerr << (void *)t << " is on heap." << endl;
   	t->DebugPrint();
   }

   
   // Test ANLPairCombiner Class

   cerr << endl << "-----------------------------------------------";
   cerr << endl << "------------------";
   cerr << endl << "Pair Combiner Test";
   cerr << endl << "------------------" << endl;

   ANLPairCombiner pair(tracks,tracks);
   ANLPair *p;
   while ( (p = (ANLPair *)pair()) )  (*p)().DebugPrint();

   
   // Test ANLJetFinder Class

   cerr << endl << "-----------------------------------------------";
   cerr << endl << "---------------";
   cerr << endl << "Jet Finder Test";
   cerr << endl << "---------------" << endl;

   Double_t ycut = 0.005;
   cerr << "Test 1: initilize and do find jets with Ycut = " << ycut << endl;
   ANLJadeEJetFinder jclust(ycut);
   jclust.Initialize(tracks);
   jclust.FindJets();
   cerr << "Njets = " << jclust.GetNjets() << endl;
   TObjArray &jets = jclust.GetJets();
   TIter nextjet(&jets);
   ANLJet *jet;
   while ( (jet = (ANLJet *)nextjet()) ) jet->DebugPrint();
   
   ycut = 0.015;
   cerr << "Test 2: resume with Ycut = " << ycut << endl;
   jclust.SetYcut(ycut);
   jclust.FindJets();
   cerr << "Njets = " << jclust.GetNjets() << endl;
   nextjet.Reset();
   while ( (jet = (ANLJet *)nextjet()) ) jet->DebugPrint();
   
   ycut = 0.05;
   cerr << "Test 3: copy and resume with Ycut = " << ycut << endl;
   ANLJadeEJetFinder jclust2(jclust);
   jclust2.SetYcut(ycut);
   jclust2.FindJets();
   cerr << "Njets = " << jclust2.GetNjets() << endl;
   TObjArray &jets2 = jclust2.GetJets();
   TIter nextjet2(&jets2);
   while ( (jet = (ANLJet *)nextjet2()) ) jet->DebugPrint();
   
   Int_t njets = 2;
   cerr << "Test 4: force " << njets << " jets" << endl;
   jclust.ForceNJets(njets);
   cerr << "Ycut = " << jclust.GetYcut() << endl;
   nextjet.Reset();
   while ( (jet = (ANLJet *)nextjet()) ) jet->DebugPrint();
}

int main (int argc, char **argv)
{
   TApplication("App", &argc, argv);

   UserAnalysis();
   return 0;
}
