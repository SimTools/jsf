#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "bs_hist.h"
#define SCALE 51
                                                                            /*
******************************************************************************
*         ==============================================================     */
      void  bs_hist :: init( int id, 
                               double Xmin, double Xmax, int Nbin,
                                       char title[] )                       /*
*         ==============================================================     *
******************************************************************************
* ( Purpose )                                                                *
*   To initialize the histogram with identity number id.                     *
******************************************************************************/
{
  ID       = id;

  x_min    = (float) Xmin;
  x_max    = (float) Xmax;
  if( Nbin > BINSIZE-2 ) 
    {
      printf("Histogram Warning:");
      printf(" Bin_size(%d) is too large for Hist ID(%d).\n", Nbin, ID );
      printf("                   Bin_size was reduced to %d.\n", BINSIZE );
      Nbin = BINSIZE - 2;
    }

  no_bin   = Nbin;
  dev       = (float) ( Xmax - Xmin )/Nbin;

  int j;
  for( j =  0; j <  BINSIZE; j++ )
     {
       freq.buff[j]  = 0.0;
      }

  int leng = strlen( title );

  for( j = 0; j < leng; j++ ) name[j] = title[j];
  name[leng] = '\0';
/*
  printf("Histogram ID(%d) is initialized.\n", ID );
  printf(" x_min(%8.4e) x_max(%8.4e) no_bin(%d), dev(%8.4e)\n",
           x_min, x_max, no_bin, dev );
*/
}
                                                                            /*
******************************************************************************
*             =============================================                  */
            void  bs_hist :: fill( int id, double Xn )                   /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To fill the histogram of identity number id                              *
******************************************************************************/
{

  if( ID != id )
    {
      printf("Histogram error in xh_fill: ID(%d) is illegal !!\n", id );
      printf(" ID = %d\n", ID ) ;
      exit(0);
     }
  float x  = (float) Xn;
  int   ix    = -1;


  if( x < x_min ) ix = 0;
  else
  if( x > x_max ) ix = no_bin + 1;
  else {
    ix  = (int) (( x - x_min )/dev + 1.0 );
    if( ix > no_bin ) ix = no_bin;
   }

  freq.ibuf[ix]++;

}
                                                                            /*
******************************************************************************
*             =============================================                  */
                     void  bs_hist :: plot()                                /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To add the content of histogram buffer of current iteration into         *
