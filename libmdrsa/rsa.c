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
static int possibleEs[] = {3, 5, 7, 11, 13, 17};

void extendedEuclidean(const vU1024 *x, const vU1024 *y,
                       vU1024 *d, vS1024 *a, vS1024 *b) {
    if (bignum_isZero((vU1024 *)y)) {
        vU1024 one = bignum_from64(1);
        vU1024 zero = bignum_from64(0);
        
        *d = *x;
        *a = bignum_signed(&one);
        *b = bignum_signed(&zero);
        
        return;
    }
    
    vU1024 retD;
    vS1024 retA;
    vS1024 retB;
    
    vU1024 xMody;
    vU1024 xOverY;
    vU1024Divide(x, y, &xOverY, &xMody);
    
    extendedEuclidean(y, &xMody, &retD, &retA, &retB);
    
    vS1024 aDelta;
    vS1024 signedXOverY = bignum_signed(&xOverY);
    vS1024HalfMultiply(&signedXOverY, &retB, &aDelta);
    
    vS1024 newB;
    vS1024Sub((vS1024 *)&retA, (vS1024 *)&aDelta, &newB);
    
    *d = retD;
    *a = retB;
    *b = newB;
}

void generateKeys(vU1024 *pubE, vU1024 *pubN, vS1024 *privD) {
    vU1024 p = findPrime(kMDRSAPrimeLength);
    vU1024 q = findPrime(kMDRSAPrimeLength);
    vU1024 n;
    
    vU1024HalfMultiply(&p, &q, &n);
    
    vU1024 one = bignum_from64(1);
    vU1024 pMinus1;
    vU1024 qMinus1;
    vU1024 pq;
    
    vU1024Sub(&p, &one, &pMinus1);
    vU1024Sub(&q, &one, &qMinus1);
    vU1024HalfMultiply(&pMinus1, &qMinus1, &pq);
    
    vU1024 e;
    vU1024 divisor;
    vS1024 a;
    vS1024 b;
    
    int eSearchIndex = 0;
    
    while (!bignum_equal(&divisor, &one)) {
        if (eSearchIndex >= (sizeof(possibleEs) / sizeof(int))) {
            printf("Can't generate keys");
            exit(-1);
        }
        
        e = bignum_from64(possibleEs[eSearchIndex]);
        extendedEuclidean(&pq, &e, &divisor, &a, &b);
        eSearchIndex++;
    }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconditional-uninitialized"
    *pubE = e;
    *pubN = n;
    *privD = a;
#pragma clang diagnostic pop
    
#ifdef DEBUG
    vS1024 signedPQ = bignum_signed(&pq);
    vS1024 signedE = bignum_signed(&e);
    
    vS1024 aTimesPQ;
    vS1024HalfMultiply(&a, &signedPQ, &aTimesPQ);
    
    vS1024 bTimesE;
    vS1024HalfMultiply(&b, &signedE, &bTimesE);
    
    vS1024 sum;
    vS1024Add(&aTimesPQ, &bTimesE, &sum);
    
    vS1024 signedOne = bignum_signedFrom64(1);
    if (!bignum_signedEqual(&sum, &signedOne)) {
        printf("DEBUG: GENERATED KEYS INVALID, DOESN'T SATISFY BEZOUT'S"
               "IDENTITY.");
        exit(-1);
    }
    
    printf("DEBUG: Valid keys!\n");
#endif
}
