//*LastUpdate:  jsf-1-13   25-Jan-2000  by Akiya Miyamoto
//*LastUpdate:  jsf-1-11   23-July-1999  by Akiya Miyamoto
//*LastUpdate:  jsf-1-7-3  20-April-1999  by Akiya Miyamoto
//*LastUpdate:  v.01.01 04/10/1998  by Akiya Miyamoto
//*-- Author :  04/10/1998  03/10/1998

/*
  25-Jan-2000  Akiya Miyamoto  Add parameter, DebugGenerator.DoHistogram
  23-July-1999 Akiya Miyamoto
          Save random seed in EndRun
  20-Apri-1999 Akiya Miyamoto
          One of the constructor of JSFGeneratorParticle calls constructor 
          twice, that causes the error.
*/

///////////////////////////////////////////////////////////////////
//
// DebugGenerator class
// 
// Generate particles for debugging.  Event type can be specified by
// parameters in jsf.conf file.
// Valid parameters and default values are
//   DebugGenerator.RandomSeed:  12345    # Start random seed.
//   DebugGenerator.Nparticles:      4    # Number of particles per event.
//   DebugGenerator.RangeP:  0.1  10.0    # Minimum and maximum momentum of 
//                                        # particle
//   DebugGenerator.RangeCosth: -1.0 1.0  # Minimu and maximum of Costh
//                                        # ( cosine of production angle.)
//   DebugGenerator.RangeAzimuth: 0.0 360.0 # Minumum and maximum of 
//                                        # particles azimuthal angle in degree.
//   DebugGenerator.RangeVTXR: 0.0 0.0  # min. and max. of radius of particle's
//                                      # production point.
//   DebugGenerator.RangeVTXPhi: 0.0 360.0 # min. and max. of azimuthal angle
//                                      # of particle's production point.
//   DebugGenerator.RangeVTXZ:  0.0 0.0 # min. and max of Z coordinate of
//                                      # particles production point.
//   DebugGenerator.Nspecies:  1  # Number of species of particles.
//   DebugGenerator.Species[n]: 22, 0.0, 0.0  # Particle ID, mass, and charge
//                                            # of [n]-th particle species.
//                                            # [n] is positive integer.
//   DebugGenerator.DoHistogram: YES     # Histogram generated event information.
//   Several kind of particles can be produced in a single event, by 
//   setting Nspecies and Species[n] properly.  
//   "Species[n]" means Species1, Species2, ... 
//
//$Id:
///////////////////////////////////////////////////////////////////


#include "TRandom.h"
#include "TDatime.h"

#include "TList.h"
#include "JSFSteer.h"
#include "DebugGenerator.h"
#include "JSFGenerator.h"


ClassImp(DebugGenerator)

TH1F  *hNparticle, *hPabs;

//_____________________________________________________________________________
DebugGenerator::DebugGenerator(const char *name, const char *title)
       : JSFGenerator(name,title)
{
  fSpec = 0;

  fRandomSeed=gJSF->Env()->GetValue("DebugGenerator.RandomSeed",12345) ;
  fNgen=gJSF->Env()->GetValue("DebugGenerator.Nparticles",4) ;
  sscanf(gJSF->Env()->GetValue("DebugGenerator.RangeP","0.1 10.0"),
	                       "%lg%lg",&fP[0],&fP[1]);
  sscanf(gJSF->Env()->GetValue("DebugGenerator.RangeCosth","-1.0,1.0"),
	                       "%lg%lg",&fCosth[0],&fCosth[1]);
  sscanf(gJSF->Env()->GetValue("DebugGenerator.RangeAzimuth","0.0, 360.0"),
	                       "%lg%lg",&fAzimA[0],&fAzimA[1]);
  sscanf(gJSF->Env()->GetValue("DebugGenerator.RangeVTXR","0.0, 0.0"),
	                       "%lg%lg",&fR[0],&fR[1]);
  sscanf(gJSF->Env()->GetValue("DebugGenerator.RangeVTXPhi","0.0, 360.0"),
	                       "%lg%lg",&fPhi[0],&fPhi[1]);
  sscanf(gJSF->Env()->GetValue("DebugGenerator.RangeVTXZ","0.0, 0.0"),
	                       "%lg%lg",&fZ[0],&fZ[1]);
  fNspecies=gJSF->Env()->GetValue("DebugGenerator.Nspecies",1);
  Char_t argname[40];
  for(Int_t i=0;i<fNspecies;i++){
    sprintf(argname,"DebugGenerator.Species%d",i+1);
    sscanf(gJSF->Env()->GetValue(argname,"22, 0.0, 0.0"),
 	                         "%d%lg%lg",&fID[i],&fMasses[i],&fCharges[i]);
  }

  hNparticle=0;
  hPabs=0;

}

