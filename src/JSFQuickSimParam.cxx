//*LastUpdate :  jsf-1-7  8-April-1999  By Akiya Miyamoto
//*LastUpdate :  jsf-1-6  30-March-1999  By Akiya Miyamoto
//*LastUpdate :  jsf-1-5  1-March-1999  By Akiya Miyamoto
//*-- Author  : A.Miyamoto  1-March-1999

/*
 8-Apr-1999 A.Miyamoto  Detector parameter is same as detect7.com
*/


///////////////////////////////////////////////////////////////////
//
// JSRSFQuickSimParam
//
// A Quicksim parameter class
// 
//$Id:
//
//////////////////////////////////////////////////////////////////


#include <TMath.h>
#include "JSFSteer.h"
#include "JSFQuickSimCommon.h"
#include "JSFQuickSimParam.h"

ClassImp(JSFQuickSimParam)
ClassImp(JSFCALGeoParam)

//_____________________________________________________________________________
JSFQuickSimParam::JSFQuickSimParam()
{
  //  Parameters of JLC QUick Simulator.
  //  

  fBfield=20.0;
  fSeed=314159 ;

  smrfld_.imodbf[0]=0 ; 
  smrfld_.imodbf[1]=0 ; 
  smrfld_.bfdata[0]=0 ; 
  smrfld_.bfdata[1]=0 ; 

  prfeld_.bfield=10.1085  ;  // kgauss
  prfeld_.ptor=329.9837713  ;
  prfeld_.ropt=3.03045206e-3 ;

  //  Tracker
  fTrack[0]  = 45.0   ; //inner radius(cm)  
  fTrack[1]  = 230.   ; // outer radius(cm)
  fTrack[2]  = -230.  ; // Z-(cm) 
  fTrack[3]  = 230.   ; // Z+(cm)
  fTrack[4]  = 100    ; // # sampling points   
  fTrack[5]  = 10     ; // cut on # sampling points   
  fTrack[6]  = 0.85E-2  ; // sigma_(r-phi)
  fTrack[7]  = 3.E-1  ; // sigma_Z 

  // EM calorimeter
  fEMCal[0]  =    156   ; // # phi segments       
  fEMCal[1]  =     50   ; // # theta segments of barrel      
  fEMCal[2]  =     18   ; // # radial segments of endcap     
  fEMCal[3]  =     45.  ; // inner radius(cm)                
  fEMCal[4]  =    250.  ; // outer radius(cm)                
  fEMCal[5]  =   -300.  ; // Z-(cm)                          
  fEMCal[6]  =    300.  ; // Z+(cm)                          
  fEMCal[7]  =    1.E-2 ; // sigma_E/E pedestal for barrel   
  fEMCal[8]  =   1.5E-1 ; // sigma_E/E at 1 GeV              
  fEMCal[9]  =    1.E-2 ; // sigma_E/E pedestal for endcap   
  fEMCal[10] =   1.5E-1 ; // sigma_E/E at 1 GeV              

   // HD calorimeter
  fHDCal[0]  =    78    ; // # phi segments                 
  fHDCal[1]  =    25    ; // # theta segments of barrel     
  fHDCal[2]  =     9    ; // # radial segments of endcap    
  fHDCal[3]  =    48.   ; // inner radius(cm)               
  fHDCal[4]  =   270.   ; // outer radius(cm)               
  fHDCal[5]  =  -324.   ; // Z-(cm)                         
  fHDCal[6]  =   324.   ; // Z+(CM)                         
  fHDCal[7]  =    1.E-2 ; // sigma_E/E pedestal for barrel  
  fHDCal[8]  =    4.E-1 ; // sigma_E/E at 1 GeV             
  fHDCal[9]  =    1.E-2 ; // sigma_E/E pedestal for endcap  
  fHDCal[10] =    4.E-1 ; // sigma_E/E at 1 GeV             


   // VTX 
  fNERRVX   =     3    ; // VTX Space point error flag
  fNSMPVX   =     5    ; // # sampling layers + 1 = NSMPVX        
  fNUMVTX   =     4    ; // # sampling layers + 1 = NSMPVX        
  fDPHIVX   =   25.E-4 ; // phi pitch (cm)                        
  fDZEEVX   =   25.E-4 ; // Z   pitch (cm)                        
  
  //  Beam pipe
  fVTXLayer[0][0]  =     2.   ; // radius (cm)                   
  fVTXLayer[0][1]  =  -100.   ; // Z-(cm)                        
  fVTXLayer[0][2]  =   100.   ; // Z+(cm)                        
  fVTXLayer[0][3]  =   0.0015 ; // thickness in radiation length 
  //  Layer #1
  fVTXLayer[1][0]  =    2.4  ; // radius (cm)                   s
  fVTXLayer[1][1]  =   -5.0  ; // Z-(cm)                        
  fVTXLayer[1][2]  =    5.0  ; // Z+(cm)                        
  fVTXLayer[1][3]  =  0.003  ; // thickness in radiation length 
  //  Layer #2
  fVTXLayer[2][0]  =    3.6  ; // radius (cm)                   
  fVTXLayer[2][1]  =   -7.5  ; // -Z-(cm)                        
  fVTXLayer[2][2]  =    7.5  ; // Z+(cm)                        
  fVTXLayer[2][3]  =  0.003  ; // thickness in radiation length 
  //  Layer #3
  fVTXLayer[3][0]  =    4.8  ; // radius (cm)                   
  fVTXLayer[3][1]  =  -10.0  ; // -Z-(cm)                        
  fVTXLayer[3][2]  =   10.0  ; // Z+(cm)                        
  fVTXLayer[3][3]  =  0.003  ; // thickness in radiation length 
  //  Layer #4
  fVTXLayer[4][0]  =    6.0  ; // radius (cm)                   
  fVTXLayer[4][1]  =  -12.5  ; // -Z-(cm)                        
  fVTXLayer[4][2]  =   12.5  ; // Z+(cm)                        
  fVTXLayer[4][3]  =  0.003  ; // thickness in radiation length 
  // layer #NSMPVX (CDC inner cylinder)
  fVTXLayer[5][0]  =    40  ; // radius (cm)                   
  fVTXLayer[5][1]  =  -230.  ; // Z-(cm)                        
  fVTXLayer[5][2]  =   230.  ; // Z+(cm)                        
  fVTXLayer[5][3]  =   0.01  ; // thickness in radiation length 

  fVTXError[0] = 4.E-4 ;
  fVTXError[1] = 4.E-4 ;
  fVTXError[2] = 40.E-4 ;
  fVTXError[3] = 40.E-4 ;
  fVTXError[4] = 40.E-4 ;

  //  Parameters for Clustering.
  Float_t clp[4][5] ={ { 100., 0.40, 0.20, 1.00, 0.3 },
		      { 100., 0.40, 0.20, 1.00, 0.3 },
		      { 100., 0.40, 0.20, 1.00, 0.25},
		      { 100., 0.40, 0.20, 1.00, 0.25} };

  for(Int_t i=0;i<4;i++){
    for(Int_t j=0;j<5;j++){
      fCLSPAR[i][j]=clp[i][j];
    }
  }

  // Load parameters

  sscanf(gJSF->Env()->GetValue("JSFQuickSim.ParameterFile","Undefined"),
	 "%s",fParamFile);
  if( strcmp("Undefined",fParamFile) != 0 ) {
    printf(" Detector parameter is obtained from %s\n",fParamFile);
    ReadParamDetector(fParamFile);
  }
  else {
    // printf("Detector parameter file is undefined.\n");
    // printf(" Will use parameters those defined in the source file, JSFQuickSim.cxx\n");
  }

  sscanf(gJSF->Env()->GetValue("JSFQuickSim.CMBCUT.ADXECT","20.0"),
	 "%g",&fCMBCUT[0]);
  sscanf(gJSF->Env()->GetValue("JSFQuickSim.CMBCUT.ENSGEM","2.0"),
	 "%g",&fCMBCUT[1]);
  sscanf(gJSF->Env()->GetValue("JSFQuickSim.CMBCUT.ENSGHD","2.0"),
	 "%g",&fCMBCUT[2]);

  // Set Calorimeter Geometry info.

  fEMCGeom = new JSFCALGeoParam(kEMC, 
	    (Int_t)fEMCal[0], (Int_t)fEMCal[1], (Int_t)fEMCal[2],
	           fEMCal[3], fEMCal[4], fEMCal[5], fEMCal[6] );		    

  fHDCGeom = new JSFCALGeoParam(kHDC, 
	    (Int_t)fHDCal[0], (Int_t)fHDCal[1], (Int_t)fHDCal[2],
	           fHDCal[3], fHDCal[4], fHDCal[5], fHDCal[6] );		    

}

