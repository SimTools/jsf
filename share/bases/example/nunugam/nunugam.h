#include <iostream.h>
#include <stdio.h>
#include <math.h>

#include "BasesSpring.h"
                                                                            /*
******************************************************************************
*                  ===================================                       */
                   class nunugam : public BasesSpring                       /*
*                  ===================================                       *
******************************************************************************
* ( Function )                                                               *
*   User defined derived class where the integrand function is re-defined.   *
******************************************************************************/
{
 public:
    double W, EM, ZM, ZGAM, CZ, CV, CA, FACTOR, XK, COSTH;
    double PI, ALP, GEVNB, GENER;

    double ans, ANS1, TAU;
    double S, S1, E, PP, D1, D2;
    double EM2, EM4, S2, S3, CA2, CV2, W2, XK2, DSDX, rez, z2, ZM2;

    double ALPHA, RAD, TWOPI, SQ, COSMIN, COSMAX, CSMIN, CSMAX, XKMIN, XKMAX;
    double PHI, PXY, WM;

    nunugam( );
    double func( );
    virtual void fill_hist( double dsdx );
};
