//*LastUpdate:  jsf-1-14 15-Feb-2000 Akiya Miyamoto
//*-- Author :  Akiya Miyamoto

//////////////////////////////////////////////////////////////////
//
//  NuNuGam
//  
// Sample program in example/NuNuGam directory.
//  
//////////////////////////////////////////////////////////////////

#include "TRandom.h"
#include "JSFSteer.h"
#include "NuNuGam.h"

ClassImp(NuNuGamSpring)
ClassImp(NuNuGamSpringBuf)
ClassImp(NuNuGamBases)



//___________________________________________________________________________
NuNuGamBases::NuNuGamBases()
{
    PI = 3.1415926536;
    ALP = 137.036;
    GEVNB = 0.38927e6;
    GENER = 3.0;

           EM    = 0.511e-3;
           ZM    = 90.0;
           WM    = 78.97;
           ZGAM  = 2.6;
           ALPHA  = 1.0/ALP;
           RAD    = PI/180.0;
           TWOPI  = 2.0*PI;
           SQ     = sqrt(ZM*ZM - WM*WM);
           CZ     = ZM*ZM/(2.0*WM*SQ);
           CA     = 0.5*CZ;
           CV     = 2.0*CA*(2.0*SQ*SQ/(ZM*ZM) - 0.5);
           FACTOR = GENER*CZ*CZ*ALPHA*ALPHA*ALPHA/12.0 *GEVNB;
           COSMIN = 15.0;
           COSMAX =180.0 - COSMIN;
           W      = 105.0;
           XKMIN  = 1.0/W;
           XKMAX  = 0.5*W;
           CSMAX  = cos( COSMAX*RAD );
           CSMIN  = cos( COSMIN*RAD );

          EM2   = EM*EM;
          EM4   = EM2*EM2;

   /****
    double XLow[2], XUpper[2];
    int    grid[2];


    int NoDim  = 2;
    int NoWild = 2;

        XLow[0]   = XKMIN;
        XUpper[0] = XKMAX;
        grid[0]   = 1;

        XLow[1]   = CSMAX;
        XUpper[1] = CSMIN;
        grid[1]   = 1;

      defineVariable( NoDim, NoWild, XLow, XUpper, grid );
     ****/

      DefineVariable( COSTH, CSMAX, CSMIN, 0, 1 );
      DefineVariable( XK,    XKMIN, XKMAX, 1, 1 );

  
      Xh_init(1, XKMIN, XKMAX, 40,"Photon Energy (GeV)" );
      Xh_init(2, CSMAX, CSMIN, 50,"Cos(theta) of Photon" );
      Dh_init(1, CSMAX, CSMIN, 50,
               XKMIN, XKMAX, 30," x : Cos(theta) --- y : Photon Energy");

      H1Init("hXk","Photon Energy (GeV)",40, XKMIN, XKMAX);
      H1Init("hCosth","Cos(theta) of Photon", 50,CSMAX, CSMIN);
      H2Init("hXkCs"," x : Cos(theta) --- y : Photon Energy",
	     50, CSMAX, CSMIN, 30, XKMIN, XKMAX);

}

//___________________________________________________________________________
NuNuGamBases::~NuNuGamBases()
{
}