//_____________________________________________________________________________
JSFQuickSimParam::~JSFQuickSimParam()
{
  delete fEMCGeom;
  delete fHDCGeom;
}

//_____________________________________________________________________________
void JSFQuickSimParam::ReadParamDetector(Char_t *file)
{
  // Read QuickSimulator parameter from a file.
  // In lclib.97a, there are two parameter files for Quick Simulator, but
  // information prepared for swimmer is included in the parameter file
  // for smearing.  Therefore, in this function, reads the smear parameter
  // file a la lclib.97a format, and saved in the JSFQuickSim class for the
  // latter use by Quick Simulator.

  FILE *fd;
  fd=fopen(file,"r");
  if( !fd ) Fatal("ReadParamFile","Unable to open file %s",file);
 
  printf("JSFQuickSimParam::ReadParamDetector \n");
  printf(" --  Read detector parameter from the file %s\n",file);
  Char_t input[256];

  // Read a file
  while(fgets(input, 256, fd) ){

    if( !strncmp("@!",input,2) ) continue;
    
    Int_t id;
    Float_t val;
    sscanf(input,"%d %g",&id, &val);
   
  // Set a data to varaiables
    Float_t *clspar=&fCLSPAR[0][0];
    if( id == 1 ) continue ;
    else if( id == 2 ) { fBfield = val; 
                    prfeld_.bfield=10.1085  ;  // kgauss
                    prfeld_.ptor=329.9837713  ;
		    prfeld_.ropt=3.03045206e-3 ;
    }
    else if( 10 < id && id < 20 ) fTrack[id-11]=val;
    else if( 30 < id && id < 50 ) fEMCal[id-31]=val;
    else if( 50 < id && id < 70 ) fHDCal[id-51]=val;
    else if( id == 70 ) fNERRVX = (Int_t)val ; //# sampling layers + 1 = NSMPVX
    else if( id == 71 ) fNSMPVX = (Int_t)val ; //# sampling layers + 1 = NSMPVX
    else if( 1000 < id && id < 1021 ) clspar[id-1001]=val;

    else { 
      if( fNERRVX ==1 || fNERRVX == 2 ) {
        if( id == 72 ) fDPHIVX = val ; // phi pitch (cm)
        else if( id == 73 ) fDZEEVX = val ; // Z pitch (cm)
	else if( 73 < id && id < 4*(fNSMPVX+1)+74 ) { // VTX layer info.
	  Int_t ilay=(id-74)/4 ;
	  fVTXLayer[ilay][id-(74+4*ilay)]=val;
	}
	else if( 4*(fNSMPVX+1)+73 < id && id < 4*(fNSMPVX+1)+79 ) { // VTX error info.
	  fVTXError[id-(4*(fNSMPVX+1)+74 )]=val;
	}
      }
      else if( fNERRVX==3 ){
	if( 73 <= id  && id <= 76 ) { fVTXError[id-73]=val; }
	else if( 76 < id && id < 4*(fNSMPVX+1)+77 ) {
	  Int_t ilay=(id-76)/4;
	  fVTXLayer[ilay][id - (4*ilay+77)]=val;
	}
      }
      else {
        Fatal("ReadParamDetector",
	      "Invalid Parameter ID %d, Error line is \n%s",id,input);
      }
    }
  }
  fclose(fd);
  

}
//_____________________________________________________________________________
void JSFQuickSimParam::SetSwimParam()
{
  // Set Swimmer parameter

  swmrnd_.iseed=fSeed ;

  prfeld_.ptor=prfeld_.ptor*prfeld_.bfield/fBfield ;
  prfeld_.ropt=1.0/prfeld_.ptor;
  prfeld_.bfield=fBfield;

}

