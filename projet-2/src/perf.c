#include <sys/time.h>
#include "perf.h"

double timeval_diff(struct timeval *t2, struct timeval *t1)
{
        double diff = (t2->tv_sec + t2->tv_usec/1000000) - (t1->tv_sec + t1->tv_usec/1000000);
        return (diff);
}

