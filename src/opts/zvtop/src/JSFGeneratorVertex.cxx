//*LastUpdate : 14-July-2000     Akiya Miyamoto 
//*-- Author  : Akiya Miyamoto  14-July-2000

///////////////////////////////////////////////////////////////////
//
//  Pickup vertices ingenertor tracks
//
//$Id:
//  
//////////////////////////////////////////////////////////////////


#include "JSFSteer.h"
#include "JSFSIMDST.h"
#include "JSFGenerator.h"
#include "JSFGeneratorVertex.h"
#include "JSFGeneratorParticle.h"
#include "JSFLCFULL.h"

ClassImp(JSFGeneratorVertex)
ClassImp(JSFGeneratorVertexing)

JSFGeneratorVertexing *gJSFGeneratorVertexing;

extern "C" {
  Int_t phheavy_(Int_t *nbhad, Int_t bhind[8], Int_t *nbchad, Int_t bcind[8],
		Int_t bcpar[8], Int_t *nchad, Int_t chind[8]){
    gJSFGeneratorVertexing->PHHEAVY(*nbhad, bhind, *nbchad, bcind,bcpar,*nchad,chind);
    return 0;
  }
  Int_t phudlt_(Int_t *imc, Int_t *multa, Int_t *multv){
    gJSFGeneratorVertexing->PHUDLT(*imc, *multa, *multv);
    return 0;
  }    
  Int_t phtmtyp_(Int_t *imc, Int_t *ipmc, Int_t *ipflav, Int_t *ipvtx){
    gJSFGeneratorVertexing->PHTMTYP(*imc, *ipmc, *ipflav, *ipvtx);
    return 0;
  }
};
  
const Int_t kMaxMCPART=500;
   
typedef struct {
  Float_t p[kMaxMCPART][3], e[kMaxMCPART], ptot[kMaxMCPART];
  Float_t charge[kMaxMCPART], xt[kMaxMCPART];
  Int_t   ptype[kMaxMCPART], origin[kMaxMCPART], parent[kMaxMCPART];
} COMMON_MCPART;

extern COMMON_MCPART mcpart_bank_;



