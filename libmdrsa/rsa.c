//
//  rsa.c
//  libmdrsa
//
//  Created by Mert Dümenci on 3/22/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#include "rsa.h"
#include "bnhelpers.h"

static int kMDRSAPrimeLength = 50;

void extendedEuclidean(const vU1024 *x, const vU1024 *y,
                       vU1024 *d, vU1024 *a, vU1024 *b) {
    if (bignum_isZero((vU1024 *)y)) {
        vU1024 one = bignum_from64(1);
        vU1024 zero = bignum_from64(0);
        
        *d = *x;
        *a = one;
        *b = zero;
        
        return;
    }
    
    vU1024 retD;
    vU1024 retA;
    vU1024 retB;
    
    vU1024 xMody;
    vU1024 xOverY;
    vU1024Divide(x, y, &xOverY, &xMody);
    
    extendedEuclidean(y, &xMody, &retD, &retA, &retB);
    
    if (bignum_isZero(&retB)) {
        *d = retD;
        *a = retA;
        *b = retB;
        
        return;
    }
    
    vU1024 aDelta;
    vU1024HalfMultiply(&xOverY, &retB, &aDelta);
    
    vU1024 newB;
    vU1024Sub(&retA, &aDelta, &newB);
    
    *d = retD;
    *a = retB;
    *b = newB;
}

void generateKeys(vU1024 *pubE, vU1024 *pubA, vU1024 *pubD) {
    vU1024 p = generatePrime(kMDRSAPrimeLength);
    vU1024 q = generatePrime(kMDRSAPrimeLength);
    vU1024 n;
    
    vU1024HalfMultiply(&p, &q, &n);
}
