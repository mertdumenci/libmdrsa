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

void generateKeys(MDRSAKeyPair *keyPair) {
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
    vS1024 signedPQ = bignum_signed(&pq);
    
    // Add k * pq to b to make it positive.
    // (Observe that b (mod pq) = b + k * pq (mod pq).)
    while (bignum_signedIsNegative(&b)) {
        vS1024 newB;
        vS1024Add(&b, &signedPQ, &newB);
        b = newB;
    }
    
    vU1024 unsignedB = bignum_unsigned(&b);
    
    MDRSAPublicKey public = { .e = e, .n = n };
    MDRSAPrivateKey private = { .d = unsignedB };
    
    *keyPair = (MDRSAKeyPair){ .publicKey = public, .privateKey = private };
#pragma clang diagnostic pop
}

vU1024 MDRSAEncrypt(vU1024 *payload, MDRSAPublicKey *publicKey) {
    vU1024 encryptedPayload;
    fastModuloPow(payload, &(publicKey->e), &(publicKey->n), &encryptedPayload);
    
    return encryptedPayload;
}

vU1024 MDRSADecrypt(vU1024 *encryptedPayload, MDRSAKeyPair *keyPair) {
    vU1024 decryptedPayload;
    fastModuloPow(encryptedPayload, &(keyPair->privateKey.d),
                  &(keyPair->publicKey.n), &decryptedPayload);
    
    
    return decryptedPayload;
}
