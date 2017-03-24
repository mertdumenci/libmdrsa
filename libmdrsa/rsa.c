//
//  rsa.c
//  libmdrsa
//
//  Created by Mert Dümenci on 3/22/17.
//  Copyright © 2017 Mert Dümenci. All rights reserved.
//

#include "rsa.h"
#include "bignum.h"

static int kMDRSAPrimeLength = 50;
static int possibleEs[] = {3, 5, 7, 11, 13, 17};

void _MDRSAExtendedEuclidean(const vU1024 *x, const vU1024 *y,
                       vU1024 *d, vS1024 *a, vS1024 *b) {
    if (MDRSABignumIsZero((vU1024 *)y)) {
        vU1024 one = MDRSABignumFromInteger(1);
        vU1024 zero = MDRSABignumFromInteger(0);
        
        *d = *x;
        *a = MDRSABignumCastSigned(&one);
        *b = MDRSABignumCastSigned(&zero);
        
        return;
    }
    
    vU1024 retD;
    vS1024 retA;
    vS1024 retB;
    
    vU1024 xMody;
    vU1024 xOverY;
    vU1024Divide(x, y, &xOverY, &xMody);
    
    _MDRSAExtendedEuclidean(y, &xMody, &retD, &retA, &retB);
    
    vS1024 aDelta;
    vS1024 signedXOverY = MDRSABignumCastSigned(&xOverY);
    vS1024HalfMultiply(&signedXOverY, &retB, &aDelta);
    
    vS1024 newB;
    vS1024Sub((vS1024 *)&retA, (vS1024 *)&aDelta, &newB);
    
    *d = retD;
    *a = retB;
    *b = newB;
}

void MDRSAGenerateKeys(MDRSAKeyPair *keyPair) {
    vU1024 p = MDRSAFindPrime(kMDRSAPrimeLength);
    vU1024 q = MDRSAFindPrime(kMDRSAPrimeLength);
    vU1024 n;
    
    vU1024HalfMultiply(&p, &q, &n);
    
    vU1024 one = MDRSABignumFromInteger(1);
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
    
    while (!MDRSABignumEqual(&divisor, &one)) {
        if (eSearchIndex >= (sizeof(possibleEs) / sizeof(int))) {
            printf("Can't generate keys");
            exit(-1);
        }
        
        e = MDRSABignumFromInteger(possibleEs[eSearchIndex]);
        _MDRSAExtendedEuclidean(&pq, &e, &divisor, &a, &b);
        eSearchIndex++;
    }
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wconditional-uninitialized"
    vS1024 signedPQ = MDRSABignumCastSigned(&pq);
    
    // Add k * pq to b to make it positive.
    // (Observe that b (mod pq) = b + k * pq (mod pq).)
    while (MDRSABignumSignedIsNegative(&b)) {
        vS1024 newB;
        vS1024Add(&b, &signedPQ, &newB);
        b = newB;
    }
    
    vU1024 unsignedB = MDRSABignumCastUnsigned(&b);
    
    MDRSAPublicKey public = { .e = e, .n = n };
    MDRSAPrivateKey private = { .d = unsignedB };
    
    *keyPair = (MDRSAKeyPair){ .publicKey = public, .privateKey = private };
#pragma clang diagnostic pop
}

vU1024 MDRSAEncrypt(vU1024 *payload, MDRSAPublicKey *publicKey) {
    vU1024 encryptedPayload;
    MDRSAFastModuloPow(payload, &(publicKey->e), &(publicKey->n), &encryptedPayload);
    
    return encryptedPayload;
}

vU1024 MDRSADecrypt(vU1024 *encryptedPayload, MDRSAKeyPair *keyPair) {
    vU1024 decryptedPayload;
    MDRSAFastModuloPow(encryptedPayload, &(keyPair->privateKey.d),
            &(keyPair->publicKey.n), &decryptedPayload);
    
    
    return decryptedPayload;
}