*          the total histogram buffers.                                      *
******************************************************************************/
{
  char cn = 0xc;

  float val[BINSIZE], vlog[BINSIZE], ymax = 49.5;
  char  disp[SCALE], scale[SCALE], line[SCALE];
  char  blank = ' ', bar = '|', star = '*', oo = 'o';

  printf("%c\n", cn );
  printf("\n\n");
  printf(" Histogram (ID = %d) for %s\n", ID, name );

  int   mxbin = no_bin + 1;
  float vmax = (float) freq.ibuf[0];
  float vmin = vmax;
  float vlmin, vlmax;

  float vlg;
  int   flg = 0;
  int i;
  for( i = 0; i < mxbin+1; i++ )
     {
       float vls = (float) freq.ibuf[i];
       if( vls > vmax ) vmax = vls;
       if( vls < vmin ) vmin = vls;
       val[i] = vls;
       if( vls > 0.0 ) 
         {
           vlg = (float) log10((double) vls );
           if( flg == 0 )
             {
               vlmax = vlg;
               vlmin = vlg;
               flg   = 1;
              }
           else
             {
               if( vlg > vlmax ) vlmax = vlg;
               if( vlg < vlmin ) vlmin = vlg;
              }
          }
       else
          vlg = 0.0;
       vlog[i] = vlg;
      }

  if( flg == 0 )
    {
      printf("    This histogram (ID=%d) has no entry.\n", ID );
      return;
     }


  int flag = 0;
  float vxmin = vmin;
  float vxmax = vmax;
  float vstep = 0.0;

  xh_range( flag, vxmin, vxmax, vmin, vmax, vstep );

  float unit = ( vmax - vmin )/ymax;

  vxmin = vlmin;
  vxmax = vlmax;
  float vlstep = 0.0;

  xh_range( flag, vxmin, vxmax, vlmin, vlmax, vlstep );

  float unitl = ( vlmax - vlmin )/ymax;

  xh_scale( flag, vmin, vmax, vstep, unit, SCALE, scale, line );

  printf("                             ");
  printf("Linear Scale indicated by *\n");
  printf("     x    Lg(dN/dx)  dN/dx %s\n", scale );
  printf(" +-------+----------+-------+%s\n", line );

  float vx =  x_max-dev ;
  if( vx < 0.0 ) vx =  -x_max+dev ;
  float xm = x_min;
  if( xm < 0.0 ) xm = -x_min;
  if( xm > vx ) vx = xm;

  float f2, ordr;
  int       jord;

  forder( vx, f2, ordr, jord );

  int size1 = SCALE - 1;
  int size2 = size1 - 1;

  disp[size1] = '\0';
  for( i = 0; i < mxbin+1; i++ )
     {
       disp[size2] = bar;
       float vx    = val[i];
       float vl    = vlog[i] - vlmin;

       int numb = -1;
       int numbl = -1;
       if( vx > 0.0 )
         {
           numb  = (int) (vx/unit);
           numbl = (int) (vl/unitl);
          }

       if( numb  > size2 ) numb  = size2;
       if( numbl > size2 ) numbl = size2;

       disp[size2] = bar;
       if( numb >= 0 )
         {
	   int j;
           for( j = 0; j < numb; j++ ) disp[j] = star;
           for( j = numb; j < size2; j++ )    disp[j] = blank;
           disp[numbl] = oo;
          }
       else
           for( int j = 0; j < size2; j++ )    disp[j] = blank;

       int nx = (int) vx;
       if( i == 0 || i == mxbin )
         {
           printf(" |  E%3.2d |%10.3E|%7d|%50s\n",
                          jord, vl, nx, disp );
          }
       else
         {
           xm = ( x_min + dev*(i-1))/ordr;

           printf(" |%6.3f |%10.3E|%7d|%.50s\n",
                             xm, vl, nx, disp );
          }
      }

  flag = 1;

  xh_scale( flag, vlmin, vlmax, vlstep, unitl, SCALE, scale, line );

  printf(" +-------+----------+-------+%s\n", line );
  printf("     x    Lg(dN/dx)  dN/dx %s\n", scale );
  printf("                             ");
  printf("Log Scale indicated by o\n");
  
}
                                                                             /*
******************************************************************************
*                 =============================                              */
      void bs_hist :: forder( float v, float &fv, float &ordr, int &iordr ) /*
*                ===============================                             *
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
       void bs_hist::xh_range( int flg, float vmin, float vmax,
                           float& vtmin, float& vtmax, float& step )        /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To determin the optimized maximum and minimum values of histogram.       *
******************************************************************************/
{
 static float window[25] = {
       1.00,  1.10,  1.20,  1.30,  1.40,  1.50,  1.60,  1.80,  2.00,  2.20,
       2.50,  2.70,  3.00,  3.30,  3.60,  4.00,  4.50,  5.00,  5.50,  6.00,
       6.50,  7.00,  8.00,  9.00, 10.00 };
 static float step1[25]  = {
      0.250, 0.275, 0.300, 0.325, 0.350, 0.375, 0.400, 0.450, 0.500, 0.550,
      0.625, 0.675, 0.750, 0.825, 0.900, 1.000, 1.125, 1.250, 1.375, 1.500,
      1.625, 1.750, 2.000, 2.250, 2.500 };
 static float step2[25]  = {
       1.00,  1.00,  1.00,  1.00,  1.00,  1.00,  1.00,  1.00,  1.00,  1.00,
       1.00,  1.00,  1.00,  1.00,  1.00,  1.00,  1.00,  1.00,  1.00,  2.00,
       2.00,  2.00,  2.00,  2.00,  2.00 };

 int   flag = flg;
 float xmax = vmax;
 float xmin = vmin;
 float xmin1, stepx;
 xmin1=0.0; 

 if( flag != 1 && vmin < 0.0 ){
     if( vmax < 0.0 ){
         flag = 2;
         xmax = -vmin;
         if( -vmax > xmax ) xmax = -vmax;
         xmin = 0.0;
       }
     else {
         float avmin = -vmin;
         xmin        = 0.0;
         if( vmax >= avmin ){
             flag = 3;
             xmax = vmax;
             xmin1= avmin;
           }
         else {
             flag = 4;
             xmax = avmin;
             xmin1= vmax;
           }
       }
   }

 float dscale = xmax - xmin;
 float  dsf2, dsord;
 int    iord;

  forder( dscale, dsf2, dsord, iord );

 int iopt;
 for( iopt = 1; iopt < 25; iopt++ )
      if( dsf2 >= window[iopt-1] && dsf2 <= window[iopt] ) break;

 if( iopt > 24 ) iopt = 24;

 xmax = window[iopt]*dsord + xmin;

 if( dsord >= 10.0 || flg != 1 ) {
     stepx = step1[iopt];
     step  = stepx*dsord;
   }
 else {
     stepx = step2[iopt];
     step  = stepx;
   }

 if( flag <= 1 ) {
     vtmax = xmax;
     vtmin = xmin;
   }
 else
 if( flag == 2 ){
     vtmax = xmin;
     vtmin = -xmax;
   }
 else {
     float xplus = 0.0;
     int j;
     for( j = 0; j < 10; j++ ){
          xplus = xplus + step;
          if( xplus > xmin1 ) break;
        }
     xmin = xplus;
     if( (int)((window[j]+0.1)/stepx) + j > 7 ) step = 2.0*step;

     if( flag == 3 ) {
         vtmax = xmax;
         vtmin = -xmin;
       }
     else {
         vtmax = xmin;
         vtmin = -xmax;
       }
   }
}
                                                                            /*
******************************************************************************
*             =============================================                  */
  void bs_hist:: xh_scale( int flag, float vmin, float vmax, 
             float vstep, float unit, int size, char scale[], char line[] ) /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To make the scale of the histogram                                       *
