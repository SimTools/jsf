#include <sys/times.h>
#include <time.h>
#include <unistd.h>
/*main()*/
float	uxtime() 
{
	struct	tms q;
	long 	t;


	times(&q);
        t = q.tms_utime;

	return (float) t/ (float) CLK_TCK;
}
