#include <fstream.h>
 enum if_bases { BASES, SPRING };
                                                                            /*
******************************************************************************
*                   =====================================                    */
                            class bases_xhist                               /*
*                   =====================================                    *
******************************************************************************
* ( Purpose )                                                                *
*    A derived class from class BasesSpring, used when histogram facility    *
*  is required for the numerical integration and event generation in terms   *
*  of program package BasesSpring.                                           *
* ( Variables )                                                              *
*  ====================                                                      *
*   Control parameters                                                       *
*  ====================                                                      *
*       ID        : Histogram ID number                                      *
*       x_min     : x_min                                                    *
*       x_max     : x_max                                                    *
*       no_bin    : number of bins                                           *
*       dev       : bin width                                                *
*      Buffer for the current iteration                                      *
*       nsamp[ ] : number of sampling points                                 *
*       freq [ ] : sum of Fi                                                 *
*       freq2[ ] : sum of Fi*Fi                                              *
*      Buffer for total accumulation                                         *
*       totfr[ ] : sum of Fi for total                                       *
*       totf2[ ] : sum of Fi*Fi for total                                    *
*     For spring                                                             *
*       x_tag         : x-tag for spring                                     *
*   ----------------------------------                                       *
*    Structure of Scatter_plot buffer                                        *
*   ----------------------------------                                       *
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
  int ID, no_bin, x_tag, if_bases;
  float x_min, x_max, dev;
  char  name[64];
  int   *nsamp;
  float *freq;
  float *freq2;
  float *totfr;
  float *totf2;                                                           /*
 ------------------
  Private function
 ------------------                                                         */
  static void xh_scale( int flag, float vmin, float vmax,
                                  float vstep, float unit,
                           int size, char scale[], char line[] );
  static void forder( float, float&, float&, int& );
  static void xh_range( int, float, float, float&, float&, float& );
                                                                            /*
=========                                                                    */
 public:                                                                    /*
=========
 ------------------
  Public function
 ------------------                                                         */
       bases_xhist( ){ };
      ~bases_xhist( );
  void deallocate( );
  void bs_write( ofstream& out );
  void bs_read( ifstream& in );
  int  getID( ) { return ID; };
  void xh_init( int id, double Xmin, double Ymin, int Nbin, char name[] );
  void xh_fill( int id, double x, double fn, double wgt, int flag );
  void bh_save();
  void xh_plot( double scalls );
  void sh_plot( double scalls );
  void sh_plot1( double scalls );
  void sh_plot2( );
  void sh_reset();
  void sh_update();
};

