#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bases_xhist.h"
                                                                            /*
******************************************************************************
*             =============================================                  */
                     bases_xhist :: ~bases_xhist( )                         /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*    To deallocate the histogram buffers from free space                     *
* ( Author )                                                                 *
*  S.Kawabata,  july `99 at KEK                                              *
******************************************************************************/
{
  deallocate();
}
                                                                            /*
******************************************************************************
*             =============================================                  */
                  void bases_xhist :: deallocate( )                         /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*    To deallocate the histogram buffers from free space                     *
* ( Author )                                                                 *
*  S.Kawabata,  july `99 at KEK                                              *
******************************************************************************/
{
  printf("Deallocator for histogram( ID = %d ) was invoked.\n", ID );

  delete [ ] nsamp;
  delete [ ] freq;
  delete [ ] freq2;
  delete [ ] totfr;
  delete [ ] totf2;
}
                                                                            /*
******************************************************************************
*             =============================================                  */
              void  bases_xhist :: bs_write( ofstream& out )                /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*    To store the contents of histogram to the file FileName.                *
* ( Author )                                                                 *
*  S.Kawabata,  july `99 at KEK                                              *
******************************************************************************/
{

  if( !out )
    {
      cout << "Output file is not defined.\n";
      exit( 1 );
     }

  out.write((char *) &ID,      sizeof( int ));
  out.write((char *) &no_bin,  sizeof( int ));
  out.write((char *) &if_bases,sizeof( int ));

  out.write((char *) &x_min,  sizeof( float ));
  out.write((char *) &x_max,  sizeof( float ));
  out.write((char *) &dev,    sizeof( float ));

  out.write(      name,            64 );

  out.write((char *)  nsamp,  sizeof( int )   * (no_bin+2));
  out.write((char *)  freq,   sizeof( float ) * (no_bin+2));
  out.write((char *)  freq2,  sizeof( float ) * (no_bin+2));
  out.write((char *)  totfr,  sizeof( float ) * (no_bin+2));
  out.write((char *)  totf2,  sizeof( float ) * (no_bin+2));

}
                                                                            /*
******************************************************************************
*             =============================================                  */
               void bases_xhist :: bs_read( ifstream& in )                  /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*    To restore the contents of histogram from the file FileName.            *
* ( Author )                                                                 *
*  S.Kawabata,  july `99 at KEK                                              *
******************************************************************************/
{

  if( !in )
    {
      cout << "Input file is not defined.\n";
      exit( 1 );
     }

  int id, Nbin;
  in.read((char *) &id,       sizeof( int ));
  in.read((char *) &Nbin,     sizeof( int ));
  in.read((char *) &if_bases, sizeof( int ));

  in.read((char *) &x_min,  sizeof( float ));
  in.read((char *) &x_max,  sizeof( float ));
  in.read((char *) &dev,    sizeof( float ));

  in.read(      name,            64 );

  if( nsamp &&  Nbin != no_bin )
    {
      no_bin = Nbin;
      ID     = id;

      delete [ ] nsamp;
      delete [ ] freq;
      delete [ ] freq2;
      delete [ ] totfr;
      delete [ ] totf2;
     }

  if( !nsamp )
    {
      nsamp = new int   [ no_bin+2 ];
      freq  = new float [ no_bin+2 ];
      freq2 = new float [ no_bin+2 ];
      totfr = new float [ no_bin+2 ];
      totf2 = new float [ no_bin+2 ];
      if( !nsamp || !freq || !freq2 || !totfr || !totf2 )
        {
          cout << "Memory allocation error in Histogram\n";
          exit( 1 );
         }
     }
  in.read((char *)  nsamp,  sizeof( int )   * (no_bin+2));
  in.read((char *)  freq,   sizeof( float ) * (no_bin+2));
  in.read((char *)  freq2,  sizeof( float ) * (no_bin+2));
  in.read((char *)  totfr,  sizeof( float ) * (no_bin+2));
  in.read((char *)  totf2,  sizeof( float ) * (no_bin+2));

}
                                                                            /*
******************************************************************************
*         ==============================================================     */
        void bases_xhist :: xh_init( int id, 
                    double Xmin, double Xmax, int Nbin, char title[] )      /* 
*         ==============================================================     *
******************************************************************************
* ( Purpose )                                                                *
*   To initialize the histogram with identity number id.                     *
******************************************************************************/
{
  if_bases = SPRING;

  ID       = id;

  x_min    = (float) Xmin;
  x_max    = (float) Xmax;

  int NSize = Nbin + 2;

  nsamp = new int   [ NSize ];
  freq  = new float [ NSize ];
  freq2 = new float [ NSize ];
  totfr = new float [ NSize ];
  totf2 = new float [ NSize ];

  if( !nsamp || !freq || !freq2 || !totfr || !totf2 )
    {
      printf("Allocation Error at Histogram \n");
      exit( 1 );
    }

  no_bin   = Nbin;
  dev       = (float) ( Xmax - Xmin )/Nbin;

  int j;
  for( j =  0; j <  NSize; j++ )
     {
       nsamp[j] = 0;
       freq [j] = 0.0;
       freq2[j] = 0.0;
       totfr[j] = 0.0;
       totf2[j] = 0.0;
      }

  int leng = strlen( title );

  for( j = 0; j < leng; j++ ) name[j] = title[j];
  name[leng] = '\0';
/*
  printf("bases_hist ID(%d) is initialized.\n", ID );
  printf(" x_min(%8.4e) x_max(%8.4e) no_bin(%d), dev(%8.4e)\n",
           x_min, x_max, no_bin, dev );
*/
}
                                                                            /*
******************************************************************************
*      ==================================================================    */
        void bases_xhist :: xh_fill( int id, double X, double fn, 
                                             double wgt, int Bases_Flag )   /*
*      ==================================================================    * 
******************************************************************************
* ( Purpose )                                                                *
*   To fill the histogram of identity number id                              *
******************************************************************************/
{
  float fxg;

  if( ID != id )
    {
      printf("Histogram error in xh_fill: ID(%d) is illegal !!\n", id );
      printf(" ID = %d\n", ID ) ;
      exit(0);
     }
  float x  = (float) X;
  int   ix    = -1;


  if( x < x_min ) ix = 0;
  else
  if( x > x_max ) ix = no_bin + 1;
  else {
    ix  = (int) (( x - x_min )/dev + 1.0 );
    if( ix > no_bin ) ix = no_bin;
   }

  if( Bases_Flag )
    {
      fxg = (float)( fn * wgt );
      nsamp[ix]++;
      freq [ix] += fxg;
      freq2[ix] += fxg*fxg;
      if_bases   = BASES;
     }
  else
      x_tag = ix;

}
                                                                            /*
******************************************************************************
*             =============================================                  */
                   void  bases_xhist :: bh_save()                           /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To add the content of histogram buffer of current iteration into         *
