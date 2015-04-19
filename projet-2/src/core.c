#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

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
 * Used to test whether a integer is a 
 * perfect square or not.
 */
bool isPerfectSquare(int n) {
        return(sqrt(n) == floor(sqrt(n)));
}

/*
 * Shanks's square forms factorization algorithm.
 * Pre-conditions : N is not a perfect square
 * and is not prime.
 * Return value : a non-trivial factor of N.
 */
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
        return(EXIT_SUCCESS);
}
