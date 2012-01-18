///////////////////////////////////////////////////////////////////
//
//  JSFWriteStdHep
//
//  Interface to StdHep data.  
//  
//  (29-Nov-2007: Interface using CLHEP library is removed.)
//
//$Id$ 
//  
//////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <stack>

#include "JSFSteer.h"
#include "JSFWriteStdHep.h"
#include "JSFReadStdHep.h"
#include "JSFParticleData.h"
#include "stdhep.h"
#include "TPythia6.h"
#include "hepev4.h"
#include "TRandom.h"

ClassImp(JSFWriteStdHep)
THEPEV4 *JSFWriteStdHep::fHEPEV4=0;

extern "C" {
  extern void stdxwinit_(const char *filename, const char *title, 
			 int *ntries, int *istream, int *iok,
			 int lfilename, int ltitle);
  extern void stdxwopen_(const char *filename, const char *title, 
			 int *ntries, int *istream, int *iok,
			 int lfilename, int ltitle);
  extern void stdxwrt_(int *ilbl, int *istream, int *lok);
  extern void jsf_mcfio_init_();
  extern void stdflpyxsec_(int *ntries);
  extern void stdxend_(int *istream);
  extern void heplst_(int *mlst);
  extern void stdrotboost_(float *the, float *phi, float *bex, float *bey, float *bez);
};

//_____________________________________________________________________________
JSFWriteStdHep::JSFWriteStdHep(const char *name, const char *title )
       : JSFModule(name,title)
{
  fEventBuf=0;
  
  fOutFileName=std::string(gJSF->Env()->GetValue("JSFWriteStdHep.OutputFileName","file.stdhep"));
  fOutTitle=std::string(gJSF->Env()->GetValue("JSFWriteStdHep.OutputTitle","sample"));

  fNTries=gJSF->Env()->GetValue("JSFWriteStdHep.NTries",10);
  fOutStream=0;
  fDebugLevel = gJSF->Env()->GetValue("JSFWriteStdHep.DebugLevel",0);
  fEventSource=gJSF->Env()->GetValue("JSFWriteStdHep.EventSource",1);
  fProcessID=gJSF->Env()->GetValue("JSFWriteStdHep.ProcessID",0);
  fComputeTime=gJSF->Env()->GetValue("JSFWriteStdHep.ComputeTime",0);
  fStdxwrtMode=gJSF->Env()->GetValue("JSFWriteStdHep.StdxwrtMode",4);
  fWriteBeginRun=kTRUE;
  fWriteEndRun=kTRUE;
  fNumberOfWriteEvents=0;

	if(fEventSource == 3){
		TList *list=gJSF->Modules();
		TIter  next(list);
		JSFModule *module;
		while( (module=(JSFModule*)next()) ){
			if( module->InheritsFrom("JSFSpring") ){ 
				if( fSpring ) { 
					printf("Find module %s , inherited from JSFSpring\n",fSpring->GetName());
					printf("More than one JSFSpring are defined.");
					printf(" Specify correspinding Hadronizer explicityly\n");
				}
				fSpring=(JSFSpring*)module;
			}
		}
		if( !fSpring ){ Error("JSFHadronizer","No JSFSpring class was found"); }            			
	}

  fCurrentOutputSizeInkB=0;
  fCurrentNumberOfFiles=0;
  fMaxOutputSizeInkB=gJSF->Env()->GetValue("JSFWriteStdHep.OutputSizePerFileInkB",1900000);
  fMaxNumberOfFiles=gJSF->Env()->GetValue("JSFWriteStdHep.MaxNumberOfFiles",99);
  fCrossingAngle=gJSF->Env()->GetValue("JSFWriteStdHep.CrossingAngle",3.144); // in unit of rad. 
  //   No crossing angle boost, if angle greater than pi (3.14159 )
}

//_____________________________________________________________________________
JSFWriteStdHep::~JSFWriteStdHep()
{

}

