#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "BasesSpring.h"

// double ran1( long &idum );
float  uxtime();
void   uxdate( int &y, int &m, int &d, int &h, int &min );

                                                                             /*
******************************************************************************
*              ===============================================               */
                     BasesSpring :: BasesSpring(  )                         /*
*             =================================================              *
******************************************************************************
* ( Function )                                                               *
*   Constructor of the object BasesSpring.                                   *
* ( Input )                                                                  *
*   dim    : the number of integral variables ( dimension )                  *
*   wild   : the number of wild variables                                    *
* ( Output )                                                                 *
*   Default values for xl[], xu[], ig[], itmx1, itmx2, acc1, and acc2        *
*   ndim   = dim;                                                            *
*   nwild  = wild;                                                           *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{

  flag_bases = 1;
  flag_neg   = 0;

  uxdate( year, month, day, hour, min );

  ndim   = 0;
  nwild  = 0;
  noMild = 0;

  var   = new bs_variable* [ MXDIM ];
  Mild  = new bs_variable* [ MXDIM ];

  for( int i = 0; i < MXDIM; i++ )
     {
       xl[i]  = 0.0;
       xu[i]  = 1.0;
       ig[i]  = 1;
      }

  itmx1 = 10;
  itmx2 = 20;
  acc1  = 0.1;
  acc2  = 0.05;
  alpha = 1.0;
}
                                                                             /*
******************************************************************************
*              ===============================================               */
                     BasesSpring :: ~BasesSpring(  )                        /*
*             =================================================              *
******************************************************************************
* ( Function )                                                               *
*   Destructor of the object BasesSpring.                                    *
* ( Author )                                                                 *
*   coded by S.kawabata July '99 at KEK                                      *
******************************************************************************/
{
  if( var[0] )
    {
      for( int i = 0; i < ndim; i++ ) delete  var[i];
      delete [ ] var;
      /**********/
      printf("The variable object var[ ] were deallocated.\n");
      /**********/
     }
}
                                                                             /*
******************************************************************************
* ========================================================================== */
  void BasesSpring :: defineVariable
      ( double& v, double low, double high, int wld, int grd )             /*
* ========================================================================== *
******************************************************************************
*                                                                            *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
   if( wld == 1 )
     {
       var[nwild] = new bs_variable;
       var[nwild]->defineVariable( v, low, high, wld, grd );

       xl[nwild] = low;
       xu[nwild] = high;
       ig[nwild] = grd;
       nwild++;
      }
    else
     {
       Mild[noMild] = new bs_variable;
       Mild[noMild]->defineVariable( v, low, high, wld, grd );
       noMild++;
      }

    ndim++;
}
                                                                             /*
******************************************************************************
*                 ======================================                     */
                    void BasesSpring :: setVariable( )                      /*
*                 ======================================                     *
******************************************************************************
*                                                                            *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
  if( noMild > 0 )
    {
      int i;
      for(    i = 0; i < noMild; i++ )
         {
           int j = i + nwild;
           var[j] = new bs_variable;
           var[j]->defineVariable( Mild[i]->getVariable(), 
                                   Mild[i]->Lower(), Mild[i]->Upper(),
                                   Mild[i]->Wild(),  Mild[i]->Grid() );
           xl[j] = var[j]->Lower();
           xu[j] = var[j]->Upper();
           ig[j] = var[j]->Grid();
          }
    
      //      for( i = 0; i < noMild; i++ ) delete  Mild[i];
      delete [ ] Mild;
      noMild=0;
     }
}
                                                                             /*
******************************************************************************
*              ===========================================                   */
                double BasesSpring :: func( double x[ ] )                   /*
*              ===========================================                   *
******************************************************************************
*                                                                            *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
   printf("***************************************************************\n");
   printf("* Fatal Error : Inconsistency was found                       *\n");
   printf("*                between use of method defineVariable( ... )  *\n");
   printf("*                    and definition of method func( x ).      *\n");
   printf("***************************************************************\n");

   printf("Since you defined the integral varables by the method   \n");
   printf("   void defineVariable( int    ndim,    : number of dimensions \n");
   printf("                        int    nWild,   : number of");
   printf(                                         " wild variables\n");
   printf("                        double low[],   : array of lower");
   printf(                                         " limit values\n");
   printf("                        double high[],  : array of upper");
   printf(                                         " limit values\n");
   printf("                        int    ig[]     : array of grid flag\n");
   printf("                       )\n");
   printf("you should overload the method func( double x[ ] ) with args.\n");

   exit( 0 );

  return 0.0;
}
                                                                             /*
******************************************************************************
*              ===========================================                   */
                    double BasesSpring :: func(  )                          /*
*              ===========================================                   *
******************************************************************************
*                                                                            *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
   printf("***************************************************************\n");
   printf("* Fatal Error : Inconsistency was found                       *\n");
   printf("*                between use of method defineVariable( ... )  *\n");
   printf("*                    and definition of method func( ).        *\n");
   printf("***************************************************************\n");

   printf("Since you defined the integral varables by the method   \n");
   printf("   void defineVariable( double &v,    : reference of variable\n");
   printf("                        double low,   : lower");
   printf(                                         " limit value\n");
   printf("                        double high,  : upper");
   printf(                                         " limit value\n");
   printf("                        int    wild,  : wild flag\n");
   printf("                        int    grid   : wild flag\n");
   printf("                       )\n");
   printf("you should overload the method func( ) without args.\n");

   exit( 0 );

  return 0.0;
}
                                                                             /*
******************************************************************************
* ========================================================================== */
  void BasesSpring :: time_conv( float t, int &h, int &m, int &s1, int &s2 )/*
* ========================================================================== *
******************************************************************************
*                                                                            *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
  static int hr = 360000, mn = 6000, n100 = 100;

  t        = t*n100;
  int isec = (int) t;

  h    = isec/hr;
  isec = isec % hr;

  m    = isec / mn;
  isec = isec % mn;

  s1   = isec / n100;
  s2   = isec % n100;
}
                                                                             /*
******************************************************************************
* ========================================================================== */
  void BasesSpring :: forder( float v, float &fv, float &ordr, int &iordr ) /*
* ========================================================================== *
******************************************************************************
*                                                                            *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
  if( v == 0.0 ) 
    {
      fv     = 0.0;
      ordr  = 1.0;
      iordr = 0;
     }
  else 
    {
      if( v > 0.0 ) ordr = (float) log10( (double)  v );
      else          ordr = (float) log10( (double) -v );

      iordr = (int) ordr;
      if( ordr < 0.0 ) iordr--;
      ordr  = (float) pow( 10.0, (double) iordr );
      fv     = v/ordr;
     }
}
                                                                             /*
******************************************************************************
*             =============================================                  */
               void BasesSpring :: listB( int i, int step )                 /*
*             =============================================                  *
******************************************************************************
*                                                                            *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
  float re, ac, are, fv, ordr;
  int   iordr, Hour, Min, s1, s2;

  time_conv( time[i][step], Hour, Min, s1, s2 );

  re  =  aestm[i][step];
  ac  =  aerro[i][step];
  are =  re;
  if( are < 0.0 ) are = -are;

  if( are > ac ) forder( are, fv, ordr, iordr );
  else           forder(  ac, fv, ordr, iordr );

  re   = re/ordr;
  ac   = ac/ordr;
  float t =  testm[i][step];
  float a =  aestm[i][step];
  if( t < 0.0 ) t = -t;
  if( a < 0.0 ) a = -a;
  float tper = terro[i][step]/ t *100.0;
  float aper = aerro[i][step]/ a *100.0;

  if( tper > 100.0 ) tper = 99.9;
  if( aper > 100.0 ) aper = 99.9;

  printf("%4d %3d%6.2f%11.3e %6.3f%10.6f(+-%8.6f)E%3.2d %6.3f",
           itr[i][step], ief[i][step], wrong[i][step], 
           testm[i][step], tper, re, ac, iordr, aper );
  printf(" %3d:%2d:%2d.%2.2d\n", Hour, Min, s1, s2 );
}
                                                                             /*
******************************************************************************
*              ======================================                        */
                  void BasesSpring :: printB(int step )                     /*
*              ======================================                        *
******************************************************************************
*                                                                            *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
 char cn = 0xc;

 static char *ss = "        ";
 if( step == 0 ) {
 printf("%c\n", cn );
 printf("\n\n");
 printf("%s                                             ", ss);
 printf("Date:  %2d/%2d/%2d  %2d:%2.2d\n", year, month, day,  hour, min );
 printf("%s**********************************************************\n",ss);
 printf("%s*                                                        *\n",ss);
 printf("%s*     BBBBBBB     AAAA     SSSSSS   EEEEEE   SSSSSS      *\n",ss);
 printf("%s*     BB    BB   AA  AA   SS    SS  EE      SS    SS     *\n",ss);
 printf("%s*     BB    BB  AA    AA  SS        EE      SS           *\n",ss);
 printf("%s*     BBBBBBB   AAAAAAAA   SSSSSS   EEEEEE   SSSSSS      *\n",ss);
 printf("%s*     BB    BB  AA    AA        SS  EE            SS     *\n",ss);
 printf("%s*     BB    BB  AA    AA  SS    SS  EE      SS    SS     *\n",ss);
 printf("%s*     BBBB BB   AA    AA   SSSSSS   EEEEEE   SSSSSS      *\n",ss);
 printf("%s*                                                        *\n",ss);
 printf("%s*                BASES Version 5.10C++                   *\n",ss);
 printf("%s*           coded by S.Kawabata KEK, March 1997          *\n",ss);
 printf("%s**********************************************************\n",ss); 
 printf("\n\n");
 printf("<<  Parameters for Bases  >>\n\n");
 printf("(1) Dimenstion of integral etc.\n");
 printf("    # of dimenstions    : ndim     = %6d   ( max. 50 )\n", ndim);
 printf("    # of wild variables : nwild    = %6d   ( max. 15 )\n", nwild);
 printf("    # of sample points  : ncall    = %6d(real) %6d(given)\n",
                                    ncall, ngiven );
 printf("    # of subregions     : ng       = %6ld / variable\n", nd );
 printf("    # of regions        : nregions = %6ld / variable\n", ng );
 printf("    # of hypercubes     : Ncube    = %6ld\n\n", nsp );
 printf("(2) Integral variables\n");
 printf("    -----+-------------+-------------+-----+------\n");
 printf("       i      xl[i]         xu[i]     ig[i]  wild\n");
 printf("    -----+-------------+-------------+-----+------\n");
 for( int i = 0; i < ndim; i++ ) {
  if( i < nwild )
     printf("    %4d   %11.4e  %11.4e  %4d    yes\n", i, xl[i], xu[i], ig[i]);
  else
     printf("    %4d   %11.4e  %11.4e  %4d    no\n", i, xl[i], xu[i], ig[i]);
  }
 printf("    -----+-------------+-------------+-----+------\n\n");
 printf("(3) Prameter for integration\n");
 printf("    --------------------+----------------+------------------\n");
 printf("                           grid opt step   integration step\n");
 printf("    --------------------+----------------+------------------\n");
 printf("    Max. # of iteration   itmx1(  %4d  )  itmx2(  %4d  )\n", 
             itmx1, itmx2 );
 printf("    required accuracy     acc1 (%6.4f %%)  acc2 (%6.4f %%)\n", 
             acc1, acc2 );
 printf("    --------------------+----------------+------------------\n");
 printf("%c\n", cn );
 printf("\n\n");
 printf("%s                                             ", ss);
 printf("Date:  %2d/%2d/%2d  %2d:%2.2d\n", year, month, day,  hour, min );
 printf("%s***** Convergency Behavior for the Grid Optimization Step *****\n",
         ss);

 printf("--------------------------------");
 printf("----------------------------------------------\n");
 printf(" <- Result of  each iteration ->");
 printf("  <-     Cumulative Result     ->< CPU  time >\n");
 printf("  IT Eff R_Neg   Estimate   Acc%%");
 printf("  Estimate(+- Error )order  Acc%%");
 printf(" ( H: M: Sec )\n");
 }
else
if( step == 1 )
 {
 printf("%c\n", cn );
 printf("\n\n");
 printf("%s                                             ", ss);
 printf("Date:  %2d/%2d/%2d  %2d:%2.2d\n", year, month, day,  hour, min );
 printf("%s****** Convergency Behavior for the Integration Step ******\n",
         ss );

 printf("--------------------------------");
 printf("----------------------------------------------\n");
 printf(" <- Result of  each iteration ->");
 printf("  <-     Cumulative Result     ->< CPU  time >\n");
 printf("  IT Eff R_Neg   Estimate   Acc%%");
 printf("  Estimate(+- Error )order  Acc%%");
 printf(" ( H: M: Sec )\n");
 }

 printf("--------------------------------");
 printf("----------------------------------------------\n");
}
                                                                             /*
******************************************************************************
*                ===============================                             */
                 void BasesSpring :: setGunif()                             /*
*                ===============================                             *
******************************************************************************
* ( Function )                                                               *
*  To determine the number of regions, sub-regions, and hypercubes in the    *
*  integral volume, and to set their sizes uniform for initial state.        *
* ( Input )                                                                  *
*   ncall   : the given number of sampling points                            *
*   nwild   : the number of wild variables                                   *
*   ndim    : the number of independent variables ( dimension of integral )  *
* ( Output )                                                                 *
*   xi[][]  : the unifom grid of the subregions                              *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
  double xin[MXDIM];
                                                                            /*
 ============================================================================
 I.   Determine the numbers of regions and sub-regions
 ============================================================================
  +--------------------------------------------------------------------------+
  | 1. the number of regions                                                 |
  +--------------------------------------------------------------------------+
                                                                            */
  ng = (long int)(pow( ((double) ncall )*0.5, 1.0/(double) nwild ));

  if(      ng > 25 ) ng = 25;
  else if( ng <  2 ) ng =  1;

  while( (long) pow( (double) ng, (double) nwild ) > LENG )
    {
      ng--;
      if( ng < 2 ) ng = 1;
     }
                                                                            /*
  +--------------------------------------------------------------------------+
  | 2. the number of sub-regions and the number of sampling points per HP    |
  +--------------------------------------------------------------------------+
                                                                            */
  long m = MXND/ng;
  nd    = m * ng;

  nsp = (long int)(pow( (double) ng, (double)nwild ));
  npg = ncall/nsp;
                                                                            /*
 ============================================================================
 II. set the size of all sub-regions uniform
 ============================================================================
                                                                            */
  ma[0]    = 1;
  dx[0]    = xu[0] - xl[0];

  for( int i = 1; i < ndim; i++ )
     {
       dx[i]    = xu[i] - xl[i];
       if( i < nwild ) ma[i] = ng*ma[i-1];
      }

  double rc = 1.0/(double) nd;

  for( int j = 0; j < ndim; j++ )
     {
       xi[0][j] = rc;
       for( int i = 1; i < nd-1;   i++ ) xi[i][j] = xi[i-1][j] + rc;
       xi[nd-1][j] = 1.0;
      }
}
                                                                             /*
******************************************************************************
*               =======================================                      */
                void BasesSpring :: adjustG(int print)                      /*
*               =======================================                      *
******************************************************************************
* ( Function )                                                               *
*  To adjust the size of each sub-region according to the resultant          *
*  histogram of the square values of integrand function.                     *
* ( Input )                                                                  *
*   d[][]   : histograms of square values of integrand function              *
* ( Output )                                                                 *
*   xi[][]  : the upper value of each sub-region. xi[nd][i] = 1.0            *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
  double xin[MXND], r[MXND], dt[MXND], ddx[MXND];

  double clog = 1.0/log(10.0);
  int ndm = (int) nd - 1;

  int line, odd;
  odd = (int) nd % 2;
  line = (int) nd/2;


  for( int j = 0; j < ndim; j++ )
     {
       if( print > 0 ) {
         printf("*** Grid information for %2d-th varable ***\n", j );
         printf(" no upper-limit   frequency     density");
         printf("  no upper-limit   frequency     density\n");
        }
                                                                            /*
       +---------------------------------------------------------------------+
       | 1. Smoothing the histogram for each variable                        |
       +---------------------------------------------------------------------+
                                                                            */
       if( ig[j] == 1 )
         {
           ddx[0]      = ( d[0][j] + d[1][j] )*0.5;
	   int i;
           for(  i = 1; i < ndm; i++ )
                ddx[i] = ( d[i+1][j] + d[i][j] + d[i-1][j])/3.0;
           ddx[ndm]    = ( d[ndm-1][j] + d[ndm][j])*0.5;

           dt[j] = 0.0;
           for( i = 0; i < nd; i++ )
              {
                d[i][j] = ddx[i];
                dt[j]   = dt[j] + d[i][j];
               }
                                                                            /*
       +---------------------------------------------------------------------+
       | 2. Take normalization used in the size adjustment                   |
       +---------------------------------------------------------------------+
                                                                            */
           double dtlog = log( dt[j] );
           double dt10  = clog*dtlog;
           double rc    = 0.0;
           double x0    = 0.0;

           for( i = 0; i < nd; i++ )
              {
                r[i] = 0.0;
                if( d[i][j] > 0.0 )
                  {
                    double dlog = log( d[i][j] );
                    if( dt10 - clog*dlog <= 70.0 )
                      {
                        x0   = dt[j]/d[i][j];
                        r[i] = pow( (x0 - 1.0)/(x0*(dtlog - dlog)), alpha );
                       }
                    else
                      {
                        x0   = dt[j]/d[i][j];
                        r[i] = pow( dtlog - dlog, -alpha );
                       }
                   }
                rc = rc + r[i];
               }
          if( print > 0 ) {
            for( i = 0; i < line; i++ )
               {
                 int c1 = 2*i;
                 int c2 = c1 + 1;

                 printf("%3d %11.8f %11.8f %11.8f %3d %11.8f %11.8f %11.8f\n",
                         c1, xi[c1][j], d[c1][j], r[c1], 
                         c2, xi[c2][j], d[c2][j], r[c2]);
                }
            if( odd )
              {
                int c1 = 2*line;
  
                printf("%3d %11.8f %11.8f %11.8f\n", 
                        c1, xi[c1][j], d[c1][j], r[c1] );
               }
           }
                                                                            /*
       +---------------------------------------------------------------------+
       | 3. Adjust the size of each sub-region                               |
       +---------------------------------------------------------------------+
                                                                            */
           double dr = 0.0;
           double xn = 0.0;
           int    k  =  -1;
           x0        = 0.0;
           rc        = rc/nd;

           for( i = 0; i < nd; i++ )
              {
                while( rc > dr )
                  {
                    k++;
                    dr   = dr + r[k];
                    x0   = xn;
                    xn   = xi[k][j];
                   }
                dr     = dr - rc;
                xin[i] = xn - ( xn - x0 )*dr/r[k];
               }
           for( i = 0; i < ndm; i++ )
              {
                xi[i][j] = xin[i];
               }
           xi[ndm][j] = 1.0;
          }
      }
}
                                                                             /*
******************************************************************************
*                  ==============================                            */
                    void BasesSpring :: listG()                             /*
*                  ==============================                            *
******************************************************************************
* ( Function )                                                               *
*  To print the grid information on printer.                                 *
* ( Input )                                                                  *
*   d[][]   : histograms of square values of integrand function              *
*   xi[][]  : the upper value of each sub-region. xi[nd][i] = 1.0            *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
   int line, odd;

   odd = (int) nd % 2;
   line = (int) nd/2;

   for( int j = 0; j < ndim; j++ )
      {
        printf("*** Grid information for %2d-th varable ***\n", j );
        printf(" no upper-limit     density  no upper-limit     density\n");
        for( int i = 0; i < line; i++ )
           {
             int c1 = 2*i;
             int c2 = c1 + 1;

             printf("%3d %11.8f %11.8f %3d %11.8f %11.8f\n", 
                     c1, xi[c1][j], d[c1][j], c2, xi[c2][j], d[c2][j]);
            }
        if( odd )
          {
             int c1 = 2*line;

             printf("%3d %11.8f %11.8f\n", c1, xi[c1][j], d[c1][j] );
           }
       }
}
                                                                             /*
******************************************************************************
*         ===================================================                */
          void BasesSpring :: param( int sample, double tune,
                                     int itr1,   double ac1, 
                                     int itr2,   double ac2 )               /*
*         ===================================================                *
******************************************************************************
* ( Function )                                                               *
*   To set the bases parameters.                                             *
* ( Output = Input )                                                         *
*   ncall  = sample  : the number of sampling points per iteration ( given ) *
*   itmx1  = itr1    : maximum number of iterations for grid opt. step       *
*   acc1   = ac1     : required accuracy for grid opt. step                  *
*   itmx2  = itr2    : maximum number of iterations for integration step     *
*   acc2   = ac2     : required accuracy for integration step                *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
  ncall = sample;
  if( tune > 0.0 ) alpha = tune;
  if( tune > 2.0 ) alpha = 2.0;
  ngiven= ncall;
  itmx1 = itr1;
  itmx2 = itr2;
  acc1  = ac1;
  acc2  = ac2;
}

                                                                             /*
******************************************************************************
*    =================================================================       */
     void BasesSpring :: defineVariable( int dim, int wild,
                                double x_l[], double x_u[], int jg[] )      /*
*    ==================================================================      *
******************************************************************************
* ( Function )                                                               *
*   To set the lower and upper limits of integral variables.                 *
* ( Output = Input )                                                         *
*   xl[]   = x_l[]   : the lower limit values                                *
*   xu[]   = x_u[]   : the upper limit values                                *
*   ig[]   = jg[]    : flag of grid optimization                             *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
  ndim  = dim;
  nwild = wild;

  for( int i = 0; i < ndim; i++ )
     {
       xl[i]     = x_l[i];
       xu[i]     = x_u[i];
       ig[i]     = jg[i];
      }
}

                                                                             /*
******************************************************************************
*            ================================================                */
                    void BasesSpring :: Bases( )                            /*
*           ==================================================               *
******************************************************************************
* ( Function )                                                               *
*   To make numerical integration and to make the histogram of generating    *
*   function by which an event generation is carried out.                    *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
                                                                            /*
 ============================================================================
 I.   Initialization
 ============================================================================
  +--------------------------------------------------------------------------+
  | 1. set the integral variables                                            |
  +--------------------------------------------------------------------------+
                                                                            */
  if( var[0] ) setVariable( );
                                                                            /*
  +--------------------------------------------------------------------------+
  | 2. set the size of sub-regions uniform                                   |
  +--------------------------------------------------------------------------+
                                                                            */
    setGunif();
                                                                            /*
  +--------------------------------------------------------------------------+
  | 2. set the seed of randum number generator                               |
  +--------------------------------------------------------------------------+
                                                                            */
    //    seed = -12345;
    //    seed = -1698174794;
    //    ran1( seed );
                                                                            /*
 ============================================================================
 II.  The grid optimization step
 ============================================================================
                                                                            */
    integ( 0 );

    EndIntegration( 0 );
                                                                            /*
 ============================================================================
 III. The integration step
 ============================================================================
                                                                            */
    for( int i = 0; i < nsp; i++ )
       {
         dxd[i] = 0.0;
         pxd[i] = 0.0;
        }

    integ( 1 );

    EndIntegration( 1 );

}


