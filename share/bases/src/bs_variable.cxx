#include "bs_variable.h"

                                                                            /*
******************************************************************************
*              ==================================                            */
                void bs_variable :: defineVariable
       ( double& v, double low, double high, int wld, int grd )             /*
*              ==================================                            *
******************************************************************************
* ( Purpose )                                                                *
*   To save the nformation of integral variable.                             *
******************************************************************************/
{
  variable = &v;
  lower    = low;
  upper    = high;
  wild     = wld;
  grid     = grd;
}