//_____________________________________________________________________________
Bool_t JSFWriteStdHep::Initialize()
{


  // Initialize
  //  if ( IsWritable() ) {
  //    jsf_mcfio_init_(); 
  //  }

  TString ftest(fOutFileName.data());
  Int_t iMNO=fMaxNumberOfFiles;
  if ( ftest.EndsWith(".stdhep") ) {
     fMaxNumberOfFiles=1;
  }




  std::cout << "#### JSFWriteStdHep::Initialize === runParameter " << std::endl;
  std::cout << "  Stdhep Output File Name : " << fOutFileName << std::endl;
  std::cout << "  Stdhep Output File Title: " << fOutTitle << std::endl;
  std::cout << "  Stdhep NTries           : " << fNTries << std::endl;
  std::cout << "  Stdhep Debug Level      : " << fDebugLevel << std::endl;
  std::cout << "  Event Source            : " << fEventSource << std::endl;
  std::cout << "    =0 /HEPEVT/, =1 /JETSET/, =2 JSFGenerator " << std::endl;
  std::cout << "  Stdhep ProcessID        : " << fProcessID << std::endl;
  std::cout << "  Data size/file in kB    : " << fMaxOutputSizeInkB << std::endl;
  std::cout << "  Max. number of files     : " << fMaxNumberOfFiles << std::endl;
  if( iMNO != fMaxNumberOfFiles ) { 
    std::cout << "     ( Max. number of files was changed from " << iMNO
  	      << " to 1 because Output File Name ends with .stdhep )" << std::endl;
  }
  std::cout << "  Crossing angle (mrad)    : " << fCrossingAngle << std::endl;
  std::cout << "==============================================" << std::endl;

  return kTRUE;
}
//_____________________________________________________________________________
Bool_t JSFWriteStdHep::BeginRun(Int_t nrun)
{

  fCurrentOutputSizeInkB=0;
  fCurrentNumberOfFiles+=1;
  TString filename=GetStdHepFileName(fCurrentNumberOfFiles);
  Int_t lfilename=filename.Length();
  const Char_t *title=fOutTitle.data();
  Int_t ltitle=fOutTitle.length();
  
  Int_t iok=0;

  if ( JSFReadStdHep::IsInitialized() ) {
    stdxwopen_(filename.Data(), title, &fNTries, &fOutStream, &iok, lfilename, ltitle);
  }
  else {
    stdxwinit_(filename, title, &fNTries, &fOutStream, &iok, lfilename, ltitle);
  }
  if( iok != 0 ) {
    std::cerr << "Fatal error to open StdHep output file. LOK=" << iok << std::endl;
    std::cerr << "  Output file name : " << filename << std::endl;
    std::cerr << "  Output title     : " << fOutTitle << std::endl;
    std::cerr << "  NTries           : " << fNTries << std::endl;
    exit(0);
  }
  std::cerr << "StdHep Output file was opened. File is " <<filename 
	    << " Title is " << fOutTitle << std::endl;

  fWriteBeginRun=kTRUE;

  return kTRUE;
} 

//_____________________________________________________________
TString JSFWriteStdHep::GetStdHepFileName(Int_t iseq)
{
//  If fOutFileName ends with ".stdhep", returns fOutFileName,
//  If Not, return fOutFileName+"_${iseq}.stdhep
//

   TString ftest(fOutFileName.data());
   if ( ftest.EndsWith(".stdhep") ) {
	return ftest;
   }
   if ( iseq < 10 ) {
     ftest+="_0";
   }
   else if ( iseq < 100 ) {
     ftest+="_";
   }
   else {
     std::cerr << "Fatal Error: File sequence number exceeds 99 "
      << " in JSFWriteStdHep::MakeStdhepFileName(..) " << std::endl;
     exit(0);
   }
   ftest+=iseq;
   ftest+=".stdhep";

//   std::cerr << " GetStdHepFileName is " << ftest << std::endl;
   return ftest;

}

//_____________________________________________________________
void JSFWriteStdHep::StdXWINIT(std::string ifilename, std::string ititle,
			 Int_t ntry, Int_t &iostream, Int_t &iok)
{
  const Char_t *filename=ifilename.data();
  Int_t lfilename=ifilename.length();
  const Char_t *title=ititle.data();
  Int_t ltitle=ititle.length();
  
  stdxwinit_(filename, title, &ntry, &iostream, &iok, lfilename, ltitle);  
  
}

//_____________________________________________________________
void JSFWriteStdHep::StdXWOPEN(std::string ifilename, std::string ititle,
			 Int_t ntry, Int_t &iostream, Int_t &iok)
{
  const Char_t *filename=ifilename.data();
  Int_t lfilename=ifilename.length();
  const Char_t *title=ititle.data();
  Int_t ltitle=ititle.length();
  
  stdxwopen_(filename, title, &ntry, &iostream, &iok, lfilename, ltitle);  
}