//___________________________________________________
void JSFGeneratorVertexing::MakeVertices(TObjArray *gen)
{
  // Create a list of vertices from Generated particle list.

  fGen=gen;
  JSFGenerator *gdst=(JSFGenerator*)gJSF->FindModule("JSFGenerator");
  JSFGeneratorBuf *sdb=(JSFGeneratorBuf*)gdst->EventBuf();
  Int_t ngen=sdb->GetNparticles();

  //  printf(" Number of gen tracks is %d\n",ngen);
  fNgen=ngen;
  //  Int_t *kbgn=new Int_t[ngen];
  //  Int_t *kend=new Int_t[ngen];
  for(Int_t i=0;i<ngen;i++){ fKbgn[i]=-99; fKend[i]=-99; }
  // fKbgn[i]=-2 : initial e+/e-
  // fKbgn[i]=-1 : initial partons ( Mother < 1 )
  // fKbgn[i]=n, : starting vertex number where this particle belong
  // fKend[i]=n  : vertex number were this track terminate.
  // fKend[i]=-1 : when particle is stable

  JSFGeneratorVertex *vtx0=0;

  JSFGeneratorVertex *initialbeam=0;
  JSFGeneratorVertex *initialparton=0;

  Int_t nvtx=0;
  Int_t ip1st=-999999;

  for(Int_t kg=0;kg<ngen;kg++){
    JSFGeneratorParticle *g=(JSFGeneratorParticle*)gen->UncheckedAt(kg);
    //    printf(" kg=%d  ",kg);
    //    printf(" M=%d #D=%d 1D=%d\n",g->GetMother(),g->GetNDaughter(),g->GetFirstDaughter());
    //    g->ls();
    if( TMath::Abs(g->GetID()) == 11 && 
	g->GetMother() == 0 && g->GetNDaughter() == 1 ) {
        fKbgn[kg]=-2 ; 
	if( initialbeam == 0 ) { 
	  initialbeam=new JSFGeneratorVertex(kInitialBeam,this); 
	}
	initialbeam->AddTrack(g);
	continue;
    }
    else if( g->GetMother() < 1 ) {
      fKbgn[kg]=-1;
      if( initialparton == 0 ) {
	initialparton=new JSFGeneratorVertex(kInitialParton, this);
      }
      initialparton->AddTrack(g);
      continue;
    }


    Int_t im=g->GetMother();
    //    printf(" kg=%d im=%d\n",kg,im);
    if( ip1st < -9999 ) { 
      ip1st = kg+1;  
      fKbgn[kg]=0; fKend[kg]=0; 
      if( vtx0 == 0 ) {
	vtx0=new JSFGeneratorVertex(kNormalVertex,this);
	Add(vtx0);
      }
      vtx0->AddTrack(g);
      if( g->GetNDaughter() == 0 ) { fKend[kg]=-1; }
      continue;
    }
    else if( im < ip1st ) {  
      fKbgn[kg]=0; fKend[kg]=0; 
      vtx0->AddTrack(g);
      if( g->GetNDaughter() == 0 ) { fKend[kg]=-1; }
      continue;
    }
    
    if( g->GetNDaughter() == 0 ){
      //      printf(" im=%d fKend[im]=%d",im,fKend[im-1]);
      fKbgn[kg]=fKend[im-1];
      fKend[kg]=-1;
      //      printf(" stable fKbgn fKend=%d %d\n",fKbgn[kg],fKend[kg]);
      JSFGeneratorVertex *vtx=(JSFGeneratorVertex*)UncheckedAt(fKbgn[kg]);
      vtx->AddTrack(g);
    }
    else if( g->GetDecayLength() < 1.0E-30 ) {
      fKbgn[kg]=fKend[im-1];
      fKend[kg]=fKbgn[kg];
      //      printf(" unstable fKbgn fKend=%d %d\n",fKbgn[kg],fKend[kg]);
      JSFGeneratorVertex *vtx=(JSFGeneratorVertex*)UncheckedAt(fKbgn[kg]);
      vtx->AddTrack(g);
    }
    else {
      fKbgn[kg]=fKend[im-1];
      JSFGeneratorVertex *tv=(JSFGeneratorVertex*)UncheckedAt(fKbgn[kg]);
      tv->AddTrack(g);
      nvtx++;
      fKend[kg]=nvtx;
      //      printf(" short lived=%d %d\n",fKbgn[kg],fKend[kg]);
      JSFGeneratorVertex *vtx=new JSFGeneratorVertex(kNormalVertex,this);
      vtx->fParentTrack=g;
      vtx->fParentVertex=tv;

      Add(vtx);
      }      
  }      

  if( initialbeam != 0 ) Add(initialbeam);
  if( initialparton != 0 ) Add(initialparton);

  // List particles of finite decay length
  /*
  for(Int_t ig=0;ig<ngen;ig++){
    if( fKend[ig] > 0 && fKbgn[ig] != fKend[ig] ) {

      printf(" fKbgn[%d]=%d fKend[%d]=%d ",ig,fKbgn[ig],ig,fKend[ig]);
      
      JSFGeneratorParticle *p=(JSFGeneratorParticle*)gen->UncheckedAt(ig);
      p->ls();
      printf(" id=%d M.=%d #Dau=%d 1stD=%d",
	     p->GetID(),p->GetMother(),p->GetNDaughter(),
	     p->GetFirstDaughter());
      printf(" dl=%g life=%g",p->GetDecayLength(),p->GetLifeTime());

      Int_t idau=p->GetFirstDaughter();
      JSFGeneratorParticle *pd=(JSFGeneratorParticle *)gen->UncheckedAt(idau-1);
      pd->ls();

      printf("\n");
      printf("\n");
    }
  }
  */

  //

  TIter next(this);
  JSFGeneratorVertex *v;
  while( (v=(JSFGeneratorVertex*)next())){
    v->SetDataMember();
  }

  return ;
}


//________________________________________________________________
JSFGeneratorVertexing::JSFGeneratorVertexing()
{
  gJSFGeneratorVertexing=this;
  fNgen=0;
}

//________________________________________________________________
JSFGeneratorVertexing::~JSFGeneratorVertexing()
{
  Delete();
}

