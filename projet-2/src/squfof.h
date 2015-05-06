#ifndef _SQUFOF_H
#define _SQUFOF_H

/*
 * gcd(int a, int b) computes the greatest common
 * divisor of a and b using euclide's algorithm
 * recursively.
 */
uint64_t gcd(uint64_t, uint64_t);

/*
 * Used to test whether a integer is a 
 * perfect square or not.
 */

bool isPerfectSquare(uint64_t); 

/*
 * Used to test whether a number is
 * prime or not.
 */
bool isPrime(uint64_t);

/*
 * Shanks's square forms factorization algorithm.
 */
uint64_t SQUFOF(uint64_t);

#endif    
