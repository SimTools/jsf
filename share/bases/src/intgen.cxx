#include "nunugam.h"
                                                                            /*
******************************************************************************
*                  ===================================                       */
                      class intgen : public nunugam                         /*
*                  ===================================                       *
******************************************************************************
* ( Function )                                                               *
*   User defined derived class for event generation                          *
******************************************************************************/
{
 public:
    intgen( );
    void fill_hist( double fx );
};
                                                                            /*
******************************************************************************
*                 ===================================                        */
                           intgen::intgen()                                 /*
*                 ===================================                        *
******************************************************************************/
{
    xh_init(1, XKMIN, XKMAX, 40,"Photon Energy (GeV)" );
  //  xh_init(2, CSMAX, CSMIN, 50,"Cos(theta) of Photon" );
  //  dh_init(1, CSMAX, CSMIN, 50,
  //             XKMIN, XKMAX, 30," x : Cos(theta) --- y : Photon Energy");
}
                                                                            /*
******************************************************************************
*                 ===================================                        */
                  void intgen::fill_hist( double fx )                       /*
*                 ===================================                        *
******************************************************************************/
{
       xh_fill(1, XK, fx );
  //  xh_fill(2, COSTH, fx );
  //  dh_fill(1, COSTH, XK, fx );
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
    intgen bs1;

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

    printf("Start Spring\n");

    int maxtry = 50;
    int no_event = 100;

    for( int event = 0; event < no_event; event++ ) bs1.Spring( maxtry );

    bs1.sp_info( );
    bs1.sh_plot( );
}
