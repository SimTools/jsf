#include "hist_buff.h"
                                                                            /*
******************************************************************************
*                   =====================================                    */
                     class bs_hist                                          /*
*                   =====================================                    *
******************************************************************************
* ( Purpose )                                                                *
*    A derived class from class bases_spring, used when histogram facility   *
*  is required for the numerical integration and event generation in terms   *
*  of program package bases_spring.                                          *
* ( Variables )                                                              *
*  ====================                                                      *
*   Control parameters                                                       *
*  ====================                                                      *
*       ID        : Histogram ID number                                      *
*       x_min     : x_min                                                    *
*       x_max     : x_max                                                    *
*       no_bin    : number of bins                                           *
*       dev       : bin width                                                *
*      Buffer for the histogram                                              *
*       freq.buff[  ] : sum of Fi                                            *
* ( Author )                                                                 *
*    Setsuya Kawabata,   March '97 at KEK                                    *
******************************************************************************/
{                                                                           /*
=========                                                                   */
 protected:                                                                 /*
=========
 ------------------
  protect members
 ------------------                                                          */
//  bases_spring *bs;
  int ID, no_bin, x_tag, if_bases;
  float x_min, x_max, dev;
  hist_buff  freq;
  char name[64];
                                                                           /*
 ------------------
  protect function
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
  bs_hist( ){ };
//  virtual void xh_init( int id, 
//             double Xmin, double Ymin, int Nbin, char name[], bases_spring& );
  void init( int id, 
             double Xmin, double Ymin, int Nbin, char name[]);
  void fill( int id, double xn );
  void plot();
};