//_____________________________________________________________
void JSFWriteStdHep::StdXWRT(Int_t ilbl, Int_t ostream, Int_t &iok)
{
  stdxwrt_(&ilbl, &ostream, &iok);

}

//_____________________________________________________________
void JSFWriteStdHep::StdFLPYXSEC(Int_t ntry)
{
  Int_t itry=ntry;
  stdflpyxsec_(&itry);
}

//_____________________________________________________________________________
void JSFWriteStdHep::StdXEND(Int_t iostream)
{
  stdxend_(&iostream);
}

//_____________________________________________________________
Bool_t JSFWriteStdHep::Process(Int_t nev)
{
// 
  int lok=0;

  if( fWriteBeginRun ) {
    fWriteBeginRun=kFALSE;
    stdflpyxsec_(&fNTries);
    Int_t ilbl=100;
    stdxwrt_(&ilbl, &fOutStream, &lok);

    if( lok != 0 ) { 
      std::cerr << "Error  JSFWriteStdHep::BeginRun failed to write BeginRun record ";
      std::cerr << fOutFileName << std::endl;
      return kFALSE;
    }
  }
  
//    Double_t esum=0.0;
//    Double_t pxsum=0.0;
//    Double_t pysum=0.0;
//    Double_t pzsum=0.0;

  // Here we convert JETSET to HEPEVT before Write-Stdhep is called.
  if( fEventSource == 1 ) {
    TPythia6 *py6=TPythia6::Instance();
    Int_t mconv=1; // Convert from JETSET to HEPEVT
    py6->Pyhepc(mconv);
  }
  
  // Fill HEPEVT common block from JSFGenerator
  else if( fEventSource==2 || fEventSource == 3) {

    JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
    JSFGeneratorBuf *gevt=(JSFGeneratorBuf*)gen->EventBuf();
    TClonesArray *pa=gevt->GetParticles();

   hepevt_.nevhep=nev;
    hepevt_.nhep=gevt->GetNparticles();

    Int_t j;

//	std::cerr << " fHEPEV4=" << fHEPEV4 << std::endl;

    hepev4_.eventweightlh = fHEPEV4 ? fHEPEV4->GetEventWeight() : 1.0 ;
    hepev4_.alphaqedlh= fHEPEV4 ? fHEPEV4->GetAlphaQED() : 0.0 ;
    hepev4_.alphaqcdlh= fHEPEV4 ? fHEPEV4->GetAlphaQCD() : 0.0 ;
    if( fHEPEV4 ) {
      for (j=0;j<10;j++) { hepev4_.scalelh[j]=fHEPEV4->GetScale(j); }
    }
    else {
      for (j=0;j<10;j++) { hepev4_.scalelh[j]=0.0; }
    }
    hepev4_.idruplh=fHEPEV4 ? fHEPEV4->GetEventID() : fProcessID ;


		Int_t nSpringParton = 0;
		if(fEventSource==3){
			JSFSpringBuf *spevt=(JSFSpringBuf*)fSpring->EventBuf();
			TClonesArray *ps=spevt->GetPartons();

			nSpringParton=spevt->GetNpartons();
			hepevt_.nhep += nSpringParton;

			for(Int_t j=0;j<nSpringParton;j++){

				JSFSpringParton *p=(JSFSpringParton*)ps->At(j);

				hepev4_.spinlh[j][0]=0.0;
      	hepev4_.spinlh[j][1]=0.0;
      	hepev4_.spinlh[j][2]=0.0;
      	hepev4_.icolorflowlh[j][0]=0;
      	hepev4_.icolorflowlh[j][1]=0;

				hepevt_.isthep[j]=102;
				hepevt_.idhep[j]=p->GetID();
				hepevt_.jmohep[j][0]=p->GetMother();
				hepevt_.jmohep[j][1]=0;
				hepevt_.jdahep[j][0]=p->GetFirstDaughter();
				hepevt_.jdahep[j][1]=p->GetFirstDaughter()+p->GetNDaughter()-1;

				hepevt_.phep[j][0]=p->GetPx();
				hepevt_.phep[j][1]=p->GetPy();
				hepevt_.phep[j][2]=p->GetPz();
				hepevt_.phep[j][3]=p->GetE();
				hepevt_.phep[j][4]=p->GetMass();
				hepevt_.vhep[j][0]=0;
				hepevt_.vhep[j][1]=0;
				hepevt_.vhep[j][2]=0;
				hepevt_.vhep[j][3]=0;
			}
		}


    // compute the production time from the parent's decay distance
    map<Int_t,Double_t> endTimeMap;
    if ( fComputeTime == 1 ) {
      for(j=0;j<gevt->GetNparticles();j++){
        JSFGeneratorParticle *p=(JSFGeneratorParticle*)pa->At(j);
        Int_t parentId = p->GetMother();
        Double_t production_time(0);
        Double_t propagation_time(0);
        if ( parentId > 0 )
          production_time = endTimeMap[parentId-1];
        Int_t dauid=p->GetFirstDaughter();
        if ( dauid > 0 ) {
          JSFGeneratorParticle *pdau = (JSFGeneratorParticle*)pa->At(dauid-1);
          TLorentzVector vec( p->GetPx(), p->GetPy(), p->GetPz(), p->GetE() );
          Double_t beta = vec.Beta();
          TVector3 vtx( p->GetX(), p->GetY(), p->GetZ() );
          TVector3 dauvtx( pdau->GetX(), pdau->GetY(), pdau->GetZ() );
          TVector3 delta( dauvtx - vtx );
          propagation_time = delta.Mag() / beta; // delta T = delta X / beta (in unit of c=1)
        }
        endTimeMap[j] = production_time + propagation_time;
        //printf("mother %d produced at %.2e, daughter %d (%d) propgated %.2e\n",
            //parentId, production_time, j+1, p->GetID(), propagation_time);
      }
    }

    for(j=nSpringParton;j<gevt->GetNparticles()+nSpringParton;j++){
      JSFGeneratorParticle *p=(JSFGeneratorParticle*)pa->At(j-nSpringParton);
#if 0
//    p->ls("all");
//    p->ls();
#endif
      if( fHEPEV4 ) { 
	hepev4_.spinlh[j][0]=fHEPEV4->GetSpin(j, 0);
      	hepev4_.spinlh[j][1]=fHEPEV4->GetSpin(j, 1);
      	hepev4_.spinlh[j][2]=fHEPEV4->GetSpin(j, 2);
      	hepev4_.icolorflowlh[j][0]=fHEPEV4->GetColorFlow(j, 0);
      	hepev4_.icolorflowlh[j][1]=fHEPEV4->GetColorFlow(j, 1);
      }
      else {
	hepev4_.spinlh[j][0]=0.0;
      	hepev4_.spinlh[j][1]=0.0;
      	hepev4_.spinlh[j][2]=0.0;
      	hepev4_.icolorflowlh[j][0]=0;
      	hepev4_.icolorflowlh[j][1]=0;
      }

      hepevt_.isthep[j]=13;
      hepevt_.idhep[j]=p->GetID();
      if (p->GetMother() < 0) {
        hepevt_.jmohep[j][0]=0;
        hepevt_.jmohep[j][1]=0;
      } else {
        hepevt_.jmohep[j][0]=p->GetMother()+nSpringParton;
        hepevt_.jmohep[j][1]=0;
      }
      hepevt_.jdahep[j][0]=p->GetFirstDaughter()+nSpringParton;
      hepevt_.jdahep[j][1]=p->GetFirstDaughter()+p->GetNDaughter()-1+nSpringParton;

      if (p->GetNDaughter() == 0 ) {
	hepevt_.isthep[j]=1; 
        if( p->GetMother() < 0 ) { hepevt_.jmohep[j][0] = 0 ; }
        hepevt_.jdahep[j][0]=0;
        hepevt_.jdahep[j][1]=0;
      }	
      else if( p->GetMother() < 0 && abs(p->GetID()) == 15 ) {
        hepevt_.isthep[j]=2;  // Special treatment of primary tau decay
      }	
      else if( p->GetMother() < 0 && abs(p->GetID()) == 1000015 ) {
        hepevt_.isthep[j]=2;  // Special treatment for semistable stau
      }	
      else if( p->GetMother() < 0 || p->GetNDaughter() > 1000 ) {
        hepevt_.isthep[j]=13;	
        hepevt_.jdahep[j][0]=0;
        hepevt_.jdahep[j][1]=0;
      }
      else if ( p->GetNDaughter() > 0 ) {
        Int_t id=p->GetFirstDaughter();
        JSFGeneratorParticle *pd=id>0 && id<1000 ? (JSFGeneratorParticle*)pa->At(id-1) : 0 ;
	if( !pd || pd->GetNDaughter() > 1000 ) {
          hepevt_.isthep[j]=13;
					//	  hepevt_.jdahep[j][0]=p->GetNDaughter();
					//hepevt_.jdahep[j][1]=p->GetFirstDaughter();
	}
	else {
          hepevt_.isthep[j]=2; 
        }
      } 
//      std::cerr << " j=" << j << " isthep=" << hepevt_.isthep[j]
//        << " jdahep=" << hepevt_.jdahep[j][0] << " " 
//	<< hepevt_.jdahep[j][1]
//	<< " ndau=" << p->fNdaughter
//	<< " n1st=" << p->fFirstDaughter
//	<< " mod="  << p->fMother
//	<< std::endl;


      hepevt_.phep[j][0]=p->GetPx();
      hepevt_.phep[j][1]=p->GetPy();
      hepevt_.phep[j][2]=p->GetPz();
      hepevt_.phep[j][3]=p->GetE();
      hepevt_.phep[j][4]=p->GetMass();
      hepevt_.vhep[j][0]=p->GetX()*10;
      hepevt_.vhep[j][1]=p->GetY()*10;
      hepevt_.vhep[j][2]=p->GetZ()*10;
      hepevt_.vhep[j][3]=p->GetT()*10;
      if ( fComputeTime == 1 ) {
        if ( p->GetMother() > 0 ) {
          hepevt_.vhep[j][3]=endTimeMap[p->GetMother()-1]*10;
        }
      }

//      if ( hepevt_.isthep[j]==1 ) {
//	esum+= hepevt_.phep[j][3];
//        pxsum+=hepevt_.phep[j][0];
//        pysum+=hepevt_.phep[j][1];
//        pzsum+=hepevt_.phep[j][2];
//      }
    }
    HepEvtCheckAndMod();
  }
//
// Output hepevt common as it is.
//
  else if ( fEventSource == 4 ) {
    ForEventSource4(nev);
  }
  else if ( fEventSource == 5 ) {
    ForEventSource5(nev);
  }

// Do boost for crossing angle.
  if( fCrossingAngle < 3.14159265 ) {
    Float_t the=0.0;
    Float_t phi=0.0;
    Float_t bex=TMath::Sin(fCrossingAngle);
    Float_t bey=0.0;
    Float_t bez=0.0;
    stdrotboost_(&the, &phi, &bex, &bey, &bez);

    if( nev < 10 ) {
      std::cout << " hepevt after crossing angle boost ... nev=" << nev << std::endl;
      int mlst=1;
      heplst_(&mlst);
    }
  }


//  std::cerr << "JSFWriteStdHep::... esum=" << esum 
//	<< " pxsum=" << pxsum << " pysum=" << pysum
//	<< " pzsum=" << pzsum << std::endl;

  if ( fCurrentOutputSizeInkB > fMaxOutputSizeInkB ) {
     if ( fCurrentNumberOfFiles >= fMaxNumberOfFiles ) {
        std::cout << "Warning: Number of output stdhep files reached limit (" 
          << fMaxNumberOfFiles << ") in JSFWriteStdhep::Process().  Job will be terminated"
          << std::endl;
        std::cout << "Last event and end run record is not written to the file." << std::endl;
        gJSF->SetReturnCode(JSFSteer::kJSFTerminate);
        return kTRUE;
     }
     StdXEND(fOutStream);
     std::cout << "stdhep file is closed after writting about " << fCurrentOutputSizeInkB 
       << " kB of data.  Current file number is " << fCurrentNumberOfFiles << std::endl;
     std::cout << "Last event number of this file is " << gJSF->GetEventNumber()-1 << std::endl;

     fCurrentNumberOfFiles++;
     fCurrentOutputSizeInkB=0;
     TString nextfile=GetStdHepFileName(fCurrentNumberOfFiles);

     const Char_t *nextfilename=nextfile.Data();
     Int_t   lnextfilename=nextfile.Length();
     const Char_t *title=fOutTitle.data();
     Int_t   ltitle=fOutTitle.length();
     Int_t iok=0;
     stdxwopen_(nextfilename, title, &fNTries, &fOutStream, &iok, lnextfilename, ltitle);
     if ( iok != 0 ) {
       std::cout << "Fatal error to open StdHep output file. LOK=" << iok << std::endl;
       std::cout << "  Output file name : " << nextfilename << std::endl;
       std::cout << "  Output title     : " << fOutTitle << std::endl;
       std::cout << "  NTries           : " << fNTries << std::endl;
       exit(0);
     }
     std::cout << "StdHep Output file was opened. File is " << nextfilename
            << " Title is " << fOutTitle << std::endl;

     stdflpyxsec_(&fNTries);
     Int_t ilbl=100;
     stdxwrt_(&ilbl, &fOutStream, &lok);

     if( lok != 0 ) {
       std::cout << "Error in JSFWriteEvent::Process() .. failed to write beging run record " << std::endl;
       std::cerr << "  Output file name : " << nextfilename << std::endl;
       exit(0);
     }
  }


  Int_t ldlb=fStdxwrtMode;
  stdxwrt_(&ldlb, &fOutStream, &lok);

  if( lok != 0 ) {
    std::cerr << " WriteStdHep::Process  Error to output event " 
	      << nev << std::endl;
    return kFALSE;
  }
  fNumberOfWriteEvents++;
// * Check Output data size.
  Int_t nhep=hepevt_.nhep;
  Double_t outsize=(Double_t)(( 8*( 13+3*nhep+9*nhep ) + 4*( 1+2*nhep+2+6*nhep) )/ 1000.0);
  if( fStdxwrtMode == 4 ) {
    outsize=(Double_t)(( 8*( 13+3*nhep+9*nhep + 13+3*nhep) 
           + 4*( 1+2*nhep+2+6*nhep+1+2*nhep) )/ 1000.0);
  }
  Int_t ioutsize=(Int_t)(outsize*0.77);
  fCurrentOutputSizeInkB+=ioutsize;

  return kTRUE;
}


