#include "bases_ran1.h"

#define IA 16807
#define IM 2147483647
#define AM ( 1.0/IM )
#define IQ 127773
#define IR 2836
// #define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RUMX (1.0-EPS)


//___________________________________________________
bases_ran1::bases_ran1()
{
  // Random number generator for Bases
  // Initial seed should be negative, otherwise random number sequence
  // does not change.

  fSeed=-12345;
  fiy=0;
}

//___________________________________________________
bases_ran1::~bases_ran1()
{
}

//___________________________________________________
double bases_ran1::Rndm()
{
  // Generate uniform random number between 0 to 1.

  int j;
  long k;
  //  static long iy = 0;
  //  static long iv[NTAB];
  double temp;

  if( fSeed <= 0 || !fiy )
    {
      if( -fSeed < 1 )  fSeed = 1;
      else             fSeed = -fSeed;

      for( j = NTAB+7; j >= 0; j-- )
         {
           k    = fSeed/IQ;
           fSeed = IA*( fSeed - k*IQ ) - IR*k;
           if( fSeed < 0 ) fSeed += IM;
           if( j < NTAB ) fiv[j] = fSeed;
          }
      fiy = fiv[0];
     }
  k    = fSeed/IQ;
  fSeed = IA*( fSeed - k*IQ ) - IR*k;

  if( fSeed < 0 ) fSeed += IM;
  j     = fiy/NDIV;
  fiy    = fiv[j];
  fiv[j] = fSeed;
  if( ( temp = AM*fiy ) > RUMX ) return RUMX;
  else                          return temp;
}

//___________________________________________________
void bases_ran1::SetSeed(long seed)
{
  fSeed=seed;
}

//___________________________________________________
void bases_ran1::SetSeed2(long iy, long iv[])
{
  fiy=iy;
  int i;
  for(i=0;i<NTAB;i++){ fiv[i]=iv[i]; }
}

//___________________________________________________
void bases_ran1::GetSeed2(long &iy, long iv[])
{
  iy=fiy;
  int i;
  for(i=0;i<NTAB;i++){ iv[i]=fiv[i]; }
}
