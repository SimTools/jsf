#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bases_hist.h"

                                                                            /*
******************************************************************************
*              ==================================                            */
                  bases_hist :: bases_hist( )                               /*
*              ==================================                            *
******************************************************************************
* ( Purpose )                                                                *
*   To construct an object of the class bases_hist.                          *
******************************************************************************/
{

  xhist = new bases_xhist* [ MXHIST ];

  no_hist = 0;

  int i,j;
  for( j = 0; j < NHASH; j++ )
  for( i = 0; i < NHS; i++ ) xhash[i][j] = 0;

  scatt = new bases_scat* [ MXHIST ];

  no_scat = 0;

  for( j = 0; j < NHASH; j++ )
  for( i = 0; i < NHS; i++ ) dhash[i][j] = 0;
}
                                                                            /*
******************************************************************************
*              ==================================                            */
                  bases_hist :: ~bases_hist( )                              /*
*              ==================================                            *
******************************************************************************
* ( Purpose )                                                                *
*   To destruct  an object of the class bases_hist.                          *
******************************************************************************/
{
  int i;
  for(     i = 0; i < no_hist; i++ ) delete  xhist[i];
  delete [ ] xhist;

  for(     i = 0; i < no_scat; i++ ) delete  scatt[i];
  delete [ ] scatt;
}
                                                                            /*
******************************************************************************
*           ==========================================                       */
            void  bases_hist :: bs_write( ofstream& out )                   /*
*           ==========================================                       *
******************************************************************************
* ( Purpose )                                                                *
*   To store the histograms and scatter plots into the file FileName.        *
******************************************************************************/
{                                                                           /*
 +---------------------------------------------------------------------------+
 |(1) Check if ofstream out is defined.  If not, terminate all.              |
 +---------------------------------------------------------------------------+
                                                                             */
  if( !out )
    {
      cout << "output file is not defined.\n";
      exit( 1 );
     }
                                                                            /*
 +---------------------------------------------------------------------------+
 |(2) write no_hist histogram data on out.                                   |
 +---------------------------------------------------------------------------+
                                                                             */
  out.write((char *) &no_hist, sizeof( int ));   // the number of histograms

  int i;
  for(     i = 0; i < no_hist; i++ )
     {
       int id = xhist[i]->getID();
       out.write((char *) &id, sizeof( int ));   // hisogram id number

       xhist[i]->bs_write( out );                // histogram content
      }
                                                                            /*
 +---------------------------------------------------------------------------+
 |(3) write no_scat scat.plot data on out.                                   |
 +---------------------------------------------------------------------------+
                                                                             */
  out.write((char *) &no_scat, sizeof( int ));   // the number of scat.plots

  for(     i = 0; i < no_scat; i++ )
     {
       int id = scatt[i]->getID();
       out.write((char *) &id, sizeof( int ));  // scat.plot id number

       scatt[i]->bs_write( out );               // scat.plot content
      }
}
                                                                            /*
******************************************************************************
*           =============================================                    */
             void  bases_hist :: bs_read( ifstream& in )                    /*
*           =============================================                    *
******************************************************************************
* ( Purpose )                                                                *
*   To store the histograms and scatter plots into the file FileName.        *
******************************************************************************/
{                                                                           /*
 +---------------------------------------------------------------------------+
 |(1) Check if ifstream out is defined.  If not, terminate all.              |
 +---------------------------------------------------------------------------+
                                                                             */
  if( !in )
    {
      cout << "input file is not defined.\n";
      exit( 1 );
     }
                                                                            /*
 +---------------------------------------------------------------------------+
 |(2) read Nhist histogram data from in.                                     |
 +---------------------------------------------------------------------------+
                                                                             */
  int Nhist;
  in.read((char *) &Nhist, sizeof( int ));          // the number of histograms

  int i;
  for( i = 0; i < Nhist; i++ )
     {
    // (2-1)  read histogram id number

       int id;
       in.read((char *) &id, sizeof( int ));

       int hash   = id % NHASH;
       int nhash  = xhash[0][hash];
       int histNo = -1;

    // Check if the histogram id is already defined.

       if( no_hist > 0 )
         {
           if( nhash > 0 )
             {
               for( int j = 1; j < nhash+1; j++ )
                  {
                    int  k = xhash[j][ hash ];

                 // if defined, read the content to the histogram id's buffer.

                    if( id == xhist[k]->getID() )
                      {
                        xhist[k]->bs_read( in );
                        histNo = k;
                        break;
                       }
                   }
              }
          }

    // if not defined, create the histogram id and read the content.

       if( histNo == -1 )
         {
           nhash++;
           xhash[nhash][hash] = no_hist;
           xhash[0][hash]     = nhash;

           xhist[no_hist] = new bases_xhist;
           xhist[no_hist]->bs_read( in );

           no_hist++;
          }

      }
                                                                            /*
 +---------------------------------------------------------------------------+
 |(3) read Nscat scat.plot data from in.                                     |
 +---------------------------------------------------------------------------+
                                                                             */
  int Nscat;
  in.read((char *) &Nscat, sizeof( int ));          // the number of scat.plots

  for(  i = 0; i < Nscat; i++ )
     {
    // (3-1)  read scat.plot id number

       int id;
       in.read((char *) &id, sizeof( int ));

       int hash   = id % NHASH;
       int nhash  = dhash[0][hash];
       int scatNo = -1;

    // (3-2) Check if the scat.plot id is already defined.

       if( no_scat > 0 )
         {
           if( nhash > 0 )
             {
               for( int j = 1; j < nhash+1; j++ )
                  {
                    int  k = dhash[j][ hash ];

                 // if defined, read the content to the scat.plot id's buffer.

                    if( id == scatt[k]->getID() )
                      {
                        scatt[k]->bs_read( in );
                        scatNo = k;
                        break;
                       }
                   }
              }
          }

    // if not defined, create the scat.plot id and read the content.

       if( scatNo == -1 )
         {
           nhash++;
           dhash[nhash][hash] = no_scat;
           dhash[0][hash]     = nhash;

           scatt[no_hist] = new bases_scat;
           scatt[no_hist]->bs_read( in );

           no_scat++;
          }

      }
}
                                                                            /*
******************************************************************************
*         ==============================================================     */
        void bases_hist :: xh_init( int id, 
                    double Xmin, double Xmax, int Nbin, char title[] )      /* 
*         ==============================================================     *
******************************************************************************
* ( Purpose )                                                                *
*   To initialize the histogram with identity number id.                     *
******************************************************************************/
{                                                                           /*
 +---------------------------------------------------------------------------+
 |(1) Check the number of histograms                                         |
 +---------------------------------------------------------------------------+
                                                                            */
  if( no_hist >= MXHIST )
    {
      printf(" The number of histograms exceeds %d\n", MXHIST );
      printf(" The histogram( ID =%d ) is neglected.\n", id );

      return;
     }
                                                                            /*
 +---------------------------------------------------------------------------+
 |(2) Check if new histogram                                                 |
 +---------------------------------------------------------------------------+
                                                                            */
  int hash  = id % NHASH;
  int nhash = xhash[0][hash];
  if( nhash > 0 )
    {
      int i;
      for(     i = 1; i < nhash+1; i++ )
         {
           if( id == xhist[ xhash[ i ][ hash ] ]->getID() )
             {
               printf(" The histogram ( ID = %d ) exists already\n", id );
               printf(" The histogram( ID =%d ) is neglected.\n", id );

               return;
              }
          }
     }

  nhash++;
  xhash[nhash][hash] = no_hist;
  xhash[0][hash]     = nhash;

  xhist[no_hist] = new bases_xhist;
  xhist[no_hist]->xh_init( id, Xmin, Xmax, Nbin,  title );

  no_hist++;

}
                                                                            /*
******************************************************************************
*      ==================================================================    */
        void bases_hist :: xh_fill( int id, double X, double fn, 
                                             double wgt, int Bases_Flag )   /*
*      ==================================================================    * 
******************************************************************************
* ( Purpose )                                                                *
*   To fill the histogram of identity number id                              *
******************************************************************************/
{
                                                                            /*
 +---------------------------------------------------------------------------+
 |(1) Check if new histogram                                                 |
 +---------------------------------------------------------------------------+
                                                                            */
  int hash   = id % NHASH;
  int nhash  = xhash[0][hash];
  int histNo = -1;
  int i;
  if( nhash > 0 )
    {
      for(     i = 1; i < nhash+1; i++ )
         {
           int  j = xhash[i][ hash ];
           if( id == xhist[j]->getID() )
             {
               xhist[j]->xh_fill( id, X, fn, wgt, Bases_Flag );
               histNo = j;
               break;
              }
          }
     }
  if( histNo == -1 )
    {
      printf("Histogram error in xh_fill: ID(%d) is illegal !!\n", id );
      exit(0);
     }
}
                                                                            /*
******************************************************************************
*         ==============================================================     */
        void bases_hist :: dh_init( int id, 
                    double Xmin, double Xmax, int Xbin,
                    double Ymin, double Ymax, int Ybin, char title[] )      /* 
*         ==============================================================     *
******************************************************************************
* ( Purpose )                                                                *
*   To initialize the scatter plot with identity number id.                  *
******************************************************************************/
{                                                                           /*
 +---------------------------------------------------------------------------+
 |(1) Check the number of scatter plots                                      |
 +---------------------------------------------------------------------------+
                                                                            */
  if( no_scat >= MXHIST )
    {
      printf(" The number of scatter plots exceeds %d\n", MXHIST );
      printf(" The scatter plot ( ID =%d ) is neglected.\n", id );

      return;
     }
                                                                            /*
 +---------------------------------------------------------------------------+
 |(2) Check if new scatter plot                                              |
 +---------------------------------------------------------------------------+
                                                                            */
  int hash  = id % NHASH;
  int nhash = dhash[0][hash];
  if( nhash > 0 )
    {
      int i;
      for(  i = 1; i < nhash+1; i++ )
         {
           if( id == scatt[ dhash[ i ][ hash ] ]->getID() )
             {
               printf(" The scatter plot ( ID = %d ) exists already\n", id );
               printf(" The scatter plot( ID =%d ) is neglected.\n", id );

               return;
              }
          }
     }

  nhash++;
  dhash[nhash][hash] = no_scat;
  dhash[0][hash]     = nhash;

  scatt[no_scat]     = new bases_scat;
  scatt[no_scat]->dh_init( id, Xmin, Xmax, Xbin, Ymin, Ymax, Ybin, title );

  no_scat++;

}
                                                                            /*
******************************************************************************
*      ==================================================================    */
        void bases_hist :: dh_fill( int id, 
              double X, double Y, double fn, double wgt, int Bases_Flag )   /*
*      ==================================================================    * 
******************************************************************************
* ( Purpose )                                                                *
*   To fill the scatter plot of identity number id                           *
******************************************************************************/
{
                                                                            /*
 +---------------------------------------------------------------------------+
 |(1) Check if new scatter plot                                              |
 +---------------------------------------------------------------------------+
                                                                            */
  int hash   = id % NHASH;
  int nhash  = dhash[0][hash];
  int histNo = -1;
  if( nhash > 0 )
    {
      for( int i = 1; i < nhash+1; i++ )
         {
           int  j = dhash[i][ hash ];
           if( id == scatt[j]->getID() )
             {
               scatt[j]->dh_fill( id, X, Y, fn, wgt, Bases_Flag );
               histNo = j;
               break;
              }
          }
     }
  if( histNo == -1 )
    {
      printf("scatter plot error in dh_fill: ID(%d) is illegal !!\n", id );
      exit(0);
     }
}
                                                                            /*
******************************************************************************
*             =============================================                  */
                    void  bases_hist :: bh_save()                           /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To add the content of histogram buffer of current iteration into         *