//_____________________________________________________________________________
void JSFQuickSimParam::SetSmearParam()
{
  // Set Swimmer parameter  ( corresponds to detect4.com )

  //  Tracker
  smrtgo_.rcytk[0] = fTrack[0] ; //inner radius(cm)  
  smrtgo_.rcytk[1] = fTrack[1] ; // outer radius(cm)
  smrtgo_.zcytk[0] = fTrack[2] ; // Z-(cm) 
  smrtgo_.zcytk[1] = fTrack[3] ; // Z+(cm)
  smrtgo_.nsmptk   = (Int_t)fTrack[4] ; // # sampling points   
  smrtgo_.nsmpct   = (Int_t)fTrack[5] ; // cut on # sampling points   
  smrres_.sigrf    = fTrack[6] ; // sigma_(r-phi)
  smrres_.sigze    = fTrack[7] ; // sigma_Z 

  // EM calorimeter
  smrego_.nphiem   =  (Int_t)fEMCal[0]  ; // # phi segments       
  smrego_.nthtem   =  (Int_t)fEMCal[1]  ; // # theta segments of barrel      
  smrego_.nradem   =  (Int_t)fEMCal[2]  ; // # radial segments of endcap     
  smrego_.rcyem[0] =  fEMCal[3]  ; // inner radius(cm)                
  smrego_.rcyem[1] =  fEMCal[4]  ; // outer radius(cm)                
  smrego_.zcyem[0] =  fEMCal[5]  ; // Z-(cm)                          
  smrego_.zcyem[1] =  fEMCal[6]  ; // Z+(cm)                          
  smrres_.sgemb0   =  fEMCal[7]  ; // sigma_E/E pedestal for barrel   
  smrres_.sgemb    =  fEMCal[8]  ; // sigma_E/E at 1 GeV              
  smrres_.sgeme0   =  fEMCal[9]  ; // sigma_E/E pedestal for endcap   
  smrres_.sgeme    =  fEMCal[10] ; // sigma_E/E at 1 GeV              

   // HD calorimeter
  smrhgo_.nphihd   =  (Int_t)fHDCal[0]  ; // # phi segments                 
  smrhgo_.nththd   =  (Int_t)fHDCal[1]  ; // # theta segments of barrel     
  smrhgo_.nradhd   =  (Int_t)fHDCal[2]  ; // # radial segments of endcap    
  smrhgo_.rcyhd[0] =  fHDCal[3]  ; // inner radius(cm)               
  smrhgo_.rcyhd[1] =  fHDCal[4]  ; // outer radius(cm)               
  smrhgo_.zcyhd[0] =  fHDCal[5]  ; // Z-(cm)                         
  smrhgo_.zcyhd[1] =  fHDCal[6]  ; // Z+(CM)                         
  smrres_.sghdb0   =  fHDCal[7]  ; // sigma_E/E pedestal for barrel  
  smrres_.sghdb    =  fHDCal[8]  ; // sigma_E/E at 1 GeV             
  smrres_.sghde0   =  fHDCal[9]  ; // sigma_E/E pedestal for endcap  
  smrres_.sghde    =  fHDCal[10] ; // sigma_E/E at 1 GeV             

   // VTX 
  smrvgo_.nerrvx   =  fNERRVX   ; // VTX Error flag
  smrvgo_.nsmpvx   =  fNSMPVX   ; // # sampling layers + 1 = NSMPVX        
  smrvgo_.dphivx   =  fDPHIVX   ; // phi pitch (cm)                        
  smrvgo_.dzeevx   =  fDZEEVX   ; // Z   pitch (cm)                        
  smrvgo_.numvtx   =  fNUMVTX   ; // Number of actual VTX layer
  
  //  fVTX Layer info
  Int_t i;
  for(i=0;i<=fNSMPVX;i++){
    smrvgo_.rcyvx[i]    = fVTXLayer[i][0] ; // radius (cm)                   
    smrvgo_.zcyvx[i][0] = fVTXLayer[i][1] ; // Z-(cm)                        
    smrvgo_.zcyvx[i][1] = fVTXLayer[i][2] ; // Z+(cm)                        
    smrvgo_.rdlvx[i]    = fVTXLayer[i][3] ; // thickness in radiation length 
  }

  for(i=0;i<5;i++){
    smrvgo_.errvx[i] = fVTXError[i];
  }

  for(i=0;i<4;i++){
    for(Int_t j=0;j<5;j++){
      smrclp_.clspar[i][j]=fCLSPAR[i][j];
    }
  }

  cmbcut_.adxect = fCMBCUT[0] ;
  cmbcut_.ensgem = fCMBCUT[1] ;
  cmbcut_.ensghd = fCMBCUT[2] ;

  // Set Calorimeter Geometry info.

  fEMCGeom->SetGeoParam(kEMC, (Int_t)fEMCal[0], (Int_t)fEMCal[1], 
	(Int_t)fEMCal[2], fEMCal[3], fEMCal[4], fEMCal[5], fEMCal[6] );
  fHDCGeom->SetGeoParam(kHDC, (Int_t)fHDCal[0], (Int_t)fHDCal[1], 
	(Int_t)fHDCal[2], fHDCal[3], fHDCal[4], fHDCal[5], fHDCal[6] );
}