//________________________________________________________________
void JSFGeneratorVertexing::Clear()
{
  Delete();
}

//________________________________________________________________
JSFGeneratorVertex::JSFGeneratorVertex(Int_t type, JSFGeneratorVertexing *vx)
{
  fType=type;
  fParentTrack=0;
  fParentVertex=0;
  fVertexing=vx;

}

//________________________________________________________________
JSFGeneratorVertex::~JSFGeneratorVertex()
{
  if( fTracks ) {
    delete fTracks;
    fTracks=0;
  }

}

//________________________________________________________________
void JSFGeneratorVertex::AddTrack(JSFGeneratorParticle *t)
{
  if( fTracks == 0 ) {
    fTracks = new TObjArray();
  }

  fTracks->Add(t);

}

//________________________________________________________________
void JSFGeneratorVertex::SetDataMember()
{
  //(Function)
  //  save data member of the class such as 
  //  vertex position, number of charged track, etc.

  TIter next(fTracks);
  JSFGeneratorParticle *g;
  fCharge=0;
  fNcharged=0;
  fLV.SetPxPyPzE(0.0,0.0,0.0,0.0);
  while( (g=(JSFGeneratorParticle*)next())) {
    if( g->GetNDaughter() == 0 || g->GetDecayLength() > 1.E-30 ) {
      if( (Int_t)g->GetCharge() != 0 ) {
        if( fNcharged == 0 ) {
	  TVector3 xv=g->GetXV3();
	  fPos[0]=xv.X(); fPos[1]=xv.Y(); fPos[2]=xv.Z();
        }
        fNcharged++;
        fCharge+=g->GetCharge();
      }
      fLV+= g->GetLorentz();
    }
  }  // End of while loop

  fMass=fLV.Mag();
  //  Float_t dl=TMath::Sqrt(fPos[0]*fPos[0]+fPos[1]*fPos[1]+fPos[2]*fPos[2]);
  //printf(" fCharge=%g fNcharged=%d mass=%g dl=%g\n",
  //	 fCharge,fNcharged,lv.Mag(),dl);

}

//________________________________________________________________
Int_t JSFGeneratorVertex::GetParentFlavourID()
{
  //(function)
  //  returns flavour ID of parent particle.
  //  5=b, 4=c, 3=s, 15=tau

  Int_t id=0;
  if( GetParent() != 0 ) {  id=GetParent()->GetID(); }
  else {  return 0 ; }
  Int_t ida=TMath::Abs(id);

  if( ida == 15  ) { return 15; }

  return ( ida > 1000 ? ida/1000 : ida/100 ) ;

}

//________________________________________________________________
JSFGeneratorParticle *JSFGeneratorVertex::GetInitialParton()
{
  //(function)
  //  returns pointers of initial parton where this vertex belongs
  //  For primary vertex, return 0

  if( GetParent() == 0 ) return 0;

  JSFGeneratorParticle *p=GetParent();

  Int_t im=p->GetMother();
  TObjArray *gen=fVertexing->fGen;
  JSFGeneratorParticle *g=0;
  while( im > 0 ) {
    g=(JSFGeneratorParticle*)gen->UncheckedAt(im-1);
    im=g->GetMother();
    printf(" im=%d id=%d\n",im,g->GetID());
  }

  return g;
  
}

//________________________________________________________________
JSFGeneratorVertex *JSFGeneratorVertexing::GetGeneratorVertex(const ANL4DVector jet)
{
  //(function)
  //  Find a primary short lived particle which is produced in the same hemisphere of 
  //  jet, and returns 4 vector of the particle, vertex position and number of 
  //  charged particle associated with the vertex.

  ANL3DVector jet3=jet.Get3D();
  TIter next(this);
  Double_t anglemin=1000.0;
  JSFGeneratorVertex *vm=0;
  JSFGeneratorVertex *v=0;
  Int_t izdcy=gJSF->Env()->GetValue("JSFGUI.Pythia.Decay.Z",5);
  while( (v=(JSFGeneratorVertex*)next())){
    if(v->IsNormalVertex() && v->GetNGeneration() == 1 && v->GetParentFlavourID() == izdcy ) {
      ANL3DVector bq(v->GetParent()->GetPx(),v->GetParent()->GetPy(),
		     v->GetParent()->GetPz());
      
      Double_t angle=bq.Angle(jet3);
      if( angle < anglemin ) {
	anglemin = angle;
	vm=v;
      }
      
    }
  }
  
  return vm;

}

