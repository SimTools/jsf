#if defined(CERNLIB_WINNT)
  #define pythia6_addressc PYTHIA^_ADDRESSC
  #define pythia6_addressf PYTHIA^_ADDRESSF
  #define pythia6_addressi PYTHIA^_ADDRESSI
  #define pythia6_addressd PYTHIA^_ADDRESSD
  #define type_of_call _stdcall
#else
  #define pythia6_addressc pythia6_addressc_
  #define pythia6_addressf pythia6_addressf_
  #define pythia6_addressi pythia6_addressi_
  #define pythia6_addressd pythia6_addressd_
  #define type_of_call
#endif

char* type_of_call pythia6_addressc(char *arg)
{
  return arg;
}
int*  type_of_call pythia6_addressi(int  *arg)
{
  return arg;
}
float* type_of_call pythia6_addressf(float *arg)
{
  return arg;
}
double* type_of_call pythia6_addressd(double *arg)
{
  return arg;
}