//____________________________________________________________________________
Int_t JSFQuickSimParam::GetVTXLayerNumber(Float_t radius)
{
//  Convert Radius of VTX hit into VTX layer ID,
//  Using the simulation parameter in smrpar[].
//  Layer number of inner most detector is 1.
//
   Float_t wid=0.01;
   Int_t i;
   for(i=1;i<GetVTXNLayer();i++){
     if( radius > GetVTXRadius(i)-wid && radius < GetVTXRadius(i)+wid ) 
       return i;
   }
   return 0;

}

//_____________________________________________________________________________
JSFCALGeoParam::JSFCALGeoParam(EJSFCALType type, 
                    Int_t nphi, Int_t ntheta, Int_t nrad,
		    Float_t rmin, Float_t rmax,  Float_t zminus, Float_t zplus )
{
   SetGeoParam(type, nphi, ntheta, nrad,
		    rmin,  rmax,  zminus, zplus );
}

//_____________________________________________________________________________
void JSFCALGeoParam::SetGeoParam(EJSFCALType type, 
		 Int_t nphi, Int_t ntheta, Int_t nrad,
	         Float_t rmin, Float_t rmax,  Float_t zminus, Float_t zplus )
{
  // Initialize geometry parameters of the calorimeter.  Inputs are,
  //     type   : kEMC or kHDC
  //     nphi   : Number of phi segmentation.
  //     ntheta : Number of theta segmentation of barrel calorimeter
  //     nrad   : Number of radial segmentation of endcap calorimeter
  //     rmin   : Inner radius of endcap calorimeter (cm)
  //     rmax   : maximum radius of endcap calorimeter (cm)
  //     zminus : Z coordinate of -Z endcap calorimeter (cm)
  //     zplus  : Z coordinate of +Z endcap calorimeter (cm)
  //              Barrel calorimeter is from zmin to zmax, 
  //              and its surface radius is rmax.
  //

  fType = type;
  fRmin = rmin;
  fRmax = rmax;
  fZminus = zminus;
  fZplus  = zplus;
  
  fPhiStep = 2.0*TMath::Pi()/(Float_t)nphi;

  Float_t csmn = zplus/TMath::Sqrt(rmax*rmax+zplus*zplus);
  Float_t csmx = zminus/TMath::Sqrt(rmax*rmax+zminus*zminus);
  fBZetaMin    = TMath::Log( (1-csmn)/(1+csmn) )/2 ;
  Float_t thmx = TMath::Log( (1-csmx)/(1+csmx) )/2 ;
  fBZetaStep   = ( thmx - fBZetaMin )/(Float_t)ntheta ;

  Float_t snmn = rmin/TMath::Sqrt(rmin*rmin+zminus*zminus);
  Float_t snmx = rmax/TMath::Sqrt(rmax*rmax+zminus*zminus);
     fMZetaMin = TMath::Log( (1+snmn)/(1-snmn) )/2 ;
  Float_t  rmx = TMath::Log( (1+snmx)/(1-snmx) )/2 ;
    fMZetaStep = ( rmx - fMZetaMin )/(Float_t)nrad ;

          snmn = rmin/TMath::Sqrt(rmin*rmin+zplus*zplus);
          snmx = rmax/TMath::Sqrt(rmax*rmax+zplus*zplus);
     fPZetaMin = TMath::Log( (1+snmn)/(1-snmn) )/2 ;
           rmx = TMath::Log( (1+snmx)/(1-snmx) )/2 ;
    fPZetaStep = ( rmx - fMZetaMin )/(Float_t)nrad ;
}


