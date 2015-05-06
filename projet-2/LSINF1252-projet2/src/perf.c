#include <sys/time.h>
#include "perf.h"

double timeval_diff(struct timeval *t2, struct timeval *t1)
{
        double sec = t2->tv_sec - t1->tv_sec;
        double usec = t2->tv_usec - t1->tv_usec;
        if(usec >= 1000000) {
                sec++;
                usec = usec - 1000000;
        }

        double diff = sec + usec/1000000;
        return(diff);
}