*          the total histogram buffers.                                      *
******************************************************************************/
{
     int n  = no_bin+2;

     for( int j = 0; j < n; j++ ) {

//          printf(" buff[%5d] = %11.6e\n", j, h.buff[j] );

         totfr[j] += freq[j];
         freq [j]   = 0.0;
         totf2[j] += freq2[j];
         freq2[j]   = 0.0;
      }
}
                                                                            /*
******************************************************************************
*             ==============================================                 */
               void  bases_xhist :: xh_plot( double scalls )                /*
*             ==============================================                 *
******************************************************************************
* ( Purpose )                                                                *
*   To add the content of histogram buffer of current iteration into         *
*          the total histogram buffers.                                      *
******************************************************************************/
{
  char cn = 0xc;

  static float ymax = 49.5;
  char disp[56], line[56], scale[56];
  char blank = ' ', star = '*', oo = 'o', bar = '|';
  float calls = (float) scalls;

  printf(" from xh_plot  calls = %10.4f\n", calls );

  float *val;
  float *verr;
  float *vlog;
  val  = new float [no_bin+2];
  verr = new float [no_bin+2];
  vlog = new float [no_bin+2];
  int i;
  for( i = 0; i< no_bin+2; i++)
     {
       val[i]  = 0.0;
       verr[i] = 0.0;
       vlog[i] = 0.0;
      }
//--------------

  printf("%c\n", cn );
  printf("\n\n");
  printf(" Histogram (ID = %d) for %s\n", ID, name );

  int   mxbin = no_bin + 1;

  float fact  = 1.0/( calls*dev );

  val[0]      = totfr[0]/calls;
  val[mxbin]  = totfr[mxbin]/calls;
  verr[0]     = 0.0;
  verr[mxbin] = 0.0;
                                                                           /*
 +-----------------------------------------------------------------+
 |     Find the minimum and maximum values in the histogram        |
 +-----------------------------------------------------------------+
                                                                           */
  float vmax  = fact*totfr[1];
  float vmin  = vmax;
  float vls;
//---------
  for( i = 1; i < mxbin; i++ )
     {
       float tx  = totfr[i];
       int   nx  = nsamp[i];

       vls = tx*fact;
       if( vmax < vls ) vmax = vls;
       if( vmin > vls ) vmin = vls;
       val[i]    = vls;
       if( nx > 1 )
         {
           float dev2 = (float) nx * totf2[i] - tx*tx;
           if( dev2 < 0.0 ) verr[i] = 0.0;
           else             verr[i] = fact*sqrt((double)( dev2/(nx - 1) ));
          }
       else
       if( nx == 1 ) verr[i] = vls;
       else          verr[i] = 0.0;
      }
//---------
                                                                           /*
 +-----------------------------------------------------------------+
 |     If both minimum and maxmum values are 0.0,                  |
 |     take the overflow and underflow bins' values into account.  |
 +-----------------------------------------------------------------+
                                                                           */
  if( vmax == 0.0 && x_min == 0.0 )
    {
      float v0 = val[0];
      float vm = val[mxbin];
      if( v0 >= 0.0 && vm >= 0.0 ) 
        {
          vmin = 0.0;
          if( v0 > vm ) vmax = v0;
          else          vmax = vm;
         }
      else
      if( v0 < 0.0 && vm < 0.0 )
        {
          vmax = 0.0;
          if( v0 < vm ) vmin = v0;
          else          vmin = vm;
         }
      else
      if( v0 > vm )
        {
          vmax = v0;
          vmin = vm;
         }
      else
        {
          vmax = vm;
          vmin = v0;
         }
    }
                                                                           /*
 +-----------------------------------------------------------------+
 |     When the minimum vale is positive, prepare the log histogram|
 +-----------------------------------------------------------------+
                                                                           */
  float vlmax, vlmin, vxmax, vxmin, unitl, vstep, vlstep;
  int flag;
  float unit;
  unitl=1.0;

  if( vmin > 0.0 )
    {
      if( vmax > 0.0 ) vlmax = log10( (double) vmax );
      else             vlmax = 2.0;

      vlmin = vlmax;
      for( i = 0; i < mxbin+1; i++ )
         {
           if( val[i] > 0.0 )
             {
               vls = log10((double) val[i]);
               if( i > 0 && i < mxbin && vls < vlmin ) vlmin = vls;
               vlog[i] = vls;
              }
           else vlog[i] = 0.0;
          }
      if( vlmin < 0.0 ) vxmin = (int) vlmin - 1.0;
      else              vxmin = (int) vlmin;
      vxmax = vlmax;

      flag  = 1;

      xh_range( flag, vxmin, vxmax, vlmin, vlmax, vlstep );

      unitl = ( vlmax - vlmin)/ymax;
     }
                                                                           /*
 +-----------------------------------------------------------------+
 |     When the maximum vale is positive, prepare the log histogram|
 +-----------------------------------------------------------------+
                                                                           */
  if( vmax > 0.0 )
    {
      if( vmin >= 0.0 )
        {
          vxmax = 1.2*vmax;
          vxmin = 0.0;
         }
      else
        {
          vxmax = 1.1*vmax;
          vxmin = 1.1*vmin;
         }
     }
  else
    {
      vxmax = 0.0;
      vxmin = 1.1*vmin;
     }

  flag = 0;

  xh_range( flag, vxmin, vxmax, vmin, vmax, vstep );

  unit = ( vmax - vmin )/ymax;
/*
           printf("vxmin(%8.2e) vxmax(%8.2e)", vxmin, vxmax );
           printf(" vmin(%8.2e) vmax(%8.2e) vstep(%8.2e)\n", 
                    vmin, vmax, vstep );
*/
                                                                           /*
 +-----------------------------------------------------------------+
 |     Make the scale of histogram                                 |
 +-----------------------------------------------------------------+
                                                                           */
  xh_scale( flag, vmin, vmax, vstep, unit, 51, scale, line );

  printf("                             ");
  printf("Linear Scale indicated by *\n");
  printf("     x        d(Sigma)/dx   %s\n", scale );
  printf(" +-------+------------------+%s\n", line );

  float vx=x_max-dev; 
  if( vx < 0.0 ) vx=dev-x_max;
  float xm=x_min;
  if( xm < 0.0 ) xm = -x_min;

  if( xm > vx ) vx = xm;

  float f2, ord, ordr;
  int   iord, jord;

  forder( vx, f2, ordr, jord );

  int numb = 0, numbl = 0;

  if( vmin < 0.0 )
    {
      float v1 = vmin;
      int i;
      for( i = 0; i < 80; i++ )
         {
           float v2 = v1 + unit;
           if( v1 <= 0.0 && v2 >= 0.0 )
             {
               numbl = i;
               break;
              }
           v1 = v2;
         }
     }
//---------
  for( i = 0; i < mxbin+1; i++ )
     {
       vx  = val[i];
       if( vmin >= 0.0 )
         {
           disp[49] = bar;

           if( vx > 0.0 )
             {
               numbl = (int) ( ( vlog[i] - vlmin )/unitl );
               numb  = (int) ( vx/unit + 1.0 );

               if( numbl > 49 ) numbl = 49;
               if( numb  > 49 ) numb  = 49;

	       int k;
               for(  k = 0;     k < numb; k++ )  disp[k] = star;
               for(  k = numb;  k < 49; k++ )    disp[k] = blank;
               disp[numbl] = oo;

              }
           else
	     {
	      int k;
              for( k = 0; k < 49; k++ ) disp[k] = blank;
	     }
          }
       else
         {
           float v1    = vmin;
           int high  = 0;
	   int j;
           for( j = 0; j < 50; j++ )
              {
                float v2 = v1 + unit;
                if( vx >= v1 && vx < v2 ) 
                  {
                    high = j;
                    break;
                   }
                v1 = v2;
               }
           int low = numbl;
           if( high < low )
             {
               int nx = high;
               high   = low;
               low    = nx;
              }
	   int k;
           for( k = 0; k < 50; k++ )
              {
                if( k == numbl ) disp[k] = bar;
                else
                if( k > high )   disp[k] = blank;
                else
                if( k < low )    disp[k] = blank;
                else
                if( k == high && k == low ) disp[k] = bar;
                else                        disp[k] = star;
               }
           disp[49] = bar;
         }
       disp[50] = '\0';

       int   nx  = nsamp[i];
             vx  = val[i];
       float vx1 = vx;

       if( vx < 0.0 ) vx1 = -vx;
       if( i == 0 || i == mxbin )
         {
           forder( vx1, f2, ord, iord );
           f2 = vx/ord;

           printf(" |  E%3.2d |%6.3f        E%3.2d|%50s\n",
                            jord, f2, iord, disp );
          }
       else
         {
           xm = ( x_min + dev*(i-1))/ordr;
           float ve = verr[i];
           if( ve > vx1 ) forder(  ve, f2, ord, iord );
           else           forder( vx1, f2, ord, iord );
           f2 = vx/ord;
           ve = ve/ord;

           printf(" |%6.3f |%6.3f+-%5.3f E%3.2d|%.50s\n",
                             xm, f2, ve, iord, disp );
          }
      }
//---------
  if( vmin >= 0.0 )
    {
      xh_scale( 1, vlmin, vlmax, vlstep, unitl, 51, scale, line );
      vxmin = (float) pow( 10.0, (double) vlmin );
     }
                    
  printf(" +-------+------------------+%s\n", line );
  printf("     x        d(Sigma)/dx   %s\n", scale );
  if( vmin >= 0.0 )
    {
      printf("                              ");
      printf("Logarithmic Scale indicated by o\n");
     }

  delete [] val;
  delete [] verr;
  delete [] vlog;

}
                                                                            /*
******************************************************************************
*             =============================================                  */
              void  bases_xhist :: sh_plot( double scalls )                 /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To print the histograms for spring.                                      *
