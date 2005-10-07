#include <time.h>

void idate_(int idtemp[])
{
  struct tm q;
  time_t    tp;

  time(&tp);
  q = *localtime(&tp);
  idtemp[0] = q.tm_year + 1900;
  idtemp[1] = q.tm_mon + 1;
  idtemp[2] = q.tm_mday;
}

void itime_(int idtemp[])
{
  struct tm q;
  time_t    tp;

  time(&tp);
  q = *localtime(&tp);
  idtemp[0] = q.tm_hour;
  idtemp[1] = q.tm_min;
  idtemp[2] = q.tm_sec;
}
