//*LastUpdate:  jsf-1-12 31-July-1999  Akiya Miyamoto
//*-- Author :  Akiya Miyamoto  31-July-1999 Akiya Miyamoto
//////////////////////////////////////////////////////////////////
//
//  JSFEnv
//
//  JSF Environment parameter. 
//  This class is inhrited from TEnv, adding functions such as
//   (1) Store use environment variables.
//   (2) In the configuration file, user can specify not only a value
//       of parameter, but also description of parameter and 
//       alias name which can be specified at the command argument.
//   
//$Id$
//////////////////////////////////////////////////////////////////

#include <strings.h>
#include <TSystem.h>
#include <TApplication.h>
#include <TROOT.h>
#include "JSFEnv.h"

ClassImp(JSFEnv)
ClassImp(JSFEnvRec)

//___________________________________________________________
JSFEnv::JSFEnv(Char_t *name) : TEnv(name)
{
  // Create a resource table and read resource file.

  fDefined  = new TOrdCollection(1000);
  fObtained = new TOrdCollection(1000);

  TOrdCollection *ftbl=GetTable();
  ftbl->Clear();
    
  Char_t *s=gSystem->ConcatFileName(gSystem->Getenv("JSFROOT"),"macro/defaults.conf");

  ReadFile(s, kEnvGlobal);

  fEnvFileName=new Char_t[strlen(name)+1];
  strcpy(fEnvFileName, name);
  ReadFile(name, kEnvLocal);


  GetArguments();
}

//__________________________________________________________
void JSFEnv::ReadFile(const char *n, EEnvLevel l)
{
  TEnv::ReadFile(n, l);

  // Read file again to prepare parameter for JSF.

  FILE *fd;
  if( !(fd=fopen(n,"r") ) ) return;

  Int_t nadd=0;
  Char_t inp[256];
  enum EStatus { kHeader, kContHeader, kNewParameter, kNewArgument, 
		kNewComment, kContComment };
  Int_t istat=kHeader;
  Int_t lstat=istat;
  Char_t name[80], value[80], type[80];
  Char_t help[4096], arg[80];  EEnvLevel level=kEnvLocal;
  Bool_t defined=kFALSE;
  Int_t  nread=0;
  while( fgets(inp,256,fd) != NULL ) {

    if(strlen(inp) == strspn(inp," ")+1 ) continue;
    nread++;
    if(inp[0] == '#' ) {
      if( inp[1] == '!' ) { istat = kNewArgument; }
      else if( nread == 1 ) { istat = kHeader; }
      else if( istat == kHeader || istat == kContHeader ) 
	{ istat = kContHeader ; }
      else if( istat == kNewComment || istat == kContComment ) 
	{ istat = kContComment; }
      else if( istat == kNewParameter || istat == kNewArgument ) 
	{ istat = kNewComment; }
      else {
	printf("Error in JSFEnv::ReadFile() .. ");
	printf("invalid sequence of input parameters from a file %s\n",n);
	printf("Last stat=%d, Last input was\n %s",lstat,inp);
	gSystem->Exit(-1);
      }
    }
    else {
      istat = kNewParameter;
    }

    if( istat == kNewParameter && defined ) {
      JSFEnvRec *er;
      if(nadd == 0 && strcmp(name,"HEADER") != 0 ) {
	er=Lookup("HEADER");
	if( er==0 ) {
	  if( nadd == 0 ) fObtained->AddFirst(new JSFEnvRec("HEADER", "HEADER", 
		  "HEADER", level, "First Dummy Entry"));
	  else fObtained->AddAt(new JSFEnvRec("HEADER", "HEADER", "HEADER", level, 
				     "First Dummy Entry"),nadd);
	  nadd++;
	}
      }
      er=Lookup(name);
      if( er!= 0 ) fObtained->Remove(er);
      if( nadd == 0 ) 
	fObtained->AddFirst(new JSFEnvRec(name, type, value, level, help, arg));
      else 
	fObtained->AddAt(new JSFEnvRec(name, type, value, level, help, arg),nadd);
      nadd++;
      defined = kFALSE;
    }

    Int_t lv=0;
    Int_t larg=0;
    char *bgn=0;
    switch (istat) {
    case kHeader:
         strcpy(name,"HEADER");
	 strcpy(value,"HEADER");
	 strcpy(type,"HEADER");
	 level=l;
	 strcpy(help,inp+1);
	 strcpy(arg,"");
	 defined=kTRUE;
	 break;

      case kContHeader:
         strcat(help,inp+1);
	 break;

      case kNewParameter:
	 defined=kTRUE;
	 char *com;
	 com=index(inp,'#');
	 if( com != NULL ) {
	   strcpy(help,com+1);
	   *com=0;
	 }
	 strcpy(name,inp);
	 char *col;
	 col=index(inp,':');
	 if( col == NULL ) {
  	    printf("Error in JSFEnv::ReadFile() .. ");
	    printf("No value is given to the parameter. Errorneous line is\n");
	    printf("%s\n",inp);
	    printf("File is %s\n",n);
 	    gSystem->Exit(-1);
	 }
	 strcpy(value, col+1);
	 lv=strlen(value);
	 if( *(value+lv-1) == '\n' ) *(value+lv-1)= 0;
         *col=0;
	 while( (*(col-1)==' ') ) { 
	   col--;  
	   if( (UInt_t)inp-(UInt_t)col < 2 ) { 
	     printf("Fatal error in JSFEnv::ReadFile ..");
	     printf("No valid parameter name is found. Last input line was\n");
	     printf("[%s]\n",inp);
	     printf("while looking at last character [%c]\n",*col);
	     printf(" col=%d inp=%d\n",(UInt_t)col, (UInt_t)inp);
	     gSystem->Exit(-1);
	   }
 	   *col=0;
	 }
	 bgn=inp;
	 while( *bgn == ' ') { 
	   bgn++; 
	   if( bgn > col ) {
	     printf("Fatal error in JSFEnv::ReadFile ..");
	     printf("No valid parameter name is found. Last input line was\n");
	     printf("[%s]\n",inp);
	     printf("while looking at begin character [%c]\n",*bgn);
	     gSystem->Exit(-1);
 	   }
	 }
         strcpy(name, bgn);
	 level=l;
	 *help=0;
	 *arg=0;
	 break;

      case kNewArgument:
        strcpy(arg,inp+2);
	larg=strlen(arg);
        if( *(arg+larg-1) == '\n' ) *(arg+larg-1)= 0;
	break;
	 
      case kNewComment:
	strcat(help,inp+1);
	break;

      case kContComment:
        strcat(help,inp+1);
	break;

    }
    lstat=istat;
  }

  if( defined ) {
    JSFEnvRec *er=Lookup(name);
    if( er!= 0 ) { fObtained->Remove(er); }
    if( nadd == 0 ) 
      fObtained->AddFirst(new JSFEnvRec(name, type, value, level, help, arg));
    else 
      fObtained->AddAt(new JSFEnvRec(name, type, value, level, help, arg),nadd);
    defined = kFALSE;
  }

  fclose(fd);

}


