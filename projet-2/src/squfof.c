#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "squfof.h"

#define VERBOSE false
/* 
 * This macro requires c99.
 * Used to debug SQUFOF algorithm.
 */
#define verbose_printf(fmt, ...) \
                    do { if (VERBOSE) printf(fmt,## __VA_ARGS__); } while (0)

double gcd(unsigned int a, unsigned int b)
{
        verbose_printf("Computing gcd  of %d and %d.\n", a, b);
        if(b == 0)
                return(a);
        else
                return(gcd(b, a % b));
}

bool isPerfectSquare(unsigned int n) 
{
        int temp = sqrt(n);
	// Try to fix the arithmetic exception caused by division by zero
	// in squfof
        return((temp*temp == n) && n != 0);
}

bool isPrime(unsigned int n) 
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

int SQUFOF(unsigned int N)
{
        /*
         * Note that by definition, the prime factorization
         * of 1 is an empty product (1 = 2^0*3^0*5^0...).
         */
        if(N == 1) {
                return(EXIT_FAILURE);
        }

        // FIX : Il y a une faille ici ! En effet si on donne 9 en entré à SQUFOF,
        // il va ici dire N = 3, et caluler le facteur premier de 3 (3). Or avec
        // 9 on aurait du avoir 3 et puis encore 3 (et donc deux fois 3, 3 n'est plus
        // unique ! Il faudrait faire cette vérification dans prime_factorizer selon moi!
        if(isPerfectSquare(N)) {
                verbose_printf("Input is a square number!\n");
                N = sqrt(N);
        }
        
        if(isPrime(N)) {
                verbose_printf("Input is prime!\n");
                return(N);
        }

        if(N % 2 == 0) {
                return(2);
        }

	int P, Pprev, Q, Qnext, b, tmp, f, i;
        int lim = 100;
	for(int k = 1;;k++) {
                verbose_printf("\nLet's try for k = %d.\n", k);
                P = floor(sqrt(k*N));
		Q = 1;
		Qnext = k*N - P*P;
		
		verbose_printf("\t\t P(0)=%d \t Q(0)=%d \t Q(1)=%d\n", P, Q, Qnext);

		while(!isPerfectSquare(Qnext)) {
	       		b = floor((floor(sqrt(k*N)) + P)/Qnext);
			Pprev = P;        	
			P = b*Qnext - P;
			
			tmp = Qnext;
		        Qnext = Q + b*(Pprev - P);
		        Q = tmp;
		
			verbose_printf("P(i-1)=%d \t P(i)=%d \t Q(i)=%d \t Q(i+1)=%d\n",Pprev, P, Q, Qnext);              
		}
		
		verbose_printf("Here, Q(i+1) is a perfect square\n");

		b = floor((floor(sqrt(k*N)) - P)/sqrt(Qnext));
		P = b*sqrt(Qnext) + P;
		Q = sqrt(Qnext);
		Qnext = (k*N - P*P)/Q;
		
		verbose_printf("\t\t P(0)=%d \t Q(0)=%d \t Q(1)=%d\n", P, Q, Qnext);
                i = 0;
		while(P != Pprev && i < lim) {      
		       	b = floor((floor(sqrt(k*N)) + P)/Qnext);
			Pprev = P;        	
			P = b*Qnext - P;

			tmp = Qnext;
		        Qnext = Q + b*(Pprev - P);
		        Q = tmp;
		        i++;

			verbose_printf("P(i-1)=%d \t P(i)=%d \t Q(i)=%d \t Q(i+1)=%d\n",Pprev, P, Q, Qnext);    	        
		}
                
                if(i == lim) {
                        verbose_printf("Failure with k = %d.\n", k);
                }
                else {
		        verbose_printf("Here P(i) = P(i-1)\n");

		        f = gcd(N, P);
		        if(f != 1 && f != N) {
			        return(f);
		        }
                }
	}
}