//_____________________________________________________________________________
DebugGenerator::~DebugGenerator()
{
  if( fHistograms ) { delete fHistograms; fHistograms=0; }
  if( hNparticle ) delete hNparticle;
  if( hPabs ) delete hPabs;
}

//_____________________________________________________________________________
Bool_t DebugGenerator::Initialize()
{
  if( !JSFGenerator::Initialize() ) { return kFALSE; }
  if( strcmp(gJSF->Env()->GetValue("DebugGenerator.DoHistograms","YES"),"YES") == 0 ) {
    if( !fHistograms ) fHistograms = new TList();
    Hcreate();
  }

  return kTRUE;
}


//_____________________________________________________________________________
void DebugGenerator::Hcreate()
{
  // Create Histogram for Debug generator

   hNparticle  = new TH1F("hNparticle", "hNparticle", 
			  100, 0, (Float_t)2*fNgen);
   hPabs       = new TH1F("hPabs",  "hPabs", 
			  100, (Float_t)fP[0], (Float_t)fP[1]);

   fHistograms->Add(hNparticle);
   fHistograms->Add(hPabs);

}

//_____________________________________________________________________________
void DebugGenerator::Hfill()
{

   JSFGeneratorBuf *buf=(JSFGeneratorBuf*)EventBuf();
   if( hNparticle ) hNparticle->Fill(buf->GetNparticles());
   
   if( hPabs ) {
     TClonesArray *p=buf->GetParticles();
     for(Int_t i=0;i<buf->GetNparticles();i++){
       JSFGeneratorParticle   *prt = (JSFGeneratorParticle*)p->At(i);
       hPabs->Fill((Float_t)prt->GetPabs());
     }
   }

}

     
//_____________________________________________________________________________
Bool_t DebugGenerator::BeginRun(Int_t nrun)
{
  if( !JSFGenerator::BeginRun(nrun) ) return kFALSE;

  if( IsWritable() ) {
    gRandom->SetSeed(fRandomSeed);  
    Write();  
  }
  else {
    Read("DebugGenerator");
  }
  return kTRUE;
}

     
//_____________________________________________________________________________
Bool_t DebugGenerator::EndRun()
{
   // Save random seed
  if( !JSFGenerator::EndRun() ) return kFALSE;

  if( fFile->IsWritable() ) {  Write(); }

  return kTRUE;

}

     
//_____________________________________________________________________________
Bool_t DebugGenerator::GetLastRunInfo()
{

  Read(GetName());
  gRandom->SetSeed(fRandomSeed);  
  printf("Random seeds for DebugGenerator was reset by ");
  printf("values from a file.\n");

  return kTRUE;

}


//_____________________________________________________________________________
Bool_t DebugGenerator::Process(Int_t ievt)
{
  if( !JSFGenerator::Process(ievt) ) { return kFALSE ; }
 
    JSFGeneratorBuf *buf=(JSFGeneratorBuf*)EventBuf();
    fRandomSeed=gRandom->GetSeed();  // random seed.
    buf->SetStartSeed(fRandomSeed);
    TClonesArray &tracks = *(buf->GetParticles());
    Int_t np=0;
    for(Int_t i=0;i<fNgen;i++){
      Double_t pabs=fP[0]+(fP[1]-fP[0])*gRandom->Rndm();
      Double_t costh=fCosth[0]
               +(fCosth[1]-fCosth[0])*gRandom->Rndm();
      Double_t azim=( fAzimA[0]+(fAzimA[1]
               -fAzimA[0])*gRandom->Rndm())*TMath::Pi()/180.0 ;
      Double_t r=fR[0]+(fR[1]-fR[0])*gRandom->Rndm();
      Double_t phi=(fPhi[0]+(fPhi[1]-fPhi[0])*gRandom->Rndm())
  	       *TMath::Pi()/180.0 ;
      Double_t z=fZ[0]+(fZ[1]-fZ[0])*gRandom->Rndm();

      Double_t sinth=TMath::Sqrt( (1.0-costh)*(1.0+costh));
      TVector p(4);
      p(1)=pabs*sinth*TMath::Cos(azim);
      p(2)=pabs*sinth*TMath::Sin(azim);
      p(3)=pabs*costh;
      TVector x(4);
      x(0)=0.0;
      x(1)=r*TMath::Cos(phi);
      x(2)=r*TMath::Sin(phi);
      x(3)=z;

      Int_t id=fID[fSpec];
      Double_t mass=fMasses[fSpec];
      Double_t charge=fCharges[fSpec];
      p(0)=TMath::Sqrt(p(1)*p(1)+p(2)*p(2)+p(3)*p(3)+mass*mass);

      fSpec++;
      if(fSpec>=fNspecies) { fSpec=0; }

      new(tracks[np]) 
       JSFGeneratorParticle(np+1, id,mass,charge, p, x ) ;
 
      np++;

  }
  buf->SetNparticles(np);
  
  if( fHistograms ) Hfill();

  return kTRUE ;
}