//__________________________________________________________
JSFEnv::~JSFEnv()
{
  if( fDefined ) {
    fDefined->Delete();
    SafeDelete(fDefined);
  }
  if( fObtained ) {
    fObtained->Delete();
    SafeDelete(fObtained);
  }
  delete fEnvFileName ;
}

//__________________________________________________________
const char *JSFEnv::GetValue(const char *name, const char *dflt)
{
  return TEnv::GetValue(name, dflt);
}

//__________________________________________________________
int JSFEnv::GetValue(const char *name, int dflt)
{
  return TEnv::GetValue(name, dflt);
}

//__________________________________________________________
void JSFEnv::SetValue(const char *name, const char *value, 
		      EEnvLevel l, const char *t)
{
  //  Change TEnv and JSFEnv parameter value.

  TEnv::SetValue(name, value, l, t);

  // Find JSFEnvRec and update it.

  JSFEnvRec *er=Lookup(name);
  if( er!= 0 ) er->ChangeValue(value, kEnvChange);
  else fObtained->AddAt(new JSFEnvRec(name, "", value, kEnvChange),1);
  return;
}

//__________________________________________________________
JSFEnvRec *JSFEnv::Lookup(const char *name)
{
  // Find JSFEnvRec.

  TIter next(fObtained);
  JSFEnvRec *er;
 
  while ((er = (JSFEnvRec*)next() )) {
    if( er->fName == name ) return er;
  }

  return 0;
}

//___________________________________________________________
JSFEnvRec::JSFEnvRec(const char *n, const char *t, const char *v, EEnvLevel l,
            const char *help, const char *arg)
{
  fName=n;
  fType=t;
  fValue=v;
  fLevel=l;
  fHelp=help;
  fArgument=arg;
}


