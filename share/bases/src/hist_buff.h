#include "hist_bin.h"

  union hist_buff {
    int   ibuf[ BINSIZE ];
    float buff[ BINSIZE ];
   };
