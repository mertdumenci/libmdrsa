//
//  prime.c
//  libmdrsa
//
//  Created by Mert Dümenci on 3/17/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#include "prime.h"
#include "bignum.h"

/*
    Applies the Fermat probable primality test with a given base `a` and
    power `p`.

    Returns true if a^(p - 1) ≡ 1 (mod p). (p is a Fermat probable prime).

    This function by itself should not be used for primality testing because of
    the high occurence of Fermat pseudoprimes--see `MDRSAFermatPrimalityTest`
    for an algorithm that makes use of this to test for primality with high
    confidence.
 */
bool _MDRSAFermatPrimalityTest(vU1024 *a, vU1024 *p) {
static bool _MDRSAFermatPrimalityTest(vU1024 *a, vU1024 *p) {
    vU1024 result;
    vU1024 power;

    vU1024 one = MDRSABignumFromInteger(1);
    vU1024Sub(p, &one, &power); // power = p - 1

    MDRSAFastModuloPow(a, &power, p, &result);

    return MDRSABignumEqual(&result, &one);
}

void MDRSAFastModuloPow(vU1024 *base, vU1024 *power,
        vU1024 *modulo, vU1024 *result) {
    bool willDoModulo = (modulo == NULL) ? false : !MDRSABignumIsZero(modulo);
    vU1024 internalResult = MDRSABignumFromInteger(1);
    vU1024 internalPower;
    vU1024 internalBase;
    
    memcpy(&internalPower, power, sizeof(vU1024));
    memcpy(&internalBase, base, sizeof(vU1024));
    
    while (!MDRSABignumIsZero(&internalPower)) {
        if (MDRSABignumIsOdd(&internalPower)) {
            vU1024 newResult;
            vU1024HalfMultiply(&internalResult, &internalBase, &newResult);
            internalResult = newResult;
            
            if (willDoModulo) {
                vU1024 newResult;
                vU1024Mod(&internalResult, modulo, &newResult);
                internalResult = newResult;
            }
        }
        
        vU1024 newPower;
        vLR1024Shift(&internalPower, 1, &newPower);
        internalPower = newPower;
        
        vU1024 newBase;
        vU1024HalfMultiply(&internalBase, &internalBase, &newBase);
        internalBase = newBase;
        
        if (willDoModulo) {
            vU1024 newBase;
            vU1024Mod(&internalBase, modulo, &newBase);
            internalBase = newBase;
        }
    }
    
    memcpy(result, &internalResult, sizeof(vU1024));
}

bool MDRSAFermatPrimalityTest(vU1024 *candidate, int numDigits) {
    int testCounter = numDigits * 5;
    
    while (testCounter > 0) {
        // TODO: Rework the random number generator to allow upper boundaries.
        //
        // We know that a number of `numDigits - 1` digits will be less than `p`,
        // thus we generate a random number of `numDigits - 1` digits.
        vU1024 randomA = MDRSABignumRand(numDigits - 1);
        if (!_MDRSAFermatPrimalityTest(&randomA, candidate)) {
            return false;
        }
        
        testCounter -= 1;
    }
    
    return true;
}

vU1024 MDRSAPrimeBound(int numDigits) {
    // The Prime Number Theorem tells us that there's approximately n/log(n)
    // primes up to n. If our random function is truly
    // uniform, if we try >= n/log(n) numbers, we're going to hit a prime.
    vU1024 ten = MDRSABignumFromInteger(10);
    vU1024 power = MDRSABignumFromInteger(numDigits);
    vU1024 max;
    MDRSAFastModuloPow(&ten, &power, NULL, &max);
    
    // Finding ln(n) from log_10(n)
    vU1024 lnN = MDRSABignumFromInteger((int)floor(numDigits / log10(M_E)));
    
    vU1024 primeBound;
    vU1024 remainder;
    vU1024Divide(&max, &lnN, &primeBound, &remainder);
    
    return primeBound;
}

vU1024 MDRSAFindPrime(int numDigits) {
    vU1024 primeBound = MDRSAPrimeBound(numDigits);
    
    while (testCounter > 0) {
        vU1024 candidate = MDRSABignumRand(numDigits);
        
        if (MDRSAFermatPrimalityTest(&candidate, numDigits)) {
            return candidate;
        }
        
        testCounter -= 1;
    }
    
    printf("Fatal error, cannot generate prime");
    exit(-1);
}
