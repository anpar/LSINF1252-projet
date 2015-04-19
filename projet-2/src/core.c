#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

/*
 * DRAFT - à nettoyer.
 */

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
 * Others works partially.
 */
int pollard_rho3(unsigned int n)
{
        if(n % 2 == 0)
                return(2);

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

void pollard_rho(unsigned int n)
{
        int i = 1;
        srand(time(NULL) + getpid());
        int x = rand() % n;
        int y = x;
        int k = 2;
        int d;
        while(true) {
                i = i + 1;
                x = (int)(pow(x, 2) - 1) % n;
                d = gcd(abs(y - x), n);
                if(d != 1 && d != n) {
                        printf("%d\n", d);
                }
                else if (i == k) {
                        y = x;
                        k = 2*k;
                }
        }
}

int main(int argc, const char *argv[])
{
        //pollard_rho(atoi(argv[1]));
        //pollard_rho2(atoi(argv[1]));
        printf("result = %d.\n", pollard_rho3(atoi(argv[1])));
        return(EXIT_SUCCESS);
}