//_____________________________________________________________________________
void DebugGenerator::Print()
{
 
  JSFGeneratorBuf *buf=(JSFGeneratorBuf*)EventBuf();
  printf("**** Number of generated track =%d *********\n",buf->GetNparticles());
  
  TIter next(buf->GetParticles());
  JSFGeneratorParticle *prt;
  
  while((prt=(JSFGeneratorParticle*)next())){
    prt->ls();
  }
 }



//_____________________________________________________________________________
void DebugGenerator::AddSpecies(Int_t ID, Double_t charge, Double_t mass)
{
//  Add particle species to the lists
//
  if( fNspecies >= 9 ) {
    Error("SetSpecies","Number of species exceeds limit(%d)",fNspecies);
    return ;
  }
  fNspecies++;
  fID[fNspecies-1]=ID;
  fCharges[fNspecies-1]=charge;
  fMasses[fNspecies-1]=mass;

}



//_____________________________________________________________________________
void DebugGenerator::PrintParam()
{
//  Print parameters

  printf("\n");
  printf("**** Parameters for Debug Generator ******\n");
  printf("     Number of particles / event : %10d\n",fNgen);
  printf("     Momentum range :  %10.5g to %10.5g (GeV)\n",fP[0],fP[1]);
  printf("     Costh range    :  %10.5g to %10.5g \n",fCosth[0],fCosth[1]);
  printf("     Azimuthal Angle:  %10.5g to %10.5g (degree)\n",fAzimA[0],fAzimA[1]);
  printf("     VTX point R    :  %10.5g to %10.5g (cm)\n",fR[0],fR[1]);
  printf("     VTX point Phi  :  %10.5g to %10.5g (degree)\n",fPhi[0],fPhi[1]);
  printf("     VTX point Z    :  %10.5g to %10.5g (cm)\n",fZ[0],fZ[1]);
  printf("     Number of species %10d\n",fNspecies);
  for(Int_t i=0;i<fNspecies;i++){
  printf("        (ID, Chagre,Mass) = (%5d,%5g,%10g(GeV)) \n",fID[i],fCharges[i],fMasses[i]);
  }

  return;
}

#if __ROOT_FULLVERSION__ >= 30000
//____________________________________________________________________________
void DebugGenerator::Streamer(TBuffer &R__b)
{
   // Stream an object of class DebugGenerator.

   if (R__b.IsReading()) {
     UInt_t R__s, R__c;
     Version_t R__v=R__b.ReadVersion(&R__s, &R__c);
     if( R__v > 1 ) {
       DebugGenerator::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
       return;
     }
     JSFGenerator::Streamer(R__b);
     R__b >> fRandomSeed;
     R__b >> fNgen;
     R__b.ReadStaticArray(fP);
     R__b.ReadStaticArray(fCosth);
     R__b.ReadStaticArray(fAzimA);
     R__b.ReadStaticArray(fR);
     R__b.ReadStaticArray(fPhi);
     R__b.ReadStaticArray(fZ);
     R__b >> fNspecies;
     R__b.ReadStaticArray(fID);
     R__b.ReadStaticArray(fMasses);
     R__b.ReadStaticArray(fCharges);
     R__b.CheckByteCount(R__s, R__c, DebugGenerator::IsA());

   } else {
     DebugGenerator::Class()->WriteBuffer(R__b, this);
   }
}

#else

//______________________________________________________________________________
void DebugGenerator::Streamer(TBuffer &R__b)
{
   // Stream an object of class DebugGenerator.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      JSFGenerator::Streamer(R__b);
      R__b >> fRandomSeed;
      R__b >> fNgen;
      R__b.ReadStaticArray(fP);
      R__b.ReadStaticArray(fCosth);
      R__b.ReadStaticArray(fAzimA);
      R__b.ReadStaticArray(fR);
      R__b.ReadStaticArray(fPhi);
      R__b.ReadStaticArray(fZ);
      R__b >> fNspecies;
      R__b.ReadStaticArray(fID);
      R__b.ReadStaticArray(fMasses);
      R__b.ReadStaticArray(fCharges);
   } else {
      R__b.WriteVersion(DebugGenerator::IsA());
      JSFGenerator::Streamer(R__b);
      R__b << fRandomSeed;
      R__b << fNgen;
      R__b.WriteArray(fP, 2);
      R__b.WriteArray(fCosth, 2);
      R__b.WriteArray(fAzimA, 2);
      R__b.WriteArray(fR, 2);
      R__b.WriteArray(fPhi, 2);
      R__b.WriteArray(fZ, 2);
      R__b << fNspecies;
      R__b.WriteArray(fID, 10);
      R__b.WriteArray(fMasses, 10);
      R__b.WriteArray(fCharges, 10);
   }
}

#endif

