#ifndef __bases_ran1__
#define __bases_ran1__

#define NTAB 32

class bases_ran1 
{
 protected:
  long fSeed;
  long fiy;
  long fiv[NTAB];
 
 public:
  bases_ran1();
  virtual ~bases_ran1();
  
  double Rndm();

  void SetSeed(long seed);
  void SetSeed2(long iy, long iv[]);
  
  long  GetSeed(){ return fSeed;}
  void GetSeed2(long &iy, long iv[]);

};

#endif