//___________________________________________________________________________
double NuNuGamBases::Func( )
{
  //*                 ===================================                        *
  //******************************************************************************
  //* ( Function )                                                               *
  //*   The integrand function re-defined by the user instead of virtual func    *
  //*  in the super class BasesSpring.                                           *
  //******************************************************************************/
          ans   = 0.0;

//          XK    = x[0];
          S     = W*W;
          S1    = W*(W-2.0*XK);
          E     = 0.5*W;
          PP    = sqrt((E + EM)*(E - EM));
//          COSTH = x[1];
          D1    = XK*(E + PP*COSTH);
          D2    = XK*(E - PP*COSTH);

          S2    = S*S;
          S3    = S2*S;
          CA2   = CA*CA;
          CV2   = CV*CV;
          W2    = W*W;
          XK2   = XK*XK;
          ZM2   = ZM*ZM;

      ANS1 = (  S2*CA2 + S2*CV2 - 2.0*S*W*XK*CA2 - 2.0*S*W*XK*CV2
              - 3.0*S*CA2*EM2 - 3.0*S*CV2*EM2 + 2.0*W*XK*CA2*EM2
              + 2.0*W*XK*CV2*EM2)/D1
           + S1*( -S2*CA2-S2*CV2 + 10.0*S*CA2*EM2 - 2.0*S*CV2*EM2
              -16.0*CA2*EM4 + 8.0*CV2*EM4 )/(2.0*D1*D2)
           + ( -S3*CA2-S3*CV2 + 2.0*S2*W*XK*CA2 + 2.0*S2*W*XK*CV2
               + 2.0*S2*CA2*EM2 + 2.0*S2*CV2*EM2 - 4.0*S*W*XK*CA2*EM2
               - 4.0*S*W*XK*CV2*EM2 - 4.0*W2*XK2*CA2*EM2
               + 4.0*W2*XK2*CV2*EM2 )/(2.0*D1*D2)
           - (D2*S1*EM2)*(CA2+CV2)/(D1*D1)
           + D2*EM2*(-S*CA2-S*CV2 + 2.0*W*XK*CA2 + 2.0*W*XK*CV2)/(D1*D1)
           + S1*EM2*(S*CA2+S*CV2 - 8.0*CA2*EM2 + 4.0*CV2*EM2)/(2.0*D1*D1)
           + S*EM2*(S*CA2+S*CV2 - 2.0*W*XK*CA2 - 2.0*W*XK*CV2)/(2.0*D1*D1);
      TAU= - (D1*S1)*(CA2+CV2)/D2
           + D1*(-S*CA2-S*CV2+2.0*W*XK*CA2 + 2.0*W*XK*CV2 + 2.0*CA2*EM2
              + 2.0*CV2*EM2)/D2
           - (D1*S1*EM2)*(CA2+CV2)/(D2*D2)
           + D1*EM2*(-S*CA2 - S*CV2 + 2.0*W*XK*CA2 + 2.0*W*XK*CV2)/(D2*D2)
           + 4.0*EM2*(CA2+CV2)
           + S1*(S*CA2+S*CV2 - 3.0*CA2*EM2 - 3.0*CV2*EM2)/D2
           + (S2*CA2+S2*CV2 - 2.0*S*W*XK*CA2 - 2.0*S*W*XK*CV2
              - 3.0*S*CA2*EM2 - 3.0*S*CV2*EM2 + 2.0*W*XK*CA2*EM2
              + 2.0*W*XK*CV2*EM2)/D2
           + S1*EM2*(S*CA2+S*CV2 - 8.0*CA2*EM2 + 4.0*CV2*EM2)/(2.0*D2*D2)
           + S*EM2*(S*CA2+S*CV2 - 2.0*W*XK*CA2 - 2.0*W*XK*CV2)/(2.0*D2*D2)
           - (D2*S1)*(CA2+CV2)/D1
           + D2*(-S*CA2-S*CV2 + 2.0*W*XK*CA2 + 2.0*W*XK*CV2 + 2.0*CA2*EM2
                 + 2.0*CV2*EM2)/D1
           + S1*(S*CA2+S*CV2 - 3.0*CA2*EM2 - 3.0*CV2*EM2)/D1
           + ANS1;

       rez  = S1 - ZM2;
       z2   = rez*rez + ZM2*ZGAM*ZGAM;

       DSDX = -FACTOR*TAU*XK/(E*z2);
       ans  = DSDX/E;

       Fill_hist( DSDX );

      return ans;
}

//___________________________________________________________________________
void NuNuGamBases::Fill_hist( double fx )
{ 
  //******************************************************************************
  //* ( Purpose )                                                                *
  //*   To fill the histograms and scatter plots.                                *
  //******************************************************************************/
  /*
    Xh_fill(1, XK, fx );
    Xh_fill(2, COSTH, fx );
    Dh_fill(1, COSTH, XK, fx );
  */
    H1Fill("hXk",XK, fx);
    H1Fill("hCosth",COSTH, fx);
    H2Fill("hXkCs",COSTH,XK,fx);

}


//___________________________________________________________________________
NuNuGamSpring::NuNuGamSpring(const char *name, const char *title,
			 NuNuGamBases *bases)
  : JSFSpring(name, title, bases)
{
  fEventBuf = new NuNuGamSpringBuf("NuNuGamSpringBuf", 
	 "NuNuGamSpring event buffer", this);
  if( !bases ) { 
    NuNuGamBases *bs=new NuNuGamBases();
    SetBases(bs);
  }
}



//___________________________________________________________________________
NuNuGamSpring::~NuNuGamSpring()
{
  if( fEventBuf ) { delete fEventBuf; fEventBuf=NULL ; }
}

//___________________________________________________________________________
Bool_t NuNuGamSpringBuf::SetPartons()
{
  // Save only photon 

  TClonesArray &partons = *fPartons;
  NuNuGamBases *bases=(NuNuGamBases*)((NuNuGamSpring*)Module())->GetBases();

  fCosth = bases->COSTH;
  fXk    = bases->XK;
  TVector p1(4);
  p1(0)  = fXk;
  Double_t phi=2.0*TMath::Pi()*gRandom->Rndm();
  Double_t sinth=TMath::Sqrt( (1.0-fCosth)*(1.0+fCosth) ); 
  p1(1)  = fXk*sinth*TMath::Cos(phi);
  p1(2)  = fXk*sinth*TMath::Sin(phi);
  p1(3)  = fXk*fCosth;

  Double_t charge=0.0;
  Int_t id=22;

  new(partons[0]) JSFSpringParton(1,  id, 0.0,  charge, p1,
		  0, 0, 0, 0, 0, 0);

  return kTRUE ;
}