******************************************************************************/
{
 static char plus = '+', minus = '-', blank = ' ';
 float sc;
 int nscl = 0;
 char number[10];
 int size1 = size - 1;
 int sizep = size - 17;
 int sizeq = size - 9;

 int i;
 for( i = 0; i < size1; i++ ) scale[i] = blank;

 if(flag == 1 ) sc = (float) pow( 10.0, (double) vmin);
 else           sc = vmin;

 sprintf( number,"%8.1e ", sc );
 for( int k = 0; k < 8; k++ ) scale[k] = number[k];
 int i2 = 8;

 float stv  = vstep + vmin;
 float stv1 = stv;
 float val1 = vmin;
 line[49]   = plus;
 for( i = 0; i < size1; i++ ){
      float val2 = val1 + unit;

      if( stv >= val1 && stv <  val2 ){
          line[i] = plus;
          nscl++;
          if( flag == 1 ) sc = (float) pow( 10.0, (double) stv);
          else {
              if( stv1 == 0.0 ) sc = stv;
              else
              if( stv/stv1 < 1.0e-2 && stv/stv1 > -1.0e-2 ) sc = 0.0;
              else                                   sc = stv;
              stv1 = stv;
            }

          if(  i > i2 && i < sizep ) 
            {
              sprintf( number,"%8.1e", sc );
              for( int k = 0; k < 8; k++ ) scale[k+i] = number[k];
              i2 = i + 8;
            }
          else
          if( i == size1-1 && i2 < sizeq )
            {
              sprintf( number,"%8.1e", sc );
              i2 = sizeq;
              for( int k = 0; k < 8; k++ ) scale[k+i2] = number[k];
             }
          stv = stv + vstep;
         }
      else line[i] = minus;

      val1 = val2;
    }
 if( nscl == 0 ){
     if( flag == 1 ) sc = (float) pow( 10.0, (double) vmax );
     else            sc = vmax;

     sprintf( number,"%8.1e", sc );
     for( int k = 0; k < 8; k++ ) scale[k+sizep] = number[k];
   }
 scale[size1] = '\0';
 line[size1]  = '\0';

}
