#include <iostream>
#include <stdio.h>
#include <math.h>

#include "nunugam.h"
                                                                            /*
******************************************************************************
*                 ===================================                        */
                         nunugam::nunugam()                                 /*
*                 ===================================                        *
******************************************************************************/
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

      defineVariable( COSTH, CSMAX, CSMIN, 0, 1 );
      defineVariable( XK,    XKMIN, XKMAX, 1, 1 );
}
                                                                            /*
******************************************************************************
*                 ===================================                        */
                       double nunugam::func( )                              /*
*                 ===================================                        *
******************************************************************************
* ( Function )                                                               *
*   The integrand function re-defined by the user instead of virtual func    *
*  in the super class BasesSpring.                                           *
******************************************************************************/
{
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

       fill_hist( DSDX );

      return ans;
}
                                                                            /*
******************************************************************************
*                 ===================================                        */
                  void nunugam::fill_hist( double fx )                      /*
*                 ===================================                        *
******************************************************************************
* ( Purpose )                                                                *
*   To fill the histograms and scatter plots.                                *
******************************************************************************/
{ }