//________________________________________________________________
JSFGeneratorVertex *JSFGeneratorVertexing::GetGeneratorVertex2(const ANL4DVector jet)
{
  //(function)
  //  Find a Charmed vertex which is a decay daughter of B particle and
  //  belong to the input "jet".

  ANL3DVector jet3=jet.Get3D();
  TIter next(this);
  Double_t anglemin=1000.0;
  JSFGeneratorVertex *vm=0;
  JSFGeneratorVertex *v=0;
  JSFGeneratorVertex *vp=0;
  while( (v=(JSFGeneratorVertex*)next())){
    if(v->IsNormalVertex() && v->GetNGeneration() != 1 && v->GetParentFlavourID() == 4 ) {
      vp=v->GetParentVertex();
      if( vp == 0 ) continue;
      if( vp->GetParentFlavourID() != 5 ) continue;

      ANL3DVector bq(vp->GetParent()->GetPx(),vp->GetParent()->GetPy(),
		     vp->GetParent()->GetPz());
      
      Double_t angle=bq.Angle(jet3);
      if( angle < anglemin ) {
	anglemin = angle;
	vm=v;
      }
      
    }
  }

  return vm;

}

//________________________________________________________________
Int_t JSFGeneratorVertex::GetNGeneration()
{
  //(Function)
  //  Count number of generation
  //  0=primary vertex, 1=secondary vertex, 2=trinary vertex, ...

  Int_t ngen=0;
  JSFGeneratorVertex *v=fParentVertex;
  while( v!= 0 ){
    v=v->fParentVertex;
    ngen++;
  }
  
  return ngen;
 
}



//________________________________________________________________
void JSFGeneratorVertexing::PHHEAVY(Int_t &nbhad, Int_t bhind[8], 
			Int_t &nbchad, Int_t bcind[8],
			Int_t bcpar[8], Int_t &nchad, Int_t chind[8])
{
/*
C--------------------------------------------------------->>>>phHeavy
      INTEGER FUNCTION PHHEAVY (NBHAD,BHIND,NBCHAD,BCIND,BCPAR,NCHAD,
     *CHIND)
      IMPLICIT NONE
      INTEGER NBHAD
      INTEGER BHIND(8)
      INTEGER NBCHAD
      INTEGER BCIND(8)
      INTEGER BCPAR(8)
      INTEGER NCHAD
      INTEGER CHIND(8)
C----------------------------------------------------------------------
C
C phHeavy  get a list of heavy flavour hadrons in the events
C          This only means hadrons which decay weakly.
C----------------------------------------------------------------------
C
C  Arguments   (all Outout)
C  =========
C  NBHAD ---  No. of B hadrons in event
C  BHIND ---  The list of MCPART indicies of B hadrons
C  NBCHAD --  No. of charm hadrons from B decays in event
C  BCIND ---  The list of MCPART indicies of charm hadrons from B's
C  BCPAR ---  The list of BHIND indicies of charm hadron's B parent
C  NCHAD ---  No. of charm hadrons from primary c quarks
C  CHIND ---  The list of MCPART indicies of primary charm hadrons
C
C----------------------------------------------------------------------
*/

  nbhad=0;
  nbchad=0;
  nchad=0;
  
  Int_t ngen=0;
  TIter next(this);
  JSFGeneratorVertex *v;
  while((v=(JSFGeneratorVertex*)next())){
    if( !v->IsNormalVertex() ) continue;
    JSFGeneratorVertex *pv=v->GetParentVertex();
    if( pv == 0 ) continue;

    JSFGeneratorParticle *g=v->GetParent();
    switch( v->GetParentFlavourID() ){
    case 5: 
      bhind[nbhad++]=fGen->IndexOf(g); 
      break;
    case 4: 
      ngen=v->GetNGeneration();
      if( ngen == 1 ) { chind[nchad++]=fGen->IndexOf(g); }
      else if( ngen != 0 ) { 
	JSFGeneratorVertex *vp=v->GetParentVertex();
	bcpar[nbchad]=fGen->IndexOf(vp->GetParent());
	bcind[nbchad++]=fGen->IndexOf(g); 
      }

      break;
    }
  }

}


