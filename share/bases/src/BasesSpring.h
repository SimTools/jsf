#include "bases_hist.h"
#include "bs_hist.h"
#include "bs_variable.h"
#include "bases_ran1.h"

#define MXDIM 50
#define MXND  50
#define LENG  32768
#define ITM   50

                                                                             /*
******************************************************************************
*                      ==================                                    */
                       class BasesSpring                                   /*
*                     ====================                                   * 
******************************************************************************
* ( Purpose )                                                                *
*   To make numerical integration call integ( ) and                          *
*   to generate an event call event( ) which reproduces the distribution     *
*   given as the integrand.                                                  *
* ( Variables )                                                              *
*   ndim      :  the dimension of the integral volume                        *
*   nwild     :  the number of wild variables                                *
*   ig[]      :  flag of grid optimization for each independent variavle     *
*   ncall     :  given number of sampling points per iteration               *
*   xl[], xu[]:  the lower and upper limit values of each variable           *
*   nd        :  the number of subregions for each variable                  *
*   xi[][]    :  the upper bound values of subregions                        *
*   d[][]     :  histogram of function squared for each variable             *
*   dx[]      :  xu[] - xl[] : size of integral region for each variable     *
******************************************************************************/
{                                                                           /*
  -----------------------
    protected varables
  -----------------------                                                   */
 protected:
    int year, month, day, hour, min;
    int    ndim,     nwild,   ig[MXDIM], ncall, ngiven;
    long   nd,   ng, nsp, npg, ma[MXDIM];
    int    kg[MXDIM], ia[MXDIM];
    double xl[MXDIM], xu[MXDIM];
    double xi[MXND][MXDIM], d[MXND][MXDIM], dx[MXDIM]; 
    double dxd[LENG]; 
    double pxd[LENG];
    //    long   seed;
    bases_ran1 ran1;
    int    it,       itmx1,  itmx2;
    double acc1,     acc2;
    double alpha,    scalls;
    double xnd,      jacob,  dxg,      wgt,    dxmax;
    double si,       swgt,   estim,    error;
    int    flag_neg, maxtry, no_event, no_try, no_miss;

    int   itr[ITM][2], ief[ITM][2];
    float wrong[ITM][2],time [ITM][2],
          aestm[ITM][2],aerro[ITM][2], 
          testm[ITM][2],terro[ITM][2];

    bases_hist  his;     // For the default histogramming
    bs_hist     hist;    // histogram for the generation efficiency of SPRING

    int         noMild;
    bs_variable **Mild;
    bs_variable **var;
                                                                           /* 
  ---------------------
    private functions
  ---------------------                                                    */
    void printB( int step );
    void time_conv( float ctime, int &hour, int &min, int &s1, int &s2 );
    void listB( int i, int step );
    void setVariable( );
    void setGunif();
    void adjustG( int print );
    void listG();
    void integ( int step );
                                                /*
=========                                                                  */
 public:                                                                   /*
========= 
  ---------------------
    public variables
  ---------------------                                                    */
    int flag_bases;
                                                                           /*
  ---------------------
    public functions
  ---------------------                                                    */
           BasesSpring( );
	   virtual ~BasesSpring( );

	   virtual void EndIntegration( int step );

	   bases_ran1 *GetRan1(){ return &ran1; }

//  For defining parameters for BasesSpring
    void   param( int sample, double tune,
                          int itr1, double ac1, int itr2, double ac2 );
    void   setNoOfSample( int sample ){ ncall = sample; };
    void   setTuneValue ( double tune ){ alpha = tune; };
    void   setIteration1( double ac1, int itx1 ) { acc1 = ac1;  itmx1 = itx1; };
    void   setIteration2( double ac2, int itx2 ) { acc2 = ac2;  itmx2 = itx2; };
    void   defineVariable( double &v, 
                           double low, double high, int wld, int grd );
    void   defineVariable( int ndim, int nwild, 
                                    double x_l[], double x_u[], int jg[] );
//  For integration  and evet generation
    void   Bases( );
    void   bs_write( char* FileName );
    void   bs_read( char* FileName );
    void   Spring( int mxtry );
    void   sp_info( );

//  Utilities
    void   forder( float v, float &fv, float &order, int &iorder );
    double getwgt( )        { return wgt; }
    double getscalls( )     { return scalls; }
    int    Flag_bases( )    { return flag_bases; }
    double getEstimate( )   { return estim; };
    double getError( )      { return error; };
    int    getNoOfIterate( ){ return it; };

// For histogramming package
    void   xh_init( int id, double x_low, double x_high, int bin, char title[] )
                        { his.xh_init( id, x_low, x_high, bin, title ); }
    void   dh_init( int id, double x_low, double x_high, int Xbin,
                            double y_low, double y_high, int Ybin, char title[] )
                        { his.dh_init( id, x_low, x_high, Xbin,
                                           y_low, y_high, Ybin, title ); }
    void   bh_plot( )   { his.bh_plot( getscalls() ); };
    void   xh_fill( int id, double x, double fx )
                       { his.xh_fill( id, x, fx, getwgt(), Flag_bases() ); }
    void   dh_fill( int id, double x, double y, double fx )
                       { his.dh_fill( id, x, y, fx, getwgt(), Flag_bases() ); }
    void   bh_save( )  { his.bh_save(); };
    virtual void   sh_reset( ) { his.sh_reset();};
    virtual void   sh_update( ){ his.sh_update(); };
    void   sh_plot( )  { his.sh_plot( getscalls() ); };

    virtual double func( double x[] );
    virtual double func( );
};