******************************************************************************/
{
  if( if_bases == BASES ) sh_plot1( scalls );
  else                    sh_plot2( );
}
                                                                            /*
******************************************************************************
*             =============================================                  */
              void  bases_xhist :: sh_plot1( double scalls )                /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To print the original histograms for spring.                             *
******************************************************************************/
{
  char cn = 0xc;

  static float ymax = 49.5;
  char disp[56], line[56], scale[56];
  float calls = (float) scalls;
  char blank = ' ', star = '*', oo = 'o', bar = '|';

  float *val;
  float *verr;
  float *vlog;
  val  = new float [no_bin+2];
  verr = new float [no_bin+2];
  vlog = new float [no_bin+2];

  int i;
  for( i = 0; i< no_bin+2; i++)
     {
       val[i]  = 0.0;
       verr[i] = 0.0;
       vlog[i] = 0.0;
      }


      printf("%c\n", cn );
      printf("\n\n");
      printf(" Original Histogram (ID = %d) for %s\n", ID, name );

      int   mxbin = no_bin + 1;

      float fact  = 1.0/( calls*dev );

      val[0]      = totfr[0]/calls;
      val[mxbin]  = totfr[mxbin]/calls;
      float vmax  = 0.0;
      float vevt  = 0.0;
      float vorg  = 0.0;
      float vevt1 = freq[0] + freq[mxbin];
                                                                           /*
     +--------------------------------------------------------------+
     |     Find the minimum and maximum values in the histogram     |
     +--------------------------------------------------------------+
                                                                           */
      float vls;
//    ---------
      for( i = 1; i < mxbin; i++ )
         {
           float tx  = totfr[i];
           int   nx  = (int)totf2[i];

           vls = tx*fact;
           if( vmax < vls ) vmax = vls;
           val[i]    = vls;
           if( nx > 1 )
             {
               float dev2 = (float) nx * totf2[i] - tx*tx;
               if( dev2 < 0.0 ) verr[i] = 0.0;
               else        verr[i] = fact*sqrt((double)( dev2/(nx - 1) ));
              }
           else
           if( nx == 1 ) verr[i] = vls;
           else          verr[i] = 0.0;

           vorg  = vls + vorg;
           vevt  = freq[i] + vevt;
          }
//    ---------
      int ntot = (int) ( vevt + vevt1 );
                                                                           /*
     +--------------------------------------------------------------+
     |     Prepare the log histogram                                |
     +--------------------------------------------------------------+
                                                                           */
      float vlmax, vlmin, vxmax, vxmin, unitl, vlstep;
      int flag;
      unitl=1.0;
      float vnorm = vorg/vevt;
      float xnorm = vnorm*dev;

      vlmax = log10( (double) vmax );
      vlmin = vlmax;

      for( i = 0; i < mxbin+1; i++ )
         {
           if( val[i] > 0.0 )
             {
               vls = log10((double) val[i]);
               if( i > 0 && i < mxbin && vls < vlmin ) vlmin = vls;
               vlog[i] = vls;
              }
           else vlog[i] = 0.0;
          }
      if( vlmin < 0.0 ) vxmin = (int) vlmin - 1.0;
      else              vxmin = (int) vlmin;
      vxmax = vlmax;

      flag  = 1;

      xh_range( flag, vxmin, vxmax, vlmin, vlmax, vlstep );

      unitl = ( vlmax - vlmin)/ymax;
/*
              printf("vxmin(%8.2e) vxmax(%8.2e) vlmin(%8.2e) vlmax(%8.2e)",
                      vxmin, vxmax, vlmin, vlmax );
              printf(" unitl(%8.2e)\n", unitl );
*/
                                                                           /*
     +--------------------------------------------------------------+
     |     Make the scale of histogram                              |
     +--------------------------------------------------------------+
                                                                           */
      xh_scale( flag, vlmin, vlmax, vlstep, unitl, 51, scale, line );

      printf(" Total =%10d events   *: Orig. Dist. in Log Scale.\n", ntot );
      vxmin = (float) pow( 10.0, (double) vlmin );

      printf("     x     d(Sig)/dx  dN/dx %s\n", scale );
      printf(" +-------+----------+-------+%s\n", line );

      float vx=x_max-dev; 
      if( vx < 0.0 ) vx=dev-x_max;
      float xm=x_min;
      if( xm < 0.0 ) xm = -x_min;

      if( xm > vx ) vx = xm;

      float f2, ord, ordr;
      int   iord, jord;

      forder( vx, f2, ordr, jord );

      int numb = 0, numbl = 0, nerl, nerup;

//    ---------
      for( i = 0; i < mxbin+1; i++ )
         {
           float rnorm = vnorm;
           if( i == 0 || i == mxbin ) rnorm = xnorm;
           vx    = val[i];
           float xl  = freq[i];
           int   nx  = (int) xl;
           if( vx > 0 ) numbl = (int) ((vlog[i] - vlmin)/unitl + 1.0);
           else         numbl =  0;
           if( nx > 0 )
             {
               numb       = (int)((log10((double)(xl*rnorm)) - vlmin )/unitl);
               float erl  = (float) sqrt((double) xl );
               float deru = ( xl + erl )*rnorm;
               nerup      = (int)((log10((double)deru) - vlmin)/unitl);
               float derl = ( xl - erl )*rnorm;
               if( derl > 0.0 )
                     nerl = (int)((log10((double)derl) - vlmin)/unitl);
               else
                     nerl = 0;
              }
           else
             {
               numb  = 0;
               nerup = 0;
               nerl  = 0;
              }

           if( numb  > 49 ) numb  = 49;
           if( numbl > 49 ) numbl = 49;
           if( nerup > 49 ) nerup = 49;
           if( nerl  > 49 ) nerl  = 49;

           disp[49] = bar;
	   int j;
           for( j = 0; j < numbl; j++ ) disp[j] = star;
           for( j = numbl; j <    49; j++ ) disp[j] = blank;
           if( nerup == nerl ) 
             {
               if( nerup > 0 ) disp[nerl] = oo;
              }
           else
             {
               disp[nerl] = '<';
               disp[nerup]= '>';
              }
           disp[50] = '\0';

           forder( vx, f2, ord, iord );
           f2 = vx/ord;

           if( i == 0 || i == mxbin )
             {
               printf(" |  E%3.2d |%6.3fE%3.2d|%7d|%50s\n",
                                       jord, f2, iord, nx, disp );
              }
           else
             {
               xm = ( x_min + dev*(i-1))/ordr;

               printf(" |%6.3f |%6.3fE%3.2d|%7d|%.50s\n",
                             xm, f2, iord, nx, disp );
              }
          }

   printf(" +-------+----------+-------+%s\n", line );
   printf("     x     d(Sig)/dx  dN/dx %s\n", scale );
   printf("                          ");
   printf(" o or <>: Generating events ( Arbitrary unit  in Log.)\n");

  delete [] val;
  delete [] verr;
  delete [] vlog;
}
                                                                            /*
******************************************************************************
*             =============================================                  */
                  void  bases_xhist :: sh_plot2(  )                         /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To print the additional histograms for spring.                           *