//____________________________________________________________________________
void JSFWriteStdHep::HepEvtCheckAndMod()
{
// Make sure that Particles of Status 1 or 2 is not the parent of particles 
// of status =13.  Mokka (lcio's StdhepReader fails in such case, because 
// it re-creates the mother-daughter relation from daughter information.
// This is the case of Mokka-v01-06
//
  bool imod=false;
  for(int i=hepevt_.nhep-1; i>=0 ; i--) {
    if ( hepevt_.jdahep[i][0] == 0 && hepevt_.jdahep[i][1]==0 ) { continue; }
    if ( hepevt_.isthep[i] == 13 ) { continue ; }
    
    std::stack<int> s;
    for(int ip=hepevt_.jdahep[i][0];ip<=hepevt_.jdahep[i][1];ip++) {
      s.push(ip-1);
    }
    while( ! s.empty() ) {
      int j=s.top();
      s.pop();
      if ( hepevt_.isthep[j] == 13 ) {
        if ( ! imod ) {
          std::cout << "JSFWriteStdhep::HepEvtCheckAndMod ... Special editting of HepEvt record to avoid error in Mokka" << std::endl;
        }
        std::cout << "JSFWriteStdhep::HepEvtCheckAndMod:  Status code of line " << i << " is changed from " << hepevt_.isthep[i] << " to 13" << std::endl;
        hepevt_.isthep[i] = 13;
        imod=true;
        break; 
      }
      else if ( hepevt_.jdahep[j][0] != 0 && hepevt_.jdahep[j][1] != 0 ) {
        for(int jp=hepevt_.jdahep[j][0];jp<=hepevt_.jdahep[j][1];jp++) {
          s.push(jp-1);
        }
        if( s.size() > (unsigned int)hepevt_.nhep ) {
	  std::cerr << "######## F A T A L    E R R O R ##########################################################" << std::endl;
	  std::cerr << "Fatal error in JSFWriteStdHep::HepEvtCheckAndMod -- Stack size exceeded the number of particles " << std::endl;
          std::cerr << " Stack size=" << s.size() << " number of particles =" << hepevt_.nhep << " Event number=" << gJSF->GetEventNumber() << std::endl;
	  std::cerr << " Error happened while verifying " << i << "-th partile " << std::endl;
          std::cerr << " This error can happen by an error in generator data, wrong mother-daughter relation, etc." << std::endl;
          std::cerr << " List of HepEvt buffer will follow." << std::endl;
          int mlst=1;
          heplst_(&mlst);
          std::cerr << " Please check your generator.  The execution is fored to end here." << std::endl;
          exit(-1);
        }
      }
    }
  }
  if ( imod ) {
    int mlst=1;
    std::cout << "JSFWriteStdHep::HepEvtCheckAndMod ... Dump hepevt record after modification." << std::endl; 
    heplst_(&mlst);
  }
}

