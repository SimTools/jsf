#include "nunugam.h"
                                                                            /*
******************************************************************************
*                  ===================================                       */
                     class testint : public nunugam                         /*
*                  ===================================                       *
******************************************************************************
* ( Function )                                                               *
*   User defined derived class for numerical integration bases.              *
******************************************************************************/
{
 public:
    testint( );
    void fill_hist( double fx );
};
                                                                            /*
******************************************************************************
*                 ===================================                        */
                         testint::testint()                                 /*
*                 ===================================                        *
******************************************************************************/
{
    xh_init(1, XKMIN, XKMAX, 40,"Photon Energy (GeV)" );
    xh_init(2, CSMAX, CSMIN, 50,"Cos(theta) of Photon" );
    dh_init(1, CSMAX, CSMIN, 50,
               XKMIN, XKMAX, 30," x : Cos(theta) --- y : Photon Energy");
}
                                                                            /*
******************************************************************************
*                 ===================================                        */
                  void testint::fill_hist( double fx )                      /*
*                 ===================================                        *
******************************************************************************/
{
    xh_fill(1, XK, fx );
    xh_fill(2, COSTH, fx );
    dh_fill(1, COSTH, XK, fx );
}

                                                                            /*
******************************************************************************
*                            ============                                    */
                                main()                                      /*
*                            ============                                    *
******************************************************************************
* ( Function )                                                               *
*   The main program which control the numerical integration.                *
******************************************************************************/
{
    testint bs1;

    int    ncall = 5000;
    int    itr1  =  10;
    double acc1  = 0.1;
    int    itr2  = 100;
    double acc2  = 0.05;
    double tune  = 1.5;

//    bs1.param( ncall, tune, itr1, acc1, itr2, acc2 );

    bs1.setNoOfSample( ncall );
    bs1.setTuneValue ( tune );
    bs1.setIteration1( acc1, itr1 );
    bs1.setIteration2( acc2, itr2 );

    bs1.Bases( );

    bs1.bh_plot( );

    bs1.bs_write( "bases.data" );

}
