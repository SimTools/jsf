//*LastUpdate :  jsf-1-11  27-July-1999  By A.Miyamoto
//*-- Author  : A.Miyamoto  27-Julyr-1998

////////////////////////////////////////////////////////////////////////
//
// JSFReadPaarton
//
// Read Parton data from a file, and store them into JSFSpringParton class
// for subsequent hadronization by JSFHadronize.
// Here parton means quarks, leptons and EW gauge bosons.
// Input data could be Fortran binary file or ASC text file.
// In case of a Fortran binary file, format of each event data is as follows:
//   
// INTEGER*4 NENDIAN
// INTEGER*4 NVERS
// INTEGER*4 NEVENT
// INTEGER*4 NPART
// INTEGER*4 IDAT(7,NPART)
// REAL*4    RDAT(6,NPART)
//
//   IU=1
//   NENDIAN=1296651082
//   NVERS  =101
//   WRITE(IU) NENDIAN, NVERS, NEVENT, NPART,
//  .          ((IDAT(I,J),I=1,7),J=1,NPART),
//  .          ((RDAT(I,J),I=1,6),J=1,NPART)
//
//   where
//    NEVENT : Event number
//    NPART  : Number of partons
//    IDAT(1,i) : Particle ID a la PDG of i-th parton
//        (2,i) : Number of child particles
//        (3,i) : Serial number of 1st child particle. 
//        (4,i) : Serial number of parent particle.
//        (5,i) : helicity of parton.
//        (6,i) : Color singlet group ID.
//        (7,i) : Shower infomation ( 100*Shower level + Shower pair ID)
//      ( Serial number is a number from 1 to NPART )
//    RDAT(1,i) : Mass of i-th parton (GeV)
//        (2,i) : Charge
//        (3,i) : Px (GeV)
//        (4,i) : Py (GeV)
//        (5,i) : Pz (GeV)
//        (6,i) : E(GeV)
//
//   For more information about IDAT, please see
//   begin_html <a href="http://www-jlc.kek.jp/subg/offl/lib/docs/luhadr/node8.html">How to prepare Psring:Parton_List</a> end_html.
// 
//   In case of an ASC File, input data must be prepared so that they can
//   be read by following fortran statements.
//
//     READ(IU,*,END=300) NVERS, NEVENT, NPART
//     DO 200 J = 1, NPART
//       READ(IU,*) (IDAT(I,J),I=1,7),(RDAT(K,J),K=1,6)
// 200 CONTINUE
//
// 300 continue  ! end of data  
// 
//$Id$
//
////////////////////////////////////////////////////////////////////////

#include <TObjectTable.h>
#include <TKey.h>
#include "TRandom.h"
#include "JSFSteer.h"
#include "JSFSpring.h"
#include "JSFSpringParton.h"
#include "JSFReadParton.h"

extern "C" {
extern void readpartonopen_(Int_t *unit, Int_t *type, Char_t *name, Int_t len);
extern void readpartonclose_(Int_t *unit);
extern void readparton_(Int_t *iunit, Int_t *type, 
	Int_t *endian, Int_t *nvers, Int_t *nevent, Int_t *npart,
        Int_t idat[][7], Float_t rdat[][6], Int_t *nret);
};

ClassImp(JSFReadPartonBuf)
ClassImp(JSFReadParton)

//_____________________________________________________________________________
JSFReadParton::JSFReadParton(const char *name, const char *title)
  : JSFSpring(name, title, NULL)
{
  fEventBuf = new JSFReadPartonBuf("JSFReadPartonBuf", 
	 "JSFReadParton event buffer", this);
 
  sscanf(gJSF->Env()->GetValue("JSFReadParton.DataFile","parton.dat"),
	 "%s",fDataFileName);
  sscanf(gJSF->Env()->GetValue("JSFReadParton.Unit","10"),"%d",&fUnit);
  fType=gJSF->Env()->GetValue("JSFReadParton.Format",1);

}


//_____________________________________________________________________________
JSFReadParton::~JSFReadParton()
{
  if( fEventBuf ) { delete fEventBuf; fEventBuf=NULL ; }
}

//_____________________________________________________________________________
Bool_t JSFReadPartonBuf::SetPartons()
{
  //  Read one record from a file and save it in a JSFSpringParton class.

  TClonesArray &partons = *fPartons;

  //  Int_t endian=1296651082 ; 
  Int_t nvers=101;
  Int_t nevent, npart, ivers, iendian;
  static const Int_t MAXPRT=50;
  Int_t idat[MAXPRT][7];
  Float_t rdat[MAXPRT][6];
  Int_t nret;
  Int_t unit=((JSFReadParton*)Module())->GetUnit();
  Int_t type=((JSFReadParton*)Module())->GetType();
  
  readparton_(&unit, &type, &iendian, &ivers, &nevent, &npart,
	      idat, rdat, &nret);

  // *************** Check return status
  if(nret == -1) { 
    printf("In JSFReadPartonBuf::SetParton .. ");
    printf("read end-of-file of the input file.\n");
    return kFALSE;
  }
  else if( nret == -2 ) {
    printf("In JSFReadPartonBuf::SetParton .. ");
    printf("read error detected.\n");
    return kFALSE;
  }
  else if( nvers != ivers ) {
    printf("Errror at JSFReadPartonBuf.. Version number of ");
    printf("data is %d, while program can process only ",nvers);
    printf("version %d\n",ivers);
    return kFALSE;
  }

  // ****************************
  fNparton=npart;
  TVector p(4);
  for(Int_t i=0;i<fNparton;i++){
    p(1)=rdat[i][2];
    p(2)=rdat[i][3];
    p(3)=rdat[i][4];
    p(0)=rdat[i][5];
    new(partons[i]) JSFSpringParton(i+1, idat[i][0], rdat[i][0], rdat[i][1],
		    p, idat[i][1], idat[i][2], idat[i][3], 
		    idat[i][4], idat[i][5], idat[i][6] );
   }

  return kTRUE ;
}


//_____________________________________________________________________________
Bool_t JSFReadParton::BeginRun(Int_t nrun)
{
  if( !IsWritable() ) return kTRUE; // Quit when not output mode.
// Open a file 
  Int_t lenf=strlen(fDataFileName);
  readpartonopen_(&fUnit, &fType, fDataFileName, lenf);
   
  return kTRUE;
}


//_____________________________________________________________________________
Bool_t JSFReadParton::EndRun()
{
  if( !IsWritable() ) return kTRUE;
  readpartonclose_(&fUnit);
  return kTRUE;

}

// ---------------------------------------------------------------
Bool_t JSFReadParton::Process(Int_t nev)
{
//
  JSFReadPartonBuf *simb=(JSFReadPartonBuf*)EventBuf();
  return simb->SetPartons();
}


