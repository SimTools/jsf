                                                                            /*
******************************************************************************
*                   =====================================                    */
                            class bases_scat                                /*
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
*       no_xbin   : number of x bins                                         *
*       x_dev     : x bin width                                              *
*       y_min     : y_min                                                    *
*       y_max     : y_max                                                    *
*       no_ybin   : number of y bins                                         *
*       y_dev     : y bin width                                              *
*      Buffer                                                                *
*       freq [ ][ ] : sum of Fi                                              *
*     For spring                                                             *
*       x_tag         : x-tag for spring                                     *
*       y_tag         : y-tag for spring                                     *
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
  int ID;
  int no_xbin, no_ybin;
  int x_tag, y_tag;
  float x_min, x_max, x_dev;
  float y_min, y_max, y_dev;
  char  name[64];
  float** freq;
                                                                             /*
 ------------------
  Private function
 ------------------                                                         */
  static void forder( float, float&, float&, int& );
                                                                            /*
=========                                                                    */
 public:                                                                    /*
=========
 ------------------
  Public function
 ------------------                                                         */
        bases_scat( ){ };
       ~bases_scat( );
  void  deallocate();
  void  bs_write( ofstream& out );
  void  bs_read( ifstream& in );
  int   getID() { return ID; };
  void  dh_init( int id, double Xmin, double Xmax, int Xbin, 
                         double Ymin, double Ymax, int Ybin, char title[] );
  void  dh_fill( int id, double x, double y, double fn, double wgt, int flag );
  void  dh_plot( );
  void  sh_reset();
  void  sh_update();
};