******************************************************************************/
{
  char cn = 0xc;

  static float ymax = 49.5;
  char disp[56], line[56], scale[56];
  char blank = ' ', star = '*', oo = 'o', bar = '|';

  float *vlog;
  vlog = new float [no_bin+2];


      printf("%c\n", cn );
      printf("\n\n");
      printf(" Additional Histogram (ID = %d) for %s\n", ID, name );

  if( freq[0] > 0.0 ) vlog[0] = log10( (double) freq[0] );
  else                vlog[0] = 0.0;
  float vmax = freq[1];
  float vmin = vmax;

  float vlmax, vlmin;
  if( vmax > 0.0 ) vlmax = log10( (double) vmax );
  else             vlmax = 0.0;
  vlmin = vlmax;

  float vls, vl;
  int i;
  for( i = 1; i < no_bin+1; i++ )
     {
       vls = freq[i];
       if( vls > 0.0 ) vl = log10( (double) vls );
       else            vl = 0.0;
       vlog[i]  = vl;
       if( vmin > vls ) vmin = vls;
       if( vmax < vls ) vmax = vls;
       if( vlmin > vl ) vlmin = vl;
       if( vlmax < vl ) vlmax = vl;
      }

  if( freq[no_bin+1] > 0.0 ) vlog[no_bin+1] = log10( (double) freq[no_bin+1] );
  else                       vlog[no_bin+1] = 0.0;

  int   flag = 0;
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

  xh_scale( flag, vmin, vmax, vstep, unit, 51, scale, line );

  printf( "                             ");
  printf( "Linear Scale indicated by *\n");
  printf( "     x    Lg(dN/dx)  dN/dx %s\n", scale );
  printf( " +-------+----------+-------+%s\n", line );


  float vx =  x_max - dev;
        if( vx < 0.0 ) vx = -vx;
  float xm =  x_min;
        if( xm < 0.0 ) xm = -xm;
        if( xm > vx ) vx = xm;
  float f2, ordr;
  int   jord;
  forder( vx, f2, ordr, jord );


  disp[50] = '\0';
  for( i = 0; i < no_bin+2; i++ )
     {
             disp[49] = bar;
             vx    = freq[i];
       float vl    = vlog[i] - vlmin;

       int numb = -1;
       int numbl = -1;
       if( vx > 0.0 )
         {
           numb  = (int) (vx/unit);
           numbl = (int) (vl/unitl);
          }

       if( numb  > 49 ) numb  = 49;
       if( numbl > 49 ) numbl = 49;

       disp[49] = bar;
       if( numb >= 0 )
         {
	   int j;
           for( j =    0; j <  numb; j++ ) disp[j] = star;
           for( j = numb; j <  49;   j++ ) disp[j] = blank;
           disp[numbl] = oo;
          }
       else
           for( int j =    0; j < 49; j++ ) disp[j] = blank;

       int nx = (int) vx;
       if( i == 0 || i == no_bin+1 )
         {
           printf( " |  E%3.2d |%10.3E|%7d|%50s\n",
                         jord, vlog[i], nx, disp );
          }
       else
         {
           xm = ( x_min + dev*(i-1))/ordr;
           printf( " |%6.3f |%10.3E|%7d|%.50s\n",
                                xm, vlog[i], nx, disp );
          }
      }

  flag = 1;

  xh_scale( flag, vlmin, vlmax, vlstep, unitl, 51, scale, line );

  printf( " +-------+----------+-------+%s\n", line );
  printf( "     x    Lg(dN/dx)  dN/dx %s\n", scale );
  printf( "                             ");
  printf("Log Scale indicated by o\n");

  delete [] vlog;
}
                                                                            /*
******************************************************************************
*             =============================================                  */
                  void bases_xhist :: sh_reset( )                           /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To reset the histograms before starting spring.                          *
