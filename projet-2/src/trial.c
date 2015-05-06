#include "trial.h"

uint64_t trial_division(uint64_t n) 
{
	if(n == 2)
		return IS_PRIME;

        uint64_t sqrt_n = (uint64_t) ceill(sqrtl((long double) n));
        uint64_t x;
        for(x = 2; x <= sqrt_n; x++) {
                if((n % x) == 0) {
                        return x;
                }
        }

        return IS_PRIME;
}