//_____________________________________________________________________________
Bool_t JSFWriteStdHep::EndRun()
{
  //  if ( IsWritable() ) {
  Int_t lok=0;
  if( fWriteEndRun ) {
    fWriteEndRun=kFALSE;
    Int_t ilbl=200;
    stdxwrt_(&ilbl, &fOutStream, &lok);
    if( lok != 0 ) { 
      std::cerr << "Error  JSFWriteStdHep::EndRun failed to write EndRun record ";
      std::cerr << fOutFileName << std::endl;
      return kFALSE;
    }
  }

  std::cout << "JSFWriteStdHep reached EndRun.. " << std::endl;
  std::cout << "   Last event number      : " << gJSF->GetEventNumber() << std::endl;
  std::cout << "   Number of write events : " << fNumberOfWriteEvents << std::endl;
  std::cout << "   Last file number       : " << fCurrentNumberOfFiles << std::endl;
  std::cout << "   Size of last file      : " << fCurrentOutputSizeInkB << " kB " << std::endl;
  std::cout << "   Last file name         : " << GetStdHepFileName(fCurrentNumberOfFiles) << std::endl; 

  //  }
  return kTRUE;
}

//____________________________________________________________________________
void JSFWriteStdHep::ForEventSource4(int nev)
{
    JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
    JSFGeneratorBuf *gevt=(JSFGeneratorBuf*)gen->EventBuf();
    TClonesArray *pa=gevt->GetParticles();

    hepevt_.nevhep=nev;
    hepevt_.nhep=gevt->GetNparticles();

    Int_t j;

    hepev4_.eventweightlh = 1.0;
    hepev4_.alphaqedlh= 0.0 ;
    hepev4_.alphaqcdlh= 0.0 ;
    for (j=0;j<10;j++) { hepev4_.scalelh[j]=0.0; }
    hepev4_.idruplh=fProcessID ;


    for(j=0;j<gevt->GetNparticles();j++){
      JSFGeneratorParticle *p=(JSFGeneratorParticle*)pa->At(j);
      const float *spinv=p->GetSpin();
      const int   *colorflow=p->GetColorFlow();
      hepev4_.spinlh[j][0]=spinv[0];
      hepev4_.spinlh[j][1]=spinv[1];
      hepev4_.spinlh[j][2]=spinv[2];
      hepev4_.icolorflowlh[j][0]=colorflow[0];
      hepev4_.icolorflowlh[j][1]=colorflow[1];

      hepevt_.isthep[j]=p->GetStatus();
      hepevt_.idhep[j]=p->GetID();
      hepevt_.jmohep[j][0]=p->GetMother();
      hepevt_.jmohep[j][1]=p->GetSecondMother();
      hepevt_.jdahep[j][0]=p->GetFirstDaughter();
      hepevt_.jdahep[j][1]=p->GetFirstDaughter()+p->GetNDaughter()-1;
      if( p->GetNDaughter() == 0 ) {
        hepevt_.jdahep[j][1]=0;
      }

      hepevt_.phep[j][0]=p->GetPx();
      hepevt_.phep[j][1]=p->GetPy();
      hepevt_.phep[j][2]=p->GetPz();
      hepevt_.phep[j][3]=p->GetE();
      hepevt_.phep[j][4]=p->GetMass();
      hepevt_.vhep[j][0]=p->GetX()*10.0;
      hepevt_.vhep[j][1]=p->GetY()*10.0;
      hepevt_.vhep[j][2]=p->GetZ()*10.0;
      double lightspeed=29.792458 ; // 29.792458 mm/nsec
      hepevt_.vhep[j][3]=p->GetT()*lightspeed;

   }

//   int two=2;
//   heplst_(&two);
}