//________________________________________________________________
void JSFGeneratorVertexing::PHUDLT(Int_t imc, Int_t &multa, Int_t &multv)
{
  /*
C--------------------------------------------------------->>>>phUdMlt
      INTEGER FUNCTION PHUDMLT (IMC,MULTA,MULTV)
      IMPLICIT NONE
      INTEGER IMC
      INTEGER MULTA
      INTEGER MULTV
C----------------------------------------------------------------------
C
C phUdMlt  Routine to count the decay daughter stable charged tracks
C          for the user input parent particle
C
C !!!  This routine is only valid for LUND 6.3 Particle ID scheme !!!
C
C----------------------------------------------------------------------
C
C  Arguments
C  =========
C  IMC   --- Input  MCPART index of input Parent particle
C  MULTA --- Output All charge tracks which came from the input parent
C  MULTV --- Output Charge tracks come directly from parent vertex
C
C----------------------------------------------------------------------
*/

  JSFGeneratorVertex *v=(JSFGeneratorVertex*)UncheckedAt(imc);
  multa=v->GetNcharged();
  multv=v->GetNcharged();

}

//________________________________________________________________
void JSFGeneratorVertexing::PHTMTYP(Int_t imc, Int_t &ipmc, 
			Int_t &ipflav, Int_t &ipvtx)
{
/*
C--------------------------------------------------------->>>>PHTMTYP
      INTEGER FUNCTION PHTMTYP (IMC,IPMC,IPFLAV,IPVTX)
      IMPLICIT NONE
      INTEGER IMC
      INTEGER IPMC
      INTEGER IPFLAV
      INTEGER IPVTX
C----------------------------------------------------------------------
C
C PHTMTYP Routine to define the decay origin type of an input track.
C          This routine traces back up the MC tree for in input track
C          until reached the first parent with finite lifetime to
C          define the input track as originated from that vertex.
C          It then traces all the way back to define what type of
C          vertex is was.
C
C !!! This routine only works under LUND 6.3 particle ID scheme !!!
C
C----------------------------------------------------------------------
C
C  Arguments
C  =========
C   IMC   --- Input  MCPART index of input track to be traced back
C   IPMC  --- Output MCPART index of decay vertex parent
C   IPFLAV--- Output The parent decay particle flavour type
C                   -1 = Unkown
C                    0 = Unstable primary
C                    1 = Bottom  weak decay
C                    2 = Charm   weak decay
C                    3 = Tau     weak decay
C                    4 = Strange weak decay
C                    5 = Decay of long lived particle
C   IPVTX --- Output The parent decay vertex type
C                   -1 = Unknown
C                    0 = Primary
C                    1 = Secondary decay
C                    2 = Tertiary  decay
C                    3 = Strange & longer lived decays
C
C   ( IPFLAV & IPVTX  Parameters are defined in PHBMCDEF.PAR )
C
C----------------------------------------------------------------------
*/

  ipmc=fKend[imc];
  JSFGeneratorVertex *v=(JSFGeneratorVertex*)UncheckedAt(ipmc);

  Int_t gid=v->GetParentFlavourID();

  //                         3, 4, 5
  Int_t ifdat[16]={-1, 0, 0, 4, 2, 1, 0,0,0,0,0,  0,0,0,0,3}; 

  ipflav=ifdat[gid];

  Int_t nvgen=v->GetNGeneration();
  
  if( gid == 3 || nvgen > 2 ) { nvgen=3; }

  ipvtx=nvgen;

}