//__________________________________________________________
JSFRThPhi JSFCALGeoParam::HitPosition(Int_t iCellID)
{

// Obtain (r,th,phi) of hit cell.  
// Note that r is not the distance between point and origin.
// r is the distance to the Z axis.  (x,y,z) coordinate in
// cartesian coordinate is given as follows.
//    
//    x = r cos(phi)
//    y = r sin(phi)
//    z = r / tan(th)

// 
// c  EM Cal case
// c
// C  THT now(92/05/20) means zeta defined below. Equal spacing
// C  segmentation in zeta corresponds to equal counter size.
// C     2*zeta     = ln( ( 1 - cos(theta) )/( 1 + cos(theta) ) )
// C     cos(theta) = (1-exp(2*zeta))/(1+exp(2*zeta))
// c  

  Int_t     iabs = TMath::Abs(iCellID);
  Int_t     izee = iCellID/1000000;
  Double_t xth   = (Double_t)((iabs/1000)%1000) + 0.5 ;
  Double_t xphi  = (Double_t)(iabs%1000) + 0.5  ;

  JSFRThPhi hit;
  hit.phi = fPhiStep*xphi;
  Double_t zeta, costh, sinth;
  switch( izee ) {
    case 0:
      hit.r=fRmax;
      zeta = fBZetaMin + xth*fBZetaStep;
      costh = ( 1. - TMath::Exp(2.0*zeta))/(1.0+TMath::Exp(2.0*zeta));
      hit.th = TMath::ACos(costh);
      break;
    case 1:
      zeta = fPZetaMin + xth*fPZetaStep;
      sinth = (- 1 + TMath::Exp(2*zeta))/(1.0 + TMath::Exp(2*zeta));
      hit.th = TMath::ASin(sinth);
      hit.r  = fZplus*TMath::Tan(hit.th);
      break;
    case -1:
      zeta = fMZetaMin + xth*fMZetaStep;
      sinth = (- 1 + TMath::Exp(2*zeta))/(1.0+TMath::Exp(2*zeta));
      hit.th = TMath::Pi() - TMath::ASin(sinth);
      hit.r  = fZminus*TMath::Tan(hit.th);
      break;
    default:
      printf("In JSFCALGeoParam::HitPosition..");
      printf("Illegal cell number %d\n",iCellID);
      break;
  }

  return hit;
}


