                                                                            /*
******************************************************************************
*                   =====================================                    */
                          class bs_variable                                 /*
*                   =====================================                    *
******************************************************************************
* ( Purpose )                                                                *
*    To keep the information of an integral variable.                        *
* ( attributes )                                                             *
*    double *variable : pointer of the integral variable                     *
*    double lower     : the lower limit of integral variable                 *
*    double upper     : the upper limit of integral variable                 *
*    int    wild      : type = ( 0 / 1 ) = ( "mild" / "wild" )               *
*    int    grid      : tune = ( 0 / 1 ) = ( without grid tune / with tune ) *
* ( Author )                                                                 *
*    Setsuya Kawabata,   july  '99 at KEK                                    *
******************************************************************************/
{                                                                           /*
 ------------------
  private members
 ------------------                                                          */
  double *variable;
  double lower, upper;
  int    wild;
  int    grid;
                                                                            /*
=========                                                                    */
 public:                                                                    /*
=========
 ------------------
  Public function
 ------------------                                                         */
  bs_variable( ){ };
  void defineVariable( double &v, double low, double up, int wld, int grd );
  double  Variable( ) { return *variable; };
  void    Variable( double var ) { *variable = var; };
  double& getVariable( )         { return *variable; };
  double  Lower( )    { return lower; };
  double  Upper( )    { return upper; };
  int     Wild ( )    { return wild;  };
  int     Grid ( )    { return grid;  };
};