//_________________________________________________________________________
void JSFGeneratorVertexing::MakeMCPART()
{
  // create a common /MCPART/ from Generator information.
  //
/*
C Bank MCPART Context=MCEVENT Maxid=5000 "MC Particle parameters"
C 
C  Real    P(3)       "X,Y,Z momentum of particle at track origin"
C  Real    E          "Energy of particle"
C  Real    PTOT       "Total momentum at track origin"
C 
C  Partid  PTYPE      "Particle type"
C  Real    CHARGE     "Charge of particle"
C 
C  Bits    ORIGIN     "Where did this particle come from/go to" ! Note 1
C  Real    XT(3)      "X,Y,Z of termination"
C 
C  Key     PARENT-->MCPART "Key giving parent particle"
C!
C! Notes:
C!
C!  1) The following bits may be set in ORIGIN (to be extended).
C!
C!                          Meaning of bit if set
C  Value ORIGIN
C   Parameter DECAYED =0  ! Decayed by generator
C   Parameter DECAYFLT=1  ! Decayed in flight by swimmer
C   Parameter BEAMPIPE=2  ! Does not hit detector ( down beam pipe
C   Parameter NOINTER =3  ! Traverses detector w/o interacting
C   Parameter STOPPED =4  ! Energy below cut; other bits may say why
C   Parameter INTERACT=5  ! Interacted, no further decision to be made
C   Parameter INTSHDEP=6  ! Interacted, further decision depends on
C                         ! selection of shower deposition
C   Parameter PRIMARY =8  ! e+e- primary interaction vertex
C   Parameter ISTOP1  =9  ! GEANT ISTOP=1, KCASE ne DCAY or
C                         !   HADR or (PAIR&Gamma)
C   Parameter ISTOP2  =10 ! GEANT ISTOP=2, KCASE ne STOP
C   Parameter KALTOCDC=11 ! Shower particle albedo from KAL back into CD
C   Parameter SWERROR =12 ! Error occured in swimming the track
C   Parameter SW2MNYST=13 ! Swim aborted: too many steps (ISTOP=99)
C   Parameter SWOUTOFT=14 ! Swim aborted: out of sensitive time of
C                         !   detector (ISTOP=4)
C   Parameter EMAXTERM=15 ! EM shower AXis TERMinated for sufficient depth
C   Parameter NOTTRACK=16 ! Not tracked by user request (neutrino, etc)
C   Parameter ISR     =17 ! Initial state radiation
C   Parameter BEAM    =18 ! Initial state beam particle (*)
C   Parameter PREFRAG =19 ! Pre-fragmentation particles (*)
C                         !   (gluons, partons etc.)
C   Parameter Swum    =20 ! this particle was produced by the swimmer
C  EndValue
C!
C!     (*) Particles in these categories will not normally be stored in the
C!         bank, but may be put here at the descretion of the generator, maybe
C!         under control of some flag.
C!
CEndBank
*/

  Int_t nmc=0;
  JSFGeneratorVertex *v;
  TIter next(this);
  while( (v=(JSFGeneratorVertex*)next())){
    TIter nextp(v->GetTracks());
    JSFGeneratorParticle *p;
    while( (p=(JSFGeneratorParticle*)nextp())){
      //      p->ls();
      mcpart_bank_.p[nmc][0]=p->GetPx();
      mcpart_bank_.p[nmc][1]=p->GetPy();
      mcpart_bank_.p[nmc][2]=p->GetPz();
      mcpart_bank_.e[nmc]=p->GetE();
      mcpart_bank_.ptot[nmc]=p->GetPabs();
      mcpart_bank_.charge[nmc]=p->GetCharge();
      mcpart_bank_.ptot[nmc]=p->GetPabs();
      
      nmc++;

    }
  }
  printf(" Number of particle is %d\n",nmc);

}


