#include "nunugam.h"
                                                                            /*
******************************************************************************
*                  ===================================                       */
                     class testgen : public nunugam                         /*
*                  ===================================                       *
******************************************************************************
* ( Function )                                                               *
*   User defined derived class for event generation                          *
******************************************************************************/
{
 public:
    testgen( );
    void fill_hist( double fx );
};
                                                                            /*
******************************************************************************
*                 ===================================                        */
                         testgen::testgen()                                 /*
*                 ===================================                        *
******************************************************************************/
{
    xh_init(1, XKMIN, XKMAX, 40,"Photon Energy (GeV)" );
    xh_init(2, CSMAX, CSMIN, 50,"Cos(theta) of Photon" );

    xh_init(3, 0.0, 40.0, 40, "Photon Transverse Energy (GeV)");

    dh_init(1, CSMAX, CSMIN, 50,
               XKMIN, XKMAX, 30," x : Cos(theta) --- y : Photon Energy");
}
                                                                            /*
******************************************************************************
*                 ===================================                        */
                  void testgen::fill_hist( double fx )                      /*
*                 ===================================                        *
******************************************************************************/
{
    xh_fill(1, XK, fx );
    xh_fill(2, COSTH, fx );

    double pxy   = XK*sqrt((1.0 - COSTH)*( 1.0 + COSTH));
    xh_fill( 3, pxy, fx );
    dh_fill(1, COSTH, XK, fx );
}

                                                                            /*
******************************************************************************
*                            ============                                    */
                                main()                                      /*
*                            ============                                    *
******************************************************************************
* ( Function )                                                               *
*   The main program which control the numerical integration and event       *
*   generation.                                                              *
******************************************************************************/
{
    testgen bs1;

    bs1.bs_read( "bases.data" );


    int maxtry = 50;
    int no_event = 100000;

    for( int event = 0; event < no_event; event++ ) bs1.Spring( maxtry );

    bs1.sp_info( );
    bs1.sh_plot( );
}