// ______________________________________
void JSFWriteStdHep::ForEventSource5(int nev)
{
    JSFGenerator *gen=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
    JSFGeneratorBuf *gevt=(JSFGeneratorBuf*)gen->EventBuf();
    TClonesArray *pa=gevt->GetParticles();
 
    hepevt_.nevhep=nev;
    hepevt_.nhep=gevt->GetNparticles();

    Int_t j;
 
    hepev4_.eventweightlh = 1.0;
    hepev4_.alphaqedlh= 0.0 ;
    hepev4_.alphaqcdlh= 0.0 ;
    for (j=0;j<10;j++) { hepev4_.scalelh[j]=0.0; }
    hepev4_.idruplh=fProcessID ;
 
    // randomize particle lifetime if nonzero
    map<Int_t,Double_t> lifetimeMap;
    for(j=0;j<gevt->GetNparticles();j++){
      JSFGeneratorParticle *p=(JSFGeneratorParticle*)pa->At(j);
      double lifetime = p->GetLifeTime();
      if (lifetime > 0) {
        double gamma = p->GetE() / p->GetMass();
        double lifetimeRnd = gRandom->Exp( lifetime*gamma );
        lifetimeMap[j] = lifetimeRnd;
      } else {
        lifetimeMap[j] = 0;
      }
    }

    // compute the production time from the parent's lifetime
    map<Int_t,Double_t> endTimeMap;
    map<Int_t,Double_t> productionTimeMap;
    for(j=0;j<gevt->GetNparticles();j++){
      JSFGeneratorParticle *p=(JSFGeneratorParticle*)pa->At(j);
      Int_t parentId = p->GetMother();
      Double_t production_time(0);
      Double_t propagation_time(0);

      if ( parentId > 0 )
        production_time = endTimeMap[parentId-1];

      productionTimeMap[j] = production_time;

      propagation_time = lifetimeMap[j];

      endTimeMap[j] = production_time + propagation_time;
      //printf("particle #%d [%d] propgated from %.2e to %.2e (diff=%.2e)\n",
          //j, p->GetID(), production_time, endTimeMap[j], propagation_time);
    }

    for(j=0;j<gevt->GetNparticles();j++){
      JSFGeneratorParticle *p=(JSFGeneratorParticle*)pa->At(j);
      const float *spinv=p->GetSpin();
      const int   *colorflow=p->GetColorFlow();
      hepev4_.spinlh[j][0]=spinv[0];
      hepev4_.spinlh[j][1]=spinv[1];
      hepev4_.spinlh[j][2]=spinv[2];
      hepev4_.icolorflowlh[j][0]=colorflow[0];
      hepev4_.icolorflowlh[j][1]=colorflow[1];

      //hepevt_.isthep[j]=p->GetStatus();
      if (p->GetNDaughter() == 0 ) {
        hepevt_.isthep[j]=1; 
      } else {
        hepevt_.isthep[j]=2; 
      }
      hepevt_.idhep[j]=p->GetID();
      hepevt_.jmohep[j][0]=p->GetMother();
      hepevt_.jmohep[j][1]=p->GetSecondMother();
      hepevt_.jdahep[j][0]=p->GetFirstDaughter();
      hepevt_.jdahep[j][1]=p->GetFirstDaughter()+p->GetNDaughter()-1;
      if( p->GetNDaughter() == 0 ) {
        hepevt_.jdahep[j][1]=0;
      }

      hepevt_.phep[j][0]=p->GetPx();
      hepevt_.phep[j][1]=p->GetPy();
      hepevt_.phep[j][2]=p->GetPz();
      hepevt_.phep[j][3]=p->GetE();
      hepevt_.phep[j][4]=p->GetMass();
      hepevt_.vhep[j][0]=p->GetX()*10;
      hepevt_.vhep[j][1]=p->GetY()*10;
      hepevt_.vhep[j][2]=p->GetZ()*10;
      //hepevt_.vhep[j][3]=p->GetT()*10;
      hepevt_.vhep[j][3]=productionTimeMap[j]*10;
   }
}

//_____________________________________________________________________________
Bool_t JSFWriteStdHep::Terminate()
{
  if( fOutStream >= 0 ) {
    stdxend_(&fOutStream);
  }

  return kTRUE;
}

