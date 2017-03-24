//
//  prime.h
//  libmdrsa
//
//  Created by Mert Dümenci on 3/17/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#ifndef prime_h
#define prime_h

#include <stdio.h>
#include <Accelerate/Accelerate.h>

/*
    Fast modular binary exponentiation.
 
    Mods at every step of the exponentiation, thus allows the computation of
    really big (150-digit big) powers. Uses Accelerate for highly performant,
    vectorized arithmetic operations.
 */
void MDRSAFastModuloPow(vU1024 *base, vU1024 *power, vU1024 *modulo, vU1024 *result);

/*
    Probabilistically checks if the given number is a prime (or a Carmichael 
    number) by checking if Fermat's Little Theorem holds for k * log10(n) a's.
    (where FLT says a^(p - 1) ≡ 1 (mod p) if p is a prime number). [Fermat
    primality test]
 
    Built on the fact that if a^(p - 1) ≠ 1 (mod p) for an a, then it will be
    ≠ 1 (mod p) for half the possible a's. By randomly testing out a's,
    we reduce the possibility of p being a composite number (Fermat pseudoprime)
    by 1/2 in every iteration. If we hit an a such that a^(p - 1) ≠ 1 (mod p),
    then we know for sure that p is not prime. If we don't hit such an a in
    computationally feasible, 'enough' number of iterations, then the
    possibility of p being a composite number is so small that we can assume
    it's a prime.
 
    There are better primality tests--this is not meant to be used in a real
    project. I can actually understand how this test works, and I wanted to
    implement it myself.
 */
bool MDRSAFermatPrimalityTest(vU1024 *candidate, int numDigits);

/*
    Finds [Fermat probable] primes in the given number of digits
    (sort of[1]).
 
    Since primes are roughly log(n) numbers apart from each other, and log(n)
    is the number of digits of n, we generate k * numDigits random numbers, and
    test them for primality. We return if we hit a prime.
 
    [1]: `MDRSABignumRand` fills out log_2(10^numDigits) bits with random data.
    Chances are the significant bits are going to be non-zero, hence the number
    will be ~numDigits digits.
*/
vU1024 MDRSAFindPrime(int numDigits);

#endif /* prime_h */
