//
//  prime.c
//  libmdrsa
//
//  Created by Mert Dümenci on 3/17/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#include "prime.h"
#include "bnhelpers.h"

void fastModuloPow(vU1024 *base, vU1024 *power, vU1024 *modulo, vU1024 *result) {
    bool willDoModulo = !bignum_isZero(modulo);
    vU1024 internalResult = bignum_from64(1);
    vU1024 internalPower;
    vU1024 internalBase;
    
    memcpy(&internalPower, power, sizeof(vU1024));
    memcpy(&internalBase, base, sizeof(vU1024));
    
    while (!bignum_isZero(&internalPower)) {
        if (bignum_isOdd(&internalPower)) {
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

bool fermatTest(vU1024 *a, vU1024 *p) {
    vU1024 result;
    vU1024 power;
    
    vU1024 one = bignum_from64(1);
    vU1024Sub(p, &one, &power); // power = p - 1
    
    fastModuloPow(a, &power, p, &result);
    
    return bignum_equal(&result, &one);
}

bool isPrime(vU1024 *candidate, int numDigits) {
    int testCounter = numDigits * 5;
    
    while (testCounter > 0) {
        vU1024 random = bignum_rand(numDigits - 1); // stupid hack, but so many digits I think we'll be fine
        if (!fermatTest(&random, candidate)) {
            return false;
        }
        
        testCounter -= 1;
    }
    
    return true;
}

vU1024 generatePrime(int numDigits) {
    int testCounter = numDigits * (numDigits / 2);
    
    while (testCounter > 0) {
        vU1024 candidate = bignum_rand(numDigits);
        
        if (isPrime(&candidate, numDigits)) {
            return candidate;
        }
        
        testCounter -= 1;
    }
    
    printf("Fatal error, cannot generate prime");
    exit(-1);
}
