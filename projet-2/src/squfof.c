#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <inttypes.h>
#include "squfof.h"

#define VERBOSE false
/* 
 * This macro requires c99.
 * Used to debug SQUFOF algorithm.
 */
#define verbose_printf(fmt, ...) \
                    do { if (VERBOSE) printf(fmt,## __VA_ARGS__); } while (0)

uint64_t gcd(uint64_t a, uint64_t b)
{
        //verbose_printf("Computing gcd  of %d and %d.\n", a, b);
        if(b == 0)
                return(a);
        else
                return(gcd(b, a % b));
}

bool isPerfectSquare(uint64_t n) 
{
        n = (long double) n;
        long double temp = sqrtl(n);
	// Try to fix the arithmetic exception caused by division by zero
	// in squfof
        return((temp*temp == n) && n != 0);
}

bool isPrime(uint64_t n) 
{
        if (n <= 3) {
                return n > 1;
        }
         
        if (n % 2 == 0 || n % 3 == 0) {
                return false;
        }
             
        for (unsigned short i = 5; i * i <= n; i += 6) {
                if (n % i == 0 || n % (i + 2) == 0) {
                        return false;
                }
        }
                 
        return true;
}

uint64_t SQUFOF(uint64_t N)
{
        N = (long double) N;
        /*
         * Note that by definition, the prime factorization
         * of 1 is an empty product (1 = 2^0*3^0*5^0...).
         */

        if(N == 1) {
                return(EXIT_FAILURE);
        }
        
        if(isPrime(N)) {
                verbose_printf("Input is prime!\n");
                return(N);
        }

        if(N % 2 == 0) {
                return(2);
        }

	long double P, Pprev, Q, Qnext, b, tmp, f;
        int i;
        int lim = 100;
	for(int k = 1;;k++) {
                verbose_printf("\nLet's try for k = %d.\n", k);
                P = floorl(sqrtl(k*N));
		Q = 1;
		Qnext = k*N - P*P;
		
		//verbose_printf("\t\t P(0)=%d \t Q(0)=%d \t Q(1)=%d\n", P, Q, Qnext);
                i = 1;
		while(!(isPerfectSquare(Qnext) && (i % 2 == 0))) {
	       		b = floorl((floorl(sqrtl(k*N)) + P)/Qnext);
			Pprev = P;        	
			P = b*Qnext - P;
			
			tmp = Qnext;
		        Qnext = Q + b*(Pprev - P);
		        Q = tmp;
		        
                        i++;
			//verbose_printf("P(i-1)=%d \t P(i)=%d \t Q(i)=%d \t Q(i+1)=%d\n",Pprev, P, Q, Qnext);              
		}
		
		//verbose_printf("Here, Q(i+1) is a perfect square\n");

		b = floorl((floorl(sqrtl(k*N)) - P)/sqrtl(Qnext));
		P = b*sqrtl(Qnext) + P;
		Q = sqrtl(Qnext);
		Qnext = (k*N - P*P)/Q;
		
		//verbose_printf("\t\t P(0)=%d \t Q(0)=%d \t Q(1)=%d\n", P, Q, Qnext);
                i = 0;
		while(P != Pprev && i < lim) {      
		       	b = floorl((floorl(sqrtl(k*N)) + P)/Qnext);
			Pprev = P;        	
			P = b*Qnext - P;

			tmp = Qnext;
		        Qnext = Q + b*(Pprev - P);
		        Q = tmp;
		        i++;

			//verbose_printf("P(i-1)=%d \t P(i)=%d \t Q(i)=%d \t Q(i+1)=%d\n",Pprev, P, Q, Qnext);    	        
		}
                
                if(i == lim) {
                        //verbose_printf("Failure with k = %d.\n", k);
                }
                else {
		        //verbose_printf("Here P(i) = P(i-1)\n");

		        f = gcd(N, P);
		        if(f != 1 && f != N) {
			        return(f);
		        }
                }
	}
}
