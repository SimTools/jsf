#ifdef __USEISOCXX__
#include <iostream>
#include <fstream>
using namespace std;
#else
#include <iostream.h>
#include <fstream.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bases_scat.h"

                                                                           /*
******************************************************************************
*                  ===================================                       */
                       bases_scat :: ~bases_scat( )                         /*
*                  ===================================                       *
******************************************************************************/
{
   deallocate();
}
                                                                             /*
******************************************************************************
*                  ===================================                       */
                     void bases_scat :: deallocate( )                       /*
*                  ===================================                       *
******************************************************************************/
{
  //  printf("Deallocator for scat.plot( ID = %d ) was invoked.\n", ID );

  for( int i =  0; i <  no_xbin+2; i++ ) delete [ ] freq[i];
  delete [ ] freq;
}
                                                                             /*
******************************************************************************
*              ==============================================                */
                void bases_scat :: bs_write( ofstream& out )                /*
*              ==============================================                *
******************************************************************************
* ( Purpose )                                                                *
*   To write the scatter plot content on ofstream out.                       *
* ( Author )                                                                 *
*    S.Kawabata,  July `99 at KEK                                            *
******************************************************************************/
{
  if( !out )
    {
      cout << "Output file is not defined.\n";
      exit( 1 );
     }

  out.write((char *) &ID,      sizeof( int ));
  out.write((char *) &no_xbin, sizeof( int ));
  out.write((char *) &no_ybin, sizeof( int ));

  out.write((char *) &x_min,  sizeof( float ));
  out.write((char *) &x_max,  sizeof( float ));
  out.write((char *) &x_dev,  sizeof( float ));

  out.write((char *) &y_min,  sizeof( float ));
  out.write((char *) &y_max,  sizeof( float ));
  out.write((char *) &y_dev,  sizeof( float ));

  out.write(      name,            64 );

  for( int i = 0; i < no_xbin+2; i++ )
  out.write((char *)  freq[i],   sizeof( float ) * (no_ybin+2));
}
                                                                             /*
******************************************************************************
*              ==============================================                */
                 void bases_scat :: bs_read( ifstream& in )                 /*
*              ==============================================                *
******************************************************************************
* ( Purpose )                                                                *
*   To read the scatter plot content from ifstream in.                       *
* ( Author )                                                                 *
*    S.Kawabata,  July `99 at KEK                                            *
******************************************************************************/
{
  if( !in )
    {
      cout << "Input file is not defined.\n";
      exit( 1 );
     }

  int id, Nxbin, Nybin;
  in.read((char *) &id,     sizeof( int ));
  in.read((char *) &Nxbin,  sizeof( int ));
  in.read((char *) &Nybin,  sizeof( int ));

  in.read((char *) &x_min,  sizeof( float ));
  in.read((char *) &x_max,  sizeof( float ));
  in.read((char *) &x_dev,  sizeof( float ));

  in.read((char *) &y_min,  sizeof( float ));
  in.read((char *) &y_max,  sizeof( float ));
  in.read((char *) &y_dev,  sizeof( float ));

  in.read(      name,            64 );

  if( freq &&  ( Nxbin != no_xbin || Nybin != no_ybin ) )
    {
      no_xbin = Nxbin;
      no_ybin = Nybin;
      ID     = id;

      deallocate( );
     }

  if( !freq )
    {
      freq = new float* [no_xbin+2];
      for( int i =  0; i <  no_xbin+2; i++ )
               freq[i] = new float[ no_ybin+2 ];
     }
  for( int i = 0; i < no_xbin+2; i++ )
  in.read((char *)  freq[i],   sizeof( float ) * (no_ybin+2));
}
                                                                            /*
******************************************************************************
*         ==============================================================     */
        void bases_scat :: dh_init( int id, 
                    double Xmin, double Xmax, int Xbin,
                    double Ymin, double Ymax, int Ybin, char title[] )      /* 
*         ==============================================================     *
******************************************************************************
* ( Purpose )                                                                *
*   To initialize the histogram with identity number id.                     *
* ( Author )                                                                 *
*   S.Kawabata, July `99 at KEK                                              *
******************************************************************************/
{                                                                           /*
 +---------------------------------------------------------------------------+
 |(1) Save parameters                                                        |
 +---------------------------------------------------------------------------+
                                                                             */
  ID       = id;

  x_min    = (float) Xmin;
  x_max    = (float) Xmax;
  y_min    = (float) Ymin;
  y_max    = (float) Ymax;

  no_xbin   = Xbin;
  no_ybin   = Ybin;
  x_dev     = (float) ( x_max - x_min )/no_xbin;
  y_dev     = (float) ( y_max - y_min )/no_ybin;

  int leng = strlen( title );
  for( int j = 0; j < leng; j++ ) name[j] = title[j];
  name[leng] = '\0';
                                                                            /*
 +---------------------------------------------------------------------------+
 |(2) Create buffer in free space and celar the buffer area                  |
 +---------------------------------------------------------------------------+
                                                                             */
  int i;
  freq = new float* [no_xbin+2];
  for( i =  0; i <  no_xbin+2; i++ )
           freq[i] = new float[ no_ybin+2 ];

  for( i =  0; i <  no_xbin+2; i++ )
  for( int j =  0; j <  no_ybin+2; j++ )
       freq [i][j] = 0.0;

/*
  printf("Scatter plot ID(%d) :\n", ID );
  printf(" x_min(%8.4e) x_max(%8.4e) no_xbin(%d), x_dev(%8.4e)\n",
           x_min, x_max, no_xbin, x_dev );
  printf(" y_min(%8.4e) y_max(%8.4e) no_ybin(%d), y_dev(%8.4e)\n",
           y_min, y_max, no_ybin, y_dev );
*/
}
                                                                            /*
******************************************************************************
*      ==================================================================    */
        void bases_scat :: dh_fill( int id, 
              double X, double Y, double fn, double wgt, int Bases_Flag )   /*
*      ==================================================================    * 
******************************************************************************
* ( Purpose )                                                                *
*   To fill the scatter plot of identity number id                           *
******************************************************************************/
{                                                                           /*
 +---------------------------------------------------------------------------+
 |(1) Initial setting                                                        |
 +---------------------------------------------------------------------------+
                                                                             */
  if( ID != id )
    {
      printf("Scatter Plot error in xh_fill: ID(%d) is illegal !!\n", id );
      printf(" ID = %d\n", ID ) ;
      exit(0);
     }
  float x  = (float) X;
  float y  = (float) Y;
  int   ix    = -1;
  int   iy    = -1;
                                                                            /*
 +---------------------------------------------------------------------------+
 |(2) Determine x and y bin numbers                                          |
 +---------------------------------------------------------------------------+
                                                                             */
  if( x < x_min ) ix = 0;
  else
  if( x > x_max ) ix = no_xbin + 1;
  else {
    ix  = (int) (( x - x_min )/x_dev + 1.0 );
    if( ix > no_xbin ) ix = no_xbin;
   }

  if( y < y_min ) iy = 0;
  else
  if( y > y_max ) iy = no_ybin + 1;
  else 
    {
      iy  = (int) (( y - y_min )/y_dev + 1.0 );
      if( iy > no_ybin ) iy = no_ybin;
     }
                                                                            /*
 +---------------------------------------------------------------------------+
 |(3) Update the plot's content for BASES or set bin numbers for SPRING      |
 +---------------------------------------------------------------------------+
                                                                             */
  if( Bases_Flag )
    {
      freq [ix][iy] += (float)( fn * wgt );
     }
  else
    {
      x_tag = ix;
      y_tag = iy;
     }
}
                                                                            /*
******************************************************************************
*             ==============================================                 */
               void  bases_scat :: dh_plot( )                               /*
*             ==============================================                 *
******************************************************************************
* ( Purpose )                                                                *
*   To add the content of histogram buffer of current iteration into         *
*          the total histogram buffers.                                      *
******************************************************************************/
{
  char cn = 0xc;
  static char plus = '+', minus = '-', blnk = ' ', star = '*';
  static char num[]= {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
  static char neg[]= {'-', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i' };
  static char sharp= '#', pnt = '.';
                                                                            /*
 +---------------------------------------------------------------------------+
 |(1) Create temporary buffers for printing                                  |
 +---------------------------------------------------------------------------+
                                                                             */
  char*  desp;
  float* X;
  desp = new char  [ no_xbin+3 ];
  X    = new float [ no_xbin+2 ];
                                                                            /*
 +---------------------------------------------------------------------------+
 |(2) Print the title of scatter plot                                        |
 +---------------------------------------------------------------------------+
                                                                             */
  printf("%c\n", cn );
  printf("\n\n");
  printf(" Scat_Plot (ID = %d) for %s\n\n", ID, name );
                                                                            /*
 +---------------------------------------------------------------------------+
 |(3) calculate the orders of x_scale and y_scale values                     |
 +---------------------------------------------------------------------------+
                                                                             */
  float xm    = x_max;
  if( xm < 0.0 ) xm = -xm;
  float xx    = x_min;
  if( xx < 0.0 ) xx = -xx;
  if( xx > xm ) xm = xx;

  float f2, xordr;
  int jxord;
  forder( xm, f2, xordr, jxord );

  float ym    = y_max;
  if( ym < 0.0 ) ym = -ym;
  float yy    = y_min;
  if( yy < 0.0 ) yy = -yy;
  if( yy > ym ) ym = yy;

  float yordr;
  int   jyord;
  forder( ym, f2, yordr, jyord );
                                                                            /*
 +---------------------------------------------------------------------------+
 |(4) determine the maximun of the distribution and the unit of height.      |
 +---------------------------------------------------------------------------+
                                                                             */
  float vmax = freq[0][0];
  float vmin = vmax;
  float vls;
  int j;
  for( j = 1; j < no_ybin+1; j++ )
     {
       for( int i = 1; i < no_xbin+1; i++ )
          {
            vls = freq[i][j];
            if( vls > vmax ) vmax = vls;
            if( vls < vmin ) vmin = vls;
           }
      }

  if( vmax == 0.0 && vmin == 0.0 )
    {
      vmax = 10.0;
      vmin =  0.0;
     }

  float unit;
  if( vmax > -vmin ) unit = vmax / 11.0;
  else               unit = vmin / 11.0;

  if( unit < 0.0 ) unit = -unit;
                                                                            /*
 +---------------------------------------------------------------------------+
 |(5) Print the distribution, raw by raw ( y_bin by y_bin ).                 |
 +---------------------------------------------------------------------------+
                                                                             */
  int nyh     = no_ybin/2;
  if( nyh == 0 ) nyh = 1;

  // print the scale line
  desp[0] = plus;
  desp[no_xbin+1] = plus;
  desp[no_xbin+2] = '\0';
  int i;
  for( i = 1; i < no_xbin+1; i++ ) desp[i] = minus;

  printf( "      E%3d   %s\n", jyord, desp );

  desp[0] = '|';
  desp[no_xbin+1] = '|';
  for( j = no_ybin; j > 0; j-- )
     {
       for( int i = 1; i < no_xbin + 1; i++ )
          {
            float xnum = freq[i][j]/unit;
            if( xnum < 0.0 )
              {
                int numb  = (int)(xnum - 1.0);
                if( numb >=  -1 ) desp[i] = minus;
                else
                if( numb >= -10 ) desp[i] = neg[ -numb-1];
                else              desp[i] = sharp;
               }
            else
              {
                int numb  = (int)(xnum + 1.0);
                if( xnum == 0.0 ) desp[i] = blnk;
                else
                if( numb <=   1 ) desp[i] = pnt;
                else
                if( numb <=  10 ) desp[i] = num[ numb-1];
                else              desp[i] = star;
               }
           }

       float yy = ( (j-1)*y_dev + y_min )/yordr;

       if( j == nyh ) printf( "    %6.3f Y %s\n", yy, desp );
       else           printf( "    %6.3f   %s\n", yy, desp );

      }

  desp[0]         = plus;
  desp[no_xbin+1] = plus;
  desp[no_xbin+2] = '\0';
  for( i = 1; i < no_xbin+1; i++ ) desp[i] = minus;

  printf( "      E%3d   %s\n", jyord, desp );

  desp[0]         = blnk;
  desp[no_xbin+1] = blnk;

  int nxh  = no_xbin/2;
  if( nxh == 0 ) nxh = 1;
  for( i = 1; i < no_xbin+1; i++ )
       if( i == nxh ) desp[i] = 'X';
       else           desp[i] = blnk;

  printf( "     Low-    %s\n", desp );

  xordr    = xordr*10.0;
  for( i = 1; i < no_xbin+1; i++ )
     {
       X[i] = (( i - 1 ) * x_dev + x_min )/xordr;
       if( X[i] < 0.0 )
         {
           desp[i] = minus;
           X[i]    = -X[i];
          }
       else
           desp[i] = blnk;
      }

 printf( "     Edge    %s\n", desp );
                                                                            /*
 +---------------------------------------------------------------------------+
 |(6) Print the lower value of each x_bin                                    |
 +---------------------------------------------------------------------------+
                                                                             */
  for( i = 0; i < 5; i++ )
     {
       if( i == 1 )
         {
           for( j = 1; j < no_xbin+1; j++ ) desp[j] = pnt;
           printf( "     E%3d    %s\n", jxord, desp );
          }
       else
         {
           for( j = 1; j < no_xbin+1; j++ )
              {
                float xx      = X[j]*10.0;
                int numb    = (int) xx;
                desp[j] = num[numb];
                X[j]    = xx - (float) numb;
               }
           if( i == 3 )
               printf( "     Low-    %s\n", desp );
           else
           if( i == 4 )
               printf( "     Edge    %s\n", desp );
           else
               printf( "             %s\n", desp );
          }
      }
                                                                            /*
 +---------------------------------------------------------------------------+
 |(7) delete the temporary buffers                                           |
 +---------------------------------------------------------------------------+
                                                                             */
  delete [ ] desp;
  delete [ ] X;

}
                                                                            /*
******************************************************************************
*             =============================================                  */
                  void bases_scat :: sh_reset( )                            /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To reset the scatter plot before starting spring.                        *
******************************************************************************/
{
           x_tag = -1;
           y_tag = -1;
           for( int i = 0; i < no_xbin+2; i++ )
           for( int j = 0; j < no_ybin+2; j++ ) 
                freq[i][j] = 0.0;
}
                                                                            /*
******************************************************************************
*             =============================================                  */
                   void bases_scat :: sh_update( )                          /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To update the scatter plot for spring.                                   *
******************************************************************************/
{
  if(( x_tag >= 0 && x_tag < no_xbin+2) && ( y_tag >= 0 && y_tag < no_ybin+2))
    {
      freq[x_tag][y_tag]++;
      x_tag  = -1;
      y_tag  = -1;
     }
}

                                                                             /*
******************************************************************************
*  ========================================================================  */
   void bases_scat :: forder( float v, float &fv, float &ordr, int &iordr ) /*
*  ========================================================================  *
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
