#include <time.h>
/* long	iutime() */
/*main()*/
void uxdate(int &year, int &mon,
            int &day, int &hour, int &min)
{
	struct	tm q;
/*
        struct  tm *localtime( );
*/
        time_t  tp;
/*
        time_t  mktime();
        time_t  time( );
        char    *ctime( );
*/
	char    *date;
        

        time(&tp);
        date = ctime(&tp);
        q = *localtime(&tp);
        year = q.tm_year + 1900;
        mon  = q.tm_mon + 1;
        day  = q.tm_mday;
        hour = q.tm_hour;
        min  = q.tm_min;

	return;
}