******************************************************************************/
{
           x_tag = -1;
           for( int i = 0; i < no_bin+2; i++ ) freq[i] = 0.0;
}
                                                                            /*
******************************************************************************
*             =============================================                  */
                  void bases_xhist :: sh_update( )                          /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To reset the histograms before starting spring.                          *
******************************************************************************/
{

  if( x_tag >= 0 && x_tag < no_bin+2)
    {
      freq[x_tag]++;
      x_tag  = -1;
     }
}

                                                                             /*
******************************************************************************
*                 =============================                              */
  void bases_xhist :: forder( float v, float &fv, float &ordr, int &iordr ) /*
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
       void bases_xhist::xh_range( int flg, float vmin, float vmax,
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
  void bases_xhist:: xh_scale( int flag, float vmin, float vmax, 
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
 int k;
 for( k = 0; k < 8; k++ ) scale[k] = number[k];
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
	      int k;
              for( k = 0; k < 8; k++ ) scale[k+i] = number[k];
              i2 = i + 8;
            }
          else
          if( i == size1-1 && i2 < sizeq )
            {
              sprintf( number,"%8.1e", sc );
              i2 = sizeq;
	      int k;
              for( k = 0; k < 8; k++ ) scale[k+i2] = number[k];
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
     int k;
     for( k = 0; k < 8; k++ ) scale[k+sizep] = number[k];
   }
 scale[size1] = '\0';
 line[size1]  = '\0';

}
