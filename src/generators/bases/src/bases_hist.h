#ifdef __USEISOCXX__
#include <fstream>
#include <iostream>
#else
#include <fstream.h>
#include <iostream.h>
#endif
#include "bases_xhist.h"
#include "bases_scat.h"
const int NHS = 10, NHASH = 13, MXHIST = 110;
                                                                            /*
******************************************************************************
*                   =====================================                    */
                            class bases_hist                                /*
*                   =====================================                    *
******************************************************************************
* ( Purpose )                                                                *
*    The class for histogram control.                                        *
* ( Variables )                                                              *
*  ====================                                                      *
*   Control parameters                                                       *
*  ====================                                                      *
*   int no_xhist              : the number of histograms                     *
*                                                                            *
*   int xhash[ NHS ][ NHASH]  : hash table                                   *
*       xhash[ 0 ][ i ] =  n  : the number of histograms for i-th group      *
*       xhash[ 1 ][ i ] = k1  : serial number of the histogram               *
*              :    :      :                                                 *
*       xhash[ n ][ i ] = kn  : serial number of the histogram               *
*                                                                            *
*   bases_xhist xhist[ MXXHIST ] : histogram objects                         *
*                                                                            *
*   int no_scat               : the number of scatter plots                  *
*                                                                            *
*   int dhash[ NHS ][ NHASH]  : hash table                                   *
*       dhash[ 0 ][ i ] =  n  : the number of histograms for i-th group      *
*       dhash[ 1 ][ i ] = k1  : serial number of the histogram               *
*              :    :      :                                                 *
*       dhash[ n ][ i ] = kn  : serial number of the histogram               *
*                                                                            *
*   bases_scat  scatt[ MXXHIST ] : scatter plot objects                      *
*                                                                            *
* ( Author )                                                                 *
*    Setsuya Kawabata,   June  '99 at KEK                                    *
******************************************************************************/
{                                                                           /*
friend class JSFBases;

=========
 private:
=========
 ------------------
  Private members
 ------------------                                                          */
  int no_hist;
  int xhash[ NHS ][ NHASH ];
//  bases_xhist* xhist;
  bases_xhist** xhist;
  int no_scat;
  int dhash[ NHS ][ NHASH ];
  bases_scat** scatt;
                                                                           /*
 ------------------
  Private function
 ------------------                                                         */
                                                                            /*
=========                                                                    */
 public:                                                                    /*
=========
 ------------------
  Public function
 ------------------                                                         */
       bases_hist( );
      ~bases_hist( );
  void bs_write( ofstream& out );
  void bs_read( ifstream& in );
  void xh_init( int id, double Xmin, double Xmax, int Nbin, char name[] );
  void xh_fill( int id, double x, double fn, double wgt, int Flag );
  void dh_init( int id, double Xmin, double Xmax, int Xbin,
                        double Ymin, double Ymax, int Ybin, char name[] );
  void dh_fill( int id, double x, double y, double fn, 
                                             double wgt, int Flag );
  void bh_save();
  void bh_plot( double scalls );
  void sh_plot( double scalls );
  void sh_reset();
  void sh_update();

  inline int get_no_hist(){ return no_hist; }
  inline int get_no_scat(){ return no_scat; }
  inline int get_total_hist(){ return no_hist+no_scat; }

};

