//*LastUpdate:  v0.1.01 13-October-1998 by Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  13-October-1998

//////////////////////////////////////////////////////////////////
//
//  JSFDBS
//  
//  Base class to use data base in the JSF
//  
//  The name of the data base file is obtained from the environment 
//  variable, JSFDBS.FileName.  The default name is "jsfdbs.root".
//  As a default, the file is opened by READONLY mode. 
//  To WRITE to the data base, open by yourself with WRITE mode,
//  set tell its file name by SetFile(file).  Nomal procedure will be
//
//   TFile *f= new TFile("mydbs.root") ;
//   JSFDBS::SetFile(f);
//   MyDBS *d=new MyDBS(data.....)
//
//////////////////////////////////////////////////////////////////


#include "JSFSteer.h"
#include "JSFDBS.h"

ClassImp(JSFDBS)

TFile *JSFDBS::fFile=NULL;

//___________________________________________________________________________
JSFDBS::JSFDBS(Int_t run)
{
//  Run number is a key of the data base.
//  If run > 0, read data when this object is created. 
//  If run <= 0, create the object with empty data.
//  Use this option, when you write to the data base file.

//   if( run > 0 )  ReadDBS(run);  // Put this statement in your constructor

}

//___________________________________________________________________________
JSFDBS::~JSFDBS()
{
}

//___________________________________________________________________________
Bool_t JSFDBS::IsOpen()
{
  if( fFile ) return kTRUE;
  return kFALSE;
}

//___________________________________________________________________________
void JSFDBS::OpenDBS()
{
//  Open Data base.
//  
  if( !fFile ) { 
    fFile = new TFile(gJSF->Env()->GetValue("JSFDBS.FileName","jsfdbs.root"));
    printf(" Database file %s is opened.\n",fFile->GetPath());
  }
  else {
    Warning("Open",
	"A file %s is already opened as a database file.\n",fFile->GetPath());
  }  
}

//___________________________________________________________________________
Char_t *JSFDBS::SetKeyName(Int_t run)
{
   static Char_t name[8];
   sprintf(name,"r%5.5i",run);
   return name;
}


//___________________________________________________________________________
void JSFDBS::ReadDBS(Int_t run)
{ 
   if( !IsOpen() ) OpenDBS();
   TDirectory *last=gDirectory;
   fFile->cd(ClassName());
   Read(SetKeyName(run));
   last->cd();
}

//___________________________________________________________________________
void JSFDBS::WriteDBS(Int_t run)
{
   TDirectory *last=gDirectory;
   fFile->cd();
   if( ! fFile->cd(ClassName()) ) { 
        Warning("WriteDBS","A directory %s will be created",ClassName());
	fFile->mkdir(ClassName());
	fFile->cd(ClassName());
   }
   Write(SetKeyName(run));
   last->cd();
}
