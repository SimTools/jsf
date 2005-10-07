#include <sys/times.h>
#if !defined(__DARWIN__)
#include <time.h>
#else
#include <machine/limits.h>
#endif
#include <unistd.h>
/*main()*/
float	uxtime() 
{
	struct	tms q;
	long 	t;


	times(&q);
        t = q.tms_utime;
#if !defined __STRICT_ANSI__ && !defined __USE_XOPEN2K
	return (float) t/ (float) CLK_TCK;
#else
	return (float) t/ (float) CLOCKS_PER_SEC;
#endif
}