//_________________________________________________________
void JSFGeneratorVertexing::DebugPrint()
{
  // Print generated vertex pistion.

  JSFSIMDST *sds=(JSFSIMDST*)gJSF->FindModule("JSFSIMDST");
  JSFSIMDSTBuf *evt=(JSFSIMDSTBuf*)sds->EventBuf();
  
  Int_t ngen=evt->GetNGeneratorParticles();
  TClonesArray *genp=evt->GetGeneratorParticles();
  TClonesArray *cdct=evt->GetCDCTracks();

  for(Int_t ig=0;ig<ngen;ig++){
    JSFGeneratorParticle *gp=(JSFGeneratorParticle*)genp->UncheckedAt(ig);
    if( gp->GetDecayLength() <= 0.0 ) continue;
    //  gp->ls();
    //  printf(" decay length=%g\n",gp->GetDecayLength());
    //  printf(" Life time=%g\n",gp->GetLifeTime());

    printf(" Short lived particle: ID=%d",gp->GetID());
    printf(" Mass=%g  Charge=%g  Number Daughter=%d\n",
	   gp->GetMass(),gp->GetCharge(), gp->GetNDaughter());
    printf(" Px,Py,Pz=(%g,%g,%g)",gp->GetPx(),gp->GetPy(),gp->GetPz());
    printf(" E=%g\n",gp->GetE());
    printf(" Decay Length=%g ",gp->GetDecayLength());

    Int_t idau=gp->GetFirstDaughter();
	
    Int_t ncharged=0;
    Float_t chargesum=0;
    Int_t ncid[500];
    JSFGeneratorParticle *gdpnt[500];
    for(Int_t j=0;j<gp->GetNDaughter();j++){
      JSFGeneratorParticle *gd=(JSFGeneratorParticle*)genp->UncheckedAt(idau-1+j);
      chargesum+= gd->GetCharge();
      if( gd->GetCharge() != 0 ) {
	ncid[ncharged]=gd->GetSerial();
	gdpnt[ncharged]=gd;
	ncharged++;
      }
      TVector3 vx;
      if( j==0 ) {
	vx=gd->GetXV3();
	printf(" Decayed at (%g,%g,%g)\n",vx.X(), vx.Y(),vx.Z());
      }
	
    }
    printf(" Charge sum is %g",chargesum);
    printf(" Number of charged track is %d\n",ncharged);
    printf(" GenID of Charged track is \n");
    for(Int_t i=0;i<ncharged;i++){ 
      printf(" %d ",ncid[i]);
      printf(" pid=%d",gdpnt[i]->GetID());
      printf(" E=%g ",gdpnt[i]->GetE());
      printf(" Costh=%g ",gdpnt[i]->GetCosth());
      
      JSFCDCTrack *ct=GetCDCTrack(cdct, ncid[i]);
      if( ct != 0 ) {
	printf(" CDC Index=%d",cdct->IndexOf(ct));
	Float_t ctcos=ct->GetPz()/TMath::Sqrt(ct->GetPx()*ct->GetPx()
			      +ct->GetPy()*ct->GetPy()+ct->GetPz()*ct->GetPz());
	printf(" p=%g costh=%g",ct->GetP(),ctcos);
      }
      printf("\n");
    }
    printf("\n");
    printf("\n");
    
  }



  Int_t nw, iret, nelm;
  Char_t *bnknam="Production:CDC_VTX;Track_Parameter";
  Int_t itrkp[200], neary[200];
  Int_t ktrkp[200], kw, kret;
  gJSFLCFULL->TBNOEL(1,bnknam,nelm,neary);
  printf(" nelm-%d\n",nelm);
  for(Int_t j=1;j<=nelm;j++){
    gJSFLCFULL->TBGET(1,bnknam,j,nw,itrkp,iret);
    gJSFLCFULL->TBGET(1,"Production:CDC;Track_Parameter",itrkp[53],
		      kw, ktrkp, kret);
    printf(" j=%d  ityp=%d  cdc=%d  vtx=%d",j,itrkp[52],itrkp[53],itrkp[54]);
    printf(" Ngen=%d TrackId=%d\n",ktrkp[55], ktrkp[56]);
      
  }



}


//____________________________________________________________
JSFCDCTrack *JSFGeneratorVertexing::GetCDCTrack(TClonesArray *tracks, Int_t genid)
{
  //(Function)
  //  Returns a pointer to JSFCDCTrack class which is created
  //  by a generator track of id "genid"
  //


  TIter next(tracks);
  JSFCDCTrack *ct;
  while( (ct=(JSFCDCTrack*)next())) {
    //    printf(" genid=%d\n",ct->GetGenID());
    if( ct->GetGenID() == genid ) {
      return ct;
    }
  }


  return 0;
}




