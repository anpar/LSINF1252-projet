#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

/*
 * gcd(int a, int b) computes the greatest common
 * divisor of a and b using euclide's algorithm
 * recursively.
 */
double gcd(int a, int b)
{
        printf("Computing gcd  of %d and %d.\n", a, b);
        if(b == 0)
                return(a);
        else
                return(gcd(b, a % b));
}

/*
 * Used to test whether a integer is a 
 * perfect square or not.
 */
bool isPerfectSquare(int n) {
        int temp = sqrt(n);
        return(temp*temp == n);
}

bool isPrime(int n) {
        return(false);
}

/*
 * Shanks's square forms factorization algorithm.
 */
int SQUFOF(int N)
{
        if(isPerfectSquare(N)) {
                printf("Input is a square number!\n");
                N = sqrt(N);
        }
        
        if(isPrime(N)) {
                printf("Input is prime!\n");
                return(N);
        }

        if(N % 2 == 0) {
                return(2);
        }

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
                /*
                 * In some case, Q(i+1) = 1, which is a perfect
                 * square. But when Q(i+1) = 1, the algorithm
                 * doesn't returns a prime number (e.g, try with 111, then
                 * 1111, then 11111, etc). I solved
                 * this by adding the condition on Qnext, now it seems
                 * to work (try again with 1111, 11111, etc).
                 */
                if(Qnext != 1) {
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
}

/*
 * I find another bug (in fact, I don't know if it's
 * a bug or not). Try running this programme with 
 * 446744073, it returns 7053, which is a factor
 * of 446744073 but not a prime factor. 
 */
int main(int argc, const char *argv[])
{
        int n = atoi(argv[1]);
        printf("Computing prime factor of %d.\n", n);
        printf("Result of SQUFOF = %d.\n", SQUFOF(n));
        return(EXIT_SUCCESS);
}