//__________________________________________________________
void JSFCALGeoParam::HitCellGeom(Int_t iCellID, Float_t geo[])
{

// Returns array of CAL hit cell information.
//   geo[0:2] = x,y,z of cell center ( at CAL surface )
//   geo[3:5] = dx,dy,dz of half width
//   geo[6:7] = th and phi
// c
// c  EM Cal case
// c
// C  THT now(92/05/20) means zeta defined below. Equal spacing
// C  segmentation in zeta corresponds to equal counter size.
// C     2*zeta     = ln( ( 1 - cos(theta) )/( 1 + cos(theta) ) )
// C     cos(theta) = (1-exp(2*zeta))/(1+exp(2*zeta))
// c  

  Int_t     iabs = TMath::Abs(iCellID);
  Int_t     izee = iCellID/1000000;
  Double_t xth   = (Double_t)((iabs/1000)%1000) + 0.5 ;
  Double_t xphi  = (Double_t)(iabs%1000) + 0.5  ;

  JSFRThPhi hit;
  JSF3DV p1,p2;  
  hit.phi = fPhiStep*xphi;
  Double_t zeta, sinth;
  Double_t zeta1, zeta2, cos1,cos2,sin1,sin2;
  switch( izee ) {
    case 0:
      hit.r=fRmax;
      zeta1 = 2*(fBZetaMin + (xth-0.5)*fBZetaStep);
      zeta2 = 2*(fBZetaMin + (xth+0.5)*fBZetaStep);
      cos1 = ( 1. - TMath::Exp(zeta1))/(1.0+TMath::Exp(zeta1));
      cos2 = ( 1. - TMath::Exp(zeta2))/(1.0+TMath::Exp(zeta2));
      sin1 = TMath::Sqrt( (1.0-cos1)*(1.0+cos1) );
      sin2 = TMath::Sqrt( (1.0-cos2)*(1.0+cos2) );
      p1.x=hit.r*sin1*TMath::Cos(hit.phi);
      p1.y=hit.r*sin1*TMath::Sin(hit.phi);  p1.z=hit.r*cos1;
      p2.x=hit.r*sin2*TMath::Cos(hit.phi);
      p2.y=hit.r*sin2*TMath::Sin(hit.phi);  p2.z=hit.r*cos2;
      geo[0]=(p1.x+p2.x)/2.0 ;  geo[1]=(p1.y+p2.y)/2.0 ;
      geo[2]=(p1.z+p2.z)/2.0 ;
      geo[3]=(p2.x-p1.x)/2.0 ;  geo[4]=(p2.y-p1.y)/2.0 ;
      geo[5]=(p2.z-p1.z)/2.0 ;
      geo[6]=0.0;
      geo[7]=hit.phi;
      break;

    case 1:
      zeta = fPZetaMin + xth*fPZetaStep;
      sinth = ( 1 - TMath::Exp(2*zeta))/(1.0+TMath::Exp(2*zeta));
      hit.th = TMath::ACos(sinth);
      hit.r  = fZplus*TMath::Tan(hit.th);
      break;
    case -1:
      zeta = fMZetaMin + xth*fMZetaStep;
      sinth = ( 1 - TMath::Exp(2*zeta))/(1.0+TMath::Exp(2*zeta));
      hit.th = TMath::Pi()/2.0 - TMath::ACos(sinth);
      hit.r  = -fZminus*TMath::Tan(hit.th);
      break;
    default:
      printf("In JSFCALGeoParam::HitPosition..");
      printf("Illegal cell number %d\n",iCellID);
      break;
  }

}



