#ifndef _SQUFOF_H
#define _SQUFOF_H

/*
 * gcd(int a, int b) computes the greatest common
 * divisor of a and b using euclide's algorithm
 * recursively.
 */
double gcd(unsigned int, unsigned int);

/*
 * Used to test whether a integer is a 
 * perfect square or not.
 */

bool isPerfectSquare(unsigned int); 

/*
 * Used to test whether a number is
 * prime or not.
 */
bool isPrime(unsigned int);

/*
 * Shanks's square forms factorization algorithm.
 */
int SQUFOF(unsigned int);

#endif    