//___________________________________________________________
void JSFEnvRec::ChangeValue(const char *v, EEnvLevel l)
{
  //  printf("JSFEnvRec-1..%s changed from %s to %s\n",fName.Data(),fValue.Data(),v);
  fValue=v;
  fLevel=l;
}

//___________________________________________________________
void JSFEnvRec::ChangeValue(const char *t, const char *v, 
			    EEnvLevel l, const char *help, const char *arg)
{
  // printf("JSFEnvRec-2..%s changed from %s to %s\n",fName.Data(),fValue.Data(),v);
  fType=t;
  fValue=v;
  fLevel=l;
  fHelp=help;
  fArgument=arg;
}

//___________________________________________________________
const Char_t *JSFEnvRec::EntryString()
{
  static Char_t str[256];
  Char_t lstr[4];
  switch (fLevel) {
    case kEnvGlobal: strcpy(lstr,"[G]"); break;
    case kEnvUser: strcpy(lstr,"[U]"); break;
    case kEnvLocal: strcpy(lstr,"[L]"); break;
    case kEnvChange: strcpy(lstr,"[C]"); break;
    case kEnvAll: strcpy(lstr,"[A]"); break;
    default: strcpy(lstr,"[?]");
  }
  sprintf(str,"%s%s:%s",lstr,fName.Data(),fValue.Data());
  return str;
}

//___________________________________________________________
Bool_t JSFEnv::WriteFile(char* fn)
{
  // Overwrite parameter file

  Char_t *fout=fn;
  Char_t *newfn=NULL;
  if( fn == NULL  ) {
    Int_t lstr=strlen(fEnvFileName)+6;
    newfn= new Char_t[lstr];
    sprintf(newfn,"%s.bak",fEnvFileName);
    gSystem->Rename(fEnvFileName, newfn);
    fout=fEnvFileName;
  }

  FILE *fd;
  if( !(fd=fopen(fout,"w") ) ) {
    printf("JSFEnv::WriteFile.. Unable to write to a file, %s\n",fout);
    if( newfn ) delete newfn;
    return kFALSE;
  }

  TIter next(fObtained);
  JSFEnvRec *er;
 
  while ((er = (JSFEnvRec*)next() )) {
    if( er->fLevel == kEnvGlobal ) continue;
    if( er->fLevel == kEnvUser ) continue;
    if( strcmp(er->fName.Data(),"HEADER") != 0 ) {
      fprintf(fd,"%s:%s\n",er->fName.Data(), er->fValue.Data());
      if( er->fArgument.Length() > 0 ) fprintf(fd,"#!%s\n",er->fArgument.Data());
    }
    if( er->fHelp.Length() > 0 ) {
      const char *ch=er->fHelp.Data();
      Int_t lhlp=er->fHelp.Length();
      fputc('#',fd);
      for(Int_t i=0;i<lhlp-1;i++){
	fputc(*(ch+i),fd);
	if( *(ch+i) == '\n' ) fputc('#',fd);
      }
      fputs("\n\n",fd);
    }
  }
  fclose(fd);
  delete newfn;
  return kTRUE;
}

//_________________________________________________________
void JSFEnv::GetArguments()
{
  // Set command line parameters to environment parameters.

  TApplication *ap=gROOT->GetApplication();
  TIter next(fObtained);
  JSFEnvRec *er;
 
  Int_t i;
  Char_t argnam[64];
  for(i=0;i<ap->Argc();i++){
    next.Reset();
    if( strcmp(ap->Argv(i),"--help") == 0 ) {
      printf("Format of command line arguments is\n");
      printf("   jsf --[name]=[value] ....\n");
      printf("Valid [name] and its meanings are as follows.\n");
      while ((er = (JSFEnvRec*)next() )) {
        if( strlen(er->fArgument.Data()) == 0 ) continue;
	printf(" --%s\n",er->fArgument.Data());
	printf("%s",er->fHelp.Data());
      }
      printf(" --help  : Show this help message.\n");
    }
    else if( strncmp(ap->Argv(i),"--",2) == 0 ) {
      
      while ((er = (JSFEnvRec*)next() )) {
        if( er->fArgument.Data() == NULL ) continue;
	sprintf(argnam,"--%s=",er->fArgument.Data());
	Int_t largnam=strlen(argnam);
	if( strncmp(ap->Argv(i),argnam,largnam) != 0 ) continue;
	SetValue(er->fName.Data(), ap->Argv(i)+largnam);
	printf("Parameter %s is set to %s\n",er->fName.Data(),er->fValue.Data());
      }
    }
  }

}








