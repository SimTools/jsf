//
//  ROOT functions prepared for CERNIB functions.
//
#include "TMatrixD.h"

extern "C" void dsinv_(int *norder, double *array, int *mxdim, int *nret)
{
   TMatrixD m(*norder, *norder);
   for(int i=0;i<*norder;i++) {
	for(int j=0;j<*norder;j++) {
		m(i,j) = *(array+*mxdim*i+j);
   	}
   }
   m.Invert();

   for(int i=0;i<*norder;i++) {
	for(int j=0;j<*norder;j++) {
	   *(array+*mxdim*i+j)=m(i,j);
        }
   }
   nret=0;
}

