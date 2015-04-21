#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#define VERBOSE false
// This macro requires c99.
#define verbose_printf(fmt, ...) \
                    do { if (VERBOSE) printf(fmt,## __VA_ARGS__); } while (0)

/*
 * gcd(int a, int b) computes the greatest common
 * divisor of a and b using euclide's algorithm
 * recursively.
 */
double gcd(unsigned int a, unsigned int b)
{
        verbose_printf("Computing gcd  of %d and %d.\n", a, b);
        if(b == 0)
                return(a);
        else
                return(gcd(b, a % b));
}

/*
 * Used to test whether a integer is a 
 * perfect square or not.
 */
bool isPerfectSquare(unsigned int n) 
{
        int temp = sqrt(n);
        return(temp*temp == n);
}

/*
 * Used to test whether a number is
 * prime or not.
 * Shamefully inspired from Wikipedia. 
 */
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

/*
 * Shanks's square forms factorization algorithm.
 */
int SQUFOF(unsigned int N)
{
        /*
         * Note that by definition, the prime factorization
         * of 1 is an empty product (1 = 2^0*3^0*5^0...).
         */
        if(N == 1) {
                return(EXIT_FAILURE);
        }

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

int main(int argc, const char *argv[])
{
        unsigned int n = atol(argv[1]);
        printf("Computing non-trivial factor of %d.\n", n);
        printf("Result of SQUFOF = %d.\n", SQUFOF(n));
        return(EXIT_SUCCESS);
}

