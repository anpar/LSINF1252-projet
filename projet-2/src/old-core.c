#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

/*
 * gcd(int a, int b) computes the greatest common
 * divisor of a and b using euclide's algorithm
 * recursively.
 */
int gcd(int a, int b)
{
        if(b == 0)
                return(a);
        else
                return(gcd(b, a % b));
}

/*
 * Polynomial function used in the rho-Pollard
 * algorithm.
 */
int g(int x, int n) 
{
        return (x*x + 1) % n;
}

void pollard_rho2(unsigned int n)
{
        int x = 2;
        int y = 2;
        int d = 1;
        
        while(d == 1) {
                x = g(x,n);
                y = g(g(y,n), n);
                d = gcd(abs(x-y), n);
                
                if(d == n)
                        printf("failure\n");
                else
                        printf("%d\n", d);
        }
}

/*
 * This version of rho-Pollard seems to work.
 * Inspired from the pseudo-code given here:
 * http://en.wikipedia.org/wiki/Pollard%27s_rho_algorithm#Algorithm
 * 
 * The version with rand works but is non-deterministic.
 * This version seems to work properly but
 * more test have to be done.
 * EDIT AFTER MORE TESTS: it doesn't work properly without
 * rand. Try for example $ ./core 169.
 * We have to find a way to handle non-determinism.
 */
int pollard_rho3(unsigned int n)
{
        srand(time(NULL) + getpid());
        int x = rand() % n;
        int c = rand() % n;
        int y = x;
        int d = 1;
        while(d == 1) {
                x = (x*x + c) % n;
                y = (y*y + c) % n;
                y = (y*y + c) % n;
                d = gcd(abs(x-y), n);

                if(d == n)
                        return(d);
        }

        return(d);
}

int pollard_rho(unsigned int n)
{
        int i = 1;
        srand(time(NULL) + getpid());
        int x = rand() % n;
        int y = x;
        int k = 2;
        int d = 1;
        while(true) {
                i = i + 1;
                x = (x*x - 1) % n;
                d = gcd(abs(y - x), n);
                if(d != 1 && d != n) {
                        return(d);
                }
                else if (i == k) {
                        y = x;
                        k = 2*k;
                }
        }
}

/*
 * Pre-conditions: n is not a perfect square
 * and n is not prime.
 * Return value: a non-trivial factor of n.
 */
bool isPerfectSquare(int n) {
        return(sqrt(n) == floor(sqrt(n)));
}

int SQUFOF(int N)
{
	int P, Pprev, Q, Qnext, b, tmp;
	for(int k = 1;;k++) {
		P = floor(sqrt(k*N));
		Q = 1;
		Qnext = k*N - P*P;
		
		printf("\t\t P(0)=%d \t Q(0)=%d \t Q(1)=%d\n", P, Q, Qnext);

		while(!isPerfectSquare(Qnext)) {
	       		b = floor((floor(sqrt(k*N)) + P)/Qnext);
			Pprev = P;        	
			P = b*Qnext - P;
			
			tmp = Qnext;
		        Qnext = Q + b*(Pprev - P);
		        Q = tmp;
		
			printf("P(i-1)=%d \t P(i)=%d \t Q(i)=%d \t Q(i+1)=%d\n",Pprev, P, Q, Qnext);              
		}
		
		printf("Here, Q(i+1) is a perfect square\n");

		b = floor((floor(sqrt(k*N)) - P)/sqrt(Qnext));
		P = b*sqrt(Qnext) + P;
		Q = sqrt(Qnext);
		Qnext = (k*N - P*P)/Q;
		
		printf("\t\t P(0)=%d \t Q(0)=%d \t Q(1)=%d\n", P, Q, Qnext);

		while(P != Pprev) {      
		       	b = floor((floor(sqrt(k*N)) + P)/Qnext);
			Pprev = P;        	
			P = b*Qnext - P;

			tmp = Qnext;
		        Qnext = Q + b*(Pprev - P);
		        Q = tmp;
		
			printf("P(i-1)=%d \t P(i)=%d \t Q(i)=%d \t Q(i+1)=%d\n",Pprev, P, Q, Qnext);    	        
		}

		printf("P(i)=%d\n",Pprev);
		printf("Here P(i) = P(i-1)\n");

		int f = gcd(N, P);
		if(f != 1 && f != N) {
			return(f);
		}
	}
}

int main(int argc, const char *argv[])
{
        printf("Result of SQUFOF = %d.\n", SQUFOF(atoi(argv[1])));
        //printf("result = %d.\n", pollard_rho(atoi(argv[1])));
        //pollard_rho2(atoi(argv[1]));
        //printf("result = %d.\n", pollard_rho3(atoi(argv[1])));
        return(EXIT_SUCCESS);
}