*          the total histogram buffers.                                      *
******************************************************************************/
{
  for( int i = 0; i < no_hist; i++ ) xhist[i]->bh_save();
}
                                                                            /*
******************************************************************************
*             ==============================================                 */
                void  bases_hist :: bh_plot( double scalls )                /*
*             ==============================================                 *
******************************************************************************
* ( Purpose )                                                                *
*   To add the content of histogram buffer of current iteration into         *
*          the total histogram buffers.                                      *
******************************************************************************/
{
  int i;
  for(  i = 0; i < no_hist; i++ ) xhist[i]->xh_plot( scalls );
  for(  i = 0; i < no_scat; i++ ) scatt[i]->dh_plot( );
}
                                                                            /*
******************************************************************************
*             =============================================                  */
               void  bases_hist :: sh_plot( double scalls )                 /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To add the content of histogram buffer of current iteration into         *
*          the total histogram buffers.                                      *
******************************************************************************/
{
  int i;
  for(  i = 0; i < no_hist; i++ ) xhist[i]->sh_plot( scalls );
  for(  i = 0; i < no_scat; i++ ) scatt[i]->dh_plot( );
}
                                                                            /*
******************************************************************************
*             =============================================                  */
                   void bases_hist :: sh_reset( )                           /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To reset the histograms before starting spring.                          *
******************************************************************************/
{
  int i;
  for(  i = 0; i < no_hist; i++ ) xhist[i]->sh_reset( );
  for(  i = 0; i < no_scat; i++ ) scatt[i]->sh_reset( );
}
                                                                            /*
******************************************************************************
*             =============================================                  */
                   void bases_hist :: sh_update( )                          /*
*             =============================================                  *
******************************************************************************
* ( Purpose )                                                                *
*   To reset the histograms before starting spring.                          *
******************************************************************************/
{
  int i;
  for(  i = 0; i < no_hist; i++ ) xhist[i]->sh_update( );
  for(  i = 0; i < no_scat; i++ ) scatt[i]->sh_update( );
}