void BasesSpring::EndIntegration( int step )
{
  return;
}

                                                                             /*
******************************************************************************
*        ==========================================================          */
                void BasesSpring :: integ( int step)                        /*
*       ============================================================         *
******************************************************************************
* ( Function )                                                               *
*   To make numerical integration and to make the histogram of generating    *
*   function by which an event generation is carried out.                    *
*   coded by S.kawabata June '99 at KEK                                      *
*****************************************************************************/
{
    double x[MXDIM], fxg;
    float c_time, start_time, end_time;
                                                                            /*
 ============================================================================
 I.   Initialization
 ============================================================================
                                                                            */
    jacob = 1.0;
    for( int i = 0; i < ndim; i++ ) jacob = jacob*dx[i];

    double calls  = (double) ( nsp*npg);
           xnd    = (double) nd;
           dxg    = 1.0/(double) ng;
    double dv2g   = pow( dxg, 2.0*(double) nwild)/npg/npg/(npg-1);
           dxg    = dxg*xnd;


    double fb, f2b;
    double ti, tsi;

    int    it1   = 1;
    int    itmx  = itmx1;
    if( step > 0 ) itmx = itmx2;

    int end_flag = 0;
    double er_percnt;

                                                                            /*
 ============================================================================
 II. Main integration loop
 ============================================================================
                                                                            */
  printB( step );

  si         = 0.0;
  swgt       = 0.0;
  scalls     = 0.0;
  start_time = uxtime();

  for( it = it1; it <= itmx; it++ ) 
     {                                                                      /*
     +-----------------------------------------------------------------------+
     | 1. Intialization for the it-th iteration of integration               |
     +-----------------------------------------------------------------------+
                                                                            */
       ti     = 0.0;
       tsi    = 0.0;
       int    good   =   0;
       int    negtiv =   0;

       scalls = scalls + calls;

       for( int j = 0; j < ndim; j++ ) 
       for( int i = 0; i <   nd; i++ ) d[i][j] = 0.0;
                                                                            /*
     +-----------------------------------------------------------------------+
     | 2. Loop over the hypercubes                                           |
     +-----------------------------------------------------------------------+
                                                                            */
/*****
       printf("nsp = %d\n", nsp );
****/

       for( int ncb = 0; ncb < nsp; ncb++ )
          {                                                                 /*
          +------------------------------------------------------------------+
          | 2.1. Initialization
          +------------------------------------------------------------------+
                                                                            */
            fb   = 0.0;
            f2b  = 0.0;

            if( nwild > 1 )
                for( int i = 0; i < nwild-1; i++ )
                   {
                     int num   = (int) ( ncb % ma[i+1] );
                         kg[i] = num/(int) ma[i] + 1;
                    }
            kg[nwild-1] = ( ncb/(int) ma[nwild-1]) + 1;

/***
           for( int i = 0; i < nwild; i++ ) printf("kg[%2d] =%4d\n", i, kg[i]);
           printf("npg =%4d,  dxg = %8.4e\n", npg, dxg );
****/
                                                                            /*
          +------------------------------------------------------------------+
          | 2.2. Loop over the sampling points in the hypercube   
          +------------------------------------------------------------------+
                                                                            */
            for( int nsamp = 0; nsamp < npg; nsamp++ )
               {                                                            /*
               +-------------------------------------------------------------+
               | 2.2.1. Determine the integral variables by random number    |
               +-------------------------------------------------------------+
                                                                            */
                 double xn, x0, rc;
                 wgt = jacob;


		 int j;
                 for( j = 0; j < ndim; j++ )
                    {
		      //                      if( j <= nwild-1 ) xn = ( kg[j] - ran1(seed))*dxg;
		      //                      else               xn = (double)( nd )*ran1(seed);
                      if( j <= nwild-1 ) xn = ( kg[j] - ran1.Rndm())*dxg;
                      else               xn = (double)( nd )*ran1.Rndm();

                      int iaj = (int) xn;
                      ia[j]   = iaj;

                      if( iaj == 0 )
                        {
                          x0  = xi[iaj][j];  // <== This was error
                          rc  = xn*x0;
                         }
                      else
                        {
                          x0  = xi[iaj][j] - xi[iaj-1][j];
                          rc  = xi[iaj-1][j] + ( xn - (double)iaj)*x0;
                         }
                      x[j] = xl[j] + rc*dx[j];
                      wgt  = wgt*x0*xnd;
                     }
/****************/

                                                                            /*
               +-------------------------------------------------------------+
               | 2.2.2. call the integrand function                          |
               +-------------------------------------------------------------+
                                                                            */
                 if( var[0] )
                   {
                     for( int m = 0; m < ndim; m++ ) var[m]->Variable( x[m] );
                     fxg = func(  );
                    }
                  else 
                     fxg = func( x );

//                 cout << " fxg = " << fxg << " wgt = " << wgt << "\n";

                 double f2;

                 fxg = fxg*wgt;
                                                                            /*
               +-------------------------------------------------------------+
               | 2.2.3. Check the value of integrand function                |
               +-------------------------------------------------------------+
                                                                            */
                 if( fxg != 0.0 )
                   { 
                     good++;
                     if( fxg > 0.0 )
                       {
                         if( step == 1 )
                           {
                             dxd[ncb] += fxg;
                             if( fxg > pxd[ncb] ) pxd[ncb] = fxg;
                            }
                        }
                     else
                       {
                         flag_neg = 1;
                         negtiv++;
                        }
                    }
                 f2  = fxg*fxg;

                 fb  = fb  + fxg;
                 f2b = f2b + f2;

                 for( j = 0; j < ndim; j++ ) d[ia[j]][j] = d[ia[j]][j] + f2;
                }
                                                                            /*
          +------------------------------------------------------------------+
          | 2.3. Calculate the estimate andvariance in the hypercube
          +------------------------------------------------------------------+
                                                                            */
            f2b  = sqrt( f2b*npg );
            ti   = ti  + fb;
            tsi  = tsi + (f2b + fb)*(f2b - fb );

           }
                                                                            /*
     +-----------------------------------------------------------------------+
     | 3. Calculate the estimate and variance for the it-th iteration        |
     +-----------------------------------------------------------------------+
                                                                            */
       ti   = ti/calls;
       tsi  = tsi*dv2g;
                                                                            /*
     +-----------------------------------------------------------------------+
     | 4. Calculate the estimate and variance upto the it-th iteration       |
     +-----------------------------------------------------------------------+
                                                                            */
       wgt   = 1.0/tsi;
       si    = si  +  ti*wgt;
       swgt  = swgt + wgt;
       estim = si/swgt;
       error = sqrt( 1.0/swgt );
       float abestim = estim;
       if( abestim < 0.0 ) abestim = -abestim;
       er_percnt = error/abestim *100.0;
                                                                            /*
     +-----------------------------------------------------------------------+
     | 5. Adjust the grid intervals for the grid optimization step           |
     +-----------------------------------------------------------------------+
                                                                            */
       if( step == 0 ) {
           if( it < itmx ) adjustG( 0 );

           if( er_percnt < acc1 ) end_flag = 1;

        }
       else {
           if( er_percnt < acc2 ) end_flag = 1;
        }

                                                                            /*
     +-----------------------------------------------------------------------+
     | 6. Save temporary histogram buffers into the permanent ones           |
     +-----------------------------------------------------------------------+
                                                                            */
       bh_save();
                                                                            /*
     +-----------------------------------------------------------------------+
     | 6. Print and store the result of current iteration                    |
     +-----------------------------------------------------------------------+
                                                                            */

       int itx = it % ITM;
       itr  [itx][step] = it;
       ief  [itx][step] = (int)( (float) good/calls *100.0 );
       wrong[itx][step] = (float) negtiv/calls * 100.0;
       testm[itx][step] = (float) ti;
       terro[itx][step] = (float) sqrt( tsi );
       aestm[itx][step] = (float) estim;
       aerro[itx][step] = (float) error;

       end_time = uxtime();
       c_time   = end_time - start_time;
       time [itx][step] = c_time;

       listB( itx, step );

       if( end_flag == 1 ) break;

     }

  printB( -1 );
}
                                                                             /*
******************************************************************************
*            ================================================                */
              void BasesSpring :: bs_write( char* FileName )                /*
*           ==================================================               *
******************************************************************************
* ( Function )                                                               *
*   To store the bases data into the file FileName.                          *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
  ofstream out( FileName );
  if( !out )
    {
      cout << "Cannot open output file(" << FileName << ").\n";
      exit( 1 );
     }

  out.write((char *) &ndim,   sizeof( int ));
  out.write((char *) &nwild,  sizeof( int ));
  out.write((char *)  ig,     sizeof( int ) * ndim );
  out.write((char *) &ncall,  sizeof( int ));
  out.write((char *) &ngiven, sizeof( int ));

  out.write((char *)  xl,     sizeof( double ) * ndim );
  out.write((char *)  xu,     sizeof( double ) * ndim );

  out.write((char *)  xi,     sizeof( double ) * MXND * MXDIM );
  out.write((char *)  dx,     sizeof( double ) * MXDIM );

  out.write((char *) &nd,     sizeof( long ));
  out.write((char *) &ng,     sizeof( long ));
  out.write((char *) &nsp,    sizeof( long ));
  out.write((char *) &npg,    sizeof( long ));
  out.write((char *)  ma,     sizeof( long ) * ndim );

  out.write((char *)  dxd,    sizeof( double ) * nsp );
  out.write((char *)  pxd,    sizeof( double ) * nsp );

  //  out.write((char *) &seed,   sizeof( long ));

  long seed=ran1.GetSeed();
  long iy;
  long iv[NTAB];
  ran1.GetSeed2(iy,iv);
  out.write((char*)&seed, sizeof(long));
  out.write((char*)&iy,   sizeof(long));
  out.write((char*)&iv,   sizeof(long)*NTAB);


  out.write((char *) &it,     sizeof( int ));
  out.write((char *) &itmx1,  sizeof( int ));
  out.write((char *) &itmx2,  sizeof( int ));

  out.write((char *) &acc1,   sizeof( double ) );
  out.write((char *) &acc2,   sizeof( double ) );

  out.write((char *) &si,     sizeof( double ) );
  out.write((char *) &swgt,   sizeof( double ) );
  out.write((char *) &scalls, sizeof( double ) );
  out.write((char *) &xnd,    sizeof( double ) );
  out.write((char *) &jacob,  sizeof( double ) );
  out.write((char *) &dxg,    sizeof( double ) );

  out.write((char *) &estim, sizeof( double ) );
  out.write((char *) &error, sizeof( double ) );

  his.bs_write( out );

  out.close( );
}
                                                                             /*
******************************************************************************
*            ================================================                */
              void BasesSpring :: bs_read( char *FileName )                 /*
*           ==================================================               *
******************************************************************************
* ( Function )                                                               *
*   To restore the bases data from the file FileName.                        *
*   coded by S.kawabata July '99 at KEK                                      *
******************************************************************************/
{
  ifstream in( FileName );
  if( !in )
    {
      cout << "Cannot open output file(" << FileName << ").\n";
      exit( 1 );
     }

  in.read((char *) &ndim, sizeof( int ));
//                                    cout << "in  ndim = " << ndim << "\n";
  in.read((char *) &nwild, sizeof( int ));
//                                    cout << "in  nwild = " << nwild << "\n";
  in.read((char *)  ig,    sizeof( int )*ndim );
  in.read((char *) &ncall, sizeof( int ));
//                                    cout << "in  ncall = " << ncall << "\n";
  in.read((char *) &ngiven, sizeof( int ));
//                                    cout << "in  ngiven = " << ngiven << "\n";

  in.read((char *)  xl,     sizeof( double ) * ndim );
  in.read((char *)  xu,     sizeof( double ) * ndim );

  in.read((char *)  xi,     sizeof( double ) * MXND * MXDIM );
  in.read((char *)  dx,     sizeof( double ) * MXDIM );

  in.read((char *) &nd,     sizeof( long ));
  in.read((char *) &ng,     sizeof( long ));
  in.read((char *) &nsp,    sizeof( long ));
  in.read((char *) &npg,    sizeof( long ));
  in.read((char *)  ma,     sizeof( long ) * ndim );

  in.read((char *)  dxd,    sizeof( double ) * nsp );
  in.read((char *)  pxd,    sizeof( double ) * nsp );

  //  in.read((char *) &seed,   sizeof( long ));

  long seed;
  long iy;
  long iv[NTAB];
  in.read((char*)&seed, sizeof(long));
  in.read((char*)&iy,   sizeof(long));
  in.read((char*)&iv,   sizeof(long)*NTAB);
  ran1.SetSeed(seed);
  ran1.SetSeed2(iy,iv);


  in.read((char *) &it,     sizeof( int ));
  in.read((char *) &itmx1,  sizeof( int ));
  in.read((char *) &itmx2,  sizeof( int ));

  in.read((char *) &acc1,   sizeof( double ) );
  in.read((char *) &acc2,   sizeof( double ) );

  in.read((char *) &si,     sizeof( double ) );
  in.read((char *) &swgt,   sizeof( double ) );
  in.read((char *) &scalls, sizeof( double ) );
  in.read((char *) &xnd,    sizeof( double ) );
  in.read((char *) &jacob,  sizeof( double ) );
  in.read((char *) &dxg,    sizeof( double ) );

  in.read((char *) &estim,  sizeof( double ) );
  in.read((char *) &error,  sizeof( double ) );

  his.bs_read( in );

  in.close( );
}
                                                                             /*
******************************************************************************
*            ================================================                */
               void BasesSpring :: Spring( int mxtry )                      /*
*           ==================================================               *
******************************************************************************
* ( Function )                                                               *
*   To generate a set of random numbers with the distribution given by       *
*   the given integrand function "func".                                     *
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
                                                                            /*
 ============================================================================
  Initialization of spring
 ============================================================================
                                                                             */
   if( flag_bases > 0 )
     {
       if( flag_neg > 0 )
         {
           printf("************ Fatal Error found by spring *************\n");
           printf("*                                                    *\n");
           printf("* The current integrand function has given           *\n");
           printf("*      the negative value during the integration.    *\n");
           printf("*                                                    *\n");
           printf("* Since spring cannot generate a correct set of      *\n");
           printf("*   events, program is forced to stop.               *\n");
           printf("*                                                    *\n");
           printf("******************************************************\n");
                                                                            /*
             ============                                                    */
               exit( 0 );                                                   /*
             ============                                                    */
          }

       if( var[0] ) setVariable( );

       sh_reset( );

       maxtry     = mxtry;
       flag_bases = 0;

       if( maxtry < 10 ) maxtry = 50;

       hist.init( 1, 1.0, (double) (maxtry+1), maxtry, 
           "Number of trials for generating an Event." );

       no_event = 0;
       no_try   = 0;
       no_miss  = 0;
                                                                            /*
     ------------------------------------------------------------------------
       make the cumulative distribution of probability distribution
     ------------------------------------------------------------------------
                                                                            */
       dxmax    = 0.0;
       for( int i = 0; i < nsp; i++ )
          {
            dxmax += dxd[i];
            dxd[i] = dxmax;
           }
      }
                                                                            /*
 ============================================================================
    Sample an Hypercube according to the distribution made by bases
 ============================================================================
                                                                             */
   int accept = 0;

   while( accept == 0 )
    {
      //      double rx = ran1( seed )*dxmax;
      double rx = ran1.Rndm()*dxmax;

      int pmin = 0, pmax = (int) nsp-1, ic;

      while( pmax - pmin > 2 )
       {
         ic = ( pmin + pmax )/2;
         if( rx < dxd[ic] ) pmax = ic;
         else               pmin = ic;
        }

      ic = pmin - 1;
      if( ic < 0 ) ic = 0;

      while( rx > dxd[ic] ) ic++;

      int ix = ic;
      kg[nwild-1] = ix/(int) ma[nwild-1] + 1;

      if( nwild > 1 )
          for( int i = 0; i < nwild - 1; i++ )
             {
               int num  = ix % (int) ma[i+1];
               kg[i]    = num/(int) ma[i] + 1;
              }
                                                                            /*
 ============================================================================
    Sample a point in the hypercube and test it by the probability
 ============================================================================
                                                                             */
      double fmax = pxd[ic];
      double xn, x0, rc;
      double x[MXDIM];
      int    ntry = 0;

      while( ntry < maxtry && accept == 0 )
      {
        ntry++;
        wgt = jacob;
        for( int j = 0; j < ndim; j++ )
           {
	     //             if( j <= nwild-1 ) xn = ( kg[j] - ran1(seed))*dxg;
	     //             else               xn = (double)( nd )*ran1(seed);
             if( j <= nwild-1 ) xn = ( kg[j] - ran1.Rndm())*dxg;
             else               xn = (double)( nd )*ran1.Rndm();

             int iaj     = (int) xn;
                 ia[j]   = iaj;

             if( iaj == 0 )
               {
                 x0  = xi[iaj][j];
                 rc  = xn*x0;
                }
             else
               {
                 x0  = xi[iaj][j] - xi[iaj-1][j];
                 rc  = xi[iaj-1][j] + ( xn - (double)iaj)*x0;
                }
             x[j] = xl[j] + rc*dx[j];
             wgt  = wgt*x0*xnd;
            }

        double ff;
        if( var[0] )
          {
            for( int m = 0; m < ndim; m++ ) var[m]->Variable( x[m] );
            ff = func(  );
           }
         else 
            ff = func( x );

        double fx    = ff*wgt;
        double frac  = fx/fmax;

	//        if( fx > 0.0 &&  ran1( seed ) <= frac ) 
        if( fx > 0.0 &&  ran1.Rndm() <= frac ) 
          {
            no_event++;
            accept = 1;
           }
       }

      no_try += ntry + 1;

      if( accept == 1 ) sh_update( );
      else              no_miss++;

      hist.fill( 1, (double) ntry );

     }
}
                                                                             /*
******************************************************************************
*                  =================================                         */
                     void BasesSpring :: sp_info( )                         /*
*                  =================================                         *
******************************************************************************
*   coded by S.kawabata June '99 at KEK                                      *
******************************************************************************/
{
   hist.plot();
}