//______________________________________________________________________________
void JSFQuickSimParam::Streamer(TBuffer &R__b)
{
   // Stream an object of class JSFQuickSimParam.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TNamed::Streamer(R__b);
      R__b >> fSeed;
      R__b >> fBfield;
      R__b.ReadStaticArray(fTrack);
      R__b.ReadStaticArray(fEMCal);
      R__b.ReadStaticArray(fHDCal);
      R__b >> fNSMPVX;
      R__b >> fDPHIVX;
      R__b >> fDZEEVX;
      R__b >> fNUMVTX;
      R__b.ReadStaticArray((float*)fVTXLayer);
      R__b.ReadStaticArray((float*)fVTXError);
      R__b.ReadStaticArray((float*)fCLSPAR);
   } else {
      R__b.WriteVersion(JSFQuickSimParam::IsA());
      TNamed::Streamer(R__b);
      R__b << fSeed;
      R__b << fBfield;
      R__b.WriteArray(fTrack, 8);
      R__b.WriteArray(fEMCal, 11);
      R__b.WriteArray(fHDCal, 11);
      R__b << fNSMPVX;
      R__b << fDPHIVX;
      R__b << fDZEEVX;
      R__b << fNUMVTX;
      Int_t nout=4*(fNSMPVX+1);
      R__b.WriteArray((float*)fVTXLayer, nout);
      R__b.WriteArray((float*)fVTXError,5);
      R__b.WriteArray((float*)fCLSPAR, 20);
   }
}


