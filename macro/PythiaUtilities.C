//____________________________________________________________
void PrintEventInfo()
{
  // Get information about geneterated event from Generator Class

   PythiaGenerator *py
     =(PythiaGenerator*)jsf->FindModule("PythiaGenerator");
   if( py->GetVersionNumber() <= 5 ) {
     TPythia *tpy=py->GetPythia();
   }
   else {
     TPythia6 *tpy=py->GetPythia();
   }

   // See page 140. of the Pythia document 
   //**********************************************
   // GetMSTI(1)
   // specifies the general type of subprocess that has occured,
   // According to the ISUB code.
 
   Int_t msti1=tpy->GetMSTI(1);
   printf("Generated process is %s\n",MSTIInfo(1,msti1));

   printf("Ecm=%g\n",py->GetEcm());

   //**********************************************
   JSFSIMDST *sds=(JSFSIMDST*)jsf->FindModule("JSFSIMDST");
   JSFSIMDSTBuf *evt=(JSFSIMDSTBuf*)sds->EventBuf();
   
   Int_t ngen=evt->GetNGeneratorParticles();
   TClonesArray *gen=(TClonesArray*)evt->GetGeneratorParticles();
   /*
   printf(" ngen=%d\n",ngen);
   for(Int_t i=0;i<ngen;i++){
     JSFGeneratorParticle *p=(JSFGeneratorParticle*)gen->UncheckedAt(i);
     //     if( p->GetNDaughter() != 0 ) continue;
     printf(" Serial %d",p->GetSerial());
     printf(" Id=%d",p->GetID());
     printf(" Mass=%g",p->GetMass());
     printf(" NDau=%d",p->GetNDaughter());
     printf(" 1stD=%d",p->GetFirstDaughter());
     printf(" Mother=%d",p->GetMother());

     printf("\n");
     printf(" LifeTime=%g\n",p->GetLifeTime());
     printf(" DecayLength=%g\n",p->GetDecayLength());
     p->ls();
   }
   */
   PrintGeneratedEventInfo(ngen, gen);

}

//_________________________________________________________________
void PrintGeneratedEventInfo(Int_t ngen, TClonesArray *gen)
{
  // Scan TClonesArray of JSFGeneratorParticle and
  // print initial parton information.

  const Int_t maxp=20;
  JSFGeneratorParticle *p[maxp];
  // Save pointers to documentation line particles ( Mother <= 0 )
  Int_t n=0;
  Int_t i;
  Int_t ndau[maxp]={0};
  //  for(i=0;i<maxp;i++){ ndau[i]=0; }

  for(i=0;i<ngen;i++){
    JSFGeneratorParticle *pt=(JSFGeneratorParticle*)gen->UncheckedAt(i);
    if(pt->GetMother()>0) break;
    p[n++]=pt;
    ndau[TMath::Abs(pt->GetMother())]++;
  }

  // printf(" There are %d documentation lines\n",n);

  Int_t nprim=0;
  Int_t nppnt[maxp]={0};

  printf(" e+e- -> ");
  for(i=2;i<n;i++){
    if( p[i]->GetMother() != 0 ) continue;
    printf(" %s ",ParticleName(p[i]->GetID()));
    nppnt[nprim++]=i;
  }

  printf(",");

  for(i=0;i<nprim;i++){
    if( ndau[p[nppnt[i]]->GetSerial()] ==0 ) continue;
    printf(" %s -> ",ParticleName(p[nppnt[i]]->GetID()));
    for(Int_t j=2;j<n;j++){
      if( TMath::Abs(p[j]->GetMother()) == p[nppnt[i]]->GetSerial()) {
	printf(" %s ",ParticleName(p[j]->GetID()));
      }
    }
    if( i < nprim-1 ) printf(",");
  }

  printf("\n");

}



//_________________________________________________________________
Char_t *ParticleName(Int_t id)
{
  // Return name of particle names


  const Int_t maxp=300;
  static Char_t *pname[maxp]={0};
  pname[1]="d"   ; pname[2]="u"; 
  pname[3]="s"   ; pname[4]="c" ; 
  pname[5]="b"   ; pname[6]="t";
  pname[7]="D"   ; pname[8]="U";
  pname[11]="e"  ; pname[12]="neu_e";
  pname[13]="mu" ; pname[14]="neu_mu";
  pname[15]="tau"; pname[16]="neu_tau";
  pname[17]=" L"  ; pname[18]="neu_L";
  pname[21]="gluon" ; pname[22]="gamma";
  pname[23]="Z0" ; pname[24]="W+";  pname[25]="H0";
  static Char_t str[20];

  if( id > 0 && id < 26 )  return pname[id];
  elseif( id > -20&& id < 0 )  {
    sprintf(str,"%s-bar",pname[TMath::Abs(id)]);
    return str;
  }
  elseif( id == -24 ) return "W-";    
  
  return "Undefined pid";
}  

//_________________________________________________________________
Char_t *MSTIInfo(Int_t id, Int_t ival)
{
  // Returns strings explaining meanings of MSTI array.
  // See page 141 of Pythia document.
  // ISUB code is described in p.98 of Pythia Document 

  const Int_t maxprocess=200;
  Char_t *processname[maxprocess]={0};
  processname[1]="e+e- -> gamma*/Z0 ";
  processname[22]="e+e- -> Z0 Z0";
  processname[24]="e+e- -> Z0 H0";
  processname[25]="e+e- -> W+ W-";
  processname[103]="e+e- -> e+e- H0 (gamma+gamma->H0)";
  processname[123]="e+e- -> e+e- H0 (ZZ fusion)";
  processname[124]="e+e- -> e+e- H0 (WW fusion)";

  switch (id) {
    //*****************************************************
    case 1:
      switch (ival) {
        case 0 :
	  printf("ISUB code and process name\n");
	  for(Int_t i=0;i<maxprocess;i++){
	    if( processname[i] == 0 ) continue;
	    printf(" ISUB=%3d : %s\n",i,processname[i]);
	  } 
	  break;  // end of id=1, ival=0

        default:  // default of id=1, ival switch
	  if( processname[ival] == 0 ) {
	    printf("No process name available for ISUB=%d\n",ival);
	  }
	  else {
	    return processname[ival];
	  }
      } 
      break;  // End of id=1 


    //*****************************************************
    default:  // Default of id switch
      printf("MSTIInfo .. id=%d is invalid.\n");
 }

  return "\00";

}

