#include "trial.h"

/*
 * Retourne le plus petit facteur premier de n si
 * n n'est pas premier, IS_PRIME (= 0) sinon.
 */
uint64_t trial_division(uint64_t n) 
{
	if(n == 2)
		return IS_PRIME;
	
	long double n_ld = (long double) n;
	uint64_t sqrt_n = (uint64_t) ceill(sqrtl(n_ld));
        
        uint64_t x;
        for(x = 2; x <= sqrt_n; x++) {
                if((n % x) == 0) {
                        return x;
                }
        }

        return IS_PRIME;
}
